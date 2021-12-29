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
Create Time:2021.08.23 Mon.
Description:protocol_CEMS_HBHeFengCEMS
TX:01 03 00 00 00 14 45 C5 
RX:01 03 28
42 38 47 ae
C3 06 80 00
41 84 00 00
41 84 00 00 
41 a6 66 66
42 5a 14 7b
41 93 99 9A
3f 9d 70 A4
45 7f 8b 5c
41 93 99 9a
A8 D7

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/

int protocol_CEMS_HBHeFengCEMS(struct acquisition_data *acq_data)
{
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
	int cmd=0;
	int val=0;
	float valf = 0;
	float SO2=0,NO=0,O2=0,orig=0,nox=0,CO2=0;
	float SO2_orig=0,NO_orig=0,O2_orig=0,nox_orig=0,CO2_orig=0;
	float flue_gas_temperature = 0,flue_gas_pressure = 0,flue_gas_speed = 0,flue_gas_humidity = 0, smoke = 0;
	char *p;

	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x14);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBHeFeng CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBHeFeng CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBHeFeng CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBHeFeng CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, 0x14);
	if(p!=NULL)
	{
		flue_gas_temperature = getFloatValue(p, 3, dataType);

		flue_gas_pressure = getFloatValue(p, 7, dataType);

		flue_gas_speed = getFloatValue(p, 11, dataType);

		flue_gas_humidity =getFloatValue(p, 15, dataType);

		O2 = getFloatValue(p, 19, dataType);

		SO2 = getFloatValue(p, 23, dataType);

		nox= getFloatValue(p, 27, dataType);
		
		NO= getFloatValue(p, 31, dataType);

		smoke= getFloatValue(p, 35, dataType);
		
		CO2 = getFloatValue(p, 39, dataType);
		
		status=0;
	}
	else
	{
		flue_gas_temperature =0 ;
		flue_gas_pressure = 0;
		flue_gas_speed = 0;
		flue_gas_humidity = 0;
		SO2 = 0;
		NO = 0;
		O2 = 0;
		nox =0;
		smoke = 0;
		CO2 = 0;
		status=1;
	}
	acqdata_set_value(acq_data,"a01012",UNIT_0C,flue_gas_temperature,&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,flue_gas_pressure,&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,flue_gas_speed,&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,flue_gas_humidity,&arg_n);

	acqdata_set_value(acq_data,"a19001a",UNIT_MG_M3,O2,&arg_n);
	acqdata_set_value_orig(acq_data,"a19001",UNIT_MG_M3,O2,O2,&arg_n);
#if 0
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,SO2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,SO2,SO2,&arg_n); 
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,NO,&arg_n);
	acqdata_set_value_orig(acq_data,"a21003",UNIT_MG_M3,NO,NO,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,smoke,smoke,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value_orig(acq_data,"a05001",UNIT_MG_M3,CO2,CO2,&arg_n);
#endif

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
