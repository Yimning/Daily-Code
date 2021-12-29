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

int protocol_VOCs_SZHengFuSheng2(struct acquisition_data *acq_data)
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

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x1B);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZHengFuSheng2 voc SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZHengFuSheng2 voc protocol,voc 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZHengFuSheng2 voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZHengFuSheng2 voc RECV:",com_rbuf,size);
	if((size>=59)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		acqdata_set_value(acq_data,"g09001",UNIT_MG_M3,val/10.0,&arg_n);

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,val/10.0,&arg_n);

		f.ch[0]=com_rbuf[8];
		f.ch[1]=com_rbuf[7];
		f.ch[2]=com_rbuf[10];
		f.ch[3]=com_rbuf[9];
		acqdata_set_value(acq_data,"g19501",UNIT_PECENT,f.f,&arg_n);

		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		acqdata_set_value(acq_data,"g28001",UNIT_0C,val/10.0,&arg_n);

		val=com_rbuf[13];
		val<<=8;
		val+=com_rbuf[14];
		acqdata_set_value(acq_data,"g28201",UNIT_PA,(float)val,&arg_n);

		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		acqdata_set_value(acq_data,"g20201",UNIT_M_S,val/100.0,&arg_n);

		val=com_rbuf[19];
		val<<=8;
		val+=com_rbuf[20];
		val<<=8;
		val+=com_rbuf[17];
		val<<=8;
		val+=com_rbuf[18];
		acqdata_set_value(acq_data,"g20101",UNIT_M3_H,(float)val,&arg_n);

		for(i=0;i<4;i++)
		{
			sprintf(polcode,"e702%02d",i+1);
			val=com_rbuf[21+i*8];
			val<<=8;
			val+=com_rbuf[22+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_A,val/100.0,&arg_n);

			sprintf(polcode,"e701%02d",i+1);
			val=com_rbuf[23+i*8];
			val<<=8;
			val+=com_rbuf[24+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_KW,val/100.0,&arg_n);

			sprintf(polcode,"e700%02d",i+1);
			val=com_rbuf[27+i*8];
			val<<=8;
			val+=com_rbuf[28+i*8];
			val<<=8;
			val+=com_rbuf[25+i*8];
			val<<=8;
			val+=com_rbuf[26+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_KWH,val/100.0,&arg_n);
		}

		f.ch[0]=com_rbuf[54];
		f.ch[1]=com_rbuf[53];
		f.ch[2]=com_rbuf[56];
		f.ch[3]=com_rbuf[55];
		acqdata_set_value(acq_data,"g20301",UNIT_KG_H,f.f,&arg_n);
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"g09001",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"g19501",UNIT_PECENT,0,&arg_n);
		acqdata_set_value(acq_data,"g28001",UNIT_0C,0,&arg_n);
		acqdata_set_value(acq_data,"g28201",UNIT_PA,0,&arg_n);
		acqdata_set_value(acq_data,"g20201",UNIT_M_S,0,&arg_n);
		acqdata_set_value(acq_data,"g20101",UNIT_M3_H,0,&arg_n);
		for(i=0;i<4;i++)
		{
			sprintf(polcode,"e702%02d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_A,0,&arg_n);
			sprintf(polcode,"e701%02d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_KW,0,&arg_n);
			sprintf(polcode,"e700%02d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_KWH,0,&arg_n);
		}
		status=1;
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


