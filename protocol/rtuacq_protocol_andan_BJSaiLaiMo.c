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

static float ANDAN_QDJiaMing_orig=0;
//rtuacq_protocol_andan_QDJiaMing.c
/*
Author:Yimning
Time:2021.07.21 Wed.
Description:protocol_ANDAN_QDJiaMing
TX:01 03 00 31 00 04 15 C6 
RX:01 03 08
65 A8 42 DF
65 00 42 DF
FF FF
DataType and Analysis:
	(FLOAT_CDAB) 65 A8 42 DF   = 111.699
*/
int protocol_ANDAN_BJSaiLaiMo(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_andan=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0;
	float valf = 0;

	MODBUS_DATA_TYPE dataType;
	struct hebei_dynamic_info_water *data;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x31,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJSaiLaiMo ANDAN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJSaiLaiMo ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJSaiLaiMo ANDAN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJSaiLaiMo ANDAN RECV:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getFloatValue(com_rbuf, 3,dataType);
		total_andan=valf;

		valf = getFloatValue(com_rbuf, 7,dataType);
		ANDAN_QDJiaMing_orig=valf;
		status=0;
	}
	else
	{
		total_andan=0;
		ANDAN_QDJiaMing_orig = 0;
		status=1;
	}

	ret=acqdata_set_value_orig(acq_data,"w21003",UNIT_MG_L,total_andan,ANDAN_QDJiaMing_orig,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


 
/*
Author:Yimning
Time:2021.07.21 Wed.
Description:protocol_ANDAN_BJSaiLaiMo_info
TX:01 03 00 01 00 3C 14 1B
RX:01 03 78
00 00
00 01
00 01
00 00
00 1F
00 00
07 E3 00 05 00 01 00 0F 00 00 00 07
00 00
07 E3 00 05 00 01 00 0F 00 00 00 07
07 E3 00 05 00 01 00 0F 00 00 00 07
00 00
00 00
00 1E
00 00 00 00
65 A8 42 DF
65 00 42 DF
00 00 3F 00
01 00 3E 00
65 00 42 DF
65 00 42 DF
65 00 42 DF
65 00 42 DF
07 E3 00 05 00 01 00 0F 00 00 00 07
00 00 00 00
FF FF

DataType and Analysis:
	(FLOAT_CDAB) 65 00 42 DF    = 111.697
*/
int protocol_ANDAN_BJSaiLaiMo_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_andan=0;
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
	struct hebei_dynamic_info_water data;
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
	SYSLOG_DBG("protocol_ANDAN_BJSaiLaiMo_info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,0x01,0x36);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_ANDAN_BJSaiLaiMo_info SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_ANDAN_BJSaiLaiMo_info,Info1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_ANDAN_BJSaiLaiMo Info1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_ANDAN_BJSaiLaiMo_info RECV1:",com_rbuf,size);
	if((size>=113)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = getInt16Value(com_rbuf,5, INT_AB);
		switch(val){
			//case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			//case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			//case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			//case 5: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			//case 6: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			//case 7: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		#if 0
		val = getInt16Value(com_rbuf,5, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
		}
		#endif 
		
		val = getInt16Value(com_rbuf,7, INT_AB);
		switch(val){
			//case 1: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			//case 3: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			//case 4: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			//case 5: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			//case 6: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 9: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 11: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		
		val=getInt16Value(com_rbuf,11, INT_AB);
		data.i13116 = val;
		
	        val = getUInt16Value(com_rbuf, 29, INT_AB);
 		year=(val);
		mon=(com_rbuf[32]);
		day=(com_rbuf[34]);
		hour=(com_rbuf[36]);
		min=(com_rbuf[38]);
		sec=(com_rbuf[40]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		data.i13101=t3;

		val = getUInt16Value(com_rbuf, 41, INT_AB);
 		year=(val);
		mon=(com_rbuf[44]);
		day=(com_rbuf[46]);
		hour=(com_rbuf[48]);
		min=(com_rbuf[50]);
		sec=(com_rbuf[52]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		data.i13107=t3;

		val= getUInt16Value(com_rbuf,57, INT_AB);
		data.i13003 = val;

		data.i13105 = ANDAN_QDJiaMing_orig;

		valf= getFloatValue(com_rbuf,71, dataType);
		data.i13008 = valf;

		valf= getFloatValue(com_rbuf,75, dataType);
		data.i13007 = valf;

		valf= getFloatValue(com_rbuf,79, dataType);
		data.i13104 = valf;

		valf= getFloatValue(com_rbuf,87, dataType);
		data.i13108 = valf;

		valf= getFloatValue(com_rbuf,91, dataType);
		data.i13110 = valf;
		status=0;
	}
	else {
		status = 1;
	}
	
	acqdata_set_value_flag(acq_data,"i13116",UNIT_L,data.i13116,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13101",data.i13101,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13107",data.i13107,0.0,INFOR_ARGUMENTS,&arg_n);
        acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,data.i13003,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,data.i13105,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13008",UNIT_L,data.i13008,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13007",UNIT_L,data.i13007,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13104",UNIT_L,data.i13104,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13108",UNIT_L,data.i13108,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13110",UNIT_L,data.i13110,INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
