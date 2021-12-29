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

int protocol_CEMS_SZCuiYunGu_TLGAS280(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	float no=0,so2=0,o2=0,co=0,co2=0,nox=0;
	float nox_orig=0,so2_orig=0,o2_orig=0,co_orig=0;
	int val;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	char *p;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_CDAB;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x0A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZCuiYunGu TL-GAS280 CEMS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 CEMS RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x0A);
	if(p!=NULL)
	{
		no=getFloatValue(p, 3, dataType);
		
		so2=getFloatValue(p, 7, dataType);
		
		o2=getFloatValue(p, 11, dataType);
		
		co=getFloatValue(p, 15, dataType);

		co2=getFloatValue(p, 19, dataType);
		
		status=0;
	}
	else
	{
		no=0;
		so2=0;
		o2=0;
		co=0;
		co2=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x46,0x21);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZCuiYunGu TL-GAS280 CEMS SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 CEMS RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x21);
	if(p!=NULL)
	{
		so2_orig=getFloatValue(p, 11, dataType);
		
		nox_orig=getFloatValue(p, 23, dataType);
		
		o2_orig=getFloatValue(p, 35, dataType);
		
		co_orig=getFloatValue(p, 47, dataType);

		val=getUInt16Value(p, 67, INT_AB);

		nox=(val==30) ? NO_to_NOx(no) : no;
	}
	else
	{
		nox_orig=0;
		so2_orig=0;
		o2_orig=0;
		co_orig=0;
		nox=0;
		status=1;
	}
	

	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,nox_orig,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,so2,so2_orig,&arg_n);
	acqdata_set_value_orig(acq_data,"a19001",UNIT_PECENT,o2,o2_orig,&arg_n);
	acqdata_set_value_orig(acq_data,"a21005",UNIT_MG_M3,co,co_orig,&arg_n);
	acqdata_set_value_orig(acq_data,"a05001",UNIT_MG_M3,co2,0,&arg_n);

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,co,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



int protocol_CEMS_SZCuiYunGu_TLGAS280_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	float span;
	int val=0;
	int devaddr=0;
	char *p;
	union uf f;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SZCuiYunGu_TLGAS280_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_CDAB;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x67,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZCuiYunGu TL-GAS280 SO2 STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 SO2 STATUS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x02);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 2:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 3:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 4:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 5:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 6:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 7:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 8:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			default:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val=getUInt16Value(p, 5, INT_AB);
		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);

			if((val&0x0001)==0x0001)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x0002)==0x0002)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x0004)==0x0004)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x0008)==0x0008)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x0010)==0x0010)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x0020)==0x0020)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x0040)==0x0040)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x0080)==0x0080)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x0100)==0x0100)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x0200)==0x0200)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x0400)==0x0400)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,8,INFOR_STATUS,&arg_n);
			else if((val&0x0800)==0x0800)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n);
			else if((val&0x1000)==0x1000)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x2000)==0x2000)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x4000)==0x4000)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x8000)==0x8000)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0C,0x6F);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZCuiYunGu TL-GAS280 SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 SO2 INFO RECV1:",com_rbuf,200);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 SO2 INFO RECV2:",&com_rbuf[200],size-200);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x6F);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		span=valf;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		valf=valf*100/span;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 63, dataType);
		valf=valf*100/span;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 76, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 80, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 119, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 123, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 127, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 201, INT_AB);
		t3=getTimeValue(val, p[204], p[206], p[208], p[210], p[212]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 213, INT_AB);
		t3=getTimeValue(val, p[216], p[218], p[220], p[222], p[224]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



int protocol_CEMS_SZCuiYunGu_TLGAS280_NOx_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	float span;
	int val=0;
	int devaddr=0;
	union uf f;
	char *p;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SZCuiYunGu_TLGAS280_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_CDAB;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x77);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZCuiYunGu TL-GAS280 NOx INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 NOx INFO RECV1:",com_rbuf,200);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 NOx INFO RECV2:",&com_rbuf[200],size-200);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x77);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		span=valf;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		valf=valf*100/span;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 63, dataType);
		valf=valf*100/span;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 91, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 95, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 135, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 139, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 143, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 205, INT_AB);
		t3=getTimeValue(val, p[208], p[210], p[212], p[214], p[216]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 229, INT_AB);
		t3=getTimeValue(val, p[232], p[234], p[236], p[238], p[240]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_SZCuiYunGu_TLGAS280_O2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	float span;
	int val=0;
	int devaddr=0;
	char *p;
	union uf f;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SZCuiYunGu_TLGAS280_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_CDAB;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0E,0x79);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZCuiYunGu TL-GAS280 O2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 O2 INFO RECV1:",com_rbuf,200);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZCuiYunGu TL-GAS280 O2 INFO RECV2:",&com_rbuf[200],size-200);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x79);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		span=valf;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		valf=valf*100/span;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 63, dataType);
		valf=valf*100/span;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 91, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 95, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 139, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 143, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 147, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 197, INT_AB);
		t3=getTimeValue(val, p[200], p[202], p[204], p[206], p[208]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 233, INT_AB);
		t3=getTimeValue(val, p[236], p[238], p[240], p[242], p[244]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


