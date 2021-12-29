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

int SHHeRu_APxx370_STATUS;

int protocol_CEMS_SHHeRu_APxx370_v1_6(struct acquisition_data *acq_data)
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
	unsigned int devaddr=0,cmd=0,cnt=0;
	float so2=0,no=0,no2=0 ,nox=0;
	int unit=0;
	char *p=NULL;
	char flag=0;
	int di[4]={1,1,1,1};
	int val = 0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	
	if(isPolcodeEnable(modbusarg, "a21026"))
	{
		devaddr=modbusarg->devaddr&0xffff;
		cmd=0x03;
		cnt=0x10;

		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,cmd,0x1003,cnt);
		size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 CEMS", 1);
		p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
		if(p!=NULL)
		{
			so2=getFloatValue(p, 7, FLOAT_ABCD);
			unit=getUInt16Value(p, 11, INT_AB);
			switch(unit)
			{
				case 1:	so2=gas_ppm_to_mg_m3(so2,M_SO2);break;
				case 2:	so2=gas_ppb_to_mg_m3(so2,M_SO2);break;
				case 5:	so2=so2*0.001;break;
				default:	break;
			}
	/*
			no=getFloatValue(p, 23, FLOAT_ABCD);
			unit=getUInt16Value(p, 27, INT_AB);
			switch(unit)
			{
				case 1:	no=gas_ppm_to_mg_m3(no,M_NO);break;
				case 2:	no=gas_ppb_to_mg_m3(no,M_NO);break;
				case 5:	no=no*0.001;break;
				default:	break;
			}
			nox=NO_to_NOx(no);
	*/
			status=0;
		}
		else
		{
			so2=0;
			//nox=0;
			status=1;
		}

	
		acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
		acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
		acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	}

	
	if(isPolcodeEnable(modbusarg, "a21002"))
	{
		devaddr=modbusarg->devaddr&0xffff;
		cmd=0x03;
		cnt=0x02;

		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,cmd,0x3018,cnt);
		size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 CEMS", 1);
		p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
		if(p!=NULL)
		{
			nox=getFloatValue(p, 3, FLOAT_ABCD);
			nox*=100;
			//nox=gas_ppm_to_mg_m3(nox,M_NOx);
	
			status=0;
		}
		else
		{
			nox=0;
			status=1;
		}
	
		acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
		acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
		acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	}


	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0A;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x3010,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 CEMS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{   
		val = getInt16Value(p, 3, INT_AB);
		if(val == 1)
		{
			 no = getFloatValue(p, 11, FLOAT_ABCD);
			 no *= 100;

			 no2 = getFloatValue(p, 15, FLOAT_ABCD);
			 no2 *= 100;
		} 
		
		status=0;
	}
	else
	{
		nox = 0;
		no = 0;
		no2 = 0;
		status=1;  
	}

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);


	read_device(DEV_DI0,&di[0],sizeof(int));
	read_device(DEV_DI1,&di[1],sizeof(int));
	read_device(DEV_DI2,&di[2],sizeof(int));
	read_device(DEV_DI3,&di[3],sizeof(int));

	if(di[3]==0)
		flag='D';
	else if(di[1]==0)
		flag='z';
	else if(di[2]==0)
		flag='C';
	else if(di[0]==0)
		flag='N';
	else
		flag=0;

	SHHeRu_APxx370_STATUS=flag;

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


int protocol_CEMS_SHHeRu_APxx370_v1_6_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;
	int di[4]={1,1,1,1};

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SHHeRu_APxx370_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;

	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x21;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	read_device(DEV_DI0,&di[0],sizeof(int));
	read_device(DEV_DI1,&di[1],sizeof(int));
	read_device(DEV_DI2,&di[2],sizeof(int));
	read_device(DEV_DI3,&di[3],sizeof(int));

	if(di[3]==0)
		acqdata_set_value_flag(acq_data,"i12007",UNIT_MG_M3,2,INFOR_STATUS,&arg_n);
	else if(di[1]==0)
		acqdata_set_value_flag(acq_data,"i12007",UNIT_MG_M3,5,INFOR_STATUS,&arg_n);
	else if(di[2]==0)
		acqdata_set_value_flag(acq_data,"i12007",UNIT_MG_M3,3,INFOR_STATUS,&arg_n);
	else if(di[0]==0)
		acqdata_set_value_flag(acq_data,"i12007",UNIT_MG_M3,0,INFOR_STATUS,&arg_n);

	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x2200,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 SO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(p, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 25, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 29, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 33, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(BCD(p[37])+2000, BCD(p[38]), BCD(p[39]), BCD(p[40]), BCD(p[41]), BCD(p[42]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(BCD(p[43])+2000, BCD(p[44]), BCD(p[45]), BCD(p[46]), BCD(p[47]), BCD(p[48]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 49, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 53, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 61, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf*2.86,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 65, dataType);
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


int protocol_CEMS_SHHeRu_APxx370_v1_6_NOx_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;
	int di[4]={1,1,1,1};

	struct tm timer; 
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SHHeRu_APxx370_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a21002"))
		return 0;

	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x21;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	read_device(DEV_DI0,&di[0],sizeof(int));
	read_device(DEV_DI1,&di[1],sizeof(int));
	read_device(DEV_DI2,&di[2],sizeof(int));
	read_device(DEV_DI3,&di[3],sizeof(int));

	if(di[3]==0)
		acqdata_set_value_flag(acq_data,"i12007",UNIT_MG_M3,2,INFOR_STATUS,&arg_n);
	else if(di[1]==0)
		acqdata_set_value_flag(acq_data,"i12007",UNIT_MG_M3,5,INFOR_STATUS,&arg_n);
	else if(di[2]==0)
		acqdata_set_value_flag(acq_data,"i12007",UNIT_MG_M3,3,INFOR_STATUS,&arg_n);
	else if(di[0]==0)
		acqdata_set_value_flag(acq_data,"i12007",UNIT_MG_M3,0,INFOR_STATUS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x2300,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 NOx INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(p, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 25, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 29, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 33, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(BCD(p[37])+2000, BCD(p[38]), BCD(p[39]), BCD(p[40]), BCD(p[41]), BCD(p[42]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(BCD(p[43])+2000, BCD(p[44]), BCD(p[45]), BCD(p[46]), BCD(p[47]), BCD(p[48]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 49, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 53, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
 
		valf=getFloatValue(p, 61, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf*2.05,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 65, dataType);
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



int protocol_CEMS_SHHeRu_APxx370_v1_6_NO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;
	int di[4]={1,1,1,1};

	struct tm timer; 
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SHHeRu_APxx370_NO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a21003"))
		return 0;

	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x02;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x231F,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 NO INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{

		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	status = 1;
	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0A;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x3010,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 NO INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		if(val == 1)
			val=getInt16Value(p, 5, INT_AB);

		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
 
	return arg_n;
}



int protocol_CEMS_SHHeRu_APxx370_v1_6_NO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;
	int di[4]={1,1,1,1};

	struct tm timer; 
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SHHeRu_APxx370_NO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a21004"))
		return 0;

	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x02;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x231F,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 NO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{

		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	status = 1;
	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0A;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x3010,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHHeRu APxx370 v1.6 NO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		if(val == 1)
			val=getInt16Value(p, 7, INT_AB);

		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
 
	return arg_n;
}



