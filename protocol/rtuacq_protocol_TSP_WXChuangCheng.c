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

int protocol_TSP_WXChuangChen_DA600(struct acquisition_data *acq_data)
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
	float smoke=0,orig=0,valf=0;
	int val=0;
	char *p=NULL;
	char flag=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x06);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "WXChuangChen DA600 PM", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x06);
	if(p!=NULL)
	{
		smoke=getFloatValue(p, 3, dataType);
		orig=getFloatValue(p, 7, dataType);

		valf=getFloatValue(p, 11, dataType);
		val=(int)valf;
		switch(val)
		{
			case 0:	flag='N';break;
			case 1:	flag='M';break;
			case 2:	flag='D';break;
			case 3:	flag='C';break;
			default:	flag='N';break;
		}
		
		status=0;
	}
	else
	{
		smoke=0;
		orig=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,smoke,orig,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	

	if(status == 0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_TSP_WXChuangChen_DA600_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[50]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	union uf f;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_TSP_WXChuangChen_DA600_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x3A;
	dataType=FLOAT_CDAB;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x02,cnt);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "WXChuangChen DA600 PM INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 7, dataType);
		val=(int)valf;
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
		
		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 51, dataType);
		year=(int)valf;
		valf=getFloatValue(p, 55, dataType);
		val=(int)valf;
		mon=val/100;
		day=val%100;
		valf=getFloatValue(p, 59, dataType);
		hour=(int)valf;
		valf=getFloatValue(p, 63, dataType);
		val=(int)valf;
		min=val/100;
		sec=val%100;
		t3=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 67, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 71, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 75, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 79, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 83, dataType);
		year=(int)valf;
		valf=getFloatValue(p, 87, dataType);
		val=(int)valf;
		mon=val/100;
		day=val%100;
		valf=getFloatValue(p, 91, dataType);
		hour=(int)valf;
		valf=getFloatValue(p, 95, dataType);
		val=(int)valf;
		min=val/100;
		sec=val%100;
		t3=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 99, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 103, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 107, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 111, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 115, dataType);
		acqdata_set_value_flag(acq_data,"i13015",UNIT_MINUTE,valf/60,INFOR_ARGUMENTS,&arg_n);

		
		status=0;
	}


	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

