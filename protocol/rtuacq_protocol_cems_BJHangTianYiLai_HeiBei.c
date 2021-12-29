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
Create Time:2021.11.25 	Fri.
Description:protocol_CEMS_BJHangTianYiLai_HeiBei
TX:01 03 00 00 00 16 C5 C8
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
42 48 00 00
FF FF

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/
static char BJHangTianYiLai_HeiBei_Flag = 'N';
int protocol_CEMS_BJHangTianYiLai_HeiBei(struct acquisition_data *acq_data)
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
	float SO2 = 0,NO = 0,O2 = 0,orig = 0,nox = 0,CO = 0,CO2 = 0,NO2 = 0 ,hum = 0 ,HCL = 0;
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
	regcnt = 0x16;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai_HeiBei CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai_HeiBei CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai_HeiBei CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai_HeiBei CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		SO2 = getFloatValue(p, 3, dataType);

		nox = getFloatValue(p, 7, dataType);

		CO = getFloatValue(p, 11, dataType);

		HCL = getFloatValue(p, 19, dataType);

		NO = getFloatValue(p, 35, dataType);

		NO2 = getFloatValue(p, 39, dataType);

		hum = getFloatValue(p, 31, dataType);
		
		status=0;
	}
	else
	{
		SO2 = 0;
		NO = 0;
		NO2 = 0;
		nox =0;
		hum = 0;
		status=1;
	}
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,SO2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,SO2,SO2_orig,&arg_n); 
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,NO,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,NO,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,NO2,&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,NO2,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,CO,&arg_n);
	acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,CO,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21024a",UNIT_MG_M3,HCL,&arg_n);
	acqdata_set_value(acq_data,"a21024",UNIT_MG_M3,HCL,&arg_n);
	acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,hum,&arg_n);

	
	if(status == 0)
	{
		acq_data->dev_stat = BJHangTianYiLai_HeiBei_Flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



int protocol_CEMS_BJHangTianYiLai_HeiBei_STATUS_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year,mon,day,hour,min,sec;
	int index[3];
	unsigned int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_HeiBei_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(isPolcodeEnable(modbusarg, "a21026"))
		acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21003"))
		acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21004"))
		acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	status=1;
	cmd=0x01;
	regpos = 0x00;
	regcnt = 0x05;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai  STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai  protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLa data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai STATUS RECV:",com_rbuf,size);
	//p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	//if(p != NULL)
	if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		//val=p[3];
		val=com_rbuf[3];
	
		if(val == 0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else if((val&0x01)==0x01)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else if((val&0x02)==0x02)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else if((val&0x04)==0x04)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n);
		}
		else if((val&0x08)==0x08)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else if((val&0x10)==0x10)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		
		status=0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}




int protocol_CEMS_BJHangTianYiLai_HeiBei_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year,mon,day,hour,min,sec;
	int index[3];
	unsigned int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_HeiBei_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;
	
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x36;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai SO2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai  protocol,SO2 INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai SO2 INFO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai SO2 INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
		
	}


	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x6A;
	regcnt = 0x12;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai SO2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,SO2 INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai SO2 INFO data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai SO2 INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
		
	}

	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x84;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai SO2 INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,SO2 INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai SO2 INFO data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai SO2 INFO RECV3:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
		
	}

	sleep(1);
	cmd=0x03;
	regpos = 0xB8;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai SO2 INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,SO2 INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai SO2 INFO data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai SO2 INFO RECV4:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	status = 1;
	cmd=0x03;
	regpos = 0xE8;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai SO2 INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,SO2 INFO 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai SO2 INFO data5",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai SO2 INFO RECV5:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}





int protocol_CEMS_BJHangTianYiLai_HeiBei_NO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year,mon,day,hour,min,sec;
	int index[3];
	unsigned int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_HeiBei_NO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21004"))
		return 0;
	
	acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;
	cmd=0x03;
	regpos = 0x36;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai  protocol,NO2 INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO2 INFO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO2 INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 71, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
		
	}


	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x6A;
	regcnt = 0x12;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,NO2 INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO2 INFO data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO2 INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	

	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x84;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO2 INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,NO2 INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO2 INFO data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO2 INFO RECV3:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 121, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
		
	}

	sleep(1);
	cmd=0x03;
	regpos = 0x118;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO2 INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,NO2 INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO2 INFO data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO2 INFO RECV4:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	status = 1;
	cmd=0x03;
	regpos = 0x11D;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO2 INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,NO2 INFO 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO2 INFO data5",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO2 INFO RECV5:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



int protocol_CEMS_BJHangTianYiLai_HeiBei_NO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year,mon,day,hour,min,sec;
	int index[3];
	unsigned int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_HeiBei_NO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21003"))
		return 0;
	
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;
	cmd=0x03;
	regpos = 0x36;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai  protocol,NO INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO INFO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
		
	}


	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x6A;
	regcnt = 0x12;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,NO INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO INFO data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	

	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x84;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,NO INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO INFO data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO INFO RECV3:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
		
	}

	sleep(1);
	cmd=0x03;
	regpos = 0xBE;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,NO INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO INFO data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO INFO RECV4:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	status = 1;
	cmd=0x03;
	regpos = 0xEE;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai NO INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,NO INFO 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai NO INFO data5",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai NO INFO RECV5:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_BJHangTianYiLai_HeiBei_CO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year,mon,day,hour,min,sec;
	int index[3];
	unsigned int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_HeiBei_CO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21005"))
		return 0;
	
	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;
	cmd=0x03;
	regpos = 0x36;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai CO INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai  protocol,CO INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai CO INFO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai CO INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
		
	}


	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x6A;
	regcnt = 0x12;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai CO INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,CO INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai CO INFO data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai CO INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	

	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x84;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai CO INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,CO INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai CO INFO data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai CO INFO RECV3:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
		
	}

	sleep(1);
	cmd=0x03;
	regpos = 0xC4;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai CO INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,CO INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai CO INFO data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai CO INFO RECV4:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	status = 1;
	cmd=0x03;
	regpos = 0xF4;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai CO INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,CO INFO 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai CO INFO data5",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai CO INFO RECV5:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



int protocol_CEMS_BJHangTianYiLai_HeiBei_HCL_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year,mon,day,hour,min,sec;
	int index[3];
	unsigned int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJHangTianYiLai_HeiBei_HCL_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21024"))
		return 0;
	
	acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;
	cmd=0x03;
	regpos = 0x36;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai HCL INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai  protocol,HCL INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai HCL INFO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai HCL INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
		
	}


	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x6A;
	regcnt = 0x12;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai HCL INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,HCL INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai HCL INFO data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai HCL INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	

	sleep(1);
	status=1;
	cmd=0x03;
	regpos = 0x84;
	regcnt = 0x24;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai HCL INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,HCL INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai HCL INFO data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai HCL INFO RECV3:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
		
	}

	sleep(1);
	cmd=0x03;
	regpos = 0xD0;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai HCL INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,HCL INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai HCL INFO data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai HCL INFO RECV4:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	status = 1;
	cmd=0x03;
	regpos = 0x100;
	regcnt = 0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai HCL INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai protocol,HCL INFO 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai HCL INFO data5",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai HCL INFO RECV5:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		year = getInt16Value(p, 3, INT_AB);  
		mon = getInt16Value(p, 5, INT_AB); 
		day = getInt16Value(p, 7, INT_AB); 
		hour = getInt16Value(p, 9, INT_AB); 
		min = getInt16Value(p, 11, INT_AB); 
		sec = getInt16Value(p, 13, INT_AB); 
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



//01 03 0E 19 01 19 01 19 01 19 01 19 01 00 00 00 03 5E 16

int protocol_PLC_BJHangTianYiLai_HeiBei(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 5
	char *polcode[POLCODE_NUM]={
		"a34013a","a19001a","a01012","a01013","a01017"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,	UNIT_PECENT,	UNIT_0C,	UNIT_PA , UNIT_PA
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
	int val=0;
	float valf = 0;
	int devaddr;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	float speed = 0,PTC = 0;
	float valfArray[POLCODE_NUM]={0};

	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;
	
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x07;
	dataType = FLOAT_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJHangTianYiLai HeiBei PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJHangTianYiLai HeiBei PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJHangTianYiLai HeiBei PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJHangTianYiLai HeiBei PLC RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		for(i = 0; i < POLCODE_NUM; i++)
		{
			val= getInt16Value(p, i*2+3, INT_AB);
			
			valfArray[i] = PLCtoValue(modbusarg, 5530 , 27648, val, polcode[i]);
			
			acqdata_set_value(acq_data,polcode[i],unit[i],valfArray[i],&arg_n);
		}

		speed=PTC*sqrt(fabs(valfArray[4])*2/SAD);
		acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);

		val = getInt16Value(p,13 , INT_AB);		
		switch(val){
			case 3: BJHangTianYiLai_HeiBei_Flag= 'C'; break;
			case 4: BJHangTianYiLai_HeiBei_Flag = 'M'; break;
			case 7: BJHangTianYiLai_HeiBei_Flag = 'D'; break;
			case 8: BJHangTianYiLai_HeiBei_Flag = 'N'; break;
			default: BJHangTianYiLai_HeiBei_Flag = 'N'; break;
		}
		
		acq_data->dev_stat = BJHangTianYiLai_HeiBei_Flag;
		
		status=0;
	}
	else
	{
		for(i = 0; i < POLCODE_NUM; i++)
		{
			valfArray[i]  = 0;
			acqdata_set_value(acq_data,polcode[i],unit[i],valfArray[i],&arg_n);
		}
		status=1;
	}
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valfArray[0],&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,valfArray[1],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_H,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_H,0,&arg_n);


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

