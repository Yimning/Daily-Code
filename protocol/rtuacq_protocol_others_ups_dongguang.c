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

int protocol_others_ups_dongguang(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ups=0,water=0;
	int ret=0;
	int arg_n=0;
	int devaddr;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x51;
	com_tbuf[1]=0x31;
	com_tbuf[2]=0x0D;
	size=3;
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"dongguang UPS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("dongguang UPS protocol,UPS: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("dongguang UPS data %s\n",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"dongguang UPS RECV:",com_rbuf,size);
	if((size>=46)&&(com_rbuf[0]==0x28))
	{
		ups=com_rbuf[38]-0x30;
		status=0;
	}
	else
	{
		ups=0;
		status=1;
	}
	read_device(DEV_DI0,&water,sizeof(int));
	water=!water;

	ret=acqdata_set_value(acq_data,"wg0007",UNIT_NONE,ups,&arg_n);
	ret=acqdata_set_value(acq_data,"eg0000",UNIT_NONE,water,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}



