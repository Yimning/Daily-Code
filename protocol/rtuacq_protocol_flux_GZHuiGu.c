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


int protocol_FLUX_GZHuiGu(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[20]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_flux_M3=0,speed=0;
   //char databuf[30]={0};
   char *ppack;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,":990471C2\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data),com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
    SYSLOG_DBG("flux GZHuiGu protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG("flux GZHuiGu data:%s\n",com_rbuf);
	
	if(((ppack=strchr(com_rbuf,':'))!=NULL)&&((ppack=strchr(ppack,'\n'))!=NULL)&&(size>7))
	{
		ppack-=5;
		if(ppack!=NULL) speed=atof(ppack);
		else goto ERROR;
			
	}
	else goto ERROR;

	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,":990472C1\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
    SYSLOG_DBG("flux GZHuiGu protocol2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG("flux GZHuiGu data2:%s\n",com_rbuf);
	
	if(((ppack=strchr(com_rbuf,':'))!=NULL)&&((ppack=strchr(ppack,'\n'))!=NULL)&&(size>12))
	{
		ppack-=10;
		if(ppack!=NULL) total_flux_M3=atof(ppack);
		else goto ERROR;

		acq_data->total=total_flux_M3;

	}
 	else
 	{
 			goto ERROR;
 	}


	/*if((size>=12)&&(com_rbuf[0]==':'))
		{
		memset(databuf,0,sizeof(databuf));
		strncpy(databuf,&com_rbuf[7],20);
		speed=atof(databuf);
		status=0;
		printf("the copy string is %s\n",databuf);
		printf("the speed is %f\n",speed);
	}
	else
		{
		status=1;
		speed=0;
		printf("the data of speed recieve error\n");
	}
	sleep(2);
	if(status==0)
		{
		memset(com_tbuf,0,sizeof(com_tbuf));
		strcpy(com_tbuf,":990472C1\r\n");
		size=strlen(com_tbuf);
		size=write_device(DEV_NAME(acq_data), com_tbuf, size);
		sleep(2);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
		if((size>=17)&&(com_rbuf[0]==':'))
			{
			memset(databuf,0,sizeof(databuf));
			strncpy(databuf,&com_rbuf[7],20);
			total_flux=atof(databuf);
			status=0;
			printf("the copy string is %s\n",databuf);
			printf("the total_flux is %f\n",total_flux);
		}
		else
			{
			total_flux=0;
			status=1;
			printf("the data of totalflux recieve error\n");
		}
		}*/
		printf("the speed is %f,the total_flux is %f\n",speed,total_flux_M3);
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;

ERROR:
 			acqdata_set_value(acq_data,"w00000",UNIT_L_S,0,&arg_n);
 			acqdata_set_value(acq_data,"w00001",UNIT_M3,0,&arg_n);
 			acq_data->acq_status = ACQ_ERR;
			printf("ERROR\n");
 			return arg_n;
			
}
