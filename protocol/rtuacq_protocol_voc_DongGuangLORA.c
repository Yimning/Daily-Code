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

int protocol_VOCs_DongGuangLORA(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float speed=0,temp=0,humi=0,pres=0,voc=0;
	int val;
	int ret=0;
	int arg_n=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,modbusarg->devaddr&0xffff,modbusarg->devfun&0xffff,modbusarg->devstaddr&0xffff,modbusarg->regcnt&0xffff);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"lora voc SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("lora voc protocol,voc : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("lora voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"lora voc RECV:",com_rbuf,size);
	if((size>=25)&&(com_rbuf[0]==modbusarg->devaddr)&&(com_rbuf[1]==modbusarg->devfun))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		voc=f.f;

		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[7];
		temp=f.f;

		f.ch[0]=com_rbuf[14];
		f.ch[1]=com_rbuf[13];
		f.ch[2]=com_rbuf[12];
		f.ch[3]=com_rbuf[11];
		humi=f.f;

		f.ch[0]=com_rbuf[18];
		f.ch[1]=com_rbuf[17];
		f.ch[2]=com_rbuf[16];
		f.ch[3]=com_rbuf[15];
		speed=f.f;

		f.ch[0]=com_rbuf[22];
		f.ch[1]=com_rbuf[21];
		f.ch[2]=com_rbuf[20];
		f.ch[3]=com_rbuf[19];
		pres=f.f;
		
		status=0;
	}
	else
	{
		speed=0;
		temp=0;
		humi=0;
		pres=0;
		voc=0;
		status=1;
	}

	ret=acqdata_set_value(acq_data,"g20201",UNIT_M_S,speed,&arg_n);
	ret=acqdata_set_value(acq_data,"g28001",UNIT_0C,temp,&arg_n);
	ret=acqdata_set_value(acq_data,"g28101",UNIT_PECENT,humi,&arg_n);
	ret=acqdata_set_value(acq_data,"g28201",UNIT_PA,pres,&arg_n);
	ret=acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,voc,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



