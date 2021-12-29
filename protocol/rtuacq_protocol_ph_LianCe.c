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

int protocol_ph_LianCe(struct acquisition_data *acq_data)
{
/*
	TX: 01 03 00 00 00 02 C5 DA
	RX: 01 03 04 02 AE 00 FA 0A E9
	
	ph = 33 33 F3 40 =7.6
*/

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float ph=0;
	float temp=0;
	int val=0;
	union uf f;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 0x00, 0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LianCe PH SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("LianCe PH protocol,PH : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LianCe PH data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LianCe PH RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{

		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		ph=val/100.0;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		temp=val/10.0;
		
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