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
Create Time:2021.12.23 Thur.
Description:protocol_CEMS_BJXueDiLong_MODEL4000
DataType and Analysis:
	(FLOAT_CDAB) 
*/


int protocol_CEMS_BJXueDiLong_MODEL4000_1(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,no2=0,co=0,co2=0,hcl=0;
	float nox_ori=0,so2_ori=0,o2_ori=0,no_ori=0,no2_ori=0,co_ori=0,co2_ori=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	
	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x0E7E;
	regcnt = 0x14;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong MODEL4000 HeBei CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong MODEL4000 HeBei CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong MODEL4000 HeBei CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong MODEL4000 HeBei CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		hcl = getFloatValue(p, 7, dataType);
			
		so2 = getFloatValue(p, 11, dataType);

		no = getFloatValue(p, 15, dataType);
		
		no2 = getFloatValue(p, 23, dataType);
		
		co = getFloatValue(p, 27, dataType);
		
		co2 = getFloatValue(p, 31, dataType);

		nox=NO_and_NO2_to_NOx(no, no2);
		
		status=0;
	}
	else
	{
		hcl=0;
		so2=0;
		no=0;
		no2=0;
		co=0;
		co2=0;
		nox=0;
		status=1;
	}

	sleep(1);
	cmd = 0x01;
	regpos = 0x3778;
	regcnt = 0x08;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong MODEL4000 HeBei MARK SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong MODEL4000 HeBei MARK protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong MODEL4000 HeBei MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong MODEL4000 HeBei MARK RECV:",com_rbuf,size);
	p=modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = p[3];
		if(val == 0)
			acq_data->dev_stat = 'N';
		else if((val&0x04)==0x04)
			acq_data->dev_stat = 'N';
		else if((val&0x08)==0x08)
			acq_data->dev_stat = 'D';
		else if((val&0x01)==0x01)
			acq_data->dev_stat = 'M';
		else if((val&0x02)==0x02)
			acq_data->dev_stat = 'C';
		else 
			acq_data->dev_stat = 'N';
			
		status=0;
	}
	else
	{
		status=1;
	}

	acqdata_set_value(acq_data,"a21024a",UNIT_MG_M3,hcl,&arg_n);
	acqdata_set_value(acq_data,"a21024",UNIT_MG_M3,hcl,&arg_n);
	acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a05001",UNIT_MG_M3,co2,&arg_n);

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;

}


int protocol_CEMS_BJXueDiLong_MODEL4000_1_STATUS_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	int hcl_range=0,so2_range=0,o2_range=0,no_range=0,no2_range=0,co_range=0,co2_range=0;
	char *p = NULL;
	int val=0;
	float valf = 0;
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

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(isPolcodeEnable(modbusarg, "a21026"))
		acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21003"))
		acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21004"))
		acqdata_set_value_flag(acq_data,"a21004",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21005"))
		acqdata_set_value_flag(acq_data,"a21005",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21024"))
		acqdata_set_value_flag(acq_data,"a21024",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	cmd = 0x01;
	regpos = 0x3778;
	regcnt = 0x08;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_CEMS_BJXueDiLong_MODEL4000_INFOR_STATUS HeBei CEMS INFOR_STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_INFOR_STATUS HeBei CEMS protocol,CEMS INFOR_STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_CEMS_BJXueDiLong_MODEL4000_INFOR_STATUS HeBei CEMS INFOR_STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_CEMS_BJXueDiLong_MODEL4000_INFOR_STATUS HeBei CEMS INFOR_STATUS RECV:",com_rbuf,size);
	p=modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = p[3];
		if(val == 0)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else if((val&0x04)==0x04)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else if((val&0x08)==0x08)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if((val&0x01)==0x01)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		else if((val&0x02)==0x02) 
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			
		status=0;
	}
	else
	{
		status=1;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

#define SO2 0
#define NO 1
#define NO2 2
#define HCL 3
#define CO 4
static struct hebei_dynamic_info_gas MODEL4000_info[5];
static int MODEL4000_info_status = 0;


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.12.23 Thur.
Description:protocol_CEMS_BJXueDiLong_MODEL4000

TX:01 03 88 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 25 17 40 43 bc af
DataType and Analysis:
	(FLOAT_CDAB) 
*/

int protocol_CEMS_BJXueDiLong_MODEL4000_1_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	int hcl_range=0,so2_range=0,o2_range=0,no_range=0,no2_range=0,co_range=0,co2_range=0;
	char *p = NULL;
	int val=0;
	float valf = 0;
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

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00

	cmd = 0x01;
	regpos = 0x3783;
	regcnt = 0x08;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong_MODEL4000 RANGE INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong_MODEL4000 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong_MODEL4000 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong_MODEL4000 RANGE INFO RECV:",com_rbuf,size);
	p=modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = p[3];
		if((val&0x02)==0x02)
			hcl_range = 1;
		else hcl_range = 0;

		if((val&0x04)==0x04)
			so2_range = 1;
		else so2_range = 0;
		
		if((val&0x08)==0x08)
			no_range = 1;
		else no_range = 0;
		
		if((val&0x020)==0x20)
			no2_range = 1;
		else no2_range = 0;
		
		if((val&0x040)==0x40)
			co_range = 1;
		else co_range = 0;

		status=0;
	}
	else
	{
		status=1;
	}


	cmd = 0x03;
	regpos = 0x0EA8;
	regcnt = 0x44;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong MODEL4000 HeBei CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong MODEL4000 HeBei CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong MODEL4000 HeBei CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong MODEL4000 HeBei CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{

		valf = getFloatValue(p, 7, dataType);
		MODEL4000_info[HCL].i13026 = valf;

		valf = getFloatValue(p, 11, dataType);
		MODEL4000_info[SO2].i13026 = valf;

		valf = getFloatValue(p,15, dataType);
		MODEL4000_info[NO].i13026 = valf;

		valf = getFloatValue(p, 23, dataType);
		MODEL4000_info[NO2].i13026 = valf;

		valf = getFloatValue(p, 27, dataType);
		MODEL4000_info[CO].i13026 = valf;

		if(hcl_range == 0)
			MODEL4000_info[HCL].i13013 = getFloatValue(p, 55, dataType);
		else 
			MODEL4000_info[HCL].i13013 = getFloatValue(p, 103, dataType);

		if(so2_range == 0)
			MODEL4000_info[SO2].i13013 = getFloatValue(p, 59, dataType);
		else 
			MODEL4000_info[SO2].i13013 = getFloatValue(p, 107, dataType);

		if(no_range == 0)
			MODEL4000_info[NO].i13013 = getFloatValue(p, 63, dataType);
		else 
			MODEL4000_info[NO].i13013 = getFloatValue(p, 111, dataType);

		if(no2_range == 0)
			MODEL4000_info[NO2].i13013 = getFloatValue(p, 71, dataType);
		else 
			MODEL4000_info[NO2].i13013 = getFloatValue(p, 119, dataType);
		
		if(co_range == 0)
			MODEL4000_info[CO].i13013 = getFloatValue(p, 75, dataType);
		else 
			MODEL4000_info[CO].i13013 = getFloatValue(p, 123, dataType);

		MODEL4000_info_status=0;
	}
	else
	{
		MODEL4000_info_status=1;
	}

	read_system_time(acq_data->acq_tm);

	if(MODEL4000_info_status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}




int protocol_CEMS_BJXueDiLong_MODEL4000_1_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,no2=0,co=0,co2=0;
	float nox_ori=0,so2_ori=0,o2_ori=0,no_ori=0,no2_ori=0,co_ori=0,co2_ori=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(MODEL4000_info_status==0){
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,MODEL4000_info[SO2].i13013,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,MODEL4000_info[SO2].i13026,INFOR_ARGUMENTS,&arg_n);
		status = 0;
	}else
		status = 1;
	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_BJXueDiLong_MODEL4000_1_NO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_NO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(MODEL4000_info_status==0){
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,MODEL4000_info[NO].i13013,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,MODEL4000_info[NO].i13026,INFOR_ARGUMENTS,&arg_n);
		status = 0;
	}else
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_BJXueDiLong_MODEL4000_1_NO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_NO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(MODEL4000_info_status==0){
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,MODEL4000_info[NO2].i13013,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,MODEL4000_info[NO2].i13026,INFOR_ARGUMENTS,&arg_n);
		status = 0;
	}else
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_BJXueDiLong_MODEL4000_1_CO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_CO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(MODEL4000_info_status==0){
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,MODEL4000_info[CO].i13013,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,MODEL4000_info[CO].i13026,INFOR_ARGUMENTS,&arg_n);
		status = 0;
	}else
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL4000_1_HCL_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_HCL_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(MODEL4000_info_status==0){
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,MODEL4000_info[HCL].i13013,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,MODEL4000_info[HCL].i13026,INFOR_ARGUMENTS,&arg_n);
		status = 0;
	}else
		status = 1;
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


