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

static int is_process_pack(char *buff,int size,char *pack)
{
	char *p,*q;
	if(!buff || size==0) return 0;
	
	if((p=strstr(buff,"##"))==NULL) return 0;
	if((q=strstr(p,"&&")==NULL)) return 0;

	pack=p;
	return 1;
}

static unsigned long currtm_global;

int protocol_others_process(struct acquisition_data *acq_data)
{
//RECV:##V1 1.1 S1 0 V2 1.2 S2 0 V3 1.3 S3 0 V4 1.4 S4 0 V5 1.5 S5 0 V6 1.6 S6 0 V7 1.7 S7 0 V8 1.8 S8 0 V9 1.9 S9 0 V10 2.0 S10 0 V11 2.1 S11 0 V12 2.2 S12 0 V13 2.3 S13 0 V14 2.4 S14 0 V15 2.5 S15 0 V16 2.6 S16 0 V17 2.7 S17 0 V18 2.8 S18 0&&


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
	
	int status=0;
	char com_rbuf[2048]={0}; 
	//char com_tbuf[8]={0};
	union uf f;
	int size=0;
	int arg_n=0;
	int ret=0;
	float val[PROCESSPOL_NUM];
	int flag[PROCESSPOL_NUM];
	char *p;
	int i=0;
    struct acquisition_ctrl  *acq_ctrl;

	if(!acq_data) return -1;

	if(currtm_global==0) currtm_global=get_current_time();

	acq_ctrl=ACQ_CTRL(acq_data);

	p=com_rbuf;
	sleep(6);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("process protocol,process : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("process data: %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"process RECV:",com_rbuf,size);
	if(size>50 && is_process_pack(com_rbuf,size,p))
	{
		status=0;
		ret=sscanf(p,"##V1 %f S1 %d V2 %f S2 %d V3 %f S3 %d V4 %f S4 %d V5 %f S5 %d V6 %f S6 %d V7 %f S7 %d V8 %f S8 %d V9 %f S9 %d V10 %f S10 %d V11 %f S11 %d V12 %f S12 %d V13 %f S13 %d V14 %f S14 %d V15 %f S15 %d V16 %f S16 %d V17 %f S17 %d V18 %f S18 %d&&",
			&val[0],&flag[0],&val[1],&flag[1],&val[2],&flag[2],&val[3],&flag[3],&val[4],&flag[4],&val[5],&flag[5],
			&val[6],&flag[6],&val[7],&flag[7],&val[8],&flag[8],&val[9],&flag[9],&val[10],&flag[10],&val[11],&flag[11],
			&val[12],&flag[12],&val[13],&flag[13],&val[14],&flag[14],&val[15],&flag[15],&val[16],&flag[16],&val[17],&flag[17]);
		
		if(ret!=36)
		{
			SYSLOG_DBG("process set val num=%d\n",ret);
			for(i=0;i<PROCESSPOL_NUM;i++)
			{
				val[i]=0;
				flag[i]='D';
			}
			status=1;
		}
		else
		{// ok packet
			currtm_global=get_current_time();
		}
		
	}
	else
	{
		for(i=0;i<PROCESSPOL_NUM;i++)
		{
			val[i]=0;
			flag[i]='D';
		}
		status=1;

		if(period_timeout(currtm_global, 35))
		{//error packet
		     acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
		}

	}

	for(i=0;i<PROCESSPOL_NUM;i++)
	{
		SYSLOG_DBG("process:v%d=%.2f,s%d=%d\n",i+1,val[i],i+1,flag[i]);
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

	if(acq_data->acq_status == ACQ_ERR)
           acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	
	return arg_n;
}

