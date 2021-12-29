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

int protocol_VOCs_TianZhiLan3(struct acquisition_data *acq_data)
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

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x03EA,0x26);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"tianzhilan3 voc SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("tianzhilan3 voc protocol,voc : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("tianzhilan3 voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tianzhilan3 voc RECV:",com_rbuf,size);
	if((size>=81)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,f.f,&arg_n);

		f.ch[0]=com_rbuf[16];
		f.ch[1]=com_rbuf[15];
		f.ch[2]=com_rbuf[18];
		f.ch[3]=com_rbuf[17];
		acqdata_set_value(acq_data,"g28001",UNIT_0C,f.f,&arg_n);

		f.ch[0]=com_rbuf[20];
		f.ch[1]=com_rbuf[19];
		f.ch[2]=com_rbuf[22];
		f.ch[3]=com_rbuf[21];
		acqdata_set_value(acq_data,"g28201",UNIT_PA,f.f,&arg_n);

		f.ch[0]=com_rbuf[24];
		f.ch[1]=com_rbuf[23];
		f.ch[2]=com_rbuf[26];
		f.ch[3]=com_rbuf[25];
		acqdata_set_value(acq_data,"g20201",UNIT_M_S,f.f,&arg_n);

		f.ch[0]=com_rbuf[28];
		f.ch[1]=com_rbuf[27];
		f.ch[2]=com_rbuf[30];
		f.ch[3]=com_rbuf[29];
		acqdata_set_value(acq_data,"g20101",UNIT_M3_H,f.f,&arg_n);

		f.ch[0]=com_rbuf[32];
		f.ch[1]=com_rbuf[31];
		f.ch[2]=com_rbuf[34];
		f.ch[3]=com_rbuf[33];
		acqdata_set_value(acq_data,"g18001",UNIT_0C,f.f,&arg_n);

		f.ch[0]=com_rbuf[36];
		f.ch[1]=com_rbuf[35];
		f.ch[2]=com_rbuf[38];
		f.ch[3]=com_rbuf[37];
		acqdata_set_value(acq_data,"g18101",UNIT_PECENT,f.f,&arg_n);

		f.ch[0]=com_rbuf[48];
		f.ch[1]=com_rbuf[47];
		f.ch[2]=com_rbuf[50];
		f.ch[3]=com_rbuf[49];
		acqdata_set_value(acq_data,"g18401",UNIT_KPA,f.f,&arg_n);

		f.ch[0]=com_rbuf[72];
		f.ch[1]=com_rbuf[71];
		f.ch[2]=com_rbuf[74];
		f.ch[3]=com_rbuf[73];
		acqdata_set_value(acq_data,"e60201",UNIT_A,f.f,&arg_n);
		acqdata_set_value(acq_data,"e70201",UNIT_A,f.f,&arg_n);

		f.ch[0]=com_rbuf[76];
		f.ch[1]=com_rbuf[75];
		f.ch[2]=com_rbuf[78];
		f.ch[3]=com_rbuf[77];
		acqdata_set_value(acq_data,"e80201",UNIT_A,f.f,&arg_n);

		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"g28001",UNIT_0C,0,&arg_n);
		acqdata_set_value(acq_data,"g28201",UNIT_PA,0,&arg_n);
		acqdata_set_value(acq_data,"g20201",UNIT_M_S,0,&arg_n);
		acqdata_set_value(acq_data,"g20101",UNIT_M3_H,0,&arg_n);
		acqdata_set_value(acq_data,"g18001",UNIT_0C,0,&arg_n);
		acqdata_set_value(acq_data,"g18101",UNIT_PECENT,0,&arg_n);
		acqdata_set_value(acq_data,"g18401",UNIT_KPA,0,&arg_n);
		acqdata_set_value(acq_data,"e60201",UNIT_A,0,&arg_n);
		acqdata_set_value(acq_data,"e70201",UNIT_A,0,&arg_n);
		acqdata_set_value(acq_data,"e80201",UNIT_A,0,&arg_n);
		status=1;
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


