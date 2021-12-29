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

#include "rtuacq_protocol_error_cache.h"

#define POLCODES_NUM 5

int protocol_others_HuiguMP3100(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float val=0;
	int ret=0;
	int arg_n=0;
	int i=0;

	char *polcode[POLCODES_NUM]={"w01001","w01014","w01009","wg0316","w01010"};
	UNIT_DECLARATION unit[POLCODES_NUM]={UNIT_PH,UNIT_US_CM,UNIT_MG_L,UNIT_M_V,UNIT_0C};

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x01,0x03,0x00,0x14);

	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"MP3100 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("MP3100 protocol,MP3100 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("MP3100 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"MP3100 RECV:",com_rbuf,size);
	if((size>=45)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))

	{
		for(i=0;i<4;i++)	//ph,cond,do,orp
		{
			f.ch[0]=com_rbuf[i*12+4];
			f.ch[1]=com_rbuf[i*12+3];
			f.ch[2]=com_rbuf[i*12+6];
			f.ch[3]=com_rbuf[i*12+5];
			val=f.f;
			acqdata_set_value(acq_data,polcode[i],unit[i],val,&arg_n);
		}
		//temp
/*		
		f.ch[0]=com_rbuf[8];
		f.ch[1]=com_rbuf[7];
		f.ch[2]=com_rbuf[10];
		f.ch[3]=com_rbuf[9];
*/
		f.ch[0]=com_rbuf[20];
		f.ch[1]=com_rbuf[19];
		f.ch[2]=com_rbuf[22];
		f.ch[3]=com_rbuf[21];
		val=f.f;
		acqdata_set_value(acq_data,polcode[4],unit[4],val,&arg_n);
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODES_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;
}



