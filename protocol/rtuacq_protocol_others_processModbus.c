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

int protocol_others_processModbus(struct acquisition_data *acq_data)
{

#define PROCESSPOL_NUM 18

	//char polcode[PROCESSPOL_NUM][10]={
    char polcode[PROCESSPOL_NUM][CODE_LEN]={//20210308	
		"1w01001","1w01010","1w01009","3w01001","3w01010","2w01009",
		"4w01001","4w01010","5w01001","5w01010","3w01009","2w01001",
		"2w01010","1w01018","1w01012","6w01001","6w01010","4w01009"};

	UNIT_DECLARATION unitstr[PROCESSPOL_NUM]={
		UNIT_PH,UNIT_0C,UNIT_MG_L,UNIT_PH,UNIT_0C,UNIT_MG_L,
		UNIT_PH,UNIT_0C,UNIT_PH, UNIT_0C,UNIT_MG_L,UNIT_PH,
		UNIT_0C,UNIT_MG_L,UNIT_MG_M3,UNIT_PH,UNIT_0C,UNIT_MG_L};

	int poladdr[PROCESSPOL_NUM]={
		507, 517, 527, 537, 547, 557,
		567, 577, 587, 597, 607, 617,
		627, 637, 647, 657, 667, 677};

	int statusdaar[PROCESSPOL_NUM]={
		199, 201, 203, 205, 207, 209,
		211, 213, 215, 217, 219, 221,
		223, 225, 227, 231, 233, 235};
	
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	union uf f;
	int size=0;
	int arg_n=0;
	int ret=0;
	float val[PROCESSPOL_NUM];
	int flag[PROCESSPOL_NUM];
	int i=0;

	if(!acq_data) return -1;

	status=0;
	for(i=0;i<PROCESSPOL_NUM;i++)
	{
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,0x01,0x03,poladdr[i],0x02);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"process val SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		usleep(400*1000);

		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("process val protocol,process : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("process val data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"process val RECV:",com_rbuf,size);
		if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
			f.ch[0]=com_rbuf[4];
			f.ch[1]=com_rbuf[3];
			f.ch[2]=com_rbuf[6];
			f.ch[3]=com_rbuf[5];
			val[i]=f.f;
		}
		else
		{
			val[i]=0;
			status=1;
		}

		usleep(100*1000);

		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,0x01,0x03,statusdaar[i],0x01);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"process flag SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		usleep(400*1000);

		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("process flag protocol,process : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("process flag data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"process flag RECV:",com_rbuf,size);
		if((size>=7)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
			flag[i]=com_rbuf[3];
			flag[i]<<=8;
			flag[i]+=com_rbuf[4];
		}
		else
		{
			flag[i]=2;
			status=1;
		}

		switch(flag[i])
		{
			case 0: acqdata_set_value_flag(acq_data,polcode[i],unitstr[i],val[i],'N',&arg_n);break;
			case 1: acqdata_set_value_flag(acq_data,polcode[i],unitstr[i],val[i],'T',&arg_n);break;
			case 2: acqdata_set_value_flag(acq_data,polcode[i],unitstr[i],val[i],'D',&arg_n);break;
			default: acqdata_set_value_flag(acq_data,polcode[i],unitstr[i],val[i],'D',&arg_n);break;
		}
	}

	acq_data->dev_stat=0xaa;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data,20);
	
	return arg_n;
}

