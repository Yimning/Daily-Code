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
Description:protocol_CEMS_BJXueDiLong_SCS900CPM
TX1:01 03 00 1A 00 02 E5 CC 
RX1:01 03 04 
3C  4B  5D 5D  
7F 1C

TX2:01 03 00 0D 00 02 C4 0B 
RX2:01 03 04 
3C  4B  5D 5D  
7F 1C

DataType and Analysis:
	(FLOAT_ABCD) 3C 4B 5D 5D  = 0.0124
*/

static char BJXueDiLong_SCS900CPM_Mark = 'N';

int protocol_CEMS_BJXueDiLong_SCS900CPM(struct acquisition_data *acq_data)
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
	float smoke=0,orig=0;
	char *p;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900CPM CEMS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900CPM CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900CPM CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900CPM CEMS RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x02);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(p!=NULL))
	{
		valf =getFloatValue(p , 3, dataType);
		smoke=valf;
		status=0;
	}
	else  
	{
		smoke=0;
		status=1;
	}

        sleep(1);
	cmd = 0x03;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0C,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900CPM CEMS SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900CPM: read device2 %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900CPM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900CPM RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x02);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(p!=NULL))
	{
		valf =getFloatValue(p , 3, dataType);
		orig=valf;
		status = 0;
	}
	else 
	{
		orig=0;
		status = 1;
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,smoke,orig,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);

	if(status == 0){
		acq_data->acq_status = ACQ_OK;
		acq_data->dev_stat= BJXueDiLong_SCS900CPM_Mark;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

/*

RX1:01 03 00 0E 00 01 FD CA 
TX1:01 03 02 00 01 79 84

RX2:01 03 00 02 00 0A AC 0A 
TX2:01 03 14 3C 4B 5D 5D 3C 4B 5D 5D 3C 4B 5D 5D 3C 4B 5D 5D 3C 4B 5D 5D 70 59

DataType and Analysis:
	(INT_AB) 01 = 1
*/

int protocol_CEMS_BJXueDiLong_SCS900CPM_info(struct acquisition_data *acq_data)
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
	char *p ;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_scs900CPM_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0E,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900CPM INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900CPM protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900CPM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900CPM INFO RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x01);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(p!=NULL))
	{
	        val = getInt16Value(p, 3,  INT_AB);
		switch(val){
        		case 1:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);BJXueDiLong_SCS900CPM_Mark='N';
				break;
        		case 16:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);BJXueDiLong_SCS900CPM_Mark='z';
				break;
        		case 32:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,6,INFOR_STATUS,&arg_n);BJXueDiLong_SCS900CPM_Mark='C';
				break;
        		case 64:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);BJXueDiLong_SCS900CPM_Mark='M';
				break;
        		case 128:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);BJXueDiLong_SCS900CPM_Mark='D';
				break;
        		default:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);BJXueDiLong_SCS900CPM_Mark='N';
				break;
		}
		status=0;
	}


	sleep(1);
	cmd = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x02,0x0A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900CPM INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900CPM protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900CPM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900CPM INFO RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x0A);
	if((size>=25)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(p!=NULL))
	{
		valf= getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

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
