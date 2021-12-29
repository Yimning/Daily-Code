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

int protocol_PH_02(struct acquisition_data *acq_data)
{
/*
	TX: 01 03 00 00 00 04 44 09
	RX: 01 03 08 40 D4 CC CD 41 C8 00 00 FF FF
	ph= 40 D4 CC CD = 6.65
	temp= 41 C8 00 00 =25.0
*/
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float ph=0,temp=0;
	int ret=0;
	int arg_n=0;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x01,0x03,0x00,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"02 PH SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("02 PH protocol,PH : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("02 PH data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"02 PH RECV:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		ph=f.f;

		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[7];
		temp=f.f;
		status=0;
	}
	else
	{
		ph=0;
		temp=0;
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w01001",UNIT_PH,ph,&arg_n);
	ret=acqdata_set_value(acq_data,"w01010",UNIT_0C,temp,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



