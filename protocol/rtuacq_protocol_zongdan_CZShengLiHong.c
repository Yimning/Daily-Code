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

int protocol_ZONGDAN_CZShengLiHong(struct acquisition_data *acq_data)
{
/*
	TX:	01 03 00 00 00 02 ** **
	RX:	01 03 04 40 50 62 4E FF FF

	zongdan = 40 50 62 4E = 3.256
*/
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float zongdan=0;
	int alarm=0,devstatus=0;
	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x10);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("CZShengLiHong zongdan protocol,zongdan : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CZShengLiHong zongdan data ",com_rbuf,size);	

	if((size>=37)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		f.ch[3] = com_rbuf[3];
		f.ch[2] = com_rbuf[4];
		f.ch[1] = com_rbuf[5];
		f.ch[0] = com_rbuf[6];
		zongdan=f.f;

		acq_data->acq_tm[5]=(com_rbuf[7]<<8)&0xFF00;
		acq_data->acq_tm[5]+=com_rbuf[8];
		acq_data->acq_tm[4]=(com_rbuf[9]<<8)&0xFF00;
		acq_data->acq_tm[4]+=com_rbuf[10];
		acq_data->acq_tm[3]=(com_rbuf[11]<<8)&0xFF00;
		acq_data->acq_tm[3]+=com_rbuf[12];
		acq_data->acq_tm[2]=(com_rbuf[13]<<8)&0xFF00;
		acq_data->acq_tm[2]+=com_rbuf[14];
		acq_data->acq_tm[1]=(com_rbuf[15]<<8)&0xFF00;
		acq_data->acq_tm[1]+=com_rbuf[16];
		acq_data->acq_tm[0]=(com_rbuf[17]<<8)&0xFF00;
		acq_data->acq_tm[0]+=com_rbuf[18];

		alarm = (com_rbuf[33]<<8)&0xFF00;
		alarm += com_rbuf[34];
		devstatus = (com_rbuf[31]<<8)&0xFF00;
		devstatus += com_rbuf[32];

		switch(alarm)
		{
			case 0: acq_data->alarm = 0;break;
			case 1: acq_data->alarm = 1;break;
			case 2: acq_data->alarm = 11;break;
			case 3: acq_data->alarm = 5;break;
			case 4: acq_data->alarm = 7;break;
			case 5: acq_data->alarm = 7;break;
			case 6: acq_data->alarm = 7;break;
			case 7: acq_data->alarm = 8;break;
			case 8: acq_data->alarm = 6;break;
			case 9: acq_data->alarm = 6;break;
			case 12: acq_data->alarm = 2;break;
		}
		
		switch(devstatus)
		{
			case 0: acq_data->instrstat = 0;break;
			case 1: acq_data->instrstat = 7;break;
			case 2: acq_data->instrstat = 3;break;
			case 3: acq_data->instrstat = 1;break;
			case 4: acq_data->instrstat = 10;break;
			case 5: acq_data->instrstat = 0;break;
		}
		
		status=0;
	}
	else
	{
		zongdan=0;
		status=1;
	}
	acqdata_set_value(acq_data,"w21001",UNIT_MG_L,zongdan,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

