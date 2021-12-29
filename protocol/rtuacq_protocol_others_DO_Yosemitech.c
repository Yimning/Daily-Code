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


int protocol_others_DO_Yosemitech(struct acquisition_data *acq_data)
{
/*
	TX: 04 03 26 00 00 04 4F 14
	RX: 04 03 08 00 00 8D 41 00 00 8D 41 12 65
	
	total_dissolved_oxygen = 00 00 8D 41 =17.625
	total_water_temp = 00 00 8D 41 =17.625
*/

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float total_dissolved_oxygen=0;
	float total_water_temp = 0;

	union uf f;
	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x04, 0x03, 0x2600, 0x04);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("Yosemitech dissolved oxygen protocol,DO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Yosemitech dissolved oxygen data",com_rbuf,size);

	if((size>=13)&&(com_rbuf[0]==0x04)&&(com_rbuf[1]==0x03))
	{

		f.ch[0]=com_rbuf[3];
		f.ch[1]=com_rbuf[4]; 
		f.ch[2]=com_rbuf[5]; 
		f.ch[3]=com_rbuf[6]; 
		total_water_temp=f.f;

		f.ch[0]=com_rbuf[7];
		f.ch[1]=com_rbuf[8]; 
		f.ch[2]=com_rbuf[9]; 
		f.ch[3]=com_rbuf[10]; 
		total_dissolved_oxygen=f.f;

		status=0;
	}
	else
	{
		total_dissolved_oxygen=0;
		total_water_temp=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"w01009",UNIT_MG_L,total_dissolved_oxygen,&arg_n);
	ret=acqdata_set_value(acq_data,"w01010",UNIT_0C,total_water_temp,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}