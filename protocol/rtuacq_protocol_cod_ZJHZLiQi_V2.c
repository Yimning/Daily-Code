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
Email:1148967988@qq.com
Create Time:2021.10.26 Tues.
Description:protocol_COD_ZJHZLiQi_V2
TX:01 03 00 0B 00 02 B5 C9
RX:01 03 04 42 48 00 00 6E 5D 
DataType and Analysis:
	(FLOAT_ABCD) 42 48 00 00 = 50
*/
int protocol_COD_ZJHZLiQi_V2 (struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	float org = 0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x0B;
	regcnt = 0x02;
	dataType = FLOAT_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJHZLiQi V2 COD SEND :",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("ZJHZLiQi V2 COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZJHZLiQi V2 COD  data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJHZLiQi V2 COD RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		
		status = 0;
    	}
	else
	{
		status = 1;
	}

	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,valf,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	
    return arg_n;
}


int protocol_COD_ZJHZLiQi_V2_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	float org = 0;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	
	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x28;
	regcnt = 0x48;
	dataType = FLOAT_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJHZLiQi V2 COD INFO SEND1 :",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("ZJHZLiQi V2 COD INFO protocol,COD INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZJHZLiQi V2 COD INFO1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJHZLiQi V2 COD INFO RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 7: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		
		val = getInt16Value(p,121, INT_AB);
		if(val == 0){
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		
		switch(val){
			case 1: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 7: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 9: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 10: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val = getInt16Value(p,19, INT_AB);  	 //Just COD
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);	
		
		valf= getFloatValue(p,41, dataType);	//Just COD
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,valf,INFOR_ARGUMENTS,&arg_n);

		year=getUInt16Value(p, 59, INT_AB);
		mon=getUInt16Value(p, 61, INT_AB);
		day=getUInt16Value(p, 63, INT_AB);
		hour=getUInt16Value(p, 65, INT_AB);
		min=getUInt16Value(p, 67, INT_AB);
		sec=0;
		t1 = getTimeValue(BCD(year), BCD(mon), BCD(day), BCD(hour), BCD(min), BCD(sec));
		acqdata_set_value_flag(acq_data,"i13101",t1,0.0,INFOR_ARGUMENTS,&arg_n);


		valf =getFloatValue(p,69 , dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);   //K value int file.
		
		valf =getFloatValue(p,73 , dataType);	
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);	//B value int file.
		
		status = 0;
    	}
	else
	{
		status = 1;
	}

	cmd = 0x03;
	regpos = 0x77;
	regcnt = 0x06;
	dataType = FLOAT_ABCD ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJHZLiQi V2 COD INFO SEND2 :",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("ZJHZLiQi V2 COD INFO protocol,COD INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZJHZLiQi V2 COD INFO2  data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJHZLiQi V2 COD INFO RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf =getFloatValue(p,7 , dataType);
		//acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf =getFloatValue(p,11 , dataType);	
		//acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);		
		
		
		status = 0;
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

