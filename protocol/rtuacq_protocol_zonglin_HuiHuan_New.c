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

static float getStr2Float32Val(char* str1,int offset,int rLen)  
{  
	int a= 0;
	char str[20];
	float valf=0;
	char* p = str1;
	char* ret;
	union uf f;
	memset(str,0,sizeof(str));
	memcpy(str,&p[offset],rLen);
	SYSLOG_DBG("Str2Float32:%s\n",str);
	if(rLen !=8) return 0;
	if(rLen == 8)
	{
		f.ch[0]=HexCharToDec(str[0])*16+HexCharToDec(str[1]);
		f.ch[1]=HexCharToDec(str[2])*16+HexCharToDec(str[3]);
		f.ch[2]=HexCharToDec(str[4])*16+HexCharToDec(str[5]);
		f.ch[3]=HexCharToDec(str[6])*16+HexCharToDec(str[7]);
		valf  = f.f;
		SYSLOG_DBG("Str2Float32 result:%f\n",valf);
	}
       return valf;  
}

static char* BCC_Check_OK(char *string_ok, char* string, int len)
{
	int bccRes = 0;
	char res[20]={0};
	char *check_ok = NULL;
	char bccResToHex[2] = {0};
	int i = 0;
	
	memset(res,0,sizeof(res));
	memcpy(res,&string[len-3],2);
	check_ok = res ;
	
	for(i=0;i<=len-4;i++)
	{
		bccRes = bccRes ^ string[i];
	}
	sprintf(bccResToHex,"%X",bccRes);
	SYSLOG_DBG("res:%s\n",check_ok);
	SYSLOG_DBG("bccResToHex:%s\n",bccResToHex);
	if(strcmp(bccResToHex,check_ok)==0)
	{
		string_ok = string;
	}else
	{
	  	string_ok = NULL;
	}
	return string_ok;
}

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.08.07 Sun.
Description:protocol_ZONGLIN_HuiHuan
TX:%01#RDD0152101525**
RX:%01$RD1202020621400000054712
DataType and Analysis:
	(Float) 
*/
int protocol_ZONGLIN_HuiHuan_New(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[40]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	char data_buff[20]={0};
	int tem=0;
	float zonglin=0; 
	char flag = 0;
	int val = 0 ; 
	char *p = NULL;
	char *string_ok = NULL;
	char *check = NULL;
	
	if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0152001525**\r");
	size=strlen(com_tbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN SEND:",com_tbuf,size);
	size =write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN RECV:",com_rbuf,size);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,ZONGLIN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("HuiHuan ZONGLIN data: %s",com_rbuf);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=33)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&(check!=NULL))
	{
		strncpy(data_buff,&com_rbuf[22],8);
		tem=atoi(data_buff);
		zonglin=((float)tem)/100.0;


		sscanf(&com_rbuf[6],"%04X",&val);
		switch(val){
			case 0: flag='N'; break;
			case 256: flag='M'; break;
			case 512: flag='N'; break;
			case 768: flag='K'; break;
			default: flag='N'; break;
		}
	}
	else
	{
		status=1;
		flag='D';
		zonglin=0;
	}
	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,zonglin,&arg_n);

	
	if(status == 0){
		acq_data->dev_stat=flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.08.07 Sun.
Description:protocol_ZONGLIN_HuiHuan_New_info
TX1:%01#RDD0079800800**<CR>
RX1:%01$RD01000000010017<CR>

RX2:%01$RD2100010011000900040000000000000000000000000000000000000000100000000000000000001921000100110009000500<CR>
TX2:%01#RDD0187001894**<CR>

RX3:%01$RD6E7F864100000000000000000000000000000000E7088C4200000000000000000000803F0000000016<CR>
TX3:%01#RDD3269632715**<CR>

RX4:%01$RD480D0000480D000094110000480d00009411000030006E<CR>
TX4:%01#RDD0025000260**<CR>

RX5:%01$RD0000A0400000A0400000A04000000000000000000000C84263<CR>
TX5:%01#RDD3266032671**<CR>

RX6:%01$RD210001001100090005000000000000000000C84215<CR>
TX6:%01#RDD0178501794**<CR>

RX7:%01$RD28001B<CR>
TX7:%01#RDD0173701737**<CR>

RX8:%01$RD08001B<CR>
TX8:%01#RDD0190701907**<CR>

DataType and Analysis:
	(FLOAT_DCBA):0000A040 =  5.0
	(INT_BA):0500 = 5

*/
protocol_ZONGLIN_HuiHuan_New_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[255]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int val = 0;
	float valf = 0;
	char *p = NULL;
	char *string_ok = NULL;
	char *check = NULL;
	char *ret;
	char str[20];

	struct tm timer;
	time_t t1,t2,t3;
	t1 =0;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	strcpy(com_tbuf,"%01#RDD0079800800**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN INFO SEND1:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN INFO RECV1:",com_rbuf,size);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=20)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL)&&(check!=NULL))
	{
		SYSLOG_DBG("p= %s\n",p);
		sscanf(&p[6],"%04X",&val);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 256: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 512: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 768: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		
		sscanf(&p[11],"%04X",&val);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 256: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 2304: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n); break;
			case 3072: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		#if 0
		f.ch[0]=HexCharToDec(com_rbuf[18])*16+HexCharToDec(com_rbuf[19]);
		f.ch[1]=HexCharToDec(com_rbuf[20])*16+HexCharToDec(com_rbuf[21]);
		f.ch[2]=HexCharToDec(com_rbuf[22])*16+HexCharToDec(com_rbuf[23]);
		f.ch[3]=HexCharToDec(com_rbuf[24])*16+HexCharToDec(com_rbuf[25]);
		andan=f.f;
		#endif
		
		status=0;
	}
	else
	{
		status=1;
	}

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0187001894**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN INFO SEND2:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN INFO RECV2:",com_rbuf,size);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=108)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL)&&(check!=NULL))
	{
		year = getStr2Int16Val(com_rbuf, 6, 4);
		mon=getStr2Int16Val(com_rbuf, 10, 4);
		day=getStr2Int16Val(com_rbuf, 14, 4);
		hour=getStr2Int16Val(com_rbuf, 18, 4);
		min=getStr2Int16Val(com_rbuf, 22, 4);
		sec=0;
		timer.tm_year=BCD(year)+100;
		timer.tm_mon=BCD(mon)-1;
		timer.tm_mday=BCD(day);
		timer.tm_hour=BCD(hour);
		timer.tm_min=BCD(min);
		timer.tm_sec=BCD(sec);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);	


		year = getStr2Int16Val(com_rbuf, 86, 4);
		mon=getStr2Int16Val(com_rbuf, 90, 4);
		day=getStr2Int16Val(com_rbuf, 94, 4);
		hour=getStr2Int16Val(com_rbuf, 98, 4);
		min=getStr2Int16Val(com_rbuf, 102, 4);
		sec=0;
		timer.tm_year=BCD(year)+100;
		timer.tm_mon=BCD(mon)-1;
		timer.tm_mday=BCD(day);
		timer.tm_hour=BCD(hour);
		timer.tm_min=BCD(min);
		timer.tm_sec=BCD(sec);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);			
	

		status=0;
	}
	else
	{
		status=1;
	}

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD3269632715**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN INFO SEND3:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN INFO RECV3:",com_rbuf,size);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=88)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL)&&(check!=NULL))
	{
		valf = getStr2Float32Val(com_rbuf, 6, 8);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
	 	valf = getStr2Float32Val(com_rbuf, 46, 8);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		//acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,0,INFOR_ARGUMENTS,&arg_n);
		
		valf = getStr2Float32Val(com_rbuf, 70, 8);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);	

		valf = getStr2Float32Val(com_rbuf, 78, 8);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);	
		
	
		status=0;
	}
	else
	{
		status=1;
	}

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0025000260**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN INFO SEND4:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN INFO RECV4:",com_rbuf,size);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=52)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL)&&(check!=NULL))
	{
	 	val = getStr2Int16Val(com_rbuf, 6, 4);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		val = getStr2Int16Val(com_rbuf, 14, 4);
		acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		val = getStr2Int16Val(com_rbuf, 46, 4);
       		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}
	else
	{
		status=1;
	}



	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD3266032671**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN INFO SEND5:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,INFO5 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN INFO RECV5:",com_rbuf,size);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=56)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL)&&(check!=NULL))
	{

		valf = getStr2Float32Val(com_rbuf, 6, 8);
		acqdata_set_value_flag(acq_data,"i13109",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getStr2Float32Val(com_rbuf, 14, 8);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getStr2Float32Val(com_rbuf, 46, 8);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}


	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0178501794**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN INFO SEND6:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,INFO6 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN INFO RECV6:",com_rbuf,size);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=48)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL)&&(check!=NULL))
	{

		year = getStr2Int16Val(com_rbuf, 6, 4);
		mon=getStr2Int16Val(com_rbuf, 10, 4);
		day=getStr2Int16Val(com_rbuf, 14, 4);
		hour=getStr2Int16Val(com_rbuf, 18, 4);
		min=getStr2Int16Val(com_rbuf, 22, 4);
		sec=0;
		timer.tm_year=BCD(year)+100;
		timer.tm_mon=BCD(mon)-1;
		timer.tm_mday=BCD(day);
		timer.tm_hour=BCD(hour);
		timer.tm_min=BCD(min);
		timer.tm_sec=BCD(sec);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13128",t3,0.0,INFOR_ARGUMENTS,&arg_n);	

		valf = getStr2Float32Val(com_rbuf, 38, 8);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else
	{
		status=1;
	}


	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0173701737**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN INFO SEND7:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,INFO7 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN INFO RECV7:",com_rbuf,size);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=12)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL)&&(check!=NULL))
	{

		val = getStr2Int16Val(com_rbuf, 6, 4);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}
	else
	{
		status=1;
	}


	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0190701907**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"HuiHuan ZONGLIN INFO SEND8:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuiHuan ZONGLIN protocol,INFO8 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"HuiHuan ZONGLIN INFO RECV8:",com_rbuf,size);
	check = BCC_Check_OK(string_ok, com_rbuf,size);
	if((size>=12)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0')&&((p=memstr(com_rbuf, size, "%01$RD"))!=NULL)&&(check!=NULL))
	{

		val = getStr2Int16Val(com_rbuf, 6, 4);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}
	else
	{
		status=1;
	}



	if(status == 0)
			acq_data->acq_status = ACQ_OK;
	else 
			acq_data->acq_status = ACQ_ERR;
    return arg_n;
}

