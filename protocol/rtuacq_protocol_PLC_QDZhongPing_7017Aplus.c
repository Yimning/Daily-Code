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
TX:01 04 00 00 00 08 F1 CC
RX:01 04 10
00 00
00 00
00 00
00 00
00 00
00 00
00 00
00 00
55 2C


DataType and Analysis:
	(INT_AB) 00 00 =  0
*/

static char QDZhongPing_7017Aplus_flag ='N';

int protocol_PLC_QDZhongPing_7017Aplus(struct acquisition_data *acq_data)
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
	float speed=0,temp=0,press=0,humi=0,tsp=0;
	
	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x04;
	regpos = 0x00;
	regcnt = 0x08;
	dataType = INT_AB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "QDZhongPing_7017Aplus", 1);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		temp = getInt16Value(p, 3, dataType);
		press = getInt16Value(p, 5, dataType);
		speed = getInt16Value(p, 7, dataType);
		tsp = getInt16Value(p, 9, dataType);
		humi = getInt16Value(p, 11, dataType);
		acq_data ->dev_stat = QDZhongPing_7017Aplus_flag;
		status=0;
	}
	else
	{
		temp = 0;
		press = 0;
		humi = 0;
		speed= 0;
		tsp= 0;
		status=1;
	}


	acqdata_set_value(acq_data,"a01012",UNIT_0C,temp,&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,press,&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,humi,&arg_n);
	
	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,tsp,&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,tsp,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

#if 1
int protocol_PLC_QDZhongPing_7017Aplus_PLC(struct acquisition_data *acq_data)
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
	float speed=0,temp=0,press=0,humi=0,tsp=0;
	
	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x02;
	regcnt = 0x03;
	dataType = INT_AB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "QDZhongPing_7017Aplus PLC", 1);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0:	QDZhongPing_7017Aplus_flag = 'C'; break;
			case 1:	QDZhongPing_7017Aplus_flag = 'N'; break;
			case 2:	QDZhongPing_7017Aplus_flag = 'z'; break;
			case 3:	QDZhongPing_7017Aplus_flag = 'D'; break;
			default:	QDZhongPing_7017Aplus_flag = 'N'; break;
		}
		status=0;
	}
	else
	{
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

#endif 
