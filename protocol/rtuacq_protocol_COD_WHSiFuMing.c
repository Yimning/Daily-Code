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

int protocol_COD_WHSiFuMing(struct acquisition_data *acq_data)
{
/*
	TX: 02 03 00 42 00 02 64 1F
	RX: 02 03 04 7A 14 96 41 ** ** 
	cod = 7A 14 96 41 = 18.76mg/L
*/

	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float COD=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 0x42, 0x02);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"WHSiFuMing COD SEND:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("WHSiFuMing COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("WHSiFuMing COD data ",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"WHSiFuMing COD RECV:",com_tbuf,size);

	if((size>=9)&&(com_rbuf[0]==0x02)&&(com_rbuf[1]==0x03))
	{
		f.ch[3] = com_rbuf[6];
		f.ch[2] = com_rbuf[5];
		f.ch[1] = com_rbuf[4];
		f.ch[0] = com_rbuf[3];
		COD=f.f;
		status=0;

	}
	else
	{
		COD=0;
		status=1;

	}
	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,COD,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

