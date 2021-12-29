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

int protocol_ZONGNIE_JMWeiChuang(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float Ni=0;
	int ret=0;
	int arg_n=0;

	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x09,0x3,0x2968,0x02);
	SYSLOG_DBG("JMWeiChuang Ni protocol,Ni : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JMWeiChuang Ni data",com_tbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JMWeiChuang Ni SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JMWeiChuang Ni protocol,Ni: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JMWeiChuang Ni data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JMWeiChuang Ni RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==0x09)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		Ni=f.f;
		status=0;
	}
	else
	{
		Ni=0;
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w20121",UNIT_MG_L,Ni,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}



