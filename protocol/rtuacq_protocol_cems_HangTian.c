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

int protocol_CEMS_HangTian(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 12

	char *polcode[POLCODE_NUM]={
		"a21026","a21026z","a21002","a21002z","a34013",
		"a34013z","a19001","a01012","a01013","a01011",
		"a01014","a00000"
	};


	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		
		UNIT_MG_M3,		UNIT_PECENT,	UNIT_0C,			UNIT_PA,			UNIT_M_S,
		UNIT_PECENT,	UNIT_M3_H
	};

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf;
	int i=0;
	union uf  f;
	int arg_n=0;
	char *p;
	char head[4]={0x01,0x03,0x30,0x00};
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	
	devaddr=modbusarg->devaddr&0xffff;
	head[0]=devaddr;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x18);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HangTian CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HangTian CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HangTian CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HangTian CEMS RECV:",com_rbuf,size);

	p=memstr(com_rbuf,size,head);
	if((size>=53)&&(p!=NULL))
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			f.ch[0]=p[6+i*4];
			f.ch[1]=p[5+i*4];
			f.ch[2]=p[4+i*4];
			f.ch[3]=p[3+i*4];
			valf=f.f;
			acqdata_set_value(acq_data,polcode[i],unit[i],valf,&arg_n);
		}
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}
		
		status=1;
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 20);
	return arg_n;
}


