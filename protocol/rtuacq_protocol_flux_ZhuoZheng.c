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

int protocol_FLUX_ZhuoZheng(struct acquisition_data *acq_data)
{
/*
	TX: 03 03 07 08 00 02 44 BD
	RX: 03 03 04 7A E1 40 E4 FF FF
	speed = 7A E1 40 E4 = 7.14 L/s
	
	TX: 03 03 05 04 00 02 85 06
	RX: 03 03 04 14 7A 41 96 FF FF
	total_flux_M3= 14 7A 41 96 = 18.76 m3
*/

	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float total_flux_M3=0;
	float speed=0;
	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x03, 0x03, 0x0708, 0x02);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhuoZheng speed  protocol,speed : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhuoZheng speed  data ",com_rbuf,size);

	if((size>=9)&&(com_rbuf[0]==0x03)&&(com_rbuf[1]==0x03))
	{
		f.ch[3] = com_rbuf[5];
		f.ch[2] = com_rbuf[6];
		f.ch[1] = com_rbuf[3];
		f.ch[0] = com_rbuf[4];
		speed=f.f;
		status=0;
	}
	else
	{
		speed=0;
		total_flux_M3=0;
		status=1;

	}
	
	sleep(2);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x03, 0x03, 0x0504, 0x02);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhuoZheng total protocol,total : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhuoZheng total data ",com_rbuf,size);
	
	if((size>=9)&&(com_rbuf[0]==0x03)&&(com_rbuf[1]==0x03)&&(status!=1))
	{
		f.ch[3] = com_rbuf[5];
		f.ch[2] = com_rbuf[6];
		f.ch[1] = com_rbuf[3];
		f.ch[0] = com_rbuf[4];
		total_flux_M3=f.f;
	}
	else
	{
		speed=0;
		total_flux_M3=0;
		status=1;

	}
	
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

