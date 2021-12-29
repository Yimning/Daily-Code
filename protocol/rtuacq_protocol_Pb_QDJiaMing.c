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

static float Pb_QDJiaMing_orig=0;
//rtuacq_protocol_cod_QDJiaMing.c
/*
Author:Yimning
Time:2021.07.20 Tues.
Description:protocol_Pb_QDJiaMing
TX:01 03 00 00 00 0305 CB

RX:01 03 06 3D 5D 85 20 00 01 E1 5F
DataType and Analysis:
	(FLOAT_ABCD) 3D 5D 85 20   = 0.054
	(INT_AB) 00 01   = 1
*/
int protocol_Pb_QDJiaMing(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_cod=0;
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
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDJiaMing Pb SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDJiaMing Pb protocol,Pb : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDJiaMing Pb data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDJiaMing Pb RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getFloatValue(com_rbuf, 3,dataType);
		total_cod=valf;
		status=0;
	}
	else
	{
		total_cod=0;
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w20120",UNIT_MG_L,total_cod,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


 
/*
Author:Yimning
Time:2021.07.20 Tues.
Description:protocol_Pb_QDJiaMing_info
TX1:01 03 00 64 00 03 44 14
RX1:01 03 06 00 02 00 00 00 00 58 B5

TX2:01 03 00 C8 00 46 45 C6
RX2:01 03 42 
3F A1 30 BE 
42 F0 00 00 
42 48 00 00 
00 00 00 00 
00 00 00 00 
41 C8 00 00 
42 48 00 00 
45 5E E4 00 
C1 C8 E1 47 
3F 80 00 00 
00 00 00 00 
43 25 00 00 
41 F0 00 00 
43 FA 00 00 
3F 00 00 00 
40 50 00 00 
3E 80 00 00 
3F 00 00 00 
3F A1 30 BE 
42 F0 00 00 
42 48 00 00 
41 F0 00 00 
43 FA 00 00 
41 C8 00 00 
42 48 00 00 
45 5E E4 00
07 E3 00 05 00 01 00 0F 00 00 00 07
07 E3 00 05 00 01 00 0F 00 00 00 07
07 E3 00 05 00 01 00 0F 00 00 00 07
1B 66

DataType and Analysis:
	(FLOAT_ABCD) 42 F0 00 00    = 120
*/
int protocol_Pb_QDJiaMing_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_cod=0;
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
	//dataType = INT_AB;
	SYSLOG_DBG("protocol_Pb_QDJiaMing_info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w20120",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,0x64,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_Pb_QDJiaMing_info SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_Pb_QDJiaMing_info,Info1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_Pb_QDJiaMing Info1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_Pb_QDJiaMing_info RECV1:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = getInt16Value(com_rbuf,3, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 7: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val = getInt16Value(com_rbuf,5, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
		}
		
		val = getInt16Value(com_rbuf,7, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 7: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,8,INFOR_STATUS,&arg_n); break;
			case 9: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		status=0;
	   }
	  else
	   {
		status=1;
	   }


	cmd = 0x03;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xC8,0x46);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_Pb_QDJiaMing_info SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_COD_QDJiaMing_info,Info2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_COD_QDJiaMing Info2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_Pb_QDJiaMing_info RECV2:",com_rbuf,size);
	if((size>=145)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf= getFloatValue(com_rbuf,3, dataType);
		data.i13105 = valf;

		valf= getFloatValue(com_rbuf,7, dataType);
		data.i13003 = valf;
		
		valf= getFloatValue(com_rbuf,11, dataType);
		data.i13116 = valf;
		
		valf= getFloatValue(com_rbuf,19, dataType);
		data.i13104 = valf;
		
		valf= getFloatValue(com_rbuf,23, dataType);
		data.i13108 = valf;

		valf= getFloatValue(com_rbuf,31, dataType);
		data.i13008 = valf;
		
		valf= getFloatValue(com_rbuf,35, dataType);
		data.i13007 = valf;
		
		valf= getFloatValue(com_rbuf,39, dataType);
		data.i13119 = valf;
		
		valf= getFloatValue(com_rbuf,43, dataType);
		data.i13120 = valf;
		
		valf= getFloatValue(com_rbuf,47, dataType);
		data.i13004 = valf;
		
		valf= getFloatValue(com_rbuf,51, dataType);
		data.i13005 = valf;
		
		valf= getFloatValue(com_rbuf,67, dataType);
		data.i13110 = valf;
		
		valf= getFloatValue(com_rbuf,71, dataType);
		data.i13106 = valf;
		
		valf= getFloatValue(com_rbuf,75, dataType);
		data.i13112 = valf;
		
		valf= getFloatValue(com_rbuf,79, dataType);
		data.i13103 = valf;
		
		valf= getFloatValue(com_rbuf,83, dataType);
		data.i13118 = valf;

		valf= getFloatValue(com_rbuf,87, dataType);
		data.i13109 = valf;

		valf= getFloatValue(com_rbuf,91, dataType);
		data.i13123 = valf;
		
		valf= getFloatValue(com_rbuf,95, dataType);
		data.i13127 = valf;

		valf= getFloatValue(com_rbuf,99, dataType);
		data.i13129 = valf;

		valf= getFloatValue(com_rbuf,103, dataType);
		data.i13130 = valf;

	        val = getUInt16Value(com_rbuf, 107, INT_AB);
 		year=(val);
		mon=(com_rbuf[110]);
		day=(com_rbuf[112]);
		hour=(com_rbuf[114]);
		min=(com_rbuf[116]);
		sec=(com_rbuf[118]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		data.i13101=t3;

		val = getUInt16Value(com_rbuf, 119, INT_AB);
 		year=(val);
		mon=(com_rbuf[122]);
		day=(com_rbuf[124]);
		hour=(com_rbuf[126]);
		min=(com_rbuf[128]);
		sec=(com_rbuf[130]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		data.i13107=t3;

		val = getUInt16Value(com_rbuf, 131, INT_AB);
 		year=(val);
		mon=(com_rbuf[134]);
		day=(com_rbuf[136]);
		hour=(com_rbuf[138]);
		min=(com_rbuf[140]);
		sec=(com_rbuf[142]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		data.i13128=t3;
		
		status=0;
	   }

	acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,data.i13105,INFOR_ARGUMENTS,&arg_n);
        acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,data.i13003,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13116",UNIT_L,data.i13116,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13104",UNIT_L,data.i13104,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13108",UNIT_L,data.i13108,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13008",UNIT_L,data.i13008,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13007",UNIT_L,data.i13007,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,data.i13119,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13120",UNIT_L,data.i13120,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,data.i13004,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,data.i13005,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13110",UNIT_L,data.i13110,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13106",UNIT_PECENT,data.i13106,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13112",UNIT_PECENT,data.i13112,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13103",UNIT_MINUTE,data.i13103,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13118",UNIT_NONE,data.i13118,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13109",UNIT_L,data.i13109,INFOR_ARGUMENTS,&arg_n);	
	acqdata_set_value_flag(acq_data,"i13123",UNIT_MINUTE,data.i13123,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13127",UNIT_G_L,data.i13127,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,data.i13129,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,data.i13130,INFOR_ARGUMENTS,&arg_n);	
	acqdata_set_value_flag(acq_data,"i13101",data.i13101,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13107",data.i13107,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13128",data.i13128,0.0,INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

