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

int protocol_VOCs_SHLanBao(struct acquisition_data *acq_data)
{
/*
	TX: C9 03 00 00 00 1C 54 4B
	RX: C9 03 38 CC CC 42 AA 33 33 C1 93 14 7A 41 CE EB 85 41 85 00 00 00 00 00 00 00 00 4B C6 3E F7 00 00 00 00 B8 51 3F 9E A3 D7 40 A0 51 EB 3F 38 E1 47 3F 3A A3 D7 3F 70 D8 93 42 87 FF FF
*/	
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int i=0;
	int flag=0;

	#define MAX_POLCODE 14
	//static char polcode[MAX_POLCODE][10]={
	static char polcode[MAX_POLCODE][CODE_LEN]={//20210308
	"a01012","a01013","a01011","a01014","","","a24088",
	"","a25002","a25003","a25005","a24087","a05002","a34013"};
	
	UNIT_DECLARATION unit[MAX_POLCODE]={UNIT_0C,UNIT_PA,UNIT_M_S,UNIT_PECENT,UNIT_NONE,UNIT_NONE,UNIT_MG_M3,
									UNIT_NONE,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3};

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0xC9,0x03,0x00,0x1C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SHLanBao VOC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SHLanBao VOC protocol,VOC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SHLanBao VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SHLanBao VOC RECV:",com_rbuf,size);
	if((size>=61)&&(com_rbuf[0]==0xC9)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<MAX_POLCODE;i++)
		{
			if(!strcmp(polcode[i],"")) 
				continue;

			f.ch[0]=com_rbuf[4+i*4];
			f.ch[1]=com_rbuf[3+i*4];
			f.ch[2]=com_rbuf[6+i*4];
			f.ch[3]=com_rbuf[5+i*4];
			acqdata_set_value(acq_data,polcode[i],unit[i],f.f,&arg_n);
		}

		f.ch[0]=com_rbuf[32];
		f.ch[1]=com_rbuf[31];
		f.ch[2]=com_rbuf[34];
		f.ch[3]=com_rbuf[33];
		flag=(int)f.f;

		switch(flag)
		{
			case 0: acq_data->dev_stat='N';break;
			case 1: acq_data->dev_stat='F';break;
			case 2: acq_data->dev_stat='M';break;
			case 3: acq_data->dev_stat='S';break;
			case 4: acq_data->dev_stat='D';break;
			case 5: acq_data->dev_stat='C';break;
			case 6: acq_data->dev_stat='T';break;
			case 7: acq_data->dev_stat='E';break;
			default: acq_data->dev_stat='N';break;
		}
		
		status=0;
	}
	else
	{
		for(i=0;i<MAX_POLCODE;i++)
		{
			if(!strcmp(polcode[i],"")) 
				continue;

			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}
		status=1;
	}

	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);

	return arg_n;
}



