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
Email:1148967988@qq.com
Create Time:2021.12.29 Wed.
Description:protocol_TSP_ZhongXing_DM601_Low

TX:01 03 00 00 00 04 44 09 
RX:01 03 08 42 48 00 00 42 48 00 00 4C 54

DataType and Analysis:
	(FLOAT_ABCD)   42 48 00 00  = 50.00
*/

static float smoke_orig =0 ; 
static char ZhongXing_DM601_Low_Mark = 'N';
int protocol_TSP_ZhongXing_DM601_Low(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 1

	char *polcode[POLCODE_NUM]={
		"a34013a"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3
	};


	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float smoke=0;
	int ret=0;
	int arg_n=0;
	int i;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x04;
	dataType = FLOAT_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongXing DM601 Low CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongXing DM601 Low CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongXing DM601 Low CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongXing DM601 Low CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{

		valf = getFloatValue(p, 3,  dataType);
		smoke_orig = getFloatValue(p, 7 , dataType); 
		
		acqdata_set_value_orig(acq_data,polcode[0],unit[0],valf,smoke_orig,&arg_n);
		
		if(!strcmp(polcode[i],"a34013a"))
			acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf,&arg_n);

		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
			if(!strcmp(polcode[i],"a34013a"))
				acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);			
		}
		status=1;
	}

	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	
	if(status == 0)
	{
		acq_data->acq_status = ACQ_OK;
		acq_data->dev_stat = ZhongXing_DM601_Low_Mark;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.12.29 Wed.
Description:protocol_TSP_ZhongXing_DM601_Low_info

TX1:01 03 00 64 00 03 44 14 
RX1:01 03 06 00 00 00 00 00 00 21 75

TX2:01 03 00 C8 00 2C C4 2A 
RX2:01 03 58 42 C8 00 00 20 20 10 16 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
20 20 10 16 09 00 00 00 42 C8 00 00 3F 80 00 00 00 00 00 00 FF FF FF FF 
3F 00 00 00 FF FF FF FF FF FF FF FF FF FF FF FF 40 90 00 00 00 00 00 00 40 90 00 00 00 00 00 00 00 00 00 00  48 85

DataType and Analysis:
	(FLOAT_ABCD)   42 C8 00 00  = 100.00
*/

int protocol_TSP_ZhongXing_DM601_Low_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	union uf f;
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_TSP_TL_PMM180_Low\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	cmd = 0x03;
	regpos = 0x64;
	regcnt = 0x03;
	dataType = FLOAT_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongXing DM601 Low TSP INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongXing DM601 Low TSP INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongXing DM601 Low TSP INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongXing DM601 Low TSP INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
  		val = getInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n); ZhongXing_DM601_Low_Mark = 'N';break;
			case 1: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n); ZhongXing_DM601_Low_Mark = 'M';break;
			case 2: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n); ZhongXing_DM601_Low_Mark = 'D';break;
			case 3: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n); ZhongXing_DM601_Low_Mark = 'C'; break;
			default: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n); ZhongXing_DM601_Low_Mark = 'N'; break;
		}

		val = getInt16Value(p, 5, INT_AB);
		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		}

  		val = getInt16Value(p, 7, INT_AB);
		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		status=0;
	}
	else
	{
		status = 1;
	}

	
	cmd = 0x03;
	regpos = 0xC8;
	regcnt = 0x2C;
	dataType = FLOAT_ABCD ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongXing DM601 Low TSP INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongXing DM601 Low TSP protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongXing DM601 Low TSP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongXing DM601 Low TSP INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

 		val= getInt16Value(p,7, INT_AB);
		t3 = getTimeValue(BCD(val), BCD(p[9]),BCD(p[10]),BCD(p[11]),BCD(p[12]), BCD(p[14]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		valf = getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);


		valf = getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
	
		valf = getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

 		val= getInt16Value(p,31, INT_AB);
		t3 = getTimeValue(BCD(val), BCD(p[33]),BCD(p[34]),BCD(p[35]),BCD(p[36]), BCD(p[38]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 51, dataType);
		//acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13015",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 59, dataType);  //OUTPUT 0xFFFFFFFF
		//acqdata_set_value_flag(acq_data,"i13014",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		valf = getFloatValue(p, 63, dataType);
		//acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 67, dataType);
		//acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		valf = getFloatValue(p, 71, dataType);
		//acqdata_set_value_flag(acq_data,"i13016",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 75, dataType);
		//acqdata_set_value_flag(acq_data,"i13017",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		valf = getFloatValue(p, 79, dataType);
		//acqdata_set_value_flag(acq_data,"i13018",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);


		valf = getFloatValue(p, 83, dataType);
		//acqdata_set_value_flag(acq_data,"i13019",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		
		valf = getFloatValue(p, 87, dataType);
		//acqdata_set_value_flag(acq_data,"i13020",UNIT_L_M,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status = 1;
	}
	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



