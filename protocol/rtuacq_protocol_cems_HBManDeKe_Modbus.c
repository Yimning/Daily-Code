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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.11.13 Mon.
Description:protocol_CEMS_HBManDeKe_Modbus
TX:01 03 00 00 00 08 C5 C8
RX:01 03 10
41 20 00 00
42 48 00 00
41 A0 00 00
42 48 00 00
FF FF


DataType and Analysis:
	(FLOAT_CDAB) 08 00  44 B4  = 1440.25
*/


int protocol_CEMS_HBManDeKe_Modbus(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 4
	char *polcode[POLCODE_NUM]={
		"a21026a","a21003a","a19001a","a21004a"
	};
	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,	UNIT_MG_M3,  UNIT_PECENT,	UNIT_MG_M3
	};
	
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	float nox = 0;
	char flag =0;
	float valfArray[POLCODE_NUM]={0};

	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x1000;
	regcnt = 0x08;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe_Modbus CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe_Modbus CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe_Modbus CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe_Modbus CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		for(i = 0; i < POLCODE_NUM; i++)
		{
			valfArray[i]  = getFloatValue(p, i*4+3, dataType);

			acqdata_set_value(acq_data,polcode[i],unit[i],valfArray[i],&arg_n);

		}
		nox=NO_to_NOx(valfArray[1]);
		
		status=0;
	}
	else
	{
		for(i = 0; i < POLCODE_NUM; i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}
		status=1;
	}

	
	sleep(1);
	cmd = 0x01;
	regpos = 0x00;
	regcnt = 0x1C;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe_Modbus MARK SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe_Modbus MARK protocol,MARK : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe_Modbus MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe_Modbus MARK RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if((p[3]== 0) ||(p[4]==0))
			flag = 'N';
		else if((p[3]&0x01)==0x01)
				flag = 'N';
		else if((p[3]&0x02)==0x02)
				flag = 'N';
		else if((p[3]&0x04)==0x04)
				flag = 'C';
	 	else if((p[3]&0x08)==0x08)
	 			flag = 'C';
	 	else if((p[3]&0x10)==0x10)
	 			flag = 'C';
	 	else if((p[3]&0x20)==0x20)
	 			flag = 'C';
	 	else if((p[3]&0x40)==0x40)
	 			flag = 'C';
	 	else if((p[3]&0x80)==0x80)
	 			flag = 'C';
		else if((p[4]&0x01)==0x01)
				flag = 'C';
		else if((p[4]&0x02)==0x02)
				flag = 'C';
		else if((p[4]&0x04)==0x04)
				flag = 'M';
	 	else  flag = 'N';
		
		status=0;
	}
	else
	{
		status=1;
	}

	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,valfArray[0],&arg_n); 
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,valfArray[1],&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a19001",UNIT_MG_M3,valfArray[2],&arg_n);
	
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,valfArray[3],&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	if(status == 0)
	{
		acq_data->dev_stat=flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
