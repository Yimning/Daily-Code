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

int protocol_ZONGLIN_NJhongkai(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[255]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float tp=0;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0250402508**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJhongkai zonglin protocol,zonglin : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);

	if((size>=28)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0'))
	{
		f.ch[0]=HexCharToDec(com_rbuf[18])*16+HexCharToDec(com_rbuf[19]);
		f.ch[1]=HexCharToDec(com_rbuf[20])*16+HexCharToDec(com_rbuf[21]);
		f.ch[2]=HexCharToDec(com_rbuf[22])*16+HexCharToDec(com_rbuf[23]);
		f.ch[3]=HexCharToDec(com_rbuf[24])*16+HexCharToDec(com_rbuf[25]);
		tp=f.f;
		status=0;
	}
	else
	{
		status=1;
		tp=0;
	}
	acqdata_set_value(acq_data,"w21011",UNIT_MG_L,tp,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



