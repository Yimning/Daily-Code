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
Create Time:2021.11.30 Tues.
Description:protocol_PLC_HongRuiDe
TX:02 03 00 00 00 08 44 3F
RX:02 03 06 10 42 48 7A E1 3F 9D 70 A43F 9D F3 B642 49 99 9A CRCH  CRCL

DataType and Analysis:
	(FLOAT_ABCD) 42 48 7A E1 =  50.12
*/

static char HongRuiDe_flag ='N';

int protocol_PLC_HongRuiDe(struct acquisition_data *acq_data)
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
	float speed=0,temp=0,press=0,humi=0,tsp=0,diff_press=0;
	
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
	dataType = FLOAT_ABCD;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "protocol_PLC_HongRuiDe", 1);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		
		diff_press= getFloatValue(p, 3, dataType);
		speed = getFloatValue(p, 7, dataType);
		press = getFloatValue(p, 11, dataType);
		temp = getFloatValue(p, 15, dataType);
		//humi = getFloatValue(p, 19, dataType);
		
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
	//acqdata_set_value(acq_data,"a01014",UNIT_PECENT,humi,&arg_n);
	acqdata_set_value(acq_data,"a01017",UNIT_PA,diff_press,&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
 
