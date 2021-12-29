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

int protocol_dianbiao_GZBoKong_K35A(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float eq=0,pt=0;
	int ret=0;
	int arg_n=0;
	char *p;
	char head[4];
	
	int devaddr=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"GZBoKong K35A SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("GZBoKong K35A protocol,K35A :read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("GZBoKong K35A data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"GZBoKong K35A RECV:",com_rbuf,size);
	
	head[0]=devaddr;
	head[1]=0x03;
	head[2]=0x0C;
	head[3]=0x00;
	p=memstr(com_rbuf,size,head);
	
	if((size>=17)&&(p!=NULL))
	{
		f.ch[0]=p[3];
		f.ch[1]=p[4];
		f.ch[2]=p[5];
		f.ch[3]=p[6];
		eq=f.f;

		f.ch[0]=p[11];
		f.ch[1]=p[12];
		f.ch[2]=p[13];
		f.ch[3]=p[14];
		pt=f.f;		

		status=0;
	}
	else
	{
		eq=0;
		pt=0;
		status=1;
	}


	acqdata_set_value(acq_data,"e30001",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30101",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30002",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30102",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30003",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30103",UNIT_KW,pt,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,20);
	return arg_n;
}



