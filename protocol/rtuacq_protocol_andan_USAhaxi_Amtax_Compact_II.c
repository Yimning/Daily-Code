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

/*
Author:Yimning
Time:2021.08.04 Wed.
Description:protocol_ANDAN_USAHaXi_maxII_New
TX:01 03 00 00 00 02 C4 0B 

RX:01 03 04 85 20 3D 5D  E1 5F
DataType and Analysis:
	(FLOAT_CDAB) 85 20 3D 5D   = 0.054
*/
int protocol_ANDAN_USAHaXi_maxII_New(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_andan=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);   
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"USAHaXi maxII ANDAN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("USAHaXi maxII ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("USAHaXi maxII ANDAN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"USAHaXi maxII ANDAN RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		total_andan=f.f;
		status=0;
	}
	else
	{
		total_andan=0;
		status=1;
	}
	

	ret=acqdata_set_value(acq_data,"w21003",UNIT_MG_L,total_andan,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

/*
Author:Yimning
Time:2021.08.04 Wed.
Description:protocol_ANDAN_USAHaXi_maxII_New_info
TX:01 03 00 04 00 51 C5 F7 

RX:01 03 A2
00 00 00 00
82 20 3D 5D
82 20 3D 5D
00 01 00 02
07 DF 02 72 06 C2 02 0D 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
07 DF 02 72 06 C2 02 0D
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00
82 20 3D 5D
00 00 00 00
00 00
82 20 3D 5D 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 01 33 41 03 00 00 00 00 00 00 00 01 33 41 03 00 00 00 00
82 20 3D 5D 00 00 00 00 00 00 00 01 33 41 03 00 00 00 00 00
00 00
FF FF
DataType and Analysis:
	(FLOAT_CDAB) 85 20 3D 5D   = 0.054
*/

int protocol_ANDAN_USAHaXi_maxII_New_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	unsigned long int val=0;
	int devaddr=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ANDAN_USAHaXi_maxII_New_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	sleep(2);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x04,0x51);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"USAHaXi maxII ANDAN INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	dataType = FLOAT_CDAB;
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("USAHaXi maxII ANDAN INFO protocol,INFO: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("USAHaXi maxII ANDAN INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"USAHaXi maxII ANDAN INFO RECV:",com_rbuf,size);
	if((size>=167)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val = getInt16Value(com_rbuf, 99, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			default :		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
			
		//acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		//acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);


		valf = getFloatValue(com_rbuf, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(com_rbuf, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(com_rbuf, 17, INT_AB);
		switch(val){
				case 0:	acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,0,INFOR_ARGUMENTS,&arg_n);break;
				case 1:	acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,30,INFOR_ARGUMENTS,&arg_n);break;
				case 2:	acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,60,INFOR_ARGUMENTS,&arg_n);break;
				case 3:	acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,120,INFOR_ARGUMENTS,&arg_n);break;
				case 4:	acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,240,INFOR_ARGUMENTS,&arg_n);break;
				case 5:	acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,360,INFOR_ARGUMENTS,&arg_n);break;
				case 6:	acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,1440,INFOR_ARGUMENTS,&arg_n);break;
		}
		
		year = getInt16Value(com_rbuf ,67, INT_AB);
		val = getInt16Value(com_rbuf, 69,  INT_AB);
		mon = val /100;
		day = val %100;
		val = getInt16Value(com_rbuf, 71,  INT_AB);
		hour =  val /100;
		min = val%100;
		sec = getInt16Value(com_rbuf, 73,  INT_AB);
		t1 = getTimeValue( year,  mon,  day,  hour,  min,  sec);
		acqdata_set_value_flag(acq_data,"i13101",t1,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13107",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		
		valf = getFloatValue(com_rbuf, 91, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		
	        valf = getFloatValue(com_rbuf, 101, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);	

		valf = getFloatValue(com_rbuf, 143, dataType);
		acqdata_set_value_flag(acq_data,"i13118",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		val = getInt16Value(com_rbuf, 163, INT_AB);
		switch(val){
				case 0:	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,12,INFOR_ARGUMENTS,&arg_n);break;
				case 1:	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,120,INFOR_ARGUMENTS,&arg_n);break;
				case 2:	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,1200,INFOR_ARGUMENTS,&arg_n);break;
				case 3:	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,30,INFOR_ARGUMENTS,&arg_n);break;
		}

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

