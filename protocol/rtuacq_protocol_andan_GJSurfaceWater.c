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

/*
Author:Yimning
Time:2021.08.12 Thur.
Description:protocol_ANDAN_GJSurfaceWater
TX:01 03 10 08 00 1E 40 C0 
RX:01 03 3C
41 01 40 40
54 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 
51 02 40 40 
6C 72 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 
61 03 40 40 
6C 72 00 00 00 00 00 00 00 00 00 00
FF FF
DataType and Analysis:
	(FLOAT_CDAB) 32 33 41 03   = 8.200
*/
int protocol_ANDAN_GJSurfaceWater(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0;
	int val = 0;
	float valf = 0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1008,0x1E);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"GJSurfaceWater ANDAN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("GJSurfaceWater ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("GJSurfaceWater ANDAN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"GJSurfaceWater ANDAN RECV:",com_rbuf,size);
	if((size>=65)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getFloatValue(com_rbuf, 3,dataType);
		
		status=0;
	}
	else
	{
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w21003",UNIT_MG_L,valf,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


 
/*
Author:Yimning
Time:2021.08.02 Mon.
Description:protocol_ANDAN_GJSurfaceWater_info
TX1:01 03 10 80 00 11 80 EE 
RX1:01 03 22
00 00 00 00 00 00
00 01
00 01
00 00
00 00
00 11
00 11
00 30
00 00
00 00
00 00
00 00 00 00
00 00 00 00
FF FF

TX2:01 03 10 A0 00 32 C0 FD 
RX2:01 03 64
00 00
00 30
00 30
00 00 42 42 
00 00 42 00 
00 00 40 40 
00 00 42 00 
00 00 00 00 00 00
01 00 42 42
01 00 42 42
02 00 42 42
02 00 42 42
03 00 42 42
03 00 42 42
04 00 42 42
04 00 42 42
05 00 42 42
05 00 42 42
00 00 00 00
00 00 00 00
00 00 00 01
17 0C 09 01 00 00
17 0C 09 01 00 00
00 00 00 00
00 00 00 00
FF FF

DataType and Analysis:
	(INT_CDAB) 00 01    = 1
	(FLOAT_CDAB) 32 33 41 03   = 8.200

*/
int protocol_ANDAN_GJSurfaceWater_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0;
	float valf=0;
	int val=0;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	dataType = FLOAT_CDAB;
	SYSLOG_DBG("protocol ANDAN GJSurfaceWater info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1080,0x11);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol ANDAN GJSurfaceWater info SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol ANDAN GJSurfaceWater, Info1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol ANDAN GJSurfaceWater Info1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol ANDAN GJSurfaceWater info RECV1:",com_rbuf,size);
	if((size>=39)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = getInt16Value(com_rbuf,9, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 9: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 10: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val = getInt16Value(com_rbuf,13, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 10: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n); break;
			case 11: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,8,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val = getInt16Value(com_rbuf,15, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
		}		
		
		val = getInt16Value(com_rbuf,21, INT_AB);
		 acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
				
		status=0;
	}
	else {
		status = 1;
	}

	cmd = 0x03;
	dataType = FLOAT_CDAB;
	t1=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x10A0,0x32);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol ANDAN GJSurfaceWater info SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol ANDAN GJSurfaceWater, Info2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol ANDAN GJSurfaceWater Info2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol ANDAN GJSurfaceWater info RECV2:",com_rbuf,size);
	if((size>=105)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = getInt16Value(com_rbuf,5, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);
		
		val = getInt16Value(com_rbuf,7, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);		

		valf =getFloatValue(com_rbuf,17 , dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf =getFloatValue(com_rbuf,21 , dataType);	
		acqdata_set_value_flag(acq_data,"i13007",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);		

		valf =getFloatValue(com_rbuf,31 , dataType);	
		acqdata_set_value_flag(acq_data,"i13108",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf =getFloatValue(com_rbuf,35 , dataType);	
		acqdata_set_value_flag(acq_data,"i13109",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[83])+100;
		timer.tm_mon=BCD(com_rbuf[84])-1;
		timer.tm_mday=BCD(com_rbuf[85]);
		timer.tm_hour=BCD(com_rbuf[86]);
		timer.tm_min=BCD(com_rbuf[87]);
		timer.tm_sec=BCD(com_rbuf[88]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[89])+100;
		timer.tm_mon=BCD(com_rbuf[90])-1;
		timer.tm_mday=BCD(com_rbuf[91]);
		timer.tm_hour=BCD(com_rbuf[92]);
		timer.tm_min=BCD(com_rbuf[93]);
		timer.tm_sec=BCD(com_rbuf[94]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		//acqdata_set_value_flag(acq_data,"i13107",UNIT_L,t3,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


