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


int protocol_FLUX_SHYingShen(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
  float speed=0;
   unsigned long total_flux_M3=0;
   UNIT_DECLARATION funit;
   union uf f;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x01, 0x09);
	size=write_device(DEV_NAME(acq_data),com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("flux SHYingShen protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("flux SHYingShen protocol",com_rbuf,size);
	
	if((size>=23)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		speed=(float)f.l;
		if(com_rbuf[7]!=0x0b) goto ERROR;
		switch(com_rbuf[8]){
			case 0x3D:speed*=100;
				break;
			case 0x3C:speed*=10;
				break;
			case 0x3B:speed=speed;
				break;
			default:goto ERROR;
				}
		speed=speed/3600;
		f.ch[0]=com_rbuf[16];
		f.ch[1]=com_rbuf[15];
		f.ch[2]=com_rbuf[18];
		f.ch[3]=com_rbuf[17];
		total_flux_M3=f.l;
		if(com_rbuf[19]!=0x0c) goto ERROR;
		switch(com_rbuf[20]){
			case 0x13:total_flux_M3=total_flux_M3;
				break;
			case 0x15:total_flux_M3*=100;
				break;
			case 0x16:total_flux_M3*=1000;
				break;
			default:goto ERROR;
		}
		status=0;
		printf("speed=%f,total_flux=%d\n",speed,total_flux_M3);

		acq_data->total=total_flux_M3/1000;// L => M3
	}
	else
		{
		ERROR:
		acqdata_set_value(acq_data,"w00000",UNIT_L_S,0,&arg_n);	
		acqdata_set_value(acq_data,"w00001",UNIT_L,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		printf("error of recieve data\n");
		 return arg_n;
	}
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,(float)speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_L,(float)total_flux_M3,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
