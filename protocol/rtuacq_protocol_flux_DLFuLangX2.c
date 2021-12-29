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


int protocol_FLUX_DLFuLangX2(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float speed1=0,speed2=0,speed;
 	int  total_flux1=0,total_flux2=0,total_flux_M3=0;
   union uf f;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x02,0x04,0x1010,0x0A);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	  SYSLOG_DBG("FLUX_DLFuLangX2 protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
      SYSLOG_DBG_HEX("FLUX_DLFuLangX2 protocol",com_rbuf,size);
	  
	if((size>=25)&&(com_rbuf[0]==0x02)&&(com_rbuf[1]==0x04))
		{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		speed1=f.f;
		f.f=0;
		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		total_flux1=f.l;
		status=0;
		printf("speed1=%f,total_flux1=%d\n",speed1,total_flux1);

		
	}
	else
	{
		status=1;
		total_flux_M3=0;
		speed=0;	
		printf("data 1 recieve error\n");
	}
	sleep(1);
	if(status==0)
    {
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,0x01,0x04,0x1010,0x0A);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(2);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	  SYSLOG_DBG("FLUX_DLFuLangX2 protocol2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
      SYSLOG_DBG_HEX("FLUX_DLFuLangX2 protocol2",com_rbuf,size);		
	  
		if((size>=25)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x04))
			{
			f.ch[3]=com_rbuf[3];
			f.ch[2]=com_rbuf[4];
			f.ch[1]=com_rbuf[5];
			f.ch[0]=com_rbuf[6];
			speed2=f.f;
			f.f=0;
			f.ch[3]=com_rbuf[19];
			f.ch[2]=com_rbuf[20];
			f.ch[1]=com_rbuf[21];
			f.ch[0]=com_rbuf[22];
			total_flux2=f.l;
			speed=speed1+speed2;
			total_flux_M3=total_flux1+total_flux2;
			status=0;
			printf("speed=%f,total_flux=%d\n",speed,total_flux_M3);
			acq_data->total=total_flux_M3;
		}
		else
			{
			status=1;
			total_flux_M3=0;
			speed=0;
			printf("error of recieve data\n");
		}
	}
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,(float)total_flux_M3,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

