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

int protocol_VOCs_TianZhiLan2(struct acquisition_data *acq_data)
{
#define VOC_POLCODE_NUM1 8
	char *vocPolcode1[VOC_POLCODE_NUM1]={
		"g19501","g09001","g29001","g20301",
		"g28001","g28201","g20201","g20101"
	};

	UNIT_DECLARATION unit1[VOC_POLCODE_NUM1]={
		UNIT_PECENT,UNIT_MG_M3,UNIT_MG_M3,UNIT_M3_H,
		UNIT_0C,UNIT_PA,UNIT_M_S,UNIT_M3_H
	};

#define VOC_POLCODE_NUM2 5
	char *vocPolcode2[VOC_POLCODE_NUM2]={
		"e70201","e70202","e70203","e70001","e70101"
	};

	UNIT_DECLARATION unit2[VOC_POLCODE_NUM2]={
		UNIT_A,UNIT_A,UNIT_A,UNIT_KWH,UNIT_KW
	};

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf;
	int i=0;
	union uf  f;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	
	devaddr=modbusarg->devaddr&0xffff;
	status=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x03E8,0x10);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"TianZhiLan voc SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("TianZhiLan voc protocol,voc 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("TianZhiLan voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"TianZhiLan voc RECV1:",com_rbuf,size);
	if((size>=37)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		for(i=0;i<VOC_POLCODE_NUM1;i++)
		{
			f.ch[0]=com_rbuf[4+i*4];
			f.ch[1]=com_rbuf[3+i*4];
			f.ch[2]=com_rbuf[6+i*4];
			f.ch[3]=com_rbuf[5+i*4];
			valf=f.f;
			acqdata_set_value(acq_data,vocPolcode1[i],unit1[i],valf,&arg_n);
		}
	}
	else
	{
		for(i=0;i<VOC_POLCODE_NUM1;i++)
		{
			acqdata_set_value(acq_data,vocPolcode1[i],unit1[i],0,&arg_n);
		}
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x040A,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"TianZhiLan voc SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("TianZhiLan voc protocol,voc 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("TianZhiLan voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"TianZhiLan voc RECV2:",com_rbuf,size);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		for(i=0;i<VOC_POLCODE_NUM2;i++)
		{
			f.ch[0]=com_rbuf[4+i*4];
			f.ch[1]=com_rbuf[3+i*4];
			f.ch[2]=com_rbuf[6+i*4];
			f.ch[3]=com_rbuf[5+i*4];
			valf=f.f;
			acqdata_set_value(acq_data,vocPolcode2[i],unit2[i],valf,&arg_n);
		}
	}
	else
	{
		for(i=0;i<VOC_POLCODE_NUM2;i++)
		{
			acqdata_set_value(acq_data,vocPolcode2[i],unit2[i],0,&arg_n);
		}
		status=1;
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


