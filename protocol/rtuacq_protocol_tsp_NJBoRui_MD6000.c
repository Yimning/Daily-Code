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
Create Time:2021.10.15 Sun.
Description:protocol_TSP_NJBoRui_MD6000

TX:01 03 01 00 00 02 C5 F7 
RX:01 03 04 42 48 00 00  6E 5D

DataType and Analysis:
	(FLOAT_CDAB)  00 00  42 48   = 50.00
*/


static char NJBoRui_MD6000_flag;
int protocol_TSP_NJBoRui_MD6000(struct acquisition_data *acq_data)
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
	
	cmd = 0x04;
	regpos = 0x100;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3,  dataType);
		
		status=0;
	}
	else
	{
		status=1;
	}
	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf,&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	
	if(status == 0)
	{
		acq_data->dev_stat = NJBoRui_MD6000_flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_TSP_NJBoRui_MD6000_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_TSP_NJBoRui_MD6000\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	cmd = 0x03;
	regpos = 0x801;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP INFO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
			val = getInt16Value(p, 3, INT_AB);

			if(val == 0){
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				NJBoRui_MD6000_flag='N';
			}
			else if((val & 0x100) == 0x100)
			{
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				NJBoRui_MD6000_flag='N';
			}
			else if((val & 0x40) == 0x40)
			{
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				NJBoRui_MD6000_flag='C';
			}
			else if((val & 0x400) == 0x400)
			{
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				NJBoRui_MD6000_flag='D';
			}else
			{
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				NJBoRui_MD6000_flag='N';
			}
		status=0;
		
	}
	else
	{
		status = 1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x05;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		//status = 1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x0F;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV3:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		//status = 1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x23;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV4:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		//status = 1;
	}


	sleep(1);
	cmd = 0x04;
	regpos = 0x2D;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data5",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV5:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		t1 = getTimeValue(val, BCD(p[5]), BCD(p[6]), BCD(p[7]), BCD(p[8]), BCD(p[9]));
		
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		//status = 1;
	}
	

	sleep(1);
	cmd = 0x04;
	regpos = 0x37;
	regcnt = 0x04;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND6:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO6: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data6",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV6:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		t1 = getTimeValue(val, BCD(p[5]), BCD(p[6]), BCD(p[7]), BCD(p[8]), BCD(p[9]));
		
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		//status = 1;
	}


	sleep(1);
	cmd = 0x04;
	regpos = 0x41;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND7:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO7: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data7",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV7:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		//status = 1;
	}


	sleep(1);
	cmd = 0x04;
	regpos = 0x4B;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND8:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO8: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data8",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV8:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		//status = 1;
	}
	
	sleep(1);
	cmd = 0x04;
	regpos = 0x55;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND9:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO9: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data9",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV9:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		//status = 1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x5F;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND10:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO 10: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data10",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV10:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		valf = valf * 0.01;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		//status = 1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x69;
	regcnt = 0x02;
	dataType = FLOAT_CDAB ;
	t1 = 0 ;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJBoRui MD6000 TSP INFO SEND11:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJBoRui MD6000 TSP protocol,INFO 11: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJBoRui MD6000 TSP data11",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJBoRui MD6000 TSP INFO RECV11:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		valf = valf * 0.01;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		//status = 1;
	}

	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



