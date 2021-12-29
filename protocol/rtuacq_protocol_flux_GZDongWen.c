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


int protocol_FLUX_GZDongWen(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret1=0,ret2=0;
   float total_flux_M3=0,speed=0;
   char *databuf1=NULL,*databuf2=NULL;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x39;
	//com_tbuf[1]=0x03;
	//com_tbuf[2]=0x90;
	size=1;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"GZDongWen flux SEND:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf,sizeof(com_rbuf)-1);
    SYSLOG_DBG("flux GZDongWen protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    //SYSLOG_DBG("flux GZDongWen data:%s\n",com_rbuf);
    SYSLOG_DBG("flux GZDongWen data : %s\n",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"GZDongWen flux RECV:",com_rbuf,size);

//	databuf1=strstr(com_rbuf,"Q=");
//	if(databuf1!=NULL) databuf2=strstr(databuf1,"M=");
	
	if(size>43 /*&& com_rbuf[0]==0x0&&com_rbuf[1]==0x0*/)
	{
		//databuf1=strstr(&com_rbuf[2],"Q=");
		databuf1=memstr(&com_rbuf[0],size,"Q=");
		if(databuf1!=NULL) databuf2=strstr(databuf1,"M=");
	}
	
	if((size>=43)&&(databuf1!=NULL)&&(databuf2!=NULL))
	{
		ret1=sscanf(databuf1,"Q=%fL",&speed);
		ret2=sscanf(databuf2,"M=%fM",&total_flux_M3);
		status=0;
		SYSLOG_DBG("speed=%f,total_flux=%f\n",speed,total_flux_M3);
		
		if((ret1!=1)||(ret2!=1))
		{
			status=1;
			speed=0;
			total_flux_M3=0;
			printf("process data error\n");
		}

		acq_data->total=total_flux_M3;

	}
	else
	{
		status=1;
		speed=0;
		total_flux_M3=0;
		printf("error of recieve data\n");
	}
	
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 20);
    return arg_n;
}

