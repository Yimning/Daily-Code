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

int protocol_VOCs_TianZhiLan(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char polcode[7]={0};
	int size=0;
	float valf;
	union uf  f;
	int val=0;
	int i;
	float ec,power,electric;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	
	devaddr=modbusarg->devaddr&0xffff;
	status=0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x03F2,0x0E);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"tianzhilan voc SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("tianzhilan voc protocol,voc 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("tianzhilan voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tianzhilan voc RECV1:",com_rbuf,size);
	if((size>=33)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		acqdata_set_value(acq_data,"g19501",UNIT_PECENT,f.f,&arg_n);

		f.ch[0]=com_rbuf[8];
		f.ch[1]=com_rbuf[7];
		f.ch[2]=com_rbuf[10];
		f.ch[3]=com_rbuf[9];
		acqdata_set_value(acq_data,"g09001",UNIT_MG_M3,f.f,&arg_n);

		f.ch[0]=com_rbuf[12];
		f.ch[1]=com_rbuf[11];
		f.ch[2]=com_rbuf[14];
		f.ch[3]=com_rbuf[13];
		acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,f.f,&arg_n);

		f.ch[0]=com_rbuf[16];
		f.ch[1]=com_rbuf[15];
		f.ch[2]=com_rbuf[18];
		f.ch[3]=com_rbuf[17];
		acqdata_set_value(acq_data,"g20301",UNIT_KG_H,f.f,&arg_n);

		f.ch[0]=com_rbuf[20];
		f.ch[1]=com_rbuf[19];
		f.ch[2]=com_rbuf[22];
		f.ch[3]=com_rbuf[21];
		acqdata_set_value(acq_data,"g28001",UNIT_0C,f.f,&arg_n);

		f.ch[0]=com_rbuf[24];
		f.ch[1]=com_rbuf[23];
		f.ch[2]=com_rbuf[26];
		f.ch[3]=com_rbuf[25];
		acqdata_set_value(acq_data,"g28201",UNIT_PA,f.f,&arg_n);

		f.ch[0]=com_rbuf[28];
		f.ch[1]=com_rbuf[27];
		f.ch[2]=com_rbuf[30];
		f.ch[3]=com_rbuf[29];
		acqdata_set_value(acq_data,"g20201",UNIT_M_S,f.f,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"g19501",UNIT_PECENT,0,&arg_n);
		acqdata_set_value(acq_data,"g09001",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"g20301",UNIT_KG_H,0,&arg_n);
		acqdata_set_value(acq_data,"g28001",UNIT_0C,0,&arg_n);
		acqdata_set_value(acq_data,"g28201",UNIT_PA,0,&arg_n);
		acqdata_set_value(acq_data,"g20201",UNIT_M_S,0,&arg_n);
		status=1;
	}



	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x040C,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"tianzhilan voc SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("tianzhilan voc protocol,voc 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("tianzhilan voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tianzhilan voc RECV2:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		acqdata_set_value(acq_data,"e70201",UNIT_A,f.f,&arg_n);

		f.ch[0]=com_rbuf[8];
		f.ch[1]=com_rbuf[7];
		f.ch[2]=com_rbuf[10];
		f.ch[3]=com_rbuf[9];
		acqdata_set_value(acq_data,"e70202",UNIT_A,f.f,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"e70201",UNIT_A,0,&arg_n);
		acqdata_set_value(acq_data,"e70202",UNIT_A,0,&arg_n);
		status=1;
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


