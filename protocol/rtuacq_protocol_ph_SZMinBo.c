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


int protocol_ph_SZMinBo(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_ph=0;
   float total_water_temp=0;
   
   union uf f;
	if(!acq_data) return -1;
   	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x01, 0x04);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZMinBo ph protocol,ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZMinBo ph data",com_rbuf,size);
	
	if((size>=13)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
/*
44 45 c6 66 = float 791.1
[3][2][1][0] = float 791.1
*/		
/*		
		f.ch[0]=com_rbuf[4]; //66
		f.ch[1]=com_rbuf[3]; //c6
		f.ch[2]=com_rbuf[6]; //45  791.1=66 c6 45 44
		f.ch[3]=com_rbuf[5]; //44	
*/		
        f.ch[0]=com_rbuf[6]; //66
		f.ch[1]=com_rbuf[5]; //c6
		f.ch[2]=com_rbuf[4]; //45  791.1=44 45 c6 66
		f.ch[3]=com_rbuf[3]; //44	
		total_ph=f.f;
/*
		f.ch[0]=com_rbuf[8]; //66
		f.ch[1]=com_rbuf[7]; //c6
		f.ch[2]=com_rbuf[10]; //45  791.1=66 c6 45 44
		f.ch[3]=com_rbuf[9]; //44	
*/		
		f.ch[0]=com_rbuf[10]; //66
		f.ch[1]=com_rbuf[9]; //c6
		f.ch[2]=com_rbuf[8]; //45  791.1=66 c6 45 44
		f.ch[3]=com_rbuf[7]; //44	
		total_water_temp=f.f;
		
		status=0;
		//printf("cod=%f\n",total_ph);
	}
	else
		{
		total_ph=0;
		total_water_temp=0;
		status=1;
		//printf("ppack:%s\n",com_rbuf);
	}
	ret=acqdata_set_value(acq_data,"w01001",UNIT_PH,total_ph,&arg_n);
	ret=acqdata_set_value(acq_data,"w01010",UNIT_0C,total_water_temp,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

