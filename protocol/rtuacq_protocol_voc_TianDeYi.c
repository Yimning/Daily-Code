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

int protocol_VOC_TianDeYi(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int index=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

#define CEMS_POLCODE_NUM 14
	char *polcode[CEMS_POLCODE_NUM]={
		"a00000","a01011","a01012","a01013","a01014",
		"a19001","a24087","a05002","a24088","a25002",
		"a25003","a25006","a25007","a25005"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_M3_S,		UNIT_M_S,		UNIT_0C,			UNIT_KPA,		UNIT_PECENT,
		UNIT_PECENT,	UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3
	};

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x1C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"TianDeYi VOC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("TianDeYi VOC protocol,VOC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("TianDeYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"TianDeYi VOC RECV:",com_rbuf,size);
	if((size>=61)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{	
			index=i*4+3;

			f.ch[0]=com_rbuf[index+3];
			f.ch[1]=com_rbuf[index+2];
			f.ch[2]=com_rbuf[index+1];
			f.ch[3]=com_rbuf[index+0];
			acqdata_set_value(acq_data,polcode[i],unit[i],f.f,&arg_n);
		}
		status=0;
	}
	else
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}
		status=1;
	}

	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,f.f,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

