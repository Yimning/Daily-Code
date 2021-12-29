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


int protocol_others_ph_PengYue(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float total_ph=0;
	float total_water_temp=0;
	int var1,var2;
	int i;

	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x04);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("PengYue ph protocol,ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("PengYue ph data",com_rbuf,size);

	if((size>=13)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		var1=com_rbuf[4];
		var1=var1<<8;
		var1+=com_rbuf[3];

		var2=com_rbuf[6];
		var2=var2<<8;
		var2+=com_rbuf[5];

		for(i=1;(var2/i)!=0;i*=10);
		total_ph=var1+(var2/(float)i);

		var1=com_rbuf[8];
		var1=var1<<8;
		var1+=com_rbuf[7];

		var2=com_rbuf[10];
		var2=var2<<8;
		var2+=com_rbuf[9];

		for(i=1;(var2/i)!=0;i*=10);
		total_water_temp=var1+(var2/(float)i);

		status=0;
	}
	else
	{
		total_ph=0;
		total_water_temp=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"w01001",UNIT_PH,total_ph,&arg_n);
	ret=acqdata_set_value(acq_data,"w01010",UNIT_0C,total_water_temp,&arg_n);

	if(status == 0)
	acq_data->acq_status = ACQ_OK;
	else 
	acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

