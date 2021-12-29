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

int protocol_water_YuXing_Modbus(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[10]={0};
	char str[50]={0};
	int size=0;
	float valf=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x02;
	dataType=FLOAT_CDAB;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	memset(str,0,sizeof(str));
	strcpy(str,"YuXing modbus ");
	strcat(str,modbusarg->polcode_arg[0].polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		
		status=0;
	}
	else
	{
		valf=0;
		status=1;
	}

	acqdata_set_value(acq_data,modbusarg->polcode_arg[0].polcode,modbusarg->polcode_arg[0].unit,valf,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_water_YuXing_Modbus_info(struct acquisition_data *acq_data)
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

	SYSLOG_DBG("protocol_water_YuXing_Modbus_info\n");
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL)
		return -2;

	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	startaddr=6;
	cnt = 0x29;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr-1,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"YuXing modbus %s info",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		pos=get_cmd_03_04_pos(startaddr, 6);
		val = getInt16Value(p,pos, INT_AB);
		switch(val)
		{
			case 0:	valf=0;	break;
			case 1:	valf=0;	break;
			case 2:	valf=4;	break;
			case 3:	valf=3;	break;
			case 4:	valf=5;	break;
			case 5:	valf=5;	break;
			case 6:	valf=5;	break;
			case 7:	valf=5;	break;
			case 8:	valf=99;	break;
			case 9:	valf=2;	break;
			case 10:	valf=2;	break;
			case 11:	valf=99;	break;
			case 12:	valf=99;	break;
			case 13:	valf=99;	break;
			case 14:	valf=99;	break;
			default:	valf=99;	break;
		}
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 7);
		val = getInt16Value(p,pos, INT_AB);
		if(val == 0)
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		
		switch(val)
		{
			case 0:	valf=99;	break;
			case 1:	valf=99;	break;
			case 2:	valf=99;	break;
			case 3:	valf=99;	break;
			case 4:	valf=99;	break;
			case 5:	valf=99;	break;
			case 6:	valf=99;	break;
			case 7:	valf=99;	break;
			case 8:	valf=99;	break;
			case 9:	valf=9;	break;
			case 10:	valf=5;	break;
			case 11:	valf=5;	break;
			case 12:	valf=5;	break;
			case 13:	valf=99;	break;
			case 14:	valf=99;	break;
			case 15:	valf=99;	break;
			case 16:	valf=99;	break;
			case 17:	valf=10;	break;
			case 18:	valf=99;	break;
			default:	valf=99;	break;
		}
		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 12);
		valf= getUInt16Value(p,pos, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 14);
		valf= getFloatValue(p,pos, dataType);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 18);
		val= getUInt16Value(p,pos, INT_AB);
		switch(val)
		{
			case 1:	valf=2;	break;
			case 2:	valf=10;	break;
			case 3:	valf=100;	break;
			default:	valf=val;	break;
		}
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 19);
		valf= getFloatValue(p,pos, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 21);
		valf= getFloatValue(p,pos, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 23);
		valf= getFloatValue(p,pos, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 25);
		valf= getFloatValue(p,pos, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 27);
		valf= getFloatValue(p,pos, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 29);
		valf= getFloatValue(p,pos, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 31);
		valf= getFloatValue(p,pos, dataType);
		acqdata_set_value_flag(acq_data,"i13115",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 33);
		year=BCD(p[pos])+2000;
		mon=BCD(p[pos+1]);
		day=BCD(p[pos+2]);
		hour=BCD(p[pos+3]);
		min=BCD(p[pos+4]);
		sec=BCD(p[pos+5]);
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13101",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13107",t1,0.0,INFOR_ARGUMENTS,&arg_n);


	    	status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

