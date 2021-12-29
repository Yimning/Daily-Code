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
//#include "devices_mng_api.h"

/*
return : com_f arguments amount;
*/

static const  char * const garbage_polcode[] = {"a01901","a01902","i33310","i33311","i33312","i33320","i33321","i33322","i33330","i33331","i33332"};

int protocol_CEMS_Furnace_modbus(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float temp=0;
	int ret=0;
	int arg_n=0;
	int len=0;
	int i=0;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x1,0x3,0x00,0x16);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("furnace temperature CEMS protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("furnace temperature CEMS data",com_rbuf,size);

	len=ARRAY_SIZE(garbage_polcode);
	if((size>=49)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<len;i++)
		{
			f.ch[0]=com_rbuf[4*i+6];
			f.ch[1]=com_rbuf[4*i+5];
			f.ch[2]=com_rbuf[4*i+4];
			f.ch[3]=com_rbuf[4*i+3];
			temp=f.f;

			acqdata_set_value(acq_data,garbage_polcode[i],UNIT_0C,temp,&arg_n);
		}
		status=0;
	}
	else
	{
		for(i=0;i<len;i++)
			acqdata_set_value(acq_data,garbage_polcode[i],UNIT_0C,0,&arg_n);
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



