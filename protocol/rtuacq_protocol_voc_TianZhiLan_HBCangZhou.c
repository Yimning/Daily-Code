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

static int TianZhiLan_HBCangZhou_VOC_status;
static int TianZhiLan_HBCangZhou_VOC_alarm_status;
static int TianZhiLan_HBCangZhou_VOC_alarm_info;
static int TianZhiLan_HBCangZhou_VOC_flag;

int protocol_VOC_TianZhiLan_HBCangZhou(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 12

	char *polcode[POLCODE_NUM]={
		"a05002",		"a24088",		"a24087",		"a01013",		"a01011",
		"a01012",		"a25002",		"a25003",		"a25005",		"a01014",
		"a19001",		"a00000"
	};
	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_KPA,			UNIT_M_S,
		UNIT_0C,			UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_PECENT,
		UNIT_PECENT,	UNIT_M3_H
	};
	int regaddr[POLCODE_NUM]={
		1,	3,	5,	7,	9,
		11,	13,	15,	17,	19,
		21,	23
	};

	int status=0;
	float valf=0;
	int val=0;
	int i=0,pos=0,bit=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0,cnt=0,startreg=0;
	char *p=NULL;
	char flag=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	startreg=0x01;
	cnt=0x1E;
	dataType=FLOAT_ABCD;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startreg,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "TianZhiLan HBCangZhou VOC", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			pos=(regaddr[i]-startreg)*2+3;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,polcode[i],unit[i],valf,&arg_n);
		}

		val=getFloatValue(p, 51, dataType);
		switch(val)
		{
			case 0:	flag='N';break;
			case 1:	flag='M';break;
			case 2:	flag='D';break;
			case 3:	flag='C';break;
			case 5:	flag='z';break;
			case 6:	flag='C';break;
			default:	flag='N';break;
		}
		TianZhiLan_HBCangZhou_VOC_status=val;
		TianZhiLan_HBCangZhou_VOC_alarm_status=getFloatValue(p, 55, dataType);
		TianZhiLan_HBCangZhou_VOC_alarm_info=getFloatValue(p, 59, dataType);
		TianZhiLan_HBCangZhou_VOC_flag=1;
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}

		TianZhiLan_HBCangZhou_VOC_flag=0;
		status=1;
	}
	
	if(status==0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
	
#undef POLCODE_NUM
}

int protocol_CEMS_TianZhiLan_HBCangZhou_STATUS(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	MODBUS_DATA_TYPE dataType;
	int devaddr=0,cmd=0,cnt=0,startreg=0;
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_TianZhiLan_HBCangZhou_STATUS\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(isPolcodeEnable(modbusarg, "a05002"))
		acqdata_set_value_flag(acq_data,"a05002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else	if(isPolcodeEnable(modbusarg, "a24088"))
		acqdata_set_value_flag(acq_data,"a24088",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else	if(isPolcodeEnable(modbusarg, "a24087"))
		acqdata_set_value_flag(acq_data,"a24087",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else	if(isPolcodeEnable(modbusarg, "a25002"))
		acqdata_set_value_flag(acq_data,"a25002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else	if(isPolcodeEnable(modbusarg, "a25003"))
		acqdata_set_value_flag(acq_data,"a25003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else	if(isPolcodeEnable(modbusarg, "a25005"))
		acqdata_set_value_flag(acq_data,"a25005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else	if(isPolcodeEnable(modbusarg, "a19001"))
		acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	if(TianZhiLan_HBCangZhou_VOC_flag==1)
	{
		acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,TianZhiLan_HBCangZhou_VOC_status,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,TianZhiLan_HBCangZhou_VOC_alarm_status,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,TianZhiLan_HBCangZhou_VOC_alarm_info,INFOR_STATUS,&arg_n);

		status=0;
	}
	else
		status=1;

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

