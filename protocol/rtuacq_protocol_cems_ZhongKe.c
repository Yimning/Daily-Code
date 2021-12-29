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
Create Time:2021.08.20 Fri.
Description:protocol_CEMS_ZhongKe

TX:01 03 00 00 00 04 44 09
RX:01 03 08 
5D 5D  3C  4B 
5D 5D  3C  4B
AF CC

DataType and Analysis:
	(FLOAT_CDAB) 5D 5D  3C  4B   = 0.0124
*/

int protocol_CEMS_ZhongKe(struct acquisition_data *acq_data)
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
	int cmd=0;
	int val=0;
	float valf = 0;
	float smoke=0,smoke_orig=0;
	char *p;

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
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongKe CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongKe CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongKe CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongKe CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x04);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(p!=NULL))
	{
		valf = getFloatValue(p, 3, dataType);
		smoke=valf;

		valf = getFloatValue(p, 7, dataType);
		smoke_orig = valf;
		status=0;
	}
	else
	{
		smoke=0;
		smoke_orig = 0;
		status=1;
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,smoke,smoke_orig,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

/*
RX1:01 03 00 64 00 03 44 14
TX1:01 03 06 00 01 00 00 00 00 1C B5


RX2:01 03 00 C8 00 2A 45 EB
TX2:01 03 54 
00 00 42 C8 
20 20 10 16 08 00 00 00 
00 00 00 00 
00 00 42 C8 
00 00 3F 80 
00 00 00 00 
20 20 10 16 09 00 00 00 
00 00 42 C8 
00 00 43 48 
00 00 3F 10
00 00 00 11  
00 00 3F 80 
3F 80 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
F5 20

DataType and Analysis:
	(INT_AB) 01 = 1
*/

int protocol_CEMS_ZhongKe_info(struct acquisition_data *acq_data)
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
	int cmd=0;
	int val=0;
	float valf = 0;
	char *p;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ZhongKe_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x64,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongKe INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongKe protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongKe data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongKe INFO RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x03);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(p!=NULL))
	{
	        val = getInt16Value(com_rbuf, 3, dataType);
		switch(val){
        		case 0:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
        		case 1:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				break;
        		case 3:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				break;
        		case 5:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				break;
			default:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
		}
		status=0;
	}

	sleep(1);
	cmd = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xC8,0x2A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongKe INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongKe protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongKe data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongKe INFO RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x2A);
	if((size>=89)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(p!=NULL))
	{
		valf = getFloatValue(com_rbuf, 3, dataType);
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

		valf= getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

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
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf= getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13015",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	//NEED_ERROR_CACHE(acq_data, 10);  //del by Yimning 20210824
	return arg_n;

}
