#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

int protocol_MARK_JiuTian(struct acquisition_data *acq_data)
{
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0}; 
	int size=0;
	int val;
	union uf f;
	float pt=0,evaporation=0,evaporation2=0;
	CONDITION_MARK mark;
	int ret=0;
	unsigned int devaddr=0;
	unsigned int cmd;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	cmd=0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JiuTian MARK SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JiuTian MARK protocol,MARK 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JiuTian MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JiuTian MARK RECV1:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		evaporation=val*0.01;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		evaporation2=val*0.01;

		status=0;
	}
	else
	{
		evaporation=0;
		evaporation2=0;
		status=1;
	}


	sleep(1);
	cmd=0x02;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JiuTian MARK SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JiuTian MARK protocol,MARK 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JiuTian MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JiuTian MARK RECV2:",com_rbuf,size);
	if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		
		if(val==0x00)
			mark=MARK_N;
		else if((val&0x01)==0x01)
			mark=MARK_DC_Sd;
		else if((val&0x02)==0x02)
			mark=MARK_DC_Fa;
		else if((val&0x04)==0x04)
			mark=MARK_DC_Fb;
		else if((val&0x08)==0x08)
			mark=MARK_DC_Sta;
		else if((val&0x10)==0x10)
			mark=MARK_DC_Stb;
		else if((val&0x20)==0x20)
			mark=MARK_DC_Sr;
		else
			mark=MARK_DC_Sr;
	}
	else
	{
		mark=MARK_DC_Sr;
	}

	val=mark;

	acqdata_set_value(acq_data,"p10101",UNIT_NONE,(float)val,&arg_n);
	acqdata_set_value(acq_data,"p10201",UNIT_T_H,evaporation,&arg_n);
	acqdata_set_value(acq_data,"p10202",UNIT_T_H,evaporation2,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	return arg_n;
}

