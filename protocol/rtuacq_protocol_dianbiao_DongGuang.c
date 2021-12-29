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

int protocol_dianbiao_DongGuang(struct acquisition_data *acq_data)
{
	int status=0;
	int val=0;
	union uf  f;
	union uf64 f64;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float ia=0,ib=0,ic=0;
	int ret=0;
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
	size=modbus_pack(com_tbuf,devaddr,0x03,0x03,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DGYongBang ec SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DGYongBang ec protocol,ec : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DGYongBang ec data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DGYongBang ec RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		ia=val*0.1;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		ib=val*0.1;
		
		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		ic=val*0.1;
	}
	else
	{
		ia=0;
		ib=0;
		ic=0;
		status=1;
	}


	acqdata_set_value(acq_data,"ec0201",UNIT_A,ia,&arg_n);
	acqdata_set_value(acq_data,"ec0202",UNIT_A,ib,&arg_n);
	acqdata_set_value(acq_data,"ec0203",UNIT_A,ic,&arg_n);
	acqdata_set_value(acq_data,"ed0201",UNIT_A,ia,&arg_n);
	acqdata_set_value(acq_data,"ed0202",UNIT_A,ib,&arg_n);
	acqdata_set_value(acq_data,"ed0203",UNIT_A,ic,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



