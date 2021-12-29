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

char CEMS_XiKeMaiHaKe_MCS100FT_STATUS = 'N';

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.10.19 Tues.
Description:protocol_CEMS_XiKeMaiHaKe_MCS100FT
TX:01 04 00 89 00 12 A1 ED 
RX: 04 24 00 00 40 C8 00 00 41 C8 00 00 42 C8 00 00 43 C8 00 00 44 C8 00 00 45 C8 00 00 46 C8 00 00 47 C8 00 00 48 C8 CF C4

DataType and Analysis:
	(FLOAT_CDAB) 00 00 42 C8  = 100
*/
int protocol_CEMS_XiKeMaiHaKe_MCS100FT(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 9

	char *polcode[POLCODE_NUM]={
		"a21024a","a21001a","a21005a","a21003a","a21026a",
		"a21004a","a01014","a19001a","a21002a"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,
		UNIT_PECENT,	UNIT_PECENT,	UNIT_MG_M3
	};


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
	int cmd = 0,regpos = 0,regcnt = 0;
	float valf = 0;
	int val = 0;
	char *p=NULL;
	char flag=0;
	float valfloat[POLCODE_NUM]= {0};

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	cmd = 0x02;
	regpos = 0xFA0;
	regcnt = 0x01;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = com_rbuf[3];
		if (val == 1)
			CEMS_XiKeMaiHaKe_MCS100FT_STATUS='D';
		else 
		{
			cmd = 0x02;
			regpos = 0xFA3;
			regcnt = 0x01;
			dataType = FLOAT_CDAB;
			t1 = 0;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS1 SEND:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS1 data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS1 RECV:",com_rbuf,size);
			p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
			if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
			{
				val = com_rbuf[3];
				if (val == 1)
					CEMS_XiKeMaiHaKe_MCS100FT_STATUS = 'N';
				status=0;
			}

			sleep(1);	
			cmd = 0x02;
			regpos = 0x1187;
			regcnt = 0x01;
			dataType = FLOAT_CDAB;
			t1 = 0;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS2 SEND:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS2 data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS2 RECV:",com_rbuf,size);
			p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
			if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
			{
				val = com_rbuf[3];
				if (val == 1)
					CEMS_XiKeMaiHaKe_MCS100FT_STATUS='M';
			}

			sleep(1);
			cmd = 0x02;
			regpos = 0xFA6;
			regcnt = 0x01;
			dataType = FLOAT_CDAB;
			t1 = 0;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS4 SEND:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS4 data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS4 RECV:",com_rbuf,size);
			p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
			if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
			{

				val = com_rbuf[3];
				if (val == 1) 
					CEMS_XiKeMaiHaKe_MCS100FT_STATUS='C';
			}


			sleep(1);
			cmd = 0x02;
			regpos = 0x1185;
			regcnt = 0x01;
			dataType = FLOAT_CDAB;
			t1 = 0;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS5 SEND:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS5 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS5 data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS5 RECV:",com_rbuf,size);
			p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
			if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
			{
				val = com_rbuf[3];
				if (val == 1)
					CEMS_XiKeMaiHaKe_MCS100FT_STATUS='z';
			}

		}
		
	}
	else
	{
		status=1;
		CEMS_XiKeMaiHaKe_MCS100FT_STATUS='D';
	}


	cmd = 0x04;
	regpos = 0x8A;
	regcnt = 0x22;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT  CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT  CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT  CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valfloat[0] = getFloatValue(p, 3, dataType);
		valfloat[1] = getFloatValue(p, 11, dataType);
		valfloat[2] = getFloatValue(p, 15, dataType);
		valfloat[3] = getFloatValue(p, 19, dataType);
		valfloat[4] = getFloatValue(p, 27, dataType);
		valfloat[5] = getFloatValue(p, 31, dataType);
		valfloat[6] = getFloatValue(p, 43, dataType);
		valfloat[7] = getFloatValue(p, 47, dataType);
		valfloat[8] = getFloatValue(p, 67, dataType);
		
		for(i=0;i<POLCODE_NUM;i++)
		{
			//valfloat[i] = getFloatValue(p, 3+i*4, dataType);
			acqdata_set_value_flag(acq_data,polcode[i],unit[i],valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21024a"))
				acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21001a"))
				acqdata_set_value_flag(acq_data,"a21001",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21005a"))
				acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21003a"))
				acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21026a"))
				acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21004a"))
				acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a19001a"))
				acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21002a"))
				acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);

		}
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			valfloat[i] = 0;
			acqdata_set_value_flag(acq_data,polcode[i],unit[i],valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21024a"))
				acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21001a"))
				acqdata_set_value_flag(acq_data,"a21001",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21005a"))
				acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21003a"))
				acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21026a"))
				acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21004a"))
				acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a19001a"))
				acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
			if(!strcmp(polcode[i],"a21002a"))
				acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valfloat[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
		}
		status=1;
	}

	acqdata_set_value_flag(acq_data,"a21024z",UNIT_MG_M3,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"a21001z",UNIT_MG_M3,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"a21005z",UNIT_MG_M3,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"a21003z",UNIT_MG_M3,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"a21026z",UNIT_MG_M3,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"a21004z",UNIT_MG_M3,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"a21002z",UNIT_MG_M3,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
	

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;

}

int protocol_CEMS_XiKeMaiHaKe_MCS100FT_SO2_info(struct acquisition_data *acq_data)
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
	long longVal=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
			
	cmd = 0x02;
	regpos = 0xFA0;
	regcnt = 0x01;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = com_rbuf[3];
		if (val == 1)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);

			sleep(1);
			cmd = 0x02;
			regpos = 0xFA3;
			regcnt = 0x01;
			dataType = FLOAT_CDAB;
			t1 = 0;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS1 SEND:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS1 data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS1 RECV:",com_rbuf,size);
			p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
			if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
			{
				val = com_rbuf[3];
				if (val == 1) 
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);

				status=0;
			}

			sleep(1);
			cmd = 0x02;
			regpos = 0x1187;
			regcnt = 0x01;
			dataType = FLOAT_CDAB;
			t1 = 0;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS2 SEND:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS2 data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS2 RECV:",com_rbuf,size);
			p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
			if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
			{
				val = com_rbuf[3];
				if (val == 1)
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				status=0;
			}

			sleep(1);
			cmd = 0x02;
			regpos = 0xFA6;
			regcnt = 0x01;
			dataType = FLOAT_CDAB;
			t1 = 0;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS4 SEND:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS4 data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS4 RECV:",com_rbuf,size);
			p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
			if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
			{
				val = com_rbuf[3];
				if (val == 1)
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				status=0;
			}

			sleep(1);
			cmd = 0x02;
			regpos = 0x1185;
			regcnt = 0x01;
			dataType = FLOAT_CDAB;
			t1 = 0;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  STATUS5 SEND:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  STATUS protocol,STATUS5 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  STATUS5 data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  STATUS5 RECV:",com_rbuf,size);
			p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
			if((size>=4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
			{
				val = com_rbuf[3];
				if (val == 1)
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				status=0;
			}

		}
	}
	else
	{
		status=1;
	}



	cmd = 0x04;
	regpos = 0x2A6;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  INFO1 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		longVal =  getInt32Value(p, 3,  LONG_CDAB);
		t1 = longVal ;
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x2C0;
	regcnt = 0x04;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  INFO1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  INFO1 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  INFO1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  INFO1 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p,3 , dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p,7 , dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x340;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  INFO2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  INFO protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  INFO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  INFO2 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		longVal =  getInt32Value(p, 3,  LONG_CDAB);
		t1 = longVal ;
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x360;
	regcnt = 0x04;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  INFO3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  INFO protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  INFO3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  INFO3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p,3 , dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p,7 , dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}



	sleep(1);
	cmd = 0x04;
	regpos = 0x4DE;
	regcnt = 0x04;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  INFO4 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  INFO protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  INFO4 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  INFO4 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf =  getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x502;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT SO2  INFO5 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT SO2  INFO protocol,INFO5 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT SO2  INFO5 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT SO2  INFO5 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

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


int protocol_CEMS_XiKeMaiHaKe_MCS100FT_HCL_info(struct acquisition_data *acq_data)
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
	long longVal=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	cmd = 0x04;
	regpos = 0x2A6;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT HCL  INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT HCL  INFO1 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT HCL  INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT HCL  INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		longVal =  getInt32Value(p, 3, LONG_CDAB);
		t1 = longVal ;
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x2A8;
	regcnt = 0x04;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT HCL  INFO1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT HCL  INFO1 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT HCL  INFO1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT HCL  INFO1 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p,3 , dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p,7 , dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x346;
	regcnt = 0x06;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT HCL  INFO2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT HCL  INFO protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT HCL  INFO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT HCL  INFO2 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		longVal =  getInt32Value(p, 3,  LONG_CDAB);
		t1 = longVal ;
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p,7 , dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p,11 , dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x4C6;
	regcnt = 0x04;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT HCL  INFO3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT HCL  INFO protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT HCL  INFO3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT HCL  INFO3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf =  getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x4F6;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT HCL  INFO4 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT HCL  INFO protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT HCL  INFO4 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT HCL  INFO4 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

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



int protocol_CEMS_XiKeMaiHaKe_MCS100FT_CO_info(struct acquisition_data *acq_data)
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
	long longVal=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	cmd = 0x04;
	regpos = 0x2A6;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT CO  INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT CO  INFO1 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT CO  INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT CO  INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		longVal =  getInt32Value(p, 3,  LONG_CDAB);
		t1 = longVal ;
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x2B4;
	regcnt = 0x04;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT CO  INFO1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT CO  INFO1 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT CO  INFO1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT CO  INFO1 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p,3 , dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p,7 , dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x344;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT CO  INFO2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT CO  INFO protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT CO  INFO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT CO  INFO2 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		longVal =  getInt32Value(p, 3,  LONG_CDAB);
		t1 = longVal ;
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x354;
	regcnt = 0x04;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT CO  INFO3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT CO  INFO protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT CO  INFO3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT CO  INFO3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p,3 , dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p,7 , dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}



	sleep(1);
	cmd = 0x04;
	regpos = 0x4D2;
	regcnt = 0x04;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT CO  INFO4 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT CO  INFO protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT CO  INFO4 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT CO  INFO4 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf =  getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x4FC;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT CO  INFO5 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT CO  INFO protocol,INFO5 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT CO  INFO5 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT CO  INFO5 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

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


int protocol_CEMS_XiKeMaiHaKe_MCS100FT_NOx_info(struct acquisition_data *acq_data)
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
	long longVal=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;

return 0;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	acqdata_set_value_flag(acq_data,"21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	cmd = 0x04;
	regpos = 0x50E;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT NOx  INFO5 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT NOx  INFO protocol,INFO5 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT NOx  INFO5 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT NOx  INFO5 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

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


int protocol_CEMS_XiKeMaiHaKe_MCS100FT_O2_info(struct acquisition_data *acq_data)
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
	long longVal=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;

return 0;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x04;
	regpos = 0x50C;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	t1 = 0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT O2  INFO5 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT O2  INFO protocol,INFO5 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT O2  INFO5 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT O2  INFO5 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

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

