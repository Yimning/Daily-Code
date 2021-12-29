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


int protocol_FLUX_DLBoKeSi(struct acquisition_data *acq_data)
{
/*
	TX:	01 03 00 00 00 0A C5 CD
	RX:	01 03 14 00 02 00 00 00 C9 00 01 8F 0B 00 00 00 00 00 00 00 00 00 00 00 ... FF FF
	speed=20.1
	total_flux_M3=102155
*/
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	int value=0;
	float speed=0;
	int total_flux_M3=0;
	unsigned int addr=1;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;
	addr = modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, addr, 0x03, 0x00, 0x0A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DLBoKeSi speed SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("flux DLBoKeSi protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("flux DLBoKeSi protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DLBoKeSi speed RECV:",com_rbuf,size);
	if((size>=25)&&(com_rbuf[0]==addr)&&(com_rbuf[1]=0x03))
	{	
		value=getInt32Value(com_rbuf,5, LONG_ABCD);
		speed=value*0.1;
		
		value=getInt32Value(com_rbuf,9, LONG_ABCD);
		total_flux_M3=value;
		
		status=0;

		acq_data->total=total_flux_M3;
	}
	else
	{
		speed=0;
		total_flux_M3=0;
		status = 1;
	}
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

