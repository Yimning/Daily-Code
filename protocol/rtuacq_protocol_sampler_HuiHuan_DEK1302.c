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
//#include "devices_mng_api.h"

/*
return : com_f arguments amount;
*/

static int getStr2Int16Val(char* str1,int offset,int rLen)  
{  
	int a= 0;
	char str[50]={0};
	char src[50]={0};
	char dest[50]={0};
	char* p = str1;
	char* ret;
	memset(str,0,sizeof(str));
	memcpy(str,&p[offset],rLen);
	if(rLen==4)
	{
		strncpy(src, &p[offset+0],2);
		strncpy(dest, &p[offset+2],2);
	    	strcat(dest, src);
		sscanf(dest,"%4X",&a);
		SYSLOG_DBG("getStr2Int16Val result:%d\n",a);
	}
       return a;  
}

int protocol_sampler_HuiHuan_DEK1302(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[40]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	char data_buff[20]={0};
	int tem=0;
	float andan=0; 
	char flag = 0;
	int val = 0 ; 
	float valf = 0;
	char *p = NULL;
	char *string_ok = NULL;
	char *check = NULL;
	int devaddr =0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0060100601**\r");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan DEK-1302 sampler SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan DEK-1302 sampler protocol,sampler : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuiHuan DEK-1302 sampler data",com_rbuf,size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan DEK-1302 sampler RECV:",com_rbuf,size);
	//check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=12)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL))
	{
		val = getStr2Int16Val(com_rbuf, 6, 4);
		if(val == 1)
			val = 0;
		else 
			val = 1;
		status=0;
	}
	else
	{
		val=0;
		status=1;
	}

	acqdata_set_value(acq_data,"i42001",UNIT_NONE,val,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_sampler_HuiHuan_DEK1302_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[40]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	char data_buff[20]={0};
	int tem=0;
	float andan=0; 
	char flag = 0;
	int val = 0 ; 
	float valf = 0;
	char *p = NULL;
	char *string_ok = NULL;
	char *check = NULL;
	int devaddr =0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_sampler_HuiHuan_DEK1302_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"SAMPLER",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0060100601**\r");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan DEK1302 SAMPLER INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan DEK1302 SAMPLER INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuiHuan DEK1302 SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan DEK1302 SAMPLER INFO RECV1:",com_rbuf,size);
	//check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=12)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL))
	{
		val = getStr2Int16Val(com_rbuf, 6, 4);
		if(val == 1)
			val = 0;
		else 
			val = 1;
		
		acqdata_set_value(acq_data,"i42001",UNIT_NONE,val,&arg_n);
		
		status=0;
	}
	else
		status=1;

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0060900610**\r");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan DEK1302 SAMPLER INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan DEK1302 SAMPLER INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuiHuan DEK1302 SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan DEK1302 SAMPLER INFO RECV2:",com_rbuf,size);
	//check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=16)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL))
	{
		val = getStr2Int16Val(com_rbuf, 6, 4);
		acqdata_set_value_flag(acq_data,"i43003",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		status=0;
	}else  status = 1;


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0061100611**\r");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan DEK1302 SAMPLER INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan DEK1302 SAMPLER INFO protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuiHuan DEK1302 SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan DEK1302 SAMPLER INFO RECV3:",com_rbuf,size);
	//check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=12)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL))
	{
		val = getStr2Int16Val(com_rbuf, 6, 4);
		switch(val)
		{
			case 1:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,1,INFOR_ARGUMENTS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,2,INFOR_ARGUMENTS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,3,INFOR_ARGUMENTS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,4,INFOR_ARGUMENTS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
		}
		
		status=0;
	}
	else 
		status = 1;


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0080600806**\r");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan DEK1302 SAMPLER INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan DEK1302 SAMPLER INFO protocol,INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuiHuan DEK1302 SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan DEK1302 SAMPLER INFO RECV4:",com_rbuf,size);
	//check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=12)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL))
	{
		val = getStr2Int16Val(com_rbuf, 6, 4);
		if(val>0 && val<26)
			acqdata_set_value_flag(acq_data,"i43006",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i43006",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else 
		status = 1;


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0080700808**\r");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan DEK1302 SAMPLER INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan DEK1302 SAMPLER INFO protocol,INFO 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuiHuan DEK1302 SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan DEK1302 SAMPLER INFO RECV5:",com_rbuf,size);
	//check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=16)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL))
	{
		val = getStr2Int16Val(com_rbuf, 6, 4);
		acqdata_set_value_flag(acq_data,"i43005",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else 
		status = 1;

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0081000810**\r");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan DEK1302 SAMPLER INFO SEND6:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan DEK1302 SAMPLER INFO protocol,INFO 6: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuiHuan DEK1302 SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan DEK1302 SAMPLER INFO RECV6:",com_rbuf,size);
	//check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=12)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL))
	{
		switch(val)
		{
			case 1:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,2,INFOR_ARGUMENTS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,5,INFOR_ARGUMENTS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,6,INFOR_ARGUMENTS,&arg_n);break;
		}
		
		status=0;
	}
	else 
		status = 1;

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

