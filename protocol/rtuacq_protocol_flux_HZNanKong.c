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


int protocol_FLUX_HZNanKong(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
  float speed=0;
   unsigned long total_flux_M3=0;
   union uf f;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x1A);
	size=write_device(DEV_NAME(acq_data),com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("flux HZNanKong protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("flux HZNanKong protocol",com_rbuf,size);	
	//printf("size=%d\n",size);
	if((size>=57)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		speed=f.f;
		f.f=0;
		f.ch[0]=com_rbuf[12];
		f.ch[1]=com_rbuf[11];
		f.ch[2]=com_rbuf[10];
		f.ch[3]=com_rbuf[9];
		total_flux_M3=f.l;
		status=0;
		printf("speed=%f,total_flux=%ld\n",speed,total_flux_M3);

		acq_data->total=total_flux_M3;
	}
	else
		{
		speed=0;
		total_flux_M3=0;
		status=1;
		printf("error of recieve data\n");
	}
	acqdata_set_value(acq_data,"w00000",UNIT_M3_H,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

