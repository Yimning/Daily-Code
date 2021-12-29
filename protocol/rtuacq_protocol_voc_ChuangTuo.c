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

int protocol_VOCs_ChuangTuo(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 16

	char *polcode[POLCODE_NUM]={
		"g29001","g28001","","","g18401",
		"e80201","e70201","","","e60201",
		"","","","","",
		"g20101"
	};

	char *polcode2[POLCODE_NUM]={
		"g29002","g28002","","","g18402",
		"e80202","e70202","","","e60202",
		"","","","","",
		"g20102"
	};

	char *polcode3[POLCODE_NUM]={
		"g29003","g28003","","","g18403",
		"e80203","e70203","","","e60203",
		"","","","","",
		"g20103"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_0C,		UNIT_KPA,	UNIT_M3_S,	UNIT_KPA,		
		UNIT_A,   		UNIT_A,		UNIT_KW,	UNIT_KWH,	UNIT_A,
		UNIT_KWH,		UNIT_KW,	UNIT_KWH,	UNIT_A,		UNIT_A,	
		UNIT_M3_S,
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
	char head[4]={0x01,0x03,0x40,0x00};
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	
	devaddr=modbusarg->devaddr&0xffff;
	head[0]=devaddr;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ChuangTuo voc SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ChuangTuo voc protocol,voc : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ChuangTuo voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ChuangTuo voc RECV:",com_rbuf,size);

	p=memstr(com_rbuf,size,head);
	if((size>=69)&&(p!=NULL))
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			if(!strcmp(polcode[i],""))
				continue;
			
			f.ch[0]=p[6+i*4];
			f.ch[1]=p[5+i*4];
			f.ch[2]=p[4+i*4];
			f.ch[3]=p[3+i*4];
			valf=f.f;
			acqdata_set_value(acq_data,polcode[i],unit[i],valf,&arg_n);
			acqdata_set_value(acq_data,polcode2[i],unit[i],valf,&arg_n);
			acqdata_set_value(acq_data,polcode3[i],unit[i],valf,&arg_n);
		}
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			if(!strcmp(polcode[i],""))
				continue;
			
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
			acqdata_set_value(acq_data,polcode2[i],unit[i],0,&arg_n);
			acqdata_set_value(acq_data,polcode3[i],unit[i],0,&arg_n);
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


