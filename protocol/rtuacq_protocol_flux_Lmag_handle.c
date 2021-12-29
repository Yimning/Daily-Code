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

#include "rtuacq_protocol_error_cache.h"

int protocol_FLUX_Lmag_handle(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float total_flux_M3=0,speed=0;
	union uf f;
	int unitn=0;

	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x03, 0x04,0x1010 ,0x12);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("FLUX_Lmag_handle protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FLUX_Lmag_handle protocol",com_rbuf,size);

	if((size>=41)&&(com_rbuf[0]==0x03)&&(com_rbuf[1]==0x04))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		speed=f.f;
		f.f=0;
		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		total_flux_M3+=f.l;
		f.l=0;
		f.ch[3]=com_rbuf[23];
		f.ch[2]=com_rbuf[24];
		f.ch[1]=com_rbuf[25];
		f.ch[0]=com_rbuf[26];
		total_flux_M3+=f.f;

		unitn=com_rbuf[35];
		unitn=unitn<<8;
		unitn+=com_rbuf[36];

		switch(unitn)
		{
		case 0:
			speed = speed*3600*0.001;
			break;
		case 1:
			speed = speed*60*0.001;
			break;
		case 2:
			speed = speed*1*0.001;
			break;
		case 3:
			speed = speed*3600*1;
			break;
		case 4:
			speed = speed*60*1;
			break;
		case 5:
			speed = speed*1*1;
			break;
		case 6:
			speed = speed*3600*1000;
			break;
		case 7:
			speed = speed*60*1000;
			break;
		case 8:
			speed = speed*1*1000;
			break;
		}

		unitn=0;
		unitn=com_rbuf[37];
		unitn=unitn<<8;
		unitn+=com_rbuf[38];
		switch(unitn)
		{
		case 0:
			total_flux_M3 = total_flux_M3*0.001;
			break;
		case 1:
			total_flux_M3 = total_flux_M3*1;
			break;
		case 2:
			total_flux_M3 = total_flux_M3*1000;
			break;
		}

		acq_data->total=total_flux_M3;
		status=0;
	}
	else
	{
		status=1;
		total_flux_M3=0;
		speed=0;
	}

	acqdata_set_value(acq_data,"ea1201",UNIT_M3_H,speed,&arg_n);	
	acqdata_set_value(acq_data,"ea1101",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

