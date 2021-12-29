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

int protocol_ANDAN_LvYe(struct acquisition_data *acq_data)
{
/*
	TX: 51 FF FF FF FF FF FF 51
	RX: 13 0C 14 0E 24 00 33 B3 80 42 0F

	datatime= 13 0C 14 0E 24 00 = 2019/12/20 14:36:00
	andan = 33 B3 80 42 = 64.35 mg/L
*/

	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float andan=0;
	int ret=0;
	int arg_n=0;
	int check=0;
	int val=0;
	int i=0;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0] = 0x51;
	com_tbuf[1] = 0xFF;
	com_tbuf[2] = 0xFF;
	com_tbuf[3] = 0xFF;
	com_tbuf[4] = 0xFF;
	com_tbuf[5] = 0xFF;
	com_tbuf[6] = 0xFF;

	for(i=0;i<7;i++)
	{
		check += com_tbuf[i];
	}
	com_tbuf[7] = check%255;
	
	size=8;
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LvYe andan protocol,andan : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LvYe andan data",com_rbuf,size);
	if(size>=11)
	{
		acq_data->acq_tm[5]=2000+com_rbuf[0];
		acq_data->acq_tm[4]=com_rbuf[1];
		acq_data->acq_tm[3]=com_rbuf[2];
		acq_data->acq_tm[2]=com_rbuf[3];
		acq_data->acq_tm[1]=com_rbuf[4];
		acq_data->acq_tm[0]=com_rbuf[5];
	
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[9];
		andan=f.f;
		status=0;
	}
	else
	{
		andan=0;
		status=1;
	}
	sleep(1);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0] = 0x52;
	com_tbuf[1] = 0xFF;
	com_tbuf[2] = 0xFF;
	com_tbuf[3] = 0xFF;
	com_tbuf[4] = 0xFF;
	com_tbuf[5] = 0xFF;
	com_tbuf[6] = 0xFF;
	check=0;
	for(i=0;i<7;i++)
	{
		check += com_tbuf[i];
	}
	com_tbuf[7] = check%255;
	
	size=8;
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LvYe andan status protocol,andan status: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LvYe andan status data",com_rbuf,size);
	if(size>=2)
	{
		val=com_rbuf[0];
		switch(val)
		{
			case 0:acq_data->instrstat=0;acq_data->alarm=0;break;
			case 1:acq_data->instrstat=7;acq_data->alarm=0;break;
			case 2:acq_data->instrstat=7;acq_data->alarm=0;break;
			case 3:acq_data->instrstat=2;acq_data->alarm=5;break;
			case 4:acq_data->instrstat=2;acq_data->alarm=8;break;
			case 5:acq_data->instrstat=2;acq_data->alarm=7;break;
			case 6:acq_data->instrstat=2;acq_data->alarm=7;break;
			case 7:acq_data->instrstat=2;acq_data->alarm=8;break;
			default:acq_data->instrstat=2;acq_data->alarm=14;break;
		}
	}
	else
	{
		acq_data->instrstat=0;
		acq_data->alarm=0;
	}
	
	ret=acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



