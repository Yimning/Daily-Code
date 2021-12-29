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



/*
Author:Yimning
Time:2021.06.05
Description:protocol_MARK_HeBeiZhongDian
TX:01 03 00 00 00 09 85 CC 
RX:01 03 12 00 00 42 C8 00 00 00 1F 42 C9 00 00 00 16 42 C7 00 00 1F B2

*/

int protocol_MARK_HeBeiZhongDian(struct acquisition_data *acq_data)
{
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0}; 
	int size=0;
	int val;
	union uf f;
	float pt=0,evaporation=0,evaporation1=0,evaporation2=0;
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
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x09);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HeBeiZhongDian MARK SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HeBeiZhongDian MARK protocol,MARK 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HeBeiZhongDian MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBeiZhongDian MARK RECV1:",com_rbuf,size);
	if((size>=23)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		
		f.ch[0]=com_rbuf[8];
		f.ch[1]=com_rbuf[7];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		evaporation=f.f;
		
		if(val== 0)
			mark=MARK_N;
		else if(val==0x01)
			mark=MARK_DC_Sd;
		else if(val==0x15)
			mark=MARK_DC_Fa;
		else if(val==0x16)
			mark=MARK_DC_Fb;
		else if(val==0x1F)
			mark=MARK_DC_Sta;
		else if(val==0x20)
			mark=MARK_DC_Stb;
		else if(val==0x04)
			mark=MARK_DC_Sr;
		else
			mark=MARK_DC_Sr;

		status=0;
	}
	else
	{
		evaporation=0;
		mark=MARK_DC_Sr;
		status=1;
	}
	val=mark;
	acqdata_set_value(acq_data,"p10101",UNIT_NONE,(float)val,&arg_n);
	acqdata_set_value(acq_data,"p10201",UNIT_T_H,evaporation,&arg_n);


	if((size>=23)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		
		f.ch[0]=com_rbuf[14];
		f.ch[1]=com_rbuf[13];
		f.ch[2]=com_rbuf[12];
		f.ch[3]=com_rbuf[11];
		evaporation1=f.f;
		
		if(val== 0)
			mark=MARK_N;
		else if(val==0x01)
			mark=MARK_DC_Sd;
		else if(val==0x15)
			mark=MARK_DC_Fa;
		else if(val==0x16)
			mark=MARK_DC_Fb;
		else if(val==0x1F)
			mark=MARK_DC_Sta;
		else if(val==0x20)
			mark=MARK_DC_Stb;
		else if(val==0x04)
			mark=MARK_DC_Sr;
		else
			mark=MARK_DC_Sr;

		status=0;
	}
	else
	{
		evaporation1=0;
		mark=MARK_DC_Sr;
		status=1;
	}
	val=mark;
	acqdata_set_value(acq_data,"p10102",UNIT_NONE,(float)val,&arg_n);
	acqdata_set_value(acq_data,"p10202",UNIT_T_H,evaporation1,&arg_n);


	if((size>=23)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		
		f.ch[0]=com_rbuf[20];
		f.ch[1]=com_rbuf[19];
		f.ch[2]=com_rbuf[18];
		f.ch[3]=com_rbuf[17];
		evaporation2=f.f;
		
		if(val== 0)
			mark=MARK_N;
		else if(val==0x01)
			mark=MARK_DC_Sd;
		else if(val==0x15)
			mark=MARK_DC_Fa;
		else if(val==0x16)
			mark=MARK_DC_Fb;
		else if(val==0x1F)
			mark=MARK_DC_Sta;
		else if(val==0x20)
			mark=MARK_DC_Stb;
		else if(val==0x04)
			mark=MARK_DC_Sr;
		else
			mark=MARK_DC_Sr;

		status=0;
	}
	else
	{
		evaporation2=0;
		mark=MARK_DC_Sr;
		status=1;
	}
	val=mark;
	acqdata_set_value(acq_data,"p10103",UNIT_NONE,(float)val,&arg_n);
	acqdata_set_value(acq_data,"p10203",UNIT_T_H,evaporation2,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	return arg_n;
}

