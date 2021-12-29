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

int protocol_others_YULV_PengYue(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	float yulv=0;
	float ph=0;
	int val=0;
	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x06);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG_HEX("PengYue yulv&ph send data :",com_tbuf,size);	
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("PengYue yulv&ph protocol,yulv&ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("PengYue yulv&ph data ",com_rbuf,size);	

	if((size>=17)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val=(val<<8)&0xFF00;
		val=val+com_rbuf[4];
		yulv=val/1000.0;

		val=com_rbuf[13];
		val=(val<<8)&0xFF00;
		val=val+com_rbuf[14];
		ph=val/1000.0;
		status=0;
		
	}
	else
	{
		ph=0;
		yulv=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w01001",UNIT_PH,ph,&arg_n);
	acqdata_set_value(acq_data,"w21024",UNIT_MG_L,yulv,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

