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

int protocol_ZONGLIN_SZZhongXingModbus(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float tp=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char *p=NULL;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0C;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1000,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SZZhongXing ZE-C310(V2) TP", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		tp=getFloatValue(p, 3, FLOAT_DCBA);
		status=0;
	}
	else
	{
		tp=0;
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,tp,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}


int protocol_water_SZZhongXingModbus_info1(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char str[50];
	int size=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0,startaddr=0;
	char *p=NULL;
	char *polcode=NULL;
	char flag=0;
	float valf=0;
	int val=0;
	int pos=0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;

	SYSLOG_DBG("protocol_water_SZZhongXingModbus_info1\n");
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL)
		return -2;

	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	startaddr=0x10C0;
	cnt = 0x06;
	dataType = FLOAT_DCBA;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"SZZhongXingModbus %s info1",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val = getInt16Value(p,5, INT_AB);
		switch(val)
		{
			case 0:	valf=1;	break;
			case 1:	valf=5;	break;
			case 2:	valf=5;	break;
			case 3:	valf=5;	break;
			case 4:	valf=2;	break;
			case 5:	valf=6;	break;
			case 6:	valf=4;	break;
			case 7:	valf=3;	break;
			case 8:	valf=3;	break;
			case 9:	valf=3;	break;
			case 10:	valf=0;	break;
			case 11:	valf=6;	break;
			case 12:	valf=6;	break;
			case 13:	valf=99;	break;
			case 14:	valf=99;	break;
			default:	valf=99;	break;
		}
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

		if((p[12]==0)&&(p[13]==0)&&(p[14]==0))
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			if((p[12]&0x01)==0x01)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			else if((p[12]&0x02)==0x02)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((p[12]&0x04)==0x04)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((p[12]&0x08)==0x08)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((p[12]&0x10)==0x10)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((p[12]&0x20)==0x20)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((p[12]&0x40)==0x40)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((p[12]&0x80)==0x80)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((p[13]&0x01)==0x01)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((p[13]&0x02)==0x02)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((p[13]&0x04)==0x04)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((p[13]&0x08)==0x08)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((p[13]&0x10)==0x10)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((p[13]&0x20)==0x20)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((p[13]&0x40)==0x40)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((p[13]&0x80)==0x80)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n);
			else if((p[14]&0x01)==0x01)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((p[14]&0x02)==0x02)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((p[14]&0x04)==0x04)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((p[14]&0x08)==0x08)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((p[14]&0x10)==0x10)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((p[14]&0x20)==0x20)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);
			else if((p[14]&0x40)==0x40)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);
			else if((p[14]&0x80)==0x80)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_water_SZZhongXingModbus_info2(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char str[50];
	int size=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0,startaddr=0;
	char *p=NULL;
	char *polcode=NULL;
	char flag=0;
	float valf=0;
	int val=0;
	int pos=0;

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
	t1=0;

	SYSLOG_DBG("protocol_water_SZZhongXingModbus_info2\n");
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL)
		return -2;

	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	startaddr=0x1040;
	cnt = 0x15;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"SZZhongXingModbus %s info2",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		year=p[39]+2000;
		mon=p[40];
		day=p[41];
		hour=p[42];
		min=p[43];
		sec=p[44];
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13107",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_water_SZZhongXingModbus_info3(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char str[50];
	int size=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0,startaddr=0;
	char *p=NULL;
	char *polcode=NULL;
	char flag=0;
	float valf=0;
	int val=0;
	int pos=0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;

	SYSLOG_DBG("protocol_water_SZZhongXingModbus_info3\n");
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL)
		return -2;

	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	startaddr=0x1440;
	cnt = 0x11;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"SZZhongXingModbus %s info3",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf= getUInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 5, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 9, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13115",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getUInt16Value(p, 31, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getUInt16Value(p, 33, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getUInt16Value(p, 35, INT_AB);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

