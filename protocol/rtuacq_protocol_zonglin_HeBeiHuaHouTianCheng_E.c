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
Time:2021.12.09 Thur.
Description:protocol_ZONGLIN_HeBeiHuaHouTianCheng_E
TX:01 03 10 10 00 02 FF FF 
RX:01 03 04 00 00 3F C8 FF FF
DataType and Analysis:
	(FLOAT_CDAB) 00 00 3F C8  = 1.5625
*/

int protocol_ZONGLIN_HeBeiHuaHouTianCheng_E(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float tp=0;
	union uf f;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
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
	regpos = 0x1010;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HuaHouTianCheng HeBei ZONGLIN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaHouTianCheng HeBei ZONGLIN protocol,ZONGLIN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaHouTianCheng HeBei ZONGLIN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HuaHouTianCheng HeBei ZONGLIN RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		tp = getFloatValue(p, 3, dataType);
		status = 0;
	}
	else
	{
		status = 1;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x100A;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HuaHouTianCheng HeBei ZONGLIN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaHouTianCheng HeBei ZONGLIN protocol,ZONGLIN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaHouTianCheng HeBei ZONGLIN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HuaHouTianCheng HeBei ZONGLIN RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		switch(val){
			case 0: acq_data->dev_stat = 'N';break;
			case 1: acq_data->dev_stat = 'C';break;
			default: acq_data->dev_stat = 'N';break;
		}
		status = 0;
	}
	else
	{
		status = 1;
	}

	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,tp,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}




int protocol_ZONGLIN_HeBeiHuaHouTianCheng_E_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	union uf f;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;

	struct tm timer;
	time_t t1,t2,t3;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_HeBeiHuaHouTianCheng_E_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);	


	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x10D0;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HuaHouTianCheng HeBei ZONGLIN STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaHouTianCheng HeBei ZONGLIN protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaHouTianCheng HeBei STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HuaHouTianCheng HeBei ZONGLIN STATUS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val = getInt16Value(p, 7, INT_AB);
		if(val == 0){
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}else{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			
			val =  getInt16Value(p, 9, INT_AB);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		}
		status=0;
	}
	else 
		status = 1;


	sleep(1);
	cmd = 0x03;
	regpos = 0x110E;
	regcnt = 0x56;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd ,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HuaHouTianCheng HeBei ZONGLIN INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaHouTianCheng HeBei ZONGLIN protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaHouTianCheng HeBei INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HuaHouTianCheng HeBei ZONGLIN INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		timer.tm_year=BCD(p[4])+100;
		timer.tm_mon=BCD(p[3])-1;
		timer.tm_mday=BCD(p[6]);
		timer.tm_hour=BCD(p[5]);
		timer.tm_min=BCD(p[8]);
		timer.tm_sec=BCD(p[7]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13103",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 31, dataType);
		acqdata_set_value_flag(acq_data,"i13106",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(p[36])+100;
		timer.tm_mon=BCD(p[35])-1;
		timer.tm_mday=BCD(p[38]);
		timer.tm_hour=BCD(p[37]);
		timer.tm_min=BCD(p[40]);
		timer.tm_sec=BCD(p[39]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13112",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13113",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(p, 63, dataType);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 79, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 83, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 83, dataType);
		acqdata_set_value_flag(acq_data,"i13118",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 91, dataType);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 95, dataType);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 99, dataType);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 103, dataType);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,valf,INFOR_ARGUMENTS,&arg_n);
		
		timer.tm_year=BCD(p[156])+100;
		timer.tm_mon=BCD(p[158])-1;
		timer.tm_mday=BCD(p[160]);
		timer.tm_hour=BCD(p[162]);
		timer.tm_min=BCD(p[164]);
		timer.tm_sec=BCD(p[166]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13128",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 167, dataType);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 171, dataType);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		status = 0;
	}
	else
	{
		status = 1;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

