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
Create Time:2021.08.16 Mon.
Description:protocol_CEMS_LvShi
TX:01 03 00 00 00 14 C5 C8
RX:01 03 18
41 20 00 00
42 48 00 00
41 A0 00 00
42 48 00 00
41 80 00 00
42 48 00 00
41 A0 00 00
42 48 00 00
41 80 00 00
42 48 00 00
FF FF

RX1:01 03 00 64 00 03 44 14
TX1:01 03 06 00 00 00 00 00 00 21 75

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/

int protocol_CEMS_LvShi(struct acquisition_data *acq_data)
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
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;

	char flag =0;
	float SO2 = 0,NO = 0,O2 = 0,orig = 0,nox = 0,CO = 0,CO2 = 0;
	float SO2_orig = 0,NO_orig = 0,O2_orig = 0,nox_orig = 0,CO_orig = 0,CO2_orig = 0;

	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x0A;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LvShi CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LvShi CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LvShi CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LvShi CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		SO2 = getFloatValue(p, 3, dataType);
		
		NO= getFloatValue(p, 11, dataType);
		
		O2 = getFloatValue(p, 19, dataType);

		nox=NO_to_NOx(NO);

		status=0;
	}
	else
	{
		SO2 = 0;
		NO = 0;
		O2 = 0;
		nox =0;
		status=1;
	}
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,SO2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,SO2,SO2_orig,&arg_n); 
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,NO,&arg_n);
	acqdata_set_value_orig(acq_data,"a21003",UNIT_MG_M3,NO,NO_orig,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a19001a",UNIT_MG_M3,O2,&arg_n);
	acqdata_set_value_orig(acq_data,"a19001",UNIT_MG_M3,O2,O2_orig,&arg_n);
	
	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,CO,&arg_n);
	acqdata_set_value_orig(acq_data,"a21005",UNIT_MG_M3,CO,CO_orig,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a05001a",UNIT_MG_M3,CO2,&arg_n);
	acqdata_set_value_orig(acq_data,"a05001",UNIT_MG_M3,CO2,CO2_orig,&arg_n);
	acqdata_set_value(acq_data,"a05001z",UNIT_MG_M3,0,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x64,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LvShi CEMS Flag SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LvShi CEMS Flag protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LvShi CEMS Flag data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LvShi CEMS Flag RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x03);
	if(p!=NULL)
	{
	        val= getInt16Value(p, 3, INT_AB);
		switch(val){
        		case 0:
				flag = 'N';break;
        		case 1:
				flag = 'M';break;
        		case 2:
				flag = 'D';break;
			case 3:
				flag = 'C';break;
			case 4:
				flag = 'z';break;
			case 5:
				flag = 'C';break;
			default:
				flag = 'N';break;
		}
		//status = 0;
	}

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


int protocol_CEMS_LvShi_STATUS_info(struct acquisition_data *acq_data)
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
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;

	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_LvShi_STATUS_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00

	if(isPolcodeEnable(modbusarg, "a21026"))
		acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21003"))
		acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a19001"))
		acqdata_set_value_flag(acq_data,"a19001",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;


	sleep(1);
	cmd = 0x03;
	regpos = 0x64;
	regcnt = 0x03;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LvShi STATUS INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LvShi STATUS protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LvShi STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LvShi STATUS INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
	        val= getInt16Value(p, 3, INT_AB);
		switch(val){
        		case 0:
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
        		case 1:
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				break;
        		case 2:
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				break;
			case 3:
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				break;
			case 4:
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				break;
			case 5:
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
				break;
			default:
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
		}
		
	        val= getInt16Value(p, 5, INT_AB);
		switch(val){
        		case 0:
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
        		case 1:
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				break;
		}
		
	        val= getInt16Value(p, 7, INT_AB);
		switch(val){
        		case 0:
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
        		case 1:
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				break;
        		case 2:
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				break;	
        		case 3:
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n);
				break;	
        		case 4:
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,8,INFOR_STATUS,&arg_n);
				break;	
        		default:
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;	
		 }
		status=0;
	}
	else
	{
		status = 1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;

}




/*
RX1:01 03 00 64 00 03 44 14
TX1:01 03 06 00 00 00 00 00 00 21 75


RX2:01 03 00 C8 00 1A 45 FF
TX2:01 03 34 43 48 00 00 20 20 10 16 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 20 20 10 16 09 00 00 00 43 48 00 00 43 48 00 00 3F 80 00 00 00 00 00 00 B4 0E

DataType and Analysis:
	(INT_AB) 01 = 1
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/

int protocol_CEMS_LvShi_SO2_info(struct acquisition_data *acq_data)
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
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_LvShi_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;
	
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	regpos = 0xC8;
	regcnt = 0x1A;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LvShi SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LvShi SO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LvShi SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LvShi SO2 INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 7, INT_AB);
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[9])-1;
		timer.tm_mday=BCD(p[10]);
		timer.tm_hour=BCD(p[11]);
		timer.tm_min=BCD(p[12]);
		timer.tm_sec=BCD(p[14]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 31, INT_AB);
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[33])-1;
		timer.tm_mday=BCD(p[34]);
		timer.tm_hour=BCD(p[35]);
		timer.tm_min=BCD(p[36]);
		timer.tm_sec=BCD(p[38]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);	

		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		//valf= getFloatValue(p, 55, dataType);
		//acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
	{
		status =1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;

}


/*

RX:01 03 00 E2 00 1A 04 34
TX:01 03 34 43 48 00 00 20 20 10 16 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 20 20 10 16 09 00 00 00 43 48 00 00 43 48 00 00 3F 80 00 00 00 00 00 00 B4 0E
DataType and Analysis:
	(INT_AB) 01 = 1
	(FLOAT_ABCD) 42 c8 00 00  = 100
*/


int protocol_CEMS_LvShi_NO_info(struct acquisition_data *acq_data)
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
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_LvShi_NO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;

	if(!isPolcodeEnable(modbusarg, "a21003"))
		return 0;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	regpos = 0xE2;
	regcnt = 0x1A;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LvShi NO INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LvShi NO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LvShi NO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LvShi NO INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 7, INT_AB);
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[9])-1;
		timer.tm_mday=BCD(p[10]);
		timer.tm_hour=BCD(p[11]);
		timer.tm_min=BCD(p[12]);
		timer.tm_sec=BCD(p[14]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 31, INT_AB);
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[33])-1;
		timer.tm_mday=BCD(p[34]);
		timer.tm_hour=BCD(p[35]);
		timer.tm_min=BCD(p[36]);
		timer.tm_sec=BCD(p[38]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);	

		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		//valf= getFloatValue(p, 55, dataType);
		//acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
	{
		status =1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;

}



/*
RX:01 03 00 FC 00 1A 45 FF
TX:01 03 34 43 48 00 00 20 20 10 16 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 20 20 10 16 09 00 00 00 43 48 00 00 43 48 00 00 3F 80 00 00 00 00 00 00 B4 0E
DataType and Analysis:
	(INT_AB) 01 = 1
	(FLOAT_ABCD) 42 c8 00 00  = 100
*/
int protocol_CEMS_LvShi_O2_info(struct acquisition_data *acq_data)
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
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;

	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_LvShi_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;

	if(!isPolcodeEnable(modbusarg, "a19001"))
		return 0;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	regpos = 0xFC;
	regcnt = 0x1A;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LvShi O2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LvShi O2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LvShi O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LvShi O2 INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 7, INT_AB);
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[9])-1;
		timer.tm_mday=BCD(p[10]);
		timer.tm_hour=BCD(p[11]);
		timer.tm_min=BCD(p[12]);
		timer.tm_sec=BCD(p[14]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 31, INT_AB);
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[33])-1;
		timer.tm_mday=BCD(p[34]);
		timer.tm_hour=BCD(p[35]);
		timer.tm_min=BCD(p[36]);
		timer.tm_sec=BCD(p[38]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);	

		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		//valf= getFloatValue(p, 55, dataType);
		//acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
	{
		status =1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;

}
