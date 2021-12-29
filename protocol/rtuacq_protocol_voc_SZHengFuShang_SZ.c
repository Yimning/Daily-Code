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

int protocol_VOCs_SZHengFuShang_SZ(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x2C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZHengFuShang_SZ voc SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZHengFuShang_SZ voc protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZHengFuShang_SZ voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZHengFuShang_SZ voc RECV:",com_rbuf,size);
	if((size>=93)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		acqdata_set_value(acq_data,"a01012",UNIT_0C,f.f,&arg_n);

		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[7];
		acqdata_set_value(acq_data,"a01013",UNIT_PA,f.f,&arg_n);

		f.ch[0]=com_rbuf[14];
		f.ch[1]=com_rbuf[13];
		f.ch[2]=com_rbuf[12];
		f.ch[3]=com_rbuf[11];
		acqdata_set_value(acq_data,"a01011",UNIT_M_S,f.f,&arg_n);

		f.ch[0]=com_rbuf[18];
		f.ch[1]=com_rbuf[17];
		f.ch[2]=com_rbuf[16];
		f.ch[3]=com_rbuf[15];
		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,f.f,&arg_n);
/*
		f.ch[0]=com_rbuf[78];
		f.ch[1]=com_rbuf[77];
		f.ch[2]=com_rbuf[76];
		f.ch[3]=com_rbuf[75];
		acqdata_set_value(acq_data,"a00000",UNIT_M3_S,f.f,&arg_n);
*/		
		f.ch[0]=com_rbuf[82];
		f.ch[1]=com_rbuf[81];
		f.ch[2]=com_rbuf[80];
		f.ch[3]=com_rbuf[79];
		acqdata_set_value(acq_data,"a24087",UNIT_MG_M3,f.f,&arg_n);
		
		f.ch[0]=com_rbuf[86];
		f.ch[1]=com_rbuf[85];
		f.ch[2]=com_rbuf[84];
		f.ch[3]=com_rbuf[83];
		acqdata_set_value(acq_data,"a05002",UNIT_MG_M3,f.f,&arg_n);
		
		f.ch[0]=com_rbuf[90];
		f.ch[1]=com_rbuf[89];
		f.ch[2]=com_rbuf[88];
		f.ch[3]=com_rbuf[87];
		acqdata_set_value(acq_data,"a24088",UNIT_MG_M3,f.f,&arg_n);
		
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"a01012",UNIT_0C,0,&arg_n);
		acqdata_set_value(acq_data,"a01013",UNIT_KPA,0,&arg_n);
		acqdata_set_value(acq_data,"a01011",UNIT_M_S,0,&arg_n);
		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,0,&arg_n);
		acqdata_set_value(acq_data,"a24087",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a05002",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a24088",UNIT_MG_M3,0,&arg_n);
		//acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



