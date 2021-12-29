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

int protocol_KMnO4_ZhuoZheng(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	float KMnO4=0;
	int arg_n=0;
	int val=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x02,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhuoZheng KMnO4 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhuoZheng KMnO4 protocol,KMnO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhuoZheng KMnO4 data ",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhuoZheng KMnO4 RECV:",com_rbuf,size);
	pf=acq_data->com_f;       
	if( (size >= 19)&&(com_rbuf[0] == devaddr)&&(com_rbuf[1] == 0x03) )
	{
		f.ch[1] = com_rbuf[5];
		f.ch[0] = com_rbuf[6];
		f.ch[3] = com_rbuf[3];
		f.ch[2] = com_rbuf[4];
		KMnO4 = f.f;

		acq_data->acq_tm[5] = 2000+com_rbuf[7];
		acq_data->acq_tm[4] = com_rbuf[8];
		acq_data->acq_tm[3] = com_rbuf[9];
		acq_data->acq_tm[2] = com_rbuf[10];
		acq_data->acq_tm[1] = com_rbuf[11];
		acq_data->acq_tm[0] = com_rbuf[12];

		val=com_rbuf[14];
		switch(val)
		{
			case 0: acq_data->instrstat=9;break;
			case 1: acq_data->instrstat=7;break;
			case 2: acq_data->instrstat=8;break;
			case 3: acq_data->instrstat=10;break;
			case 4: acq_data->instrstat=2;break;
			case 5: acq_data->instrstat=1;break;
			default: acq_data->instrstat=2;break;
		}

		val=com_rbuf[16];
		switch(val)
		{
			case 0: acq_data->alarm=0;break;
			case 1: acq_data->alarm=11;break;
			case 2: acq_data->alarm=2;break;
			case 3: acq_data->alarm=3;break;
			case 4: acq_data->alarm=5;break;
			case 5: acq_data->alarm=6;break;
			case 6: acq_data->alarm=6;break;
			case 7: acq_data->alarm=6;break;
			case 8: acq_data->alarm=7;break;
			case 9: acq_data->alarm=7;break;
			case 10: acq_data->alarm=7;break;
			case 11: acq_data->alarm=8;break;
			case 12: acq_data->alarm=4;break;
			case 13: acq_data->alarm=12;break;
			case 14: acq_data->alarm=10;break;
			case 15: acq_data->alarm=9;break;
			case 16: acq_data->alarm=13;break;
			default: acq_data->alarm=14;break;
		}

		status = 0;

	}
	else
	{
		KMnO4=0;
		status = 1;
	}

	acqdata_set_value(acq_data,"w01019",UNIT_MG_L,KMnO4,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

