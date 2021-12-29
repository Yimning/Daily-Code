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


int protocol_FLUX_DuoPuLe(struct acquisition_data *acq_data)
{
/*
	TX: 01 03 00 07 00 0C F4 0E
	RX: 01 03 18 40 0C 39 58 10 62 4D D3 40 59 A5 1E B8 51 EB 85 40 02 8F 5C 28 F5 C2 8F ** **
	speed = 40 0C 39 58 10 62 4D D3 = 3.528 M3/H
	total_flux_M3 = 40 02 8F 5C 28 F5 C2 8F = 2.3 KM3
*/

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	double speed=0;
	double total_flux_M3=0;
   
	union uf64 f;
	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x07, 0x0C);
	size=write_device(DEV_NAME(acq_data),com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("flux DuoPuLe protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("flux DuoPuLe protocol",com_rbuf,size);

	if((size>=29)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[7];
		f.ch[4]=com_rbuf[6];
		f.ch[5]=com_rbuf[5];
		f.ch[6]=com_rbuf[4];
		f.ch[7]=com_rbuf[3];
		speed=f.d;


		f.ch[0]=com_rbuf[26];
		f.ch[1]=com_rbuf[25];
		f.ch[2]=com_rbuf[24];
		f.ch[3]=com_rbuf[23];
		f.ch[4]=com_rbuf[22];
		f.ch[5]=com_rbuf[21];
		f.ch[6]=com_rbuf[20];
		f.ch[7]=com_rbuf[19];
		total_flux_M3=f.d;

		total_flux_M3=total_flux_M3*1000;// KM3 => M3

		status=0;

	}
	else
	{
		speed = 0;
		total_flux_M3 = 0;
		status = 1;
	}

	acq_data->total=total_flux_M3;

	acqdata_set_value(acq_data,"w00000",UNIT_M3_H,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
