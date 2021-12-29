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

int protocol_VOCs_SHBaoYing(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	int size=0;
	union uf  f;
	int arg_n=0;
	int i=0,j=0;
	int ret=0;
	char flag;
   
#define CEMSPOL_NUM 13
	float valf[CEMSPOL_NUM]={0};

	char *polcodestr[CEMSPOL_NUM]={
		"a05002","a24087","a24088","a25002","a25003",
		"a25005","a01017","a01011","a01012","a00000",
		"a01013","a01014","a00000z"
	};

	UNIT_DECLARATION unitstr[CEMSPOL_NUM]={
		UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
		UNIT_MG_M3,UNIT_PA,UNIT_M_S,UNIT_0C,UNIT_M3_H,
		UNIT_KPA,UNIT_PECENT,UNIT_M3_H
	};

	   
	if(!acq_data) return -1;


   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("SHBaoYing VOCs protocol,VOCs : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("SHBaoYing VOCs size=%d,data:%s",size,com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"SHBaoYing VOCs RECV:",com_rbuf,size);

	if(size>=20 && com_rbuf[0]=='#' && com_rbuf[size-1]=='*')
	{
		sscanf(com_rbuf,"#1,%f,%f,%f,%f,%f,%f,%f,%f,%f,Z,%f,%f,%f,%f,%c,",
			&valf[0],&valf[1],&valf[2],&valf[3],&valf[4],
			&valf[5],&valf[6],&valf[7],&valf[8],&valf[9],
			&valf[10],&valf[11],&valf[12],&flag);
		
		for(i=0;i<CEMSPOL_NUM;i++)
		{
			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf[i],&arg_n);
		}

		acq_data->dev_stat=flag;
		
		status=0;
	}
	else
	{
		for(i=0;i<CEMSPOL_NUM;i++)
		{
			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
		}
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,30);
    	return arg_n;
}

