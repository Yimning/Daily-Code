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

int protocol_ZONGLIN_ZONGDAN_USAhaxi(struct acquisition_data *acq_data)
{
	int status=0;
	char str[100]={0}; //  20210308
	char *p;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float zonglin=0;
	float zongdan=0;
	int ret=0;
	int arg_n=0;
	int val=0;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x44;
	com_tbuf[1]=0x20;
	com_tbuf[2]=0x20;
	com_tbuf[3]=0x30;
	com_tbuf[4]=0x31;
	com_tbuf[5]=0x0D;
	com_tbuf[6]=0x0A;
	size=7;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"haxi zonglin and zongdan SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("haxi zonglin and zongdan protocol: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("haxi zonglin and zongdan data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"haxi  zonglin and zongdan RECV:",com_rbuf,size);
	if(size>32 && com_rbuf[0]==0x44 && com_rbuf[3]==0x30 && com_rbuf[4]==0x31)
	{
		acq_data->acq_tm[5]=2000+((com_rbuf[5]-30)*10+(com_rbuf[6]-30));
		acq_data->acq_tm[4]=(com_rbuf[8]-30)*10+(com_rbuf[9]-30);
		acq_data->acq_tm[3]=(com_rbuf[11]-30)*10+(com_rbuf[12]-30);
		acq_data->acq_tm[2]=(com_rbuf[14]-30)*10+(com_rbuf[15]-30);
		acq_data->acq_tm[1]=(com_rbuf[17]-30)*10+(com_rbuf[18]-30);
		acq_data->acq_tm[0]=0;

		strncpy(str,&com_rbuf[20],8);
		p=str;
		val= (*p==0x2D)?(-1):(1);
		while(++p==0x20);
		sscanf(p,"%f",&zongdan);
		zongdan*=val;

		strncpy(str,&com_rbuf[29],8);
		p=str;
		val= (*p==0x2D)?(-1):(1);
		while(++p==0x20);
		sscanf(p,"%f",&zonglin);
		zonglin*=val;
		
		status=0;
	}
	else
	{
		zongdan=0;
		zonglin=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w21011",UNIT_MG_L,zonglin,&arg_n);
	acqdata_set_value(acq_data,"w21001",UNIT_MG_L,zongdan,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

