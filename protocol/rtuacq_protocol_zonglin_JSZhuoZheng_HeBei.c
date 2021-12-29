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
Time:2021.08.02 Mon.
Description:protocol_ZONGLIN_JSZhuoZheng_HeBei
TX:01 03 00 25 00 02 D5 C0 
RX:01 03 06 
32 33 41 03
FF FF
DataType and Analysis:
	(FLOAT_ABCD) 32 33 41 03   = 8.200
*/
int protocol_ZONGLIN_JSZhuoZheng_HeBei(struct acquisition_data *acq_data)
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
	size=modbus_pack(com_tbuf,devaddr,cmd,0x38,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JSZhuoZheng_HeBei ZONGLIN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JSZhuoZheng_HeBei ZONGLIN protocol,ZONGLIN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JSZhuoZheng_HeBei ZONGLIN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JSZhuoZheng_HeBei ZONGLIN RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getFloatValue(com_rbuf, 3,dataType);
		
		status=0;
	}
	else
	{
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,valf,&arg_n);
	
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
Description:protocol_ZONGLIN_JSZhuoZheng_HeBei_info
TX:01 03 00 22 00 03 A5 C1 
RX:01 03 06 
00 01 
00 00
00 03
FF FF

TX1:01 03 00 00 00 34 44 1D 
RX1:01 03 68
21 08 02 0C 1E 1E
32 33 41 03
32 33 41 03
14 78 40 E6
14 7A 40 E6
14 7C 40 E6
21 08 02 0C 1E 1E
14 11 40 E6
14 13 40 E6
14 15 40 E6
14 17 40 E6
14 19 40 E6
14 1B 40 E6
14 1D 40 E6
14 1F 40 E6
14 21 40 E6
14 23 40 E6
14 25 40 E6
14 27 40 E6
14 29 40 E6
00 1E
00 00 42 70
21 08 02 0D 1E 1E
00 00 3F 00
85 1E 41 03
FF FF

DataType and Analysis:
	(INT_CDAB) 00 01    = 1
	(FLOAT_ABCD) 32 33 41 03   = 8.200

*/
int protocol_ZONGLIN_JSZhuoZheng_HeBei_info(struct acquisition_data *acq_data)
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
	
	float ZONGLIN_JSZhuoZheng_HeBei=0;

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
	dataType = FLOAT_ABCD;
	SYSLOG_DBG("protocol ZONGLIN JSZhuoZheng_HeBei info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,0x35,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol ZONGLIN JSZhuoZheng_HeBei info SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol ZONGLIN JSZhuoZheng_HeBei, Info : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol ZONGLIN JSZhuoZheng_HeBei Info data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol ZONGLIN JSZhuoZheng_HeBei info RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = getInt16Value(com_rbuf,3, INT_AB);
		#if 0
			acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		#endif
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		
		val = getInt16Value(com_rbuf,5, INT_AB);
		#if 0
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		#endif
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
		}
		
		val = getInt16Value(com_rbuf,7, INT_AB);
		#if 0
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		#endif
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
			case 10: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		status=0;
	}
	else {
		status = 1;
	}

	cmd = 0x03;
	dataType = FLOAT_ABCD;
	t1=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x01,0x34);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol ZONGLIN JSZhuoZheng_HeBei info SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol ZONGLIN JSZhuoZheng_HeBei, Info1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol ZONGLIN JSZhuoZheng_HeBei Info1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol ZONGLIN JSZhuoZheng_HeBei info RECV1:",com_rbuf,size);
	if((size>=109)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{

		timer.tm_year=BCD(com_rbuf[3])+100;
		timer.tm_mon=BCD(com_rbuf[4])-1;
		timer.tm_mday=BCD(com_rbuf[5]);
		timer.tm_hour=BCD(com_rbuf[6]);
		timer.tm_min=BCD(com_rbuf[7]);
		timer.tm_sec=BCD(com_rbuf[8]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		/*
		year=(com_rbuf[3]);
		mon=(com_rbuf[4]);
		day=(com_rbuf[5]);
		hour=(com_rbuf[6]);
		min=(com_rbuf[7]);
		sec=(com_rbuf[8]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		*/
		data.i13101=t3;

		valf= getFloatValue(com_rbuf,9, dataType);
		data.i13007 = valf;
		
		valf= getFloatValue(com_rbuf,13, dataType);
		data.i13103 = valf;

		valf= getFloatValue(com_rbuf,17, dataType);
		data.i13104 = valf;

		valf= getFloatValue(com_rbuf,21, dataType);
		data.i13105 = valf;

		valf= getFloatValue(com_rbuf,25, dataType);
		data.i13106 = valf;

		timer.tm_year=BCD(com_rbuf[29])+100;
		timer.tm_mon=BCD(com_rbuf[30])-1;
		timer.tm_mday=BCD(com_rbuf[31]);
		timer.tm_hour=BCD(com_rbuf[32]);
		timer.tm_min=BCD(com_rbuf[33]);
		timer.tm_sec=BCD(com_rbuf[34]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		data.i13107=t3;


		valf= getFloatValue(com_rbuf,35, dataType);
		data.i13108 = valf;

		valf= getFloatValue(com_rbuf,39, dataType);
		data.i13109 = valf;

		valf= getFloatValue(com_rbuf,43, dataType);
		data.i13110 = valf;

		valf= getFloatValue(com_rbuf,47, dataType);
		data.i13111 = valf;

		valf= getFloatValue(com_rbuf,51, dataType);
		data.i13112 = valf;

		valf= getFloatValue(com_rbuf,55, dataType);
		data.i13113 = valf;

		valf= getFloatValue(com_rbuf,59, dataType);
		data.i13003 = valf;

		valf= getFloatValue(com_rbuf,67, dataType);
		data.i13116 = valf;

		valf= getFloatValue(com_rbuf,71, dataType);
		data.i13008 = valf;

		valf= getFloatValue(com_rbuf,75, dataType);
		data.i13118 = valf;

		valf= getFloatValue(com_rbuf,79, dataType);
		data.i13119 = valf;

		valf= getFloatValue(com_rbuf,83, dataType);
		data.i13120 = valf;

		val = getUInt16Value(com_rbuf, 87, INT_AB);
		data.i13005 = val;

		valf= getFloatValue(com_rbuf,89, dataType);
		data.i13004 = valf;

		timer.tm_year=BCD(com_rbuf[93])+100;
		timer.tm_mon=BCD(com_rbuf[94])-1;
		timer.tm_mday=BCD(com_rbuf[95]);
		timer.tm_hour=BCD(com_rbuf[96]);
		timer.tm_min=BCD(com_rbuf[97]);
		timer.tm_sec=BCD(com_rbuf[98]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		data.i13128=t3;

		valf= getFloatValue(com_rbuf,99, dataType);
		data.i13129 = valf;
		
		valf= getFloatValue(com_rbuf,103, dataType);
		data.i13130 = valf;

		status=0;
	}
	
	acqdata_set_value_flag(acq_data,"i13101",data.i13101,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13007",UNIT_L,data.i13007,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13103",UNIT_L,data.i13103,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13104",UNIT_L,data.i13104,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,data.i13105,INFOR_ARGUMENTS,&arg_n);	
	acqdata_set_value_flag(acq_data,"i13106",UNIT_PECENT,data.i13106,INFOR_ARGUMENTS,&arg_n);	
	acqdata_set_value_flag(acq_data,"i13107",data.i13107,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13108",UNIT_L,data.i13108,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13109",UNIT_L,data.i13109,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,data.i13110,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,data.i13111,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13112",UNIT_PECENT,data.i13112,INFOR_ARGUMENTS,&arg_n);	
	acqdata_set_value_flag(acq_data,"i13113",UNIT_PECENT,data.i13113,INFOR_ARGUMENTS,&arg_n);	
        acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,data.i13003,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13116",UNIT_L,data.i13116,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13008",UNIT_L,data.i13008,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13118",UNIT_L,data.i13118,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,data.i13119,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13120",UNIT_L,data.i13120,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,data.i13005,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,data.i13004,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13128",data.i13128,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,data.i13129,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,data.i13130,INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

