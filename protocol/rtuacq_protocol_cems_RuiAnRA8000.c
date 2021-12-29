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
Create Time:2021.12.09 Thur.
Description:protocol_CEMS_RuiAnRA8000
TX:01 03 00 00 00 08 C5 C8
RX:01 03 10
41 20 00 00
42 48 00 00
41 A0 00 00
42 48 00 00
...
FF FF

DataType and Analysis:
	(INT_AB) 00 10     = 16
*/

int protocol_CEMS_RuiAnRA8000(struct acquisition_data *acq_data)
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
	regpos = 0xC8;
	regcnt = 0x0A;
	dataType = FLOAT_ABCD ;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"RuiAnRA8000 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("RuiAnRA8000 CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("RuiAnRA8000 CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"RuiAnRA8000 CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		SO2 = getInt16Value(p, 13, INT_AB);

		val = getInt16Value(p, 15, INT_AB);

		switch(val){
			case 1 : SO2 = gas_ppm_to_mg_m3(SO2, M_SO2);break;
			case 2 : SO2 = gas_ppb_to_mg_m3(SO2,M_SO2);break;
			case 5 : SO2 *=1.0; break;
			default : break;
		}

		status=0;
	}
	else
	{
		SO2 = 0;
		status=1;
	}
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,SO2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,SO2,SO2_orig,&arg_n); 
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);


	if(status == 0)
	{
		//acq_data->dev_stat=flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_CEMS_RuiAnRA8000_info(struct acquisition_data *acq_data)
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

	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
	cmd = 0x03;
	regpos = 0xC8;
	regcnt = 0x0A;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"RuiAnRA8000 SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("RuiAnRA8000 CEMS protocol, INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("RuiAnRA8000  SO2 INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"RuiAnRA8000  SO2 INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 19, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

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

	return arg_n;
}

