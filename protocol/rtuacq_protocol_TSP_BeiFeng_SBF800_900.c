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

int protocol_TSP_BeiFeng_SBF800_900_2021A(struct acquisition_data *acq_data)
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
	float smoke,orig;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0519,0x0C);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BeiFeng SBF800/900-2021A TSP",1);
	if((size>=29)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		orig=getFloatValue(com_rbuf, 3, FLOAT_ABCD);
		smoke=getFloatValue(com_rbuf, 23, FLOAT_ABCD);
		
		status=0;
	}
	else
	{
		orig=0;
		smoke=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,smoke,orig,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,smoke,&arg_n);
	

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_TSP_BeiFeng_SBF800_900_2021A_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_TSP_BeiFeng_SBF800_900_2021A_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0500,0x25);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BeiFeng SBF800/900-2021A TSP INFO",1);
	if((size>=79)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(com_rbuf, 5, INT_AB);
		acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(com_rbuf, 7, INT_AB);
		acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,val,INFOR_STATUS,&arg_n);


		t3=getTimeValue(BCD(com_rbuf[9])+2000, BCD(com_rbuf[10]), BCD(com_rbuf[11]), BCD(com_rbuf[12]), BCD(com_rbuf[13]), BCD(com_rbuf[14]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(com_rbuf, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 25, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 29, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 33, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 37, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 41, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 45, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 49, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 53, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 57, dataType);
		acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 61, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 65, dataType);
		acqdata_set_value_flag(acq_data,"i13014",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 69, dataType);
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


int protocol_TSP_BeiFeng_SBF800_900_2021B(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char str[20];
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float smoke,orig;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1000,0x0F);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BeiFeng SBF800/900-2021B TSP",1);
	if((size>=35)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[17],6);
		smoke=atof(str);
		
		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[23],8);
		orig=atof(str);
		
		status=0;
	}
	else
	{
		orig=0;
		smoke=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,smoke,orig,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,smoke,&arg_n);
	

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_TSP_BeiFeng_SBF800_900_2021B_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char str[20];
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	union uf f;
	int year,mon,day,hour,min,sec;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_TSP_BeiFeng_SBF800_900_2021B_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x100F,0x52);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BeiFeng SBF800/900-2021B TSP INFO",1);
	if((size>=169)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
		
		val=getUInt16Value(com_rbuf, 5, INT_AB);
		acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(com_rbuf, 7, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
		
		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[9],14);
		sscanf(str,"%04d%02d%02d%02d%02d%02d",&year,&mon,&day,&hour,&min,&sec);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[24],8);
		valf=atof(str);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[32],8);
		valf=atof(str);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[40],8);
		valf=atof(str);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[49],14);
		sscanf(str,"%04d%02d%02d%02d%02d%02d",&year,&mon,&day,&hour,&min,&sec);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[64],8);
		valf=atof(str);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[72],8);
		valf=atof(str);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[80],8);
		valf=atof(str);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[89],8);
		valf=atof(str);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		memset(str,0,sizeof(str));
		memcpy(str,&com_rbuf[155],6);
		valf=atof(str);
		acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


