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


static const  char * const garbage_polcode[] = {"a01902","a01901","i33310","i33311","i33312","i33320","i33321","i33322","i33330","i33331","i33332"};

int protocol_CEMS_BJzhongdian(struct acquisition_data *acq_data)
{
	int status=0;
   	char com_rbuf[2048]={0}; 
   	int size=0;
   	int arg_n=0;
   	int ret=0;
  	float temp=0;
   	union uf f;
	int i=0;
	int len=0;
	
      	if(!acq_data) return -1;
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJzhongdian CEMS protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJzhongdian CEMS data",com_rbuf,size);

	len=ARRAY_SIZE(garbage_polcode);
	
	if(size>=49 && com_rbuf[1]==0x10 && com_rbuf[2]==0x2C)
	{
		for(i=0;i<len;i++)
		{
			f.ch[3] = com_rbuf[4*i+3];
			f.ch[2] = com_rbuf[4*i+4];
			f.ch[1] = com_rbuf[4*i+5];
			f.ch[0] = com_rbuf[4*i+6];
			temp = f.f;

			acqdata_set_value(acq_data,garbage_polcode[i],UNIT_0C,temp, &arg_n);
		}
	
		status = 0;
	}
	else
	{
		for(i=0;i<len;i++)
		{
			acqdata_set_value(acq_data,garbage_polcode[i],UNIT_0C,0, &arg_n);
		}
		status = 1;
	}
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    	return arg_n;
}
