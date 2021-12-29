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

int protocol_ANDAN_SZPengYue(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float andan=0,temp=0,ph=0;
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
	size=modbus_pack(com_tbuf,devaddr,0x3,0x30,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZPengYue temp SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZPengYue temp protocol,temp : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZPengYue temp data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZPengYue temp RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[3];
		f.ch[1]=com_rbuf[4];
		f.ch[2]=com_rbuf[5];
		f.ch[3]=com_rbuf[6];
		temp=f.f;
	}
	else
	{
		temp=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x4A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZPengYue ph SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZPengYue ph protocol,ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZPengYue ph data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZPengYue ph RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[3];
		f.ch[1]=com_rbuf[4];
		f.ch[2]=com_rbuf[5];
		f.ch[3]=com_rbuf[6];
		ph=f.f;
	}
	else
	{
		ph=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x52,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZPengYue andan SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZPengYue andan protocol,andan : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZPengYue andan data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZPengYue andan RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[3];
		f.ch[1]=com_rbuf[4];
		f.ch[2]=com_rbuf[5];
		f.ch[3]=com_rbuf[6];
		andan=f.f;
	}
	else
	{
		andan=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	acqdata_set_value(acq_data,"w01010",UNIT_0C,temp,&arg_n);
	acqdata_set_value(acq_data,"w01001",UNIT_PH,ph,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



