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

int protocol_DIANBIAO_AnKeRui(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float Ia,Ib,Ic,Ua,Ub,Uc;
	float Ia2,Ib2,Ic2,Ua2,Ub2,Uc2;
	

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x01,0x03,0x5000,0x0E);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	SYSLOG_DBG_HEX("AnKeRui dianbiao send data:",com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnKeRui dianbiao protocol,dianbiao : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnKeRui dianbiao data",com_rbuf,size);
	if((size>=32)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))

	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		Ua=f.f;
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		Ub=f.f;		
		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		Uc=f.f;

		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		Ia=f.f;
		f.ch[3]=com_rbuf[23];
		f.ch[2]=com_rbuf[24];
		f.ch[1]=com_rbuf[25];
		f.ch[0]=com_rbuf[26];
		Ib=f.f;
		f.ch[3]=com_rbuf[27];
		f.ch[2]=com_rbuf[28];
		f.ch[1]=com_rbuf[29];
		f.ch[0]=com_rbuf[30];
		Ic=f.f;
		status=0;
	}
	else
	{
		Ia=0;
		Ib=0;
		Ic=0;
		Ua=0;
		Ub=0;
		Uc=0;
		status=1;
	}
	sleep(1);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x01,0x03,0x5400,0x0E);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	SYSLOG_DBG_HEX("AnKeRui dianbiao send data:",com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnKeRui dianbiao protocol,dianbiao : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnKeRui dianbiao data",com_rbuf,size);
	if((size>=32)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))

	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		Ua2=f.f;
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		Ub2=f.f;		
		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		Uc2=f.f;

		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		Ia2=f.f;
		f.ch[3]=com_rbuf[23];
		f.ch[2]=com_rbuf[24];
		f.ch[1]=com_rbuf[25];
		f.ch[0]=com_rbuf[26];
		Ib2=f.f;
		f.ch[3]=com_rbuf[27];
		f.ch[2]=com_rbuf[28];
		f.ch[1]=com_rbuf[29];
		f.ch[0]=com_rbuf[30];
		Ic2=f.f;
	}
	else
	{
		Ia2=0;
		Ib2=0;
		Ic2=0;
		Ua2=0;
		Ub2=0;
		Uc2=0;
		status=1;
	}

	acqdata_set_value(acq_data,"mAIa01",UNIT_A,Ia,&arg_n);
	acqdata_set_value(acq_data,"mAIb01",UNIT_A,Ib,&arg_n);
	acqdata_set_value(acq_data,"mAIc01",UNIT_A,Ic,&arg_n);
	acqdata_set_value(acq_data,"mAUa01",UNIT_V,Ua,&arg_n);
	acqdata_set_value(acq_data,"mAUb01",UNIT_V,Ub,&arg_n);
	acqdata_set_value(acq_data,"mAUc01",UNIT_V,Uc,&arg_n);

	acqdata_set_value(acq_data,"mAIa02",UNIT_A,Ia2,&arg_n);
	acqdata_set_value(acq_data,"mAIb02",UNIT_A,Ib2,&arg_n);
	acqdata_set_value(acq_data,"mAIc02",UNIT_A,Ic2,&arg_n);
	acqdata_set_value(acq_data,"mAUa02",UNIT_V,Ua2,&arg_n);
	acqdata_set_value(acq_data,"mAUb02",UNIT_V,Ub2,&arg_n);
	acqdata_set_value(acq_data,"mAUc02",UNIT_V,Uc2,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



