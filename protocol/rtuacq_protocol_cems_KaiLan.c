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

int protocol_CEMS_KaiLan(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float nox=0,so2=0,o2=0,co=0,co2=0;
	float nox_orig=0,so2_orig=0,o2_orig=0,co_orig=0,co2_orig=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct rdtu_info * rdtuinfo;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x14);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan CEMS RECV:",com_rbuf,size);
	if((size>=45)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		so2=getFloatValue(com_rbuf, 3, dataType);
		
		nox=NO_to_NOx(getFloatValue(com_rbuf, 7, dataType));
		
		o2=getFloatValue(com_rbuf, 11, dataType);
		
		so2_orig=getFloatValue(com_rbuf, 15, dataType);
		
		nox_orig=getFloatValue(com_rbuf, 19, dataType);
		
		o2_orig=getFloatValue(com_rbuf, 23, dataType);

		co=getFloatValue(com_rbuf, 27, dataType);

		co2=getFloatValue(com_rbuf, 31, dataType);
	
		co_orig=getFloatValue(com_rbuf, 35, dataType);

		co2_orig=getFloatValue(com_rbuf, 39, dataType);
		
		status=0;
	}
	else
	{
		nox=0;
		so2=0;
		o2=0;
		co=0;
		co2=0;
		nox_orig=0;
		so2_orig=0;
		o2_orig=0;
		co_orig=0;
		co2_orig=0;
		status=1;
	}

	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,nox_orig,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,so2,so2_orig,&arg_n);
	acqdata_set_value_orig(acq_data,"a19001",UNIT_PECENT,o2,o2_orig,&arg_n);
	acqdata_set_value_orig(acq_data,"a21005",UNIT_MG_M3,co,co_orig,&arg_n);
	acqdata_set_value_orig(acq_data,"a05001",UNIT_MG_M3,co2,co2_orig,&arg_n);

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,o2,&arg_n);


    rdtuinfo=get_parent_rdtu_info(acq_data);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x64,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan  STATUS RECV:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		int stat=0;
		stat=com_rbuf[3];
		stat<<=8;
		stat+=com_rbuf[4];

		if(stat==0) acq_data->dev_stat=0; //running
		if(stat==1) acq_data->dev_stat='M'; 
		if(stat==2) acq_data->dev_stat='D'; 
		if(stat==3) acq_data->dev_stat='C'; 
		if(stat==4) acq_data->dev_stat='z'; 
		if(stat==5) acq_data->dev_stat='C'; 
		if(stat==99) acq_data->dev_stat='D';

		rdtuinfo->dev_stat= acq_data->dev_stat;
	}else
		{
			rdtuinfo->dev_stat='D';
		}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



int protocol_CEMS_KaiLan_SO2_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_CEMS_KaiLan_SO2_info\n");

	if(!acqdataIsPolcodeEnable(acq_data, "a21026"))//added by miles zhang 20211116
		return 0;
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x64,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan SO2 STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan SO2 STATUS RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val=getUInt16Value(com_rbuf, 5, INT_AB);
		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		}

		val=getUInt16Value(com_rbuf, 7, INT_AB);
		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,8,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xC8,0x1A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan SO2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan SO2 INFO RECV1:",com_rbuf,size);
	if((size>=57)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 7, INT_AB);
		t3=getTimeValue(BCD(val),BCD(com_rbuf[9]), BCD(com_rbuf[10]), BCD(com_rbuf[11]),BCD(com_rbuf[12]), BCD(com_rbuf[13]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 23, dataType);
		valf=valf/1000;
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 31, INT_AB);
		t3=getTimeValue(BCD(val),BCD(com_rbuf[33]), BCD(com_rbuf[34]), BCD(com_rbuf[35]),BCD(com_rbuf[36]), BCD(com_rbuf[37]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 47, dataType);
		valf=(valf/100)+1;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x06,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan SO2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan SO2 INFO RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_KaiLan_NOx_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_CEMS_KaiLan_NOx_info\n");

	if(!acqdataIsPolcodeEnable(acq_data, "a21002"))//added by miles zhang 20211116
		return 0;
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xE2,0x1A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan NOx INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan NOx INFO RECV1:",com_rbuf,size);
	if((size>=57)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 7, INT_AB);
		t3=getTimeValue(BCD(val),BCD(com_rbuf[9]), BCD(com_rbuf[10]), BCD(com_rbuf[11]),BCD(com_rbuf[12]), BCD(com_rbuf[13]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 23, dataType);
		valf=valf/1000;
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 31, INT_AB);
		t3=getTimeValue(BCD(val),BCD(com_rbuf[33]), BCD(com_rbuf[34]), BCD(com_rbuf[35]),BCD(com_rbuf[36]), BCD(com_rbuf[37]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 47, dataType);
		valf=(valf/100)+1;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x08,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan NOx INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan NOx INFO RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_KaiLan_O2_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_CEMS_KaiLan_O2_info\n");

	if(!acqdataIsPolcodeEnable(acq_data, "a19001"))  //added by miles zhang 20211116
		return 0;
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xFC,0x1A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan O2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan O2 INFO RECV1:",com_rbuf,size);
	if((size>=57)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 7, INT_AB);
		t3=getTimeValue(BCD(val),BCD(com_rbuf[9]), BCD(com_rbuf[10]), BCD(com_rbuf[11]),BCD(com_rbuf[12]), BCD(com_rbuf[13]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 23, dataType);
		valf=valf/1000;
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 31, INT_AB);
		t3=getTimeValue(BCD(val),BCD(com_rbuf[33]), BCD(com_rbuf[34]), BCD(com_rbuf[35]),BCD(com_rbuf[36]), BCD(com_rbuf[37]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 47, dataType);
		valf=(valf/100)+1;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KaiLan O2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KaiLan O2 INFO RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


