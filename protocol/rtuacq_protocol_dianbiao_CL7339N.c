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

int protocol_dianbiao_CL7339N(struct acquisition_data *acq_data)
{
	int status=0;
	int val=0;
	union uf  f;
	union uf64 f64;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float ec=0,power=0,pt=0;
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
	size=modbus_pack(com_tbuf,devaddr,0x3,0x16,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CL7339N ec SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CL7339N ec protocol,ec : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CL7339N ec data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CL7339N ec RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		ec=f.f;
	}
	else
	{
		ec=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x2E,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CL7339N pt SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CL7339N pt protocol,pt : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CL7339N pt data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CL7339N pt RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		pt=f.f;
	}
	else
	{
		pt=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x07D0,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CL7339N power SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CL7339N power protocol,power : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CL7339N power data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CL7339N power RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val<<=8;
		val+=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		power=val/100.0;
	}
	else
	{
		power=0;
		status=1;
	}

	//acqdata_set_value(acq_data,"e30201",UNIT_A,ec,&arg_n);
	//acqdata_set_value(acq_data,"e30101",UNIT_KW,pt,&arg_n);
	//acqdata_set_value(acq_data,"e30001",UNIT_KWH,power,&arg_n);
	//acqdata_set_value(acq_data,"e30202",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e30102",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30002",UNIT_KWH,power,&arg_n);
	//acqdata_set_value(acq_data,"e30203",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e30103",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30003",UNIT_KWH,power,&arg_n);
	//acqdata_set_value(acq_data,"e30204",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e30104",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30004",UNIT_KWH,power,&arg_n);
	//acqdata_set_value(acq_data,"e30205",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e30105",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30005",UNIT_KWH,power,&arg_n);
	/*
	acqdata_set_value(acq_data,"e70201",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e70101",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e70001",UNIT_KWH,power,&arg_n);
	acqdata_set_value(acq_data,"e70202",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e70102",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e70002",UNIT_KWH,power,&arg_n);
	acqdata_set_value(acq_data,"e70203",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e70103",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e70003",UNIT_KWH,power,&arg_n);
	acqdata_set_value(acq_data,"e70204",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e70104",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e70004",UNIT_KWH,power,&arg_n);
	*/
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



