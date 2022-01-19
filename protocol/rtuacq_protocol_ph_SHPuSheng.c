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
#include "rtuacq_protocol_error_cache.h"

/*
Author:Yimning
Time:2021.08.30 Mon.
Description:protocol_ph_SHPuSheng
TX: 01 03 00 00 00 04 44 09 
RX: 01 03 08 
41 03 32 33 
41 03 32 33 
D2 74

DataType and Analysis:
	(FLOAT_ABCD)  41 03 32 33 = 8.200
*/

int protocol_ph_SHPuSheng(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	float ph = 0,temp = 0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x04;
	dataType = FLOAT_ABCD ;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SHPuSheng PH SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("SHPuSheng PH protocol,PH : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SHPuSheng PH data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SHPuSheng PH RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		ph = getFloatValue(p, 3, dataType);
		
		temp=getFloatValue(p, 7, dataType);
		
		status=0;
	}
	else
	{
		ph=0;
		temp=0;
		status=1;
	}
	acqdata_set_value(acq_data,"w01001",UNIT_PH,ph,&arg_n);
	acqdata_set_value(acq_data,"w01010",UNIT_0C,temp,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
