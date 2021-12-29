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

char CEMS_JuGuang_HeBei_JinDing_STATUS;

int protocol_CEMS_JuGuang_HeBei_JinDing(struct acquisition_data *acq_data)
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
	float nox;
	int index=0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

#define CEMS_POLCODE_NUM 4
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026a","a21002a","a19001a","a01014"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_PECENT,  UNIT_PECENT
	};

	int r_addr[CEMS_POLCODE_NUM]={
		0,		44,		88,	188		
	};


	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0,0xBE);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang JinDing CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang JinDing CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang JinDing CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang JinDing CEMS RECV1:",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang JinDing CEMS RECV2:",&com_rbuf[200],size);
	//p = modbus_crc_check(com_rbuf,size, devaddr, 0x03, 0xBE);
	if((size>=385)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			index=(r_addr[i]-0)*2+3;
			
			f.ch[3]=com_rbuf[index+2];
			f.ch[2]=com_rbuf[index+3];
			f.ch[1]=com_rbuf[index+0];
			f.ch[0]=com_rbuf[index+1];

			if(!strcmp(polcode[i],"a21002a")) 
			{
				acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,f.f,&arg_n);   //Added  by Yimning 20211113
				acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,f.f,&arg_n);
				
				f.f *= 1.53;
			}
			acqdata_set_value(acq_data,polcode[i],unit[i],f.f,&arg_n);
		}
		status=0;
	}
	else
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}
		status=1;
	}
	

	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,0,&arg_n);

	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);

/*
    if(status == 0)
    {
    	float stat=0;
		index=(381-0)*2+3;
		f.ch[0]=com_rbuf[index+1];
		f.ch[1]=com_rbuf[index+0];
		f.ch[2]=com_rbuf[index+3];
		f.ch[3]=com_rbuf[index+2];

		stat=f.f;

		if((stat>=1.9999) && (stat<=2.0001) || 
			(stat>=2.9999) && (stat<=3.0001)) acq_data->dev_stat='C';
		if((stat>=3.9999) && (stat<=4.0001) || 
			(stat>=7.9999) && (stat<=8.0001) ||
			(stat>=8.9999) && (stat<=9.0001) ||
			(stat>=9.9999) && (stat<=10.0001)) acq_data->dev_stat='D';		
		if((stat>=10.9999) && (stat<=11.0001) ) acq_data->dev_stat='F';	
		if((stat>=4.9999) && (stat<=5.0001) || 
			(stat>=5.9999) && (stat<=6.0001) ||
			(stat>=6.9999) && (stat<=7.0001) ) acq_data->dev_stat='M';	

		CEMS_JuGuang_HeBei_JinDing_STATUS=acq_data->dev_stat;
    }
 */  

	if(status == 0)
	{
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x03,0x017C,0x01);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "JuGuang JinDing CEMS STATUS", 1);
		p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x01);
		if(p!=NULL)
		{
			switch(p[4])
			{
				case 1:	flag='N';break;
				case 2:	flag='C';break;
				case 3:	flag='C';break;
				case 4:	flag='N';break;
				case 5:	flag='M';break;
				case 6:	flag='z';break;
				case 7:	flag='z';break;
				case 8:	flag='N';break;
				case 9:	flag='N';break;
				case 10:	flag='N';break;
				case 11:	flag='N';break;
				default:	flag='N';break;
			}
		}
		else
		{
			flag='N';
		}
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
		
	else 
		acq_data->acq_status = ACQ_ERR;


	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_CEMS_JuGuang_HeBei_JinDing_SO2_info(struct acquisition_data *acq_data)
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
	char *p = NULL;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	

	if(!acq_data) return -1;
	
	SYSLOG_DBG("protocol_CEMS_JuGuang_HeBei_JinDing_SO2_info\n");

	 ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	dataType=FLOAT_CDAB;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x017C,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang JinDing CEMS STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang JinDing CEMS STATUS protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang JinDing CEMS STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang JinDing CEMS STATUS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, 0x03, 0x03);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(p != NULL))
	{

	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang JinDing CEMS STATUS RECV21:",p,11);
	
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		switch(val)
		{
			case 1:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		if(val==0)
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
		
		status=0;
	}
	else status = 1 ;
	

	sleep(1);
	 ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	 
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x02,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang JinDing SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang JinDing SO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang JinDing SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang JinDing SO2 INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, 0x03, 0x20);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(p!= NULL))
	{
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang JinDing CEMS STATUS RECV22:",p,0x20*2+5);
	
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(com_rbuf, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 27, INT_AB);
		t1=getTimeValue(val, com_rbuf[29], com_rbuf[30], com_rbuf[31], com_rbuf[32], com_rbuf[34]);
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 47, INT_AB);
		t1=getTimeValue(val, com_rbuf[49], com_rbuf[50], com_rbuf[51], com_rbuf[52], com_rbuf[54]);
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 59, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 63, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else status = 1 ;

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_JuGuang_HeBei_NOx_JinDing_iinfo(struct acquisition_data *acq_data)
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
	char *p = NULL;
	
	struct tm timer;
	time_t t1,t2,t3;

	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_JuGuang_HeBei_NOx_JinDing_iinfo\n");
	 ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_CDAB;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x2E,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JinDing NOx INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JinDing NOx protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JinDing NOx data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JinDing NOx INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, 0x03, 0x20);
	if((size>=41)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(p!= NULL))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(com_rbuf, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 27, INT_AB);
		t1=getTimeValue(val, com_rbuf[29], com_rbuf[30], com_rbuf[31], com_rbuf[32], com_rbuf[34]);
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 47, INT_AB);
		t1=getTimeValue(val, com_rbuf[49], com_rbuf[50], com_rbuf[51], com_rbuf[52], com_rbuf[54]);
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 59, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 63, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else status = 1 ;

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_JuGuang_HeBei_O2_JinDing_iinfo(struct acquisition_data *acq_data)
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
	char *p = NULL;
	
	struct tm timer;
	time_t t1,t2,t3;

	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
 ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_JuGuang_HeBei_O2_JinDing_iinfo\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_CDAB;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x5A,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JinDing O2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JinDing O2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JinDing O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JinDing O2 INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, 0x03, 0x20);
	if((size>=41)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(p!= NULL))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(com_rbuf, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 27, INT_AB);
		t1=getTimeValue(val, com_rbuf[29], com_rbuf[30], com_rbuf[31], com_rbuf[32], com_rbuf[34]);
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 39, INT_AB);
		t1=getTimeValue(val, com_rbuf[41], com_rbuf[42], com_rbuf[43], com_rbuf[44], com_rbuf[45]);
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		
		status=0;
	}
	else status = 1 ;

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


