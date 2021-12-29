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

int protocol_ZONGLIN_ChuangZao(struct acquisition_data *acq_data)
{
/*
	TX: 01 03 00 01 00 15 D5 C5
	RX: 01 03 2A 32 80 42 73 32 80 42 73 00 14 00 05 00 07 00 0F 00 0A 00 12 00 01 00 14 00 05 00 06 00 08 00 0A 00 00 FF FF 00 04 00 00 00 05 FF FF


	andan = 42 73 32 80 = 60.79 mg/L
*/

	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float zonglin=0;
	int ret=0;
	int arg_n=0;
	int val=0;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x01,0x03,0x01,0x15);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ChuangZao zonglin protocol,zonglin : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ChuangZao zonglin data",com_rbuf,size);
	if((size>=46)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))

	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		zonglin=f.f;

		val=com_rbuf[11];
		val=(val<<8)+com_rbuf[12];
		acq_data->acq_tm[5]=val+2000;
		val=com_rbuf[13];
		val=(val<<8)+com_rbuf[14];
		acq_data->acq_tm[4]=val;
		val=com_rbuf[15];
		val=(val<<8)+com_rbuf[16];
		acq_data->acq_tm[3]=val;
		val=com_rbuf[17];
		val=(val<<8)+com_rbuf[18];
		acq_data->acq_tm[2]=val;
		val=com_rbuf[19];
		val=(val<<8)+com_rbuf[20];
		acq_data->acq_tm[1]=val;
		val=com_rbuf[21];
		val=(val<<8)+com_rbuf[22];
		acq_data->acq_tm[0]=val;

		val=com_rbuf[40];   //dev_status
		switch(val)
		{
			case 0: acq_data->instrstat=0;break;
			case 1: acq_data->instrstat=9;break;
			case 2: acq_data->instrstat=3;break;
			case 3: acq_data->instrstat=10;break;
			case 4: acq_data->instrstat=7;break;
			case 5: acq_data->instrstat=1;break;
			case 6: acq_data->instrstat=2;break;
			case 7: acq_data->instrstat=8;break;
			case 8: acq_data->instrstat=2;break;
			default: acq_data->instrstat=2;break;
		}

		val=com_rbuf[44];
		switch(val)
		{
			case 0: acq_data->alarm=0;break;
			case 1: acq_data->alarm=1;break;
			case 2: acq_data->alarm=5;break;
			case 3: acq_data->alarm=7;break;
			case 4: acq_data->alarm=8;break;
			case 5: acq_data->alarm=2;break;
			case 6: acq_data->alarm=9;break;
			case 7: acq_data->alarm=14;break;
			default: acq_data->alarm=14;break;
		}
		
		status=0;
	}
	else
	{
		zonglin=0;
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,zonglin,&arg_n);
	if(status == 0)
	acq_data->acq_status = ACQ_OK;
	else 
	acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



