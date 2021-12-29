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


int protocol_COD_HNHuaShiJie(struct acquisition_data *acq_data)
{
/*
TX:23 30 31 30 0d
RX:3e 30 31 30 31 41 20 18 30 31 30 38 31 30 34 39 36 30 34 30 31 0d
*/
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float cod=0;
 	union uf f;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	com_tbuf[1]=0x30;
	com_tbuf[2]=0x31;
	com_tbuf[3]=0x30;
	com_tbuf[4]=0x0d;
	size=5;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HNHuaShiJie COD SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaShiJie COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaShiJie COD data\n",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HNHuaShiJie COD RECV:",com_rbuf,size);
	if((size>=22)&&(com_rbuf[0]==0x3e)&&(com_rbuf[1]==0x30)&&(com_rbuf[2]==0x31))
	{
		//f.ch[3]=
		char codstr[10]={0};
		memcpy(codstr,&com_rbuf[16],4);
		//valf=atoi(codstr);
		ret=sscanf(codstr,"%f",&cod);
		if(com_rbuf[20]==0x30)cod*=1000;
		if(com_rbuf[20]==0x32)cod/=1000;
		SYSLOG_DBG("HuaShiJie COD =%f\n",cod);
	}
	else
	{
	   cod=0;
	   status=1;
	}	
	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,cod,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}