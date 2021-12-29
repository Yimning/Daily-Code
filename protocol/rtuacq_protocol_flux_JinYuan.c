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


int protocol_FLUX_JinYuan(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char cmd=0x11;
   int size=0;
   int arg_n=0;
   int ret=0;
  float speed=0;
   int tem=0;
      if(!acq_data) return -1; 
	size=write_device(DEV_NAME(acq_data),&cmd,sizeof(cmd));
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("flux JinYuan protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG("flux JinYuan protocol",com_rbuf,size);
	
	if((size>=6)&&(strchr(&com_rbuf[0],0x0D)!=NULL))
	{
		tem=atoi(&com_rbuf[0]);
		speed=tem;
		speed/=100;
		status=0;
		printf("speed=%f\n",speed);
	}
	else
		{
		speed=0;
		status=1;
		printf("error of recieve data\n");
	}
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
