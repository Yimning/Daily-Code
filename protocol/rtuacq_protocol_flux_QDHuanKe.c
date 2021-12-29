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


int protocol_FLUX_QDHuanKe(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_flux_M3=0.0,speed=0.0;
   union uf f;
   char *ppack;
      if(!acq_data) return -1;

	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	if(size<12)
	{
	    sleep(5);
		size=read_device(DEV_NAME(acq_data), &com_rbuf[size], sizeof(com_rbuf)-size)+size;
	}

	SYSLOG_DBG("FLUX_QDHuanKe protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("FLUX_QDHuanKe protocol",com_rbuf,size);
	
	if(size>12 &&(ppack=strstr(com_rbuf,"@@"))&&(strstr(ppack,"&&"))) 
		{
		f.ch[0]=ppack[5];
		f.ch[1]=ppack[4];
		f.ch[2]=ppack[3];
		f.ch[3]=ppack[2];
		speed=f.f;
		printf("the speed is %f\n",speed);
		f.f=0;
		f.ch[0]=ppack[9];
		f.ch[1]=ppack[8];
		f.ch[2]=ppack[7];
		f.ch[3]=ppack[6];
		total_flux_M3=f.f;
		printf("the total_flux is %f\n",total_flux_M3);
		status=0;

		acq_data->total=total_flux_M3;
		
	}
	else
	{
		speed=0.0;
		total_flux_M3=0.0;
		status=1;
		printf("error of recieve data\n");
	}
	
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
	}
