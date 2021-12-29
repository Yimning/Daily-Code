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

int protocol_level_SZPengYue_HCS60(struct acquisition_data *acq_data)
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
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZPengYue HCS60 level SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZPengYue HCS60 level protocol,level : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZPengYue HCS60 level data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZPengYue HCS60 level RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x02))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		level=f.f;
		reservoir=level*area*density;
		
		status=0;
	}
	else
	{
		level=0;
		reservoir=0;
		status=1;
	}

	acqdata_set_value(acq_data,"we1101",UNIT_M,level,&arg_n);
	acqdata_set_value(acq_data,"we0001",UNIT_T,reservoir,&arg_n);
	acqdata_set_value(acq_data,"we1102",UNIT_M,level,&arg_n);
	acqdata_set_value(acq_data,"we0002",UNIT_T,reservoir,&arg_n);
	acqdata_set_value(acq_data,"we1103",UNIT_M,level,&arg_n);
	acqdata_set_value(acq_data,"we0003",UNIT_T,reservoir,&arg_n);
	acqdata_set_value(acq_data,"we1104",UNIT_M,level,&arg_n);
	acqdata_set_value(acq_data,"we0004",UNIT_T,reservoir,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



