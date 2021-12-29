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
int protocol_DO_WHSiFuMing(struct acquisition_data *acq_data)
{
/*
	TX: 01 03 00 32 00 02 65 C4
	RX: 01 03 04 7A 14 96 41 ** ** 
	total_dissolved_oxygen = 7A 14 96 41 =18.76mg/L
*/

	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float total_dissolved_oxygen=0;
	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x32, 0x02);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("WHSiFuMing DO protocol,DO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("WHSiFuMing DO data ",com_rbuf,size);	

	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		f.ch[3] = com_rbuf[6];
		f.ch[2] = com_rbuf[5];
		f.ch[1] = com_rbuf[4];
		f.ch[0] = com_rbuf[3];
		total_dissolved_oxygen=f.f;
		status=0;

	}
	else
	{
		total_dissolved_oxygen=0;
		status=1;

	}
	acqdata_set_value(acq_data,"w01009",UNIT_MG_L,total_dissolved_oxygen,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

