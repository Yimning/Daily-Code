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

int protocol_level_BS(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float level=0,reservoir=0;
	float area=0,density=0;
	int ret=0;
	int val,format,max,min;
	int i;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	area=modbusarg->urAt;
	density=modbusarg->irAt;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x03,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BS level SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BS level protocol,level : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BS level data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BS level RECV:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		format=com_rbuf[3];
		format<<=8;
		format+=com_rbuf[4];

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
/*
		min=com_rbuf[7];
		min<<=8;
		min+=com_rbuf[8];

		max=com_rbuf[9];
		max<<=8;
		max+=com_rbuf[10];
		
		level=(val+32768)/(32767.0+32768.0)*(max-min)+min;
*/
		level=val;
		for(i=0;i<format;i++)
			level/=10;
		
		reservoir=level*area*density;
		level*=100;
		status=0;
	}
	else
	{
		level=0;
		reservoir=0;
		status=1;
	}

	acqdata_set_value(acq_data,"we1101",UNIT_CM,level,&arg_n);
	acqdata_set_value(acq_data,"we0001",UNIT_T,reservoir,&arg_n);
	acqdata_set_value(acq_data,"we1102",UNIT_CM,level,&arg_n);
	acqdata_set_value(acq_data,"we0002",UNIT_T,reservoir,&arg_n);
	acqdata_set_value(acq_data,"we1103",UNIT_CM,level,&arg_n);
	acqdata_set_value(acq_data,"we0003",UNIT_T,reservoir,&arg_n);
	acqdata_set_value(acq_data,"we1104",UNIT_CM,level,&arg_n);
	acqdata_set_value(acq_data,"we0004",UNIT_T,reservoir,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



