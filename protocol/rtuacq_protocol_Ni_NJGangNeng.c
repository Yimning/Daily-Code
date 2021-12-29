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


int protocol_Ni_NJGangNeng(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[40]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	char data_buff[20]={0};
	int tem=0;
	float ni=0; 
	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%06#RDD0152101525**\r\n");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"NJGangNeng Ni SEND:",com_tbuf,size);
	size =write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJGangNeng Ni RECV:",com_rbuf,size);
	SYSLOG_DBG("NJGangNeng Ni protocol,Ni : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("NJGangNeng Ni data: %s",com_rbuf);
	if((size>=30)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0'))
	{
		strncpy(data_buff,&com_rbuf[18],8);
		tem=atoi(data_buff);
		ni=((float)tem)/10;
	}
	else
	{
		status=1;
		ni=0;
	}
	ret=acqdata_set_value(acq_data,"w20121",UNIT_MG_L,ni,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
