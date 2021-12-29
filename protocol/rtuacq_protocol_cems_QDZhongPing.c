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
Description:protocol_CEMS_QDZhongPing
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


int protocol_CEMS_QDZhongPing(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 4
	char *polcode[POLCODE_NUM]={
		"a21026a","a21003a","a21002a","a19001a"
	};
	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,  UNIT_PECENT
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
	regpos = 0x00;
	regcnt = 0x08;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		for(i = 0; i < POLCODE_NUM; i++)
		{
			valfArray[i]  = getFloatValue(p, i*4+3, dataType);

			acqdata_set_value(acq_data,polcode[i],unit[i],valfArray[i],&arg_n);

		}
		//nox=NO_to_NOx(NO);
		
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
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,valfArray[0],&arg_n); 
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,valfArray[1],&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,valfArray[2],&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a19001",UNIT_MG_M3,valfArray[3],&arg_n);

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


int protocol_CEMS_QDZhongPing_SO2_info(struct acquisition_data *acq_data)
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
	char pt[1024] = {0};
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
	SYSLOG_DBG("protocol_CEMS_QDZhongPing_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	regpos = 0x0F;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing SO2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing SO2 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing SO2 INFO RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
	       val = getInt16Value(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x27;
	regcnt = 0x0D;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing SO2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing SO2 protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing SO2 INFO RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		memset(pt,0,sizeof(pt));
		modbus_ascii_pack_to_hex_pack(&p[14],16,pt); //32 30 31 39 30 39 30 37 31 32 33 34 35 36(ascii) //2019-09-07-12-34-56
		SYSLOG_DBG("%02x %02x %02d %02x %02d %02x %02x\n",pt[0],pt[1],BCD(pt[2]),pt[3],BCD(pt[4]),pt[5],pt[6]);
		val = getInt16Value(pt, 0, INT_AB);
		t1 = getTimeValue(BCD(val),  BCD(pt[2]), BCD(pt[3]), BCD(pt[4]),  BCD(pt[5]), BCD(pt[6]));
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x50;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing SO2 INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing SO2 protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing SO2 INFO RECV3:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	
	sleep(1);
	cmd = 0x03;
	regpos = 0x5C;
	regcnt = 0x06;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing SO2 INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing SO2 protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing SO2 INFO RECV4:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;

}



int protocol_CEMS_QDZhongPing_NO_info(struct acquisition_data *acq_data)
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
	char pt[1024] = {0};
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
	SYSLOG_DBG("protocol_CEMS_QDZhongPing_NO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	regpos = 0x0F;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing NO INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing NO protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing NO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing NO INFO RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
	       val = getInt16Value(p, 5, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x34;
	regcnt = 0x0D;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing NO INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing NO protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing NO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing NO INFO RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		memset(pt,0,sizeof(pt));
		modbus_ascii_pack_to_hex_pack(&p[14],16,pt);
		val = getInt16Value(pt, 0, INT_AB);
		t1 = getTimeValue(BCD(val),  BCD(pt[2]), BCD(pt[3]), BCD(pt[4]),  BCD(pt[5]), BCD(pt[6]));
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x54;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing NO INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing NO protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing NO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing NO INFO RECV3:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	
	sleep(1);
	cmd = 0x03;
	regpos = 0x62;
	regcnt = 0x06;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing NO INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing NO protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing NO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing NO INFO RECV4:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;

}



int protocol_CEMS_QDZhongPing_O2_info(struct acquisition_data *acq_data)
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
	char pt[1024] = {0};
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
	SYSLOG_DBG("protocol_CEMS_QDZhongPing_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	regpos = 0x0F;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing O2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing O2 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing O2 INFO RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
	       val = getInt16Value(p, 9, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x41;
	regcnt = 0x0D;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing O2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing O2 protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing O2 INFO RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		memset(pt,0,sizeof(pt));
		modbus_ascii_pack_to_hex_pack(&p[14],16,pt);
		val = getInt16Value(pt, 0, INT_AB);
		t1 = getTimeValue(BCD(val),  BCD(pt[2]), BCD(pt[3]), BCD(pt[4]),  BCD(pt[5]), BCD(pt[6]));
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x58;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing O2 INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing O2 protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing O2 INFO RECV3:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	
	sleep(1);
	cmd = 0x03;
	regpos = 0x68;
	regcnt = 0x06;
	dataType = FLOAT_CDAB ;
	status = 1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDZhongPing O2 INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDZhongPing O2 protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDZhongPing O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDZhongPing O2 INFO RECV4:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;

}

