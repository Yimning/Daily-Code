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

int protocol_CEMS_TLD_PLC(struct acquisition_data *acq_data)
{
/*
	TX: 
	RX: 
	
	
*/
#define CEMSPOL_NUM 13

	char polcodestr[][20]={"a21005","a05001","a01014","a21024",
							"a21003","a21004","a21002","a19001",
							"a21026","a34013","a01011","a01012","a01013"};
	UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3, UNIT_PECENT, UNIT_PECENT, UNIT_MG_M3,
											UNIT_MG_M3, UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT,
											UNIT_MG_M3, UNIT_MG_M3, UNIT_M_S, UNIT_0C, UNIT_KPA};
	int poladdr[]={6024, 6036, 6040, 6020, 
					6056, 6060, 6028, 6048,
					6032, 5988, 5992, 5996, 6000};

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	int i=0;
	int addr=0;
	float val=0;

	union uf f;
	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x64);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"TLD_PLC CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("7017 cems protocol,cems : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("7017 cems data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"TLD_PLC CEMS RECV:",com_rbuf,size);
	if((size>=100)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<CEMSPOL_NUM;i++)
		{
			if(!strcmp(polcodestr[i],"")) continue;

			addr=poladdr[i]-0x175C;
			f.ch[3]=com_rbuf[addr+3];
			f.ch[2]=com_rbuf[addr+4];
			f.ch[1]=com_rbuf[addr+5];
			f.ch[0]=com_rbuf[addr+6];
			val=f.f;

			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],val,&arg_n);
		}
		
		status=0;
	}
	else
	{
		for(i=0;i<CEMSPOL_NUM;i++)
		{
			if(!strcmp(polcodestr[i],"")) continue;

			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
		}
		status=1;
	}
	
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_H,0,&arg_n);
	acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,0,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}