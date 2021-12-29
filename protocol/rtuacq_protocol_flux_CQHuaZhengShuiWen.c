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


int protocol_FLUX_CQHuaZhengShuiWen(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char cmd=0x11;
   int size=0;
   int arg_n=0;
   int ret=0;
   float speed=0,total_flux_M3=0;
   int temp=0;
   union uf f;
      if(!acq_data) return -1;
	size=1;
	size=write_device(DEV_NAME(acq_data), &cmd, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf,sizeof(com_rbuf)-1);
    SYSLOG_DBG("FLUX_CQHuaZhengShuiWen protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("FLUX_CQHuaZhengShuiWen protocol",com_rbuf,size);
	  
	if((size>=9)&&(com_rbuf[0]==0x0a)&&(com_rbuf[8]==0x0d))
	{
		temp=com_rbuf[1];
		temp<<=8;
		temp+=com_rbuf[2];
		speed=(float)temp/10;
		
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		total_flux_M3=f.f;
		
		status=0;
		printf("the total_flux is %f\n",total_flux_M3);

		acq_data->total=total_flux_M3;
	}
	else 
	{
		status=1;
		speed=0;
		total_flux_M3=0;
	}
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

