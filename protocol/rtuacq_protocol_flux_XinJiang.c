#if 0
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
#endif

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"


static const  char * const polcode2005HB[] = {"B01","B00"};
static const  char * const polcode2017HB[] = {"w00000","w00001"};
static const UNIT_DECLARATION const units[]={UNIT_M3_H,UNIT_M3};

int protocol_FLUX_XinJiang(struct acquisition_data *acq_data)
{
#if 0
	int status=0;
   	char com_rbuf[2048]={0}; 
   	int size=0;
   	int arg_n=0;
   	int ret=0;
  	float total_flux_M3=0,speed=0;
   	union uf f;
	int i=0;
	
      	if(!acq_data) return -1;
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("XinJiang flux protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XinJiang flux data",com_rbuf,size);
	
	if(size>=20 && com_rbuf[0]==0x24 && com_rbuf[2]==0x54)
	{
		for(i=3;i<size-1;i+=8)
		{
			if(!strncmp(&com_rbuf[i+1],"B01",3))
			{
				f.ch[3] = com_rbuf[i+4];
				f.ch[2] = com_rbuf[i+5];
				f.ch[1] = com_rbuf[i+6];
				f.ch[0] = com_rbuf[i+7];
				speed = f.f;
			}
			else if(!strncmp(&com_rbuf[i+1],"B00",3))
			{
				f.ch[3] = com_rbuf[i+4];
				f.ch[2] = com_rbuf[i+5];
				f.ch[1] = com_rbuf[i+6];
				f.ch[0] = com_rbuf[i+7];
				total_flux_M3= f.f;
			}
		}
	
		status = 0;
	}
	else
	{
		total_flux_M3 = 0;
		speed = 0;
		status = 1;
	}

	acq_data->total=0;
	acqdata_set_value(acq_data,"w00000",UNIT_M3_H,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    	return arg_n;
#endif

	int ret=0;
	int arg_n=0;
	ret= protocol_XinJiang(acq_data,polcode2005HB,ARRAY_SIZE(polcode2005HB),
		                            polcode2017HB,ARRAY_SIZE(polcode2017HB),
		                            units,ARRAY_SIZE(units));
	if(ret<0) 
	{
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
	}
	else
	{
		arg_n=ret;
		return arg_n;
	}

	return arg_n;

}
