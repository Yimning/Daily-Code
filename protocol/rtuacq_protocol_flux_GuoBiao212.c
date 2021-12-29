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


int protocol_FLUX_GuoBiao212(struct acquisition_data *acq_data)
{
   int status=0;
   int status1=0,status2=0;
   char com_rbuf[2048]={0};
   int size=0;
   int arg_n=0;
   int ret1=0,ret2=0; 
   float total_flux_M3=0,speed=0;
   char *ppack;
      if(!acq_data) return -1;

/*	//del by miles zhang 20190217  
	  sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	printf("pack:%s\n",com_rbuf);
	if((ppack=strstr(com_rbuf,"##") )!=NULL)
		{
		
		ret1=GuoBiao212(ppack,"B01",&speed,&status1);
		printf("speed=%f\n",speed);
		ppack=strstr(com_rbuf,"##");
		ret2=GuoBiao212(ppack,"B00", &total_flux, &status2);
		printf("total_cod=%f\n",total_flux);
		if(ret1!=0||ret2!=0||status1!='N'||status2!='N'){
			acqdata_set_value(acq_data,"B01",UNIT_L_S,0,&arg_n);
			acqdata_set_value(acq_data,"B00",UNIT_M3,0,&arg_n);
			acq_data->acq_status = ACQ_ERR;
			return arg_n;
			} 
		status=0;
		}
	else
		{
		status=1;
		speed=0;
		total_flux=0;
	}
*/	
	printf("speed=%f,total_cod=%f\n",speed,total_flux_M3);
  	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
