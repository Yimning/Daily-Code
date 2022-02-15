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
#include <time.h>s
#include <sys/time.h>
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

static int CQChuanYiModBus_PS7400_F_status;
static int CQChuanYiModBus_PS7400_F_alarm_status;
static int CQChuanYiModBus_PS7400_F_alarm_info;
static int CQChuanYiModBus_PS7400_F_flag;

int protocol_CEMS_CQChuanYiModBus_PS7400_F(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,no2=0,co=0,hcl=0;
	float humidity=0;
	int val=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	char *p;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x1C;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		so2=getFloatValue(p, 3, dataType);

		nox=getFloatValue(p, 7, dataType);

		co=getFloatValue(p, 11, dataType);

		hcl=getFloatValue(p, 19, dataType);
		
		humidity=getFloatValue(p, 31, dataType);
			
		no=getFloatValue(p, 35, dataType);

		no2=getFloatValue(p, 39, dataType);

		o2=getFloatValue(p, 55, dataType);
		
		if(nox==0) 
			nox=NO_and_NO2_to_NOx(no, no2);
		
		status=0;
	}
	else
	{
		nox=0,
		so2=0,
		o2=0,
		no=0,
		no2=0,
		co=0;
		hcl=0;
		humidity=0;
		status=1;
	}

	if(CQChuanYiModBus_PS7400_F_flag==1)
	{	
		val=CQChuanYiModBus_PS7400_F_status;
		switch(val)
		{
			case 0:	flag='N';break;
			case 1:	flag='M';break;
			case 2:	flag='D';break;
			case 3:	flag='C';break;
			case 5:	flag='z';break;
			default:	flag=0;break;
		}
	}
	else
	{
		flag=0;
	}
	

	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a21024",UNIT_MG_M3,hcl,&arg_n);
	acqdata_set_value(acq_data,"a21024a",UNIT_MG_M3,hcl,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,co,&arg_n);

	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,humidity,&arg_n);

	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);

	
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

int protocol_CEMS_CQChuanYiModBus_PS7400_F_SO2_info(struct acquisition_data *acq_data)
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
	float SO2_zero=0,SO2_range=0,SO2_K=0,SO2_PPB=0,SO2_zero_drift=0,SO2_range_drift=0,SO2_intercept=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;

	struct tm timer;
	time_t t1,t2,t3;

	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_CQChuanYiModBus_PS7400_F_SO2_info1\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	if(CQChuanYiModBus_PS7400_F_flag==1)
	{
		val=CQChuanYiModBus_PS7400_F_status;
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}

		val=CQChuanYiModBus_PS7400_F_alarm_status;
		if(val==0)
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);

		val=CQChuanYiModBus_PS7400_F_alarm_info;
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
	}
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x38,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F SO2 INFO1 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F SO2 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F SO2 data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F SO2 INFO1 RECV1:",com_rbuf,size);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		SO2_zero=f.f;

		
		f.ch[3]=com_rbuf[35];
		f.ch[2]=com_rbuf[36];
		f.ch[1]=com_rbuf[37];
		f.ch[0]=com_rbuf[38];
		SO2_range=f.f;
		status=0;
		
	}else{
		SO2_zero=0;
		SO2_range=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,SO2_zero,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,SO2_range,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x5A,0x22);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F SO2 INFO1 SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F SO2 protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F SO2 data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F SO2 INFO2 RECV2:",com_rbuf,size);
	if((size>=73)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		SO2_K=f.f;

		
		f.ch[3]=com_rbuf[39];
		f.ch[2]=com_rbuf[40];
		f.ch[1]=com_rbuf[41];
		f.ch[0]=com_rbuf[42];
		SO2_PPB=f.f;
		status=0;
		
	}else{
		SO2_K=0;
		SO2_PPB=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13026",0,SO2_K,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,SO2_PPB,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x86,0x30);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F SO2 INFO3 SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F SO2 protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F SO2 data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F SO2 INFO3 RECV3:",com_rbuf,size);
	if((size>=101)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		SO2_zero_drift=f.f;

		f.ch[3]=com_rbuf[35];
		f.ch[2]=com_rbuf[36];
		f.ch[1]=com_rbuf[37];
		f.ch[0]=com_rbuf[38];
		SO2_range_drift=f.f;
		
		f.ch[3]=com_rbuf[67];
		f.ch[2]=com_rbuf[68];
		f.ch[1]=com_rbuf[69];
		f.ch[0]=com_rbuf[70];
		SO2_intercept=f.f;
		
		status=0;
	}else{
		SO2_zero_drift=0;
		SO2_range_drift=0;
		SO2_intercept=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,SO2_zero_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,SO2_range_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",0,SO2_intercept,INFOR_ARGUMENTS,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xB8,0x60);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F SO2 INFO4 SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F SO2 protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F SO2 data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F SO2 INFO4 RECV4:",com_rbuf,size);
	if((size>=197)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		year=getUInt16Value(com_rbuf, 3, INT_AB);
		mon=getUInt16Value(com_rbuf, 5, INT_AB);
		day=getUInt16Value(com_rbuf, 7, INT_AB);
		hour=getUInt16Value(com_rbuf, 9, INT_AB);
		min=getUInt16Value(com_rbuf, 11, INT_AB);
		sec=getUInt16Value(com_rbuf, 13, INT_AB);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		//This part has writen the SO2_zero_drift_time 
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		year=getUInt16Value(com_rbuf, 99, INT_AB);
		mon=getUInt16Value(com_rbuf, 101, INT_AB);
		day=getUInt16Value(com_rbuf, 103, INT_AB);
		hour=getUInt16Value(com_rbuf, 105, INT_AB);
		min=getUInt16Value(com_rbuf, 107, INT_AB);
		sec=getUInt16Value(com_rbuf, 109, INT_AB);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		//This part has writen the SO2_range_drift_time 
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		status=0;
		
	}
	read_system_time(acq_data->acq_tm);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_CQChuanYiModBus_PS7400_F_NOx_info(struct acquisition_data *acq_data)
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
	float NOx_zero=0,NOx_range=0,NOx_K=0,NOx_PPB=0,NOx_zero_drift=0,NOx_range_drift=0,NOx_intercept=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;

	struct tm timer;
	time_t t1,t2,t3;

	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_CQChuanYiModBus_PS7400_F_NOx_info1\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x38,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F NOx INFO1 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F NOx protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F NOx data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F NOx INFO1 RECV1:",com_rbuf,size);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		NOx_zero=f.f;

		
		f.ch[3]=com_rbuf[39];
		f.ch[2]=com_rbuf[40];
		f.ch[1]=com_rbuf[41];
		f.ch[0]=com_rbuf[42];
		NOx_range=f.f;
		status=0;
		
	}else{
		NOx_zero=0;
		NOx_range=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,NOx_zero,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,NOx_range,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x5A,0x22);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F NOx INFO1 SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F NOx protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F NOx data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F NOx INFO2 RECV2:",com_rbuf,size);
	if((size>=73)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		NOx_K=f.f;

		
		f.ch[3]=com_rbuf[43];
		f.ch[2]=com_rbuf[44];
		f.ch[1]=com_rbuf[45];
		f.ch[0]=com_rbuf[46];
		NOx_PPB=f.f;
		status=0;
		
	}else{
		NOx_K=0;
		NOx_PPB=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13026",0,NOx_K,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,NOx_PPB,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x86,0x30);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F NOx INFO3 SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F NOx protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F NOx data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F NOx INFO3 RECV3:",com_rbuf,size);
	if((size>=101)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		NOx_zero_drift=f.f;

		f.ch[3]=com_rbuf[39];
		f.ch[2]=com_rbuf[40];
		f.ch[1]=com_rbuf[41];
		f.ch[0]=com_rbuf[42];
		NOx_range_drift=f.f;
		
		f.ch[3]=com_rbuf[71];
		f.ch[2]=com_rbuf[72];
		f.ch[1]=com_rbuf[73];
		f.ch[0]=com_rbuf[74];
		NOx_intercept=f.f;
		
		status=0;
	}else{
		NOx_zero_drift=0;
		NOx_range_drift=0;
		NOx_intercept=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,NOx_zero_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,NOx_range_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",0,NOx_intercept,INFOR_ARGUMENTS,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xB8,0x60);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F NOx INFO4 SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F NOx protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F NOx data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F NOx INFO4 RECV4:",com_rbuf,size);
	if((size>=197)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		year=getUInt16Value(com_rbuf, 15, INT_AB);
		mon=getUInt16Value(com_rbuf, 17, INT_AB);
		day=getUInt16Value(com_rbuf, 19, INT_AB);
		hour=getUInt16Value(com_rbuf, 21, INT_AB);
		min=getUInt16Value(com_rbuf, 23, INT_AB);
		sec=getUInt16Value(com_rbuf, 25, INT_AB);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		//This part has writen the NOx_zero_drift_time 
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		year=getUInt16Value(com_rbuf, 111, INT_AB);
		mon=getUInt16Value(com_rbuf, 113, INT_AB);
		day=getUInt16Value(com_rbuf, 115, INT_AB);
		hour=getUInt16Value(com_rbuf, 117, INT_AB);
		min=getUInt16Value(com_rbuf, 119, INT_AB);
		sec=getUInt16Value(com_rbuf, 121, INT_AB);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		//This part has writen the NOx_range_drift_time 
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		status=0;
		
	}
	read_system_time(acq_data->acq_tm);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
int protocol_CEMS_CQChuanYiModBus_PS7400_F_CO_info(struct acquisition_data *acq_data)
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
	float CO_zero=0,CO_range=0,CO_K=0,CO_PPB=0,CO_zero_drift=0,CO_range_drift=0,CO_intercept=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;

	struct tm timer;
	time_t t1,t2,t3;

	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_CQChuanYiModBus_PS7400_F_CO_info1\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x38,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F CO INFO1 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F CO protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F CO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F CO INFO1 RECV1:",com_rbuf,size);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		CO_zero=f.f;

		
		f.ch[3]=com_rbuf[43];
		f.ch[2]=com_rbuf[44];
		f.ch[1]=com_rbuf[45];
		f.ch[0]=com_rbuf[46];
		CO_range=f.f;
		status=0;
		
	}else{
		CO_zero=0;
		CO_range=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,CO_zero,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,CO_range,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x5A,0x22);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F CO INFO1 SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F CO protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F CO data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F CO INFO2 RECV2:",com_rbuf,size);
	if((size>=73)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		CO_K=f.f;

		
		f.ch[3]=com_rbuf[47];
		f.ch[2]=com_rbuf[48];
		f.ch[1]=com_rbuf[49];
		f.ch[0]=com_rbuf[50];
		CO_PPB=f.f;
		status=0;
		
	}else{
		CO_K=0;
		CO_PPB=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13026",0,CO_K,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,CO_PPB,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x86,0x30);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F CO INFO3 SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F CO protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F CO data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F CO INFO3 RECV3:",com_rbuf,size);
	if((size>=101)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		CO_zero_drift=f.f;

		f.ch[3]=com_rbuf[43];
		f.ch[2]=com_rbuf[44];
		f.ch[1]=com_rbuf[45];
		f.ch[0]=com_rbuf[46];
		CO_range_drift=f.f;
		
		f.ch[3]=com_rbuf[75];
		f.ch[2]=com_rbuf[76];
		f.ch[1]=com_rbuf[77];
		f.ch[0]=com_rbuf[78];
		CO_intercept=f.f;
		
		status=0;
	}else{
		CO_zero_drift=0;
		CO_range_drift=0;
		CO_intercept=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,CO_zero_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,CO_range_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",0,CO_intercept,INFOR_ARGUMENTS,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xB8,0x60);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F CO INFO4 SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F CO protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F CO data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F CO INFO4 RECV4:",com_rbuf,size);
	if((size>=197)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		year=getUInt16Value(com_rbuf, 27, INT_AB);
		mon=getUInt16Value(com_rbuf, 29, INT_AB);
		day=getUInt16Value(com_rbuf, 31, INT_AB);
		hour=getUInt16Value(com_rbuf, 33, INT_AB);
		min=getUInt16Value(com_rbuf, 35, INT_AB);
		sec=getUInt16Value(com_rbuf, 37, INT_AB);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		//This part has writen the NOx_zero_drift_time 
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		year=getUInt16Value(com_rbuf, 123, INT_AB);
		mon=getUInt16Value(com_rbuf, 125, INT_AB);
		day=getUInt16Value(com_rbuf, 127, INT_AB);
		hour=getUInt16Value(com_rbuf, 129, INT_AB);
		min=getUInt16Value(com_rbuf, 131, INT_AB);
		sec=getUInt16Value(com_rbuf, 133, INT_AB);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		//This part describes the CO_range_drift_time 
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		status=0;
		
	}
	read_system_time(acq_data->acq_tm);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_CQChuanYiModBus_PS7400_F_HCL_info(struct acquisition_data *acq_data)
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
	float HCL_zero=0,HCL_range=0,HCL_K=0,HCL_PPB=0,HCL_zero_drift=0,HCL_range_drift=0,HCL_intercept=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;

	struct tm timer;
	time_t t1,t2,t3;

	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_CQChuanYiModBus_PS7400_F_HCL_info1\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x38,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F HCL INFO1 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F HCL protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F HCL data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F HCL INFO1 RECV1:",com_rbuf,size);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		HCL_zero=f.f;

		
		f.ch[3]=com_rbuf[51];
		f.ch[2]=com_rbuf[52];
		f.ch[1]=com_rbuf[53];
		f.ch[0]=com_rbuf[54];
		HCL_range=f.f;
		status=0;
		
	}else{
		HCL_zero=0;
		HCL_range=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,HCL_zero,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,HCL_range,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x5A,0x22);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F HCL INFO1 SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F HCL protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F HCL data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F HCL INFO2 RECV2:",com_rbuf,size);
	if((size>=73)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		HCL_K=f.f;

		
		f.ch[3]=com_rbuf[55];
		f.ch[2]=com_rbuf[56];
		f.ch[1]=com_rbuf[57];
		f.ch[0]=com_rbuf[58];
		HCL_PPB=f.f;
		status=0;
		
	}else{
		HCL_K=0;
		HCL_PPB=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13026",0,HCL_K,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,HCL_PPB,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x86,0x30);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F HCL INFO3 SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F HCL protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F HCL data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F CO INFO3 RECV3:",com_rbuf,size);
	if((size>=101)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		HCL_zero_drift=f.f;

		f.ch[3]=com_rbuf[51];
		f.ch[2]=com_rbuf[52];
		f.ch[1]=com_rbuf[53];
		f.ch[0]=com_rbuf[54];
		HCL_range_drift=f.f;
		
		f.ch[3]=com_rbuf[83];
		f.ch[2]=com_rbuf[84];
		f.ch[1]=com_rbuf[85];
		f.ch[0]=com_rbuf[86];
		HCL_intercept=f.f;

		status=0;
	}else{
		HCL_zero_drift=0;
		HCL_range_drift=0;
		HCL_intercept=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,HCL_zero_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,HCL_range_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",0,HCL_intercept,INFOR_ARGUMENTS,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xB8,0x60);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus PS7400-F HCL INFO4 SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus PS7400-F HCL protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus PS7400-F HCL data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus PS7400-F HCL INFO4 RECV4:",com_rbuf,size);
	if((size>=197)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		year=getUInt16Value(com_rbuf, 51, INT_AB);
		mon=getUInt16Value(com_rbuf, 53, INT_AB);
		day=getUInt16Value(com_rbuf, 55, INT_AB);
		hour=getUInt16Value(com_rbuf, 57, INT_AB);
		min=getUInt16Value(com_rbuf, 59, INT_AB);
		sec=getUInt16Value(com_rbuf, 61, INT_AB);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		//This part has writen the NOx_zero_drift_time 
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		year=getUInt16Value(com_rbuf, 147, INT_AB);
		mon=getUInt16Value(com_rbuf, 149, INT_AB);
		day=getUInt16Value(com_rbuf, 151, INT_AB);
		hour=getUInt16Value(com_rbuf, 153, INT_AB);
		min=getUInt16Value(com_rbuf, 155, INT_AB);
		sec=getUInt16Value(com_rbuf, 157, INT_AB);
		t3=getTimeValue(year, mon, day, hour, min, sec);
		//This part describes the HCL_range_drift_time 
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		status=0;
		
	}
	read_system_time(acq_data->acq_tm);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;  	
}

int protocol_CEMS_CQChuanYiModBus_PS7400_F_O2_info(struct acquisition_data *acq_data)
{
    	/*   There is nothing to be added at the moment. 	*/
		return 0;
}

int protocol_CEMS_CQChuanYiModBus_PS7400_F_NO_info(struct acquisition_data *acq_data)
{
    	/*   There is nothing to be added at the moment. 	*/
		return 0;
}
int protocol_CEMS_CQChuanYiModBus_PS7400_F_NO2_info(struct acquisition_data *acq_data)
{
    	/*   There is nothing to be added at the moment. 	*/
	return 0;
}



static float setValue_CQChuanYiModBus_PS7400_F(struct modbus_arg *modbusarg, float val, char *polcode)
{
	struct modbus_polcode_arg *mpolcodearg;
	int i=0;
	float valf=0;

	if(!modbusarg || !polcode) return -1;

	for(i=0;i<modbusarg->array_count ;i++)
	{
		mpolcodearg=&modbusarg->polcode_arg[i];
		if(!strcmp(mpolcodearg->polcode,polcode))
		{
		val=(val<5530) ? 5530 : val;
		
			valf=(val-5530)/(27648-5530);
			valf*=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			valf+=mpolcodearg->alarmMin;
			return valf;
		}

	}

	return 0;
}

int protocol_PLC_CQChuanYiModBus_PS7400_F(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf[5]={0};
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float speed,atm_press,PTC;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	devaddr=modbusarg->devaddr&0xffff;
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CQChuanYiModBus_PS7400_F PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CQChuanYiModBus_PS7400_F PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CQChuanYiModBus_PS7400_F PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CQChuanYiModBus_PS7400_F PLC RECV:",com_rbuf,size);
	if((size>=15)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		//flue gas temperature
		valf[0]=setValue_CQChuanYiModBus_PS7400_F(modbusarg,val,"a01012");
		SYSLOG_DBG("val %d\n",val);
		SYSLOG_DBG("valf[0] %d\n",valf[0]);
		
		//flue gas static pressure
		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		valf[1]=setValue_CQChuanYiModBus_PS7400_F(modbusarg,val,"a01013");

		//flue gas velocity
		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		valf[2]=setValue_CQChuanYiModBus_PS7400_F(modbusarg,val,"a01011");

		//flue gas humidity
/*	//del by miles zhang 20210701 , humidity in	 8040
		val=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];	
		valf[3]=setValue_CQChuanYiModBus_PS7400_F(modbusarg,val,"a01014");
*/		
/* //del by miles zhang 20210701
		//flue gas original value
		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		valf[4]=setValue_CQChuanYiModBus_PS7400_F(modbusarg,val,"a34013");
*/
/*//del by miles zhang 20210701
		if(PTC>0 && (valf[0]+273)>0 && (valf[1]+atm_press)>0)
			speed=PTC*valf[2]*sqrt(((valf[0]+273)/273)*(101325/(valf[1]+atm_press))*(2/1.2928));
		else
			speed=0;
*/		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		valf[2]=0;
		speed=0;
		valf[3]=0;
		valf[4]=0;
		status=1;
	}
	
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[0],&arg_n);
	//acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[1],&arg_n); //del by miles zhang conflict with 8009 TL_PMM180
	//acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);//del by miles zhang conflict with 8009 TL_PMM180
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[1],&arg_n);

	//acqdata_set_value(acq_data,"a01017",UNIT_PA,valf[2],&arg_n);
	
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,valf[2],&arg_n);
	
	//acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[3],&arg_n);

	//acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[4],&arg_n); //del by miles zhang 20210701

	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;
}

int protocol_PLC_CQChuanYiModBus_PS7400_F_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x03;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x09,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "CQChuanYi PS7400-F PLC INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, INT_AB);
		CQChuanYiModBus_PS7400_F_status=val;

		val=getUInt16Value(p, 5, INT_AB);
		CQChuanYiModBus_PS7400_F_alarm_status=val;

		val=getUInt16Value(p, 7, INT_AB);
		CQChuanYiModBus_PS7400_F_alarm_info=val;

		CQChuanYiModBus_PS7400_F_flag=1;
	}
	else
	{
		CQChuanYiModBus_PS7400_F_flag=0;
	}
	
	
	acq_data->acq_status = ACQ_OK;
	
	return 0;
}



