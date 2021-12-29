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

int protocol_CEMS_BJHangTianYiLai(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,no2=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	dataType=FLOAT_ABCD;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai CEMS RECV:",com_rbuf,size);
	if((size>=17)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		no=getFloatValue(com_rbuf, 3, dataType);
		nox=NO_to_NOx(no);

		so2=getFloatValue(com_rbuf, 7, dataType);

		o2=getFloatValue(com_rbuf, 11, dataType);
		
		status=0;
	}
	else
	{
		nox=0;
		so2=0;
		o2=0;
		no=0;
		status=1;
	}
	
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

struct BJHangTianYiLai_info{
#define SO2 0
#define NO 1
#define O2 2
	
	float i13023[3];
	float i13028[3];
	float i13013[3];
	float i13022[3];
	float i13026[3];
	
	time_t i13021[3];
	time_t i13027[3];
};

struct BJHangTianYiLai_info BJTY_cems_data;

int protocol_CEMS_BJHangTianYiLai_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int y,m,d,h,min,s;
	int index[3];
	unsigned int devaddr=0,cmd=0;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	cmd=0x02;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai STATUS RECV:",com_rbuf,size);
	if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else if((val&0x01)==0x01)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else if((val&0x02)==0x02)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else if((val&0x04)==0x04)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		status=0;
	}


	sleep(1);
	cmd=0x04;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x031F,0x2E);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai INFO RECV1:",com_rbuf,size);
	if((size>=97)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=getInt16Value(com_rbuf, 3, INT_AB);
		d=val/100;
		m=val%100;
		val=getInt16Value(com_rbuf, 5, INT_AB);
		y=val;
		val=getInt16Value(com_rbuf, 7, INT_AB);
		h=val/100;
		min=val%100;
		val=getInt16Value(com_rbuf, 9, INT_AB);
		s=val;
		BJTY_cems_data.i13021[NO]=getTimeValue(y,m,d,h,min,s);
		BJTY_cems_data.i13023[NO]=getFloatValue(com_rbuf, 11, dataType);

		val=getInt16Value(com_rbuf, 19, INT_AB);
		d=val/100;
		m=val%100;
		val=getInt16Value(com_rbuf, 21, INT_AB);
		y=val;
		val=getInt16Value(com_rbuf, 23, INT_AB);
		h=val/100;
		min=val%100;
		val=getInt16Value(com_rbuf, 25, INT_AB);
		s=val;
		BJTY_cems_data.i13027[NO]=getTimeValue(y,m,d,h,min,s);
		BJTY_cems_data.i13028[NO]=getFloatValue(com_rbuf, 27, dataType);


		val=getInt16Value(com_rbuf, 35, INT_AB);
		d=val/100;
		m=val%100;
		val=getInt16Value(com_rbuf, 37, INT_AB);
		y=val;
		val=getInt16Value(com_rbuf, 39, INT_AB);
		h=val/100;
		min=val%100;
		val=getInt16Value(com_rbuf, 41, INT_AB);
		s=val;
		BJTY_cems_data.i13021[SO2]=getTimeValue(y,m,d,h,min,s);
		BJTY_cems_data.i13023[SO2]=getFloatValue(com_rbuf, 43, dataType);

		val=getInt16Value(com_rbuf, 51, INT_AB);
		d=val/100;
		m=val%100;
		val=getInt16Value(com_rbuf, 53, INT_AB);
		y=val;
		val=getInt16Value(com_rbuf, 55, INT_AB);
		h=val/100;
		min=val%100;
		val=getInt16Value(com_rbuf, 57, INT_AB);
		s=val;
		BJTY_cems_data.i13027[SO2]=getTimeValue(y,m,d,h,min,s);
		BJTY_cems_data.i13028[SO2]=getFloatValue(com_rbuf, 59, dataType);


		val=getInt16Value(com_rbuf, 67, INT_AB);
		d=val/100;
		m=val%100;
		val=getInt16Value(com_rbuf, 69, INT_AB);
		y=val;
		val=getInt16Value(com_rbuf, 71, INT_AB);
		h=val/100;
		min=val%100;
		val=getInt16Value(com_rbuf, 73, INT_AB);
		s=val;
		BJTY_cems_data.i13021[O2]=getTimeValue(y,m,d,h,min,s);
		BJTY_cems_data.i13023[O2]=getFloatValue(com_rbuf, 75, dataType);

		val=getInt16Value(com_rbuf, 83, INT_AB);
		d=val/100;
		m=val%100;
		val=getInt16Value(com_rbuf, 85, INT_AB);
		y=val;
		val=getInt16Value(com_rbuf, 87, INT_AB);
		h=val/100;
		min=val%100;
		val=getInt16Value(com_rbuf, 89, INT_AB);
		s=val;
		BJTY_cems_data.i13027[O2]=getTimeValue(y,m,d,h,min,s);
		BJTY_cems_data.i13028[O2]=getFloatValue(com_rbuf, 91, dataType);
		
		status=0;
	}


	sleep(1);
	cmd=0x04;
	index[NO]=0;
	index[SO2]=0;
	index[O2]=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x07CF,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai INFO RECV2:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		index[NO]=getInt16Value(com_rbuf, 3, INT_AB);
		index[SO2]=getInt16Value(com_rbuf, 5, INT_AB);
		index[O2]=getInt16Value(com_rbuf, 7, INT_AB);

		index[NO]=(index[NO]==1)?1:2;
		index[SO2]=(index[SO2]==1)?1:2;
		index[O2]=(index[O2]==1)?1:2;
		
		status=0;
	}

	sleep(1);
	cmd=0x04;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0833,0x24);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai INFO RECV3:",com_rbuf,size);
	if((size>=77)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		if(index[NO]==1)
			BJTY_cems_data.i13013[NO]=getFloatValue(com_rbuf, 7,  dataType);
		else
			BJTY_cems_data.i13013[NO]=getFloatValue(com_rbuf, 15,  dataType);

		if(index[SO2]==1)
			BJTY_cems_data.i13013[SO2]=getFloatValue(com_rbuf, 39, dataType);
		else
			BJTY_cems_data.i13013[SO2]=getFloatValue(com_rbuf, 47, dataType);

		
		BJTY_cems_data.i13013[O2]=getFloatValue(com_rbuf, 71, dataType);
		
		status=0;
	}


	sleep(1);
	cmd=0x04;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0BB7,0x48);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai INFO RECV4:",com_rbuf,size);
	if((size>=149)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		if(index[NO]==1)
		{
			BJTY_cems_data.i13022[NO]=getFloatValue(com_rbuf, 11, dataType);
			BJTY_cems_data.i13026[NO]=getFloatValue(com_rbuf, 15, dataType);
		}
		else
		{
			BJTY_cems_data.i13022[NO]=getFloatValue(com_rbuf, 27, dataType);
			BJTY_cems_data.i13026[NO]=getFloatValue(com_rbuf, 31, dataType);
		}

		if(index[SO2]==1)
		{
			BJTY_cems_data.i13022[SO2]=getFloatValue(com_rbuf, 75, dataType);
			BJTY_cems_data.i13026[SO2]=getFloatValue(com_rbuf, 79, dataType);
		}
		else
		{
			BJTY_cems_data.i13022[SO2]=getFloatValue(com_rbuf, 91, dataType);
			BJTY_cems_data.i13026[SO2]=getFloatValue(com_rbuf, 95, dataType);
		}

		BJTY_cems_data.i13022[O2]=getFloatValue(com_rbuf, 139, dataType);
		BJTY_cems_data.i13026[O2]=getFloatValue(com_rbuf, 143, dataType);
		
		status=0;
	}

	acqdata_set_value_flag(acq_data,"i13021",BJTY_cems_data.i13021[SO2],0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,BJTY_cems_data.i13023[SO2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13027",BJTY_cems_data.i13027[SO2],0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,BJTY_cems_data.i13028[SO2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,BJTY_cems_data.i13013[SO2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,BJTY_cems_data.i13022[SO2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,BJTY_cems_data.i13026[SO2],INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJHangTianYiLai_NO_info(struct acquisition_data *acq_data)
{
	int arg_n=0;
	

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_NO_info\n");
	
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);
	
	acqdata_set_value_flag(acq_data,"i13021",BJTY_cems_data.i13021[NO],0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,BJTY_cems_data.i13023[NO],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13027",BJTY_cems_data.i13027[NO],0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,BJTY_cems_data.i13028[NO],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,BJTY_cems_data.i13013[NO],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,BJTY_cems_data.i13022[NO],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,BJTY_cems_data.i13026[NO],INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);

	
	acq_data->acq_status = ACQ_OK;
	
	return arg_n;
}

int protocol_CEMS_BJHangTianYiLai_O2_info(struct acquisition_data *acq_data)
{
	int arg_n=0;
	

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_O2_info\n");
	
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);
	
	acqdata_set_value_flag(acq_data,"i13021",BJTY_cems_data.i13021[O2],0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,BJTY_cems_data.i13023[O2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13027",BJTY_cems_data.i13027[O2],0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,BJTY_cems_data.i13028[O2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,BJTY_cems_data.i13013[O2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,BJTY_cems_data.i13022[O2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,BJTY_cems_data.i13026[O2],INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);

	
	acq_data->acq_status = ACQ_OK;
	
	return arg_n;
}

int protocol_PLC_BJHangTianYiLai(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf[5];
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0;
	float speed,atm_press,PTC;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	devaddr=modbusarg->devaddr&0xffff;
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;
	cmd=0x03;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x08);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai  PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai  PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai  PLC RECV:",com_rbuf,size);
	if((size>=21)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=getInt16Value(com_rbuf, 3, INT_AB);
		valf[0]=PLCtoValue(modbusarg, 0, 4095, val, "a01012");
		
		val=getInt16Value(com_rbuf, 5, INT_AB);
		valf[1]=PLCtoValue(modbusarg, 0, 4095, val, "a01013");

		val=getInt16Value(com_rbuf, 7, INT_AB);
		valf[2]=PLCtoValue(modbusarg, 0, 4095, val, "a01011");

		val=getInt16Value(com_rbuf, 9, INT_AB);
		valf[3]=PLCtoValue(modbusarg, 0, 4095, val, "a34013a");

		val=getInt16Value(com_rbuf, 17, INT_AB);
		valf[4]=PLCtoValue(modbusarg, 0, 4095, val, "a01014");

		if(PTC>0 && (valf[3]+273)>0 && (valf[4]+atm_press)>0)
			speed=PTC*sqrt(valf[2])*sqrt(((valf[3]+273)/273)*(101325/(valf[4]+atm_press))*(2/1.2928));
		else
			speed=0;
		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		speed=0;
		valf[3]=0;
		status=1;
	}
	
	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[1],&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

