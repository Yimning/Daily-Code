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

int protocol_CL_JSZhuoZheng(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float cl=0,ph=0;
	int ret=0;
	int arg_n=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,modbusarg->devaddr&0xffff,modbusarg->devfun&0xffff,modbusarg->devstaddr&0xffff,modbusarg->regcnt&0xffff);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JSZhuoZheng cl SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JSZhuoZheng cl protocol,CL : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JSZhuoZheng cl data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JSZhuoZheng cl RECV:",com_rbuf,size);
	if((size>=(5+(modbusarg->regcnt&0xffff)*2))&&
		(com_rbuf[0]==modbusarg->devaddr&0xffff)&&
		(com_rbuf[1]==modbusarg->devfun&0xffff))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		cl=f.f;

		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[7];
		ph=f.f;
		
		status=0;
	}
	else
	{
		cl=0;
		ph=0;
		
		status=1;
	}

	acqdata_set_value(acq_data,"w21024",UNIT_MG_L,cl,&arg_n);
	acqdata_set_value(acq_data,"w01001",UNIT_PH,ph,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



