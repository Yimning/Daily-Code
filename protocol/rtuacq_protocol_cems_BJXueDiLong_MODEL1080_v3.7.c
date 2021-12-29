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

int protocol_CEMS_BJXueDiLong_MODEL1080_v3_7(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0;
	int val=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	char *p;
	char flag=0;
	
	int bit7 = 0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x07;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v3.7 CEMS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		if((p[4]&0x80) == 1){
			 bit7 = 1;
		}
		
		so2=getUInt16Value(p, 5, dataType);
		if(bit7 == 1){
			so2*=0.1;
		}
		
		if(p[7]==2)
			so2=gas_ppm_to_mg_m3(so2, M_SO2);


		no=getUInt16Value(p, 9, dataType);
		if(bit7 == 1){
			no*=0.1;
		}
		
		if(p[11]==2)
			no=gas_ppm_to_mg_m3(no, M_NO);

		o2=getUInt16Value(p, 13, dataType);
		o2*=0.01;
		if(p[15]==2)
			o2=gas_ppm_to_pecent(o2);

		nox=NO_to_NOx(no);  

		val=p[4]&0x0F;
		switch(val)
		{
			case 1:	flag='N';break;
			case 3:	flag='N';break;
			case 4:	flag='N';break;
			case 5:	flag='C';break;
			case 6:	flag='C';break;
			default:	flag=0;break;
		}
		
		status=0;
	}
	else
	{
		nox=0;
		so2=0;
		o2=0;
		no=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	
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

int protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0D;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v3.7 SO2 INFO1", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=p[3];
		if(val!=0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else
		{
			val=p[4]&0x0F;
			switch(val)
			{
				case 0:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 1:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 3:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 4:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 5:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 6:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				default:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
		}

		val=getUInt16Value(p, 17, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[19];
		min=p[20];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, 21, dataType);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	sleep(1);
	cnt=0x09;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x18,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v3.7 SO2 INFO2", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[5];
		min=p[6];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, 7, dataType);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_NO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_NO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0A;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x07,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v3.7 NO INFO1", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[5];
		min=p[6];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, 15, dataType);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	sleep(1);
	cnt=0x09;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x21,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v3.7 NO INFO2", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[5];
		min=p[6];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, 7, dataType);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_O2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x04;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x07,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v3.7 O2 INFO1", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[5];
		min=p[6];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, 7, dataType);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	sleep(1);
	cnt=0x09;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x2A,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v3.7 O2 INFO2", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[5];
		min=p[6];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, 7, dataType);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


