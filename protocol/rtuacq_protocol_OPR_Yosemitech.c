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


int protocol_ORP_Yosemitech(struct acquisition_data *acq_data)
{
/*
	TX: 01 03 26 00 00 04 CD AB
	RX: 01 03 08 D6 6E D5 41 00 00 00 00 B8 AD
	
	orp= D6 6E D5 41 =26.64
*/

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float orp=0;

	union uf f;
	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x2600, 0x04);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("Yosemitech ORP protocol,ORP : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Yosemitech ORP data",com_rbuf,size);

	if((size>=13)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{

		f.ch[0]=com_rbuf[3];
		f.ch[1]=com_rbuf[4]; 
		f.ch[2]=com_rbuf[5]; 
		f.ch[3]=com_rbuf[6]; 
		orp=f.f;
		
		status=0;
	}
	else
	{
		orp=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"wg0316",UNIT_M_V,orp,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}