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

int protocol_VOCs_DGZhongGeng(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 11

	char *polcode[POLCODE_NUM]={
		"g09001","g29001","g28001","g28101","g20201",
		"g28201","e70201","e80101","e30101","e30001",
		"e80201"};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,UNIT_MG_M3,UNIT_0C,UNIT_PECENT,UNIT_M_S,
		UNIT_PA,UNIT_A,UNIT_KW,UNIT_KW,UNIT_KWH,
		UNIT_A};
		
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf=0;
	int ret=0;
	int arg_n=0;
	int i;
	int devaddr;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x16);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DGZhongGeng voc SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DGZhongGeng voc protocol,voc : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DGZhongGeng voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DGZhongGeng voc RECV:",com_rbuf,size);
	if((size>=49)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			f.ch[0]=com_rbuf[6+i*4];
			f.ch[1]=com_rbuf[5+i*4];
			f.ch[2]=com_rbuf[4+i*4];
			f.ch[3]=com_rbuf[3+i*4];
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
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



