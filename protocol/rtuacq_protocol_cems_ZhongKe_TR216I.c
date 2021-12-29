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
Description:protocol_CEMS_ZhongKe_TR216I

TX:01 03 00 55 00 02 D4 1B 
RX:01 03 04 00 00 48 42 4C 02

DataType and Analysis:
	(FLOAT_DCBA)   5D 5D 4B  3C  = 0.0124
*/

int protocol_CEMS_ZhongKe_TR216I(struct acquisition_data *acq_data)
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
	dataType = FLOAT_DCBA;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x55,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongKe TR216I CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongKe TR216I  CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongKe TR216I  CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongKe TR216I  CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x02);
	if((p!=NULL))
	{
		valf = getFloatValue(p, 3, dataType);
		smoke=valf;
	}
	else
	{
		smoke=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,smoke,smoke,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

/*
RX:01 03 00 64 00 03 44 14
TX:01 03 14 
5D 5D 4B 3C
5D 5D 4B 3C
5D 5D 4B 3C
5D 5D 4B 3C
5D 5D 4B 3C 
B3 50

DataType and Analysis:
	(INT_AB) 01 = 1
*/

int protocol_CEMS_ZhongKe_TR216I_info(struct acquisition_data *acq_data)
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
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_DCBA;
	devaddr=modbusarg->devaddr&0xffff;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x02,0x0A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongKe TR216I  INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongKe TR216I  protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongKe TR216I  data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongKe TR216I  INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x0A);
	if((p!=NULL))
	{
		valf= getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
		status=0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;

}
