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
Time:2021.08.17 Tues.
Description:protocol_Pb_SZLangShi_PhotoTek6000
TX:01 03 10 08 00 02 41 09
RX:01 03 04 EC AA 41 6F 9E FF
DataType and Analysis:
	(FLOAT_CDAB) EC AA 41 6F   = 14.9953
*/
static float Pb_org =0;
int protocol_Pb_SZLangShi_PhotoTek6000(struct acquisition_data *acq_data)
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
	char flag=0;
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
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1008,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZLangShi PhotoTek6000 Pb SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JSZhuoZheng_HeBei Pb protocol,Pb : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JSZhuoZheng_HeBei Pb data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JSZhuoZheng_HeBei Pb RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getFloatValue(com_rbuf, 3,dataType);
		status=0;
	}
	else
	{
		status=1;
	}
#if 0 
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x10A0,0x0C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZLangShi PhotoTek6000 Pb SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZLangShi PhotoTek6000  Pb protocol,Pb1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZLangShi PhotoTek6000  Pb data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZLangShi PhotoTek6000  Pb RECV1:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		Pb_org = getFloatValue(com_rbuf, 3, dataType);
		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		switch(val)
		{
			case 0x0000:	flag='N';break;
			case 0x004E:	flag='N';break;
			case 0x004D:	flag='M';break;
			case 0x004B:	flag='K';break;
			case 0x0054:	flag='T';break;
			case 0x0045:	flag='D';break;
			case 0x0046:	flag='V';break;
			//default :		flag='N';break;
		}
		status = 0;
	}
	else{
		status = 1;
		flag='V';
	}

	if(status == 0)
	{
		acq_data->dev_stat=flag;
		acq_data->acq_status = ACQ_OK;
	}
#endif
	ret=acqdata_set_value(acq_data,"w20120",UNIT_MG_L,valf,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


 
/*
Author:Yimning
Time:2021.08.17 Tues.
Description:protocol_Pb_SZLangShi_PhotoTek6000_info
TX1:01 03 10 83 00 04 B1 21
RX1:01 03 08 00 00 00 00 00 00 00 01 FF FF


TX2:01 03 10 89 00 04 23 91
RX2:01 03 08 00 30 00 00 00 00 00 01 FF FF

TX3:01 03 10 A0 00 16 C0 E6
RX3:01 03 2C
00 00 
00 30 00 48 
00 00 42 42 
00 00 42 42
00 00 3F C0
00 00 3F C0
17 01 01 00 00 00
00 00 3F C0
00 00 3F C0
00 00 00 00
00 00 00 00
FF FF

DataType and Analysis:
	(INT_AB) 00 01    = 1
	(FLOAT_CDAB) 32 33 41 03   = 8.200

*/
int protocol_Pb_SZLangShi_PhotoTek6000_info(struct acquisition_data *acq_data)
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
	struct hebei_dynamic_info_water data;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	int alarmer = 0;
	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	dataType = FLOAT_CDAB;
	SYSLOG_DBG("protocol Pb SZLangShi PhotoTek6000  info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w20120",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1083,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol Pb SZLangShi PhotoTek6000  info SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol Pb SZLangShi PhotoTek6000 , Info : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol Pb SZLangShi PhotoTek6000  Info data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol Pb SZLangShi PhotoTek6000  info RECV1:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = getInt16Value(com_rbuf,3, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 9: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 10: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 1000: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 1001: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val = getInt16Value(com_rbuf,7, INT_AB);
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

		if(val == 0){
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}else acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n); 
		
		status=0;
	}
	else {
		status = 1;
	}

	cmd = 0x03;
	dataType = FLOAT_CDAB;
	t1=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1089,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol Pb SZLangShi PhotoTek6000  info SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol Pb SZLangShi PhotoTek6000 , Info2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol Pb SZLangShi PhotoTek6000  Info2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol Pb SZLangShi PhotoTek6000  info RECV2:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = getInt16Value(com_rbuf, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}

	cmd = 0x03;
	dataType = FLOAT_CDAB;
	t1=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x10A0,0x16);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol Pb SZLangShi PhotoTek6000  info SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol Pb SZLangShi PhotoTek6000 , Info3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol Pb SZLangShi PhotoTek6000  Info3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol Pb SZLangShi PhotoTek6000  info RECV3:",com_rbuf,size);
	if((size>=49)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = getInt16Value(com_rbuf, 5, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(com_rbuf, 7, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(com_rbuf, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);		

		valf = getFloatValue(com_rbuf, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(com_rbuf, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[25])+100;
		timer.tm_mon=BCD(com_rbuf[26])-1;
		timer.tm_mday=BCD(com_rbuf[27]);
		timer.tm_hour=BCD(com_rbuf[28]);
		timer.tm_min=BCD(com_rbuf[29]);
		timer.tm_sec=BCD(com_rbuf[30]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(com_rbuf,31, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(com_rbuf,35, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);	

		status=0;
	}


	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

