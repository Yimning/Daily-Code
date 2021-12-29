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

int protocol_liquid_level_2088(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float level=0,reservoir=0;
	float area=0,density=0;
	int val=0;
	int decimals=0;
	int ret=0;
	int i;
	int arg_n=0;
	unsigned int addr=1;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;
	addr = modbusarg->devaddr&0xffff;
	area=modbusarg->urAt;
	density=modbusarg->irAt;

	sleep(2);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,addr,0x3,0x03,0x3);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"liquid level 2088 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("liquid level 2088 protocol,liquid level : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("liquid level 2088 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"liquid level 2088 RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==addr)&&(com_rbuf[1]==0x03))
	{
		decimals=com_rbuf[3];
		decimals<<=8;
		decimals+=com_rbuf[4];
		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		level=val;

		for(i=0;i<decimals;i++)
		{
			level/=10;
		}
		
		reservoir=level*area*density;
		status=0;
	
	}
	else
	{
		level=0;
		reservoir=0;
		status=1;
	}
	
	
	ret=acqdata_set_value(acq_data,"we1101",UNIT_M,level,&arg_n);
	ret=acqdata_set_value(acq_data,"we0001",UNIT_T,reservoir,&arg_n);
	ret=acqdata_set_value(acq_data,"we1102",UNIT_M,level,&arg_n);
	ret=acqdata_set_value(acq_data,"we0002",UNIT_T,reservoir,&arg_n);
	ret=acqdata_set_value(acq_data,"we1103",UNIT_M,level,&arg_n);
	ret=acqdata_set_value(acq_data,"we0003",UNIT_T,reservoir,&arg_n);
	ret=acqdata_set_value(acq_data,"we1104",UNIT_M,level,&arg_n);
	ret=acqdata_set_value(acq_data,"we0004",UNIT_T,reservoir,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



