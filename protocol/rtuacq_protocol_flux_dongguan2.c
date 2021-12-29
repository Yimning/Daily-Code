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

int protocol_FLUX_dongguan2(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	long val;
	float valf;
	float total=0;
	float speed=0;
	int ret=0;
	int arg_n=0;
	int i;
	unsigned int addr=1;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;
	addr = modbusarg->devaddr&0xffff;

	status=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,addr,0x03,0x0BBA,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"dongguan2 speed SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("dongguan2 speed protocol,flux : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("dongguan2 speed data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"dongguan2 speed RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==addr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		speed=val;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		speed=speed+val/10000.0;
	}
	else
	{
		speed=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,addr,0x03,0x0BC9,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"dongguan2 total SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("dongguan2 total protocol,flux : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("dongguan2 total data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"dongguan2 total RECV:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==addr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		total=f.l;

		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		valf=f.l;
		for(i=0;i<9;i++) valf/=10;
		total=total+valf;
	}
	else
	{
		total=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w10101",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w10001",UNIT_M3,total,&arg_n);
	acqdata_set_value(acq_data,"w10102",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w10002",UNIT_M3,total,&arg_n);
	acqdata_set_value(acq_data,"w10103",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w10003",UNIT_M3,total,&arg_n);
	acqdata_set_value(acq_data,"w10104",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w10004",UNIT_M3,total,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



