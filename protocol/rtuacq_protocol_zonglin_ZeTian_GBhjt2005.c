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

int protocol_ZONGLIN_ZeTian_GBhjt2005(struct acquisition_data *acq_data)
{
	int status=0;
	float tp=0;
	char com_rbuf[2048]={0}; 
	int size=0;
	char *p,*q;
	int arg_n=0;
	int val=0;
	char flag=0;
	int ret=0;
	static char polcode[10]="101";
	char str[50];

	if(!acq_data) return -1; 

	size=getStrDataPack(DEV_NAME(acq_data), NULL, 0, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP", 1);
	if((p=strstr(com_rbuf,"CN=2011"))!=NULL)
	{
		q=p;
		if(((p=strstr(p,"CP=&&"))==NULL) || (strstr(&p[5],"&&")==NULL))
		{
			size+=getStrDataPack(DEV_NAME(acq_data), NULL, 0, &com_rbuf[size], sizeof(com_rbuf)-size, "ZeTian GBhjt2005 TP again", 1);
			p=strstr(q,"CP=&&");
		}

		memset(str,0,sizeof(str));
		sprintf(str,";%s-Rtd",polcode);
		if((p!=NULL) && ((p=strstr(p,str))!=NULL))
		{
			memset(str,0,sizeof(str));
			sprintf(str,";%s-Rtd=%%f,%s-Flag=%%c",polcode,polcode);
			sscanf(p,str,&tp,&flag);
			switch(flag)
			{
				case 'N':	flag='N';break;
				case 'M':	flag='M';break;
				case 'S':	flag='S';break;
				case 'T':	flag='T';break;
				case 'F':	flag='F';break;
				case 'D':	flag='D';break;
				case 'C':	flag='C';break;
				case 'A':	flag='N';break;
				case 'B':	flag='C';break;
				case 'Z':	flag='C';break;
				case 'Q':	flag='M';break;
				default:	flag=0;break;
			}
			status=0;
		}
		else
		{
			tp=0;
			status=1;
		}
		
	}
	else
	{
		tp=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w21011",UNIT_MG_L,tp,&arg_n);	

	if(status == 0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,30);

	return arg_n;

}

int protocol_ZONGLIN_ZeTian_GBhjt2005_status(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val=0;
	float valf=0;
	char *p;
	struct tm timer;
	time_t t1,t2,t3;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_ZeTian_GBhjt2005_status\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	acqdata_set_value_flag(acq_data,"w21011",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0062ST=32;CN=5015;PW=123456;MN=00000000000001;AL=;CP=&&DataTime=&&C181\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP status", 1);
	if(((p=strstr(com_rbuf,"CN=5015"))!=NULL) && ((p=strstr(p,";AL="))!=NULL))
	{
		sscanf(p,";AL=%d",&val);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 12:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
	}
	

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
	
}

int protocol_ZONGLIN_ZeTian_GBhjt2005_info1(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val=0;
	float valf=0,valf2=0;
	char *p;
	struct tm timer;
	time_t t1,t2,t3;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_ZeTian_GBhjt2005_info1\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	acqdata_set_value_flag(acq_data,"w21011",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0072ST=32;CN=5019;PW=123456;MN=00000000000001;RA=;MRL=;MRH=;CP=&&DataTime=&&1400\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info1.1", 1);
	if(((p=strstr(com_rbuf,"CN=5019"))!=NULL) && ((p=strstr(p,";MRH="))!=NULL))
	{
		sscanf(p,";MRH=%f",&valf);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0068ST=32;CN=5012;PW=123456;MN=00000000000001;RCK=;RCB=;CP=&&DataTime=&&0CC0\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info1.2", 1);
	if(((p=strstr(com_rbuf,"CN=5012"))!=NULL) && ((p=strstr(p,";RCK="))!=NULL))
	{
		sscanf(p,";RCK=%f;RCB=%f",&valf,&valf2);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf2,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
	}
	

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
	
}

int protocol_ZONGLIN_ZeTian_GBhjt2005_info2(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val=0;
	float valf=0,valf2=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	char *p;
	struct tm timer;
	time_t t1,t2,t3;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_ZeTian_GBhjt2005_info2\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	acqdata_set_value_flag(acq_data,"w21011",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0065ST=32;CN=5031;PW=123456;MN=00000000000001;GS=;A=;CP=&&DataTime=&&8201\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info2.1", 1);
	if(((p=strstr(com_rbuf,"CN=5031"))!=NULL) && ((p=strstr(p,";GS="))!=NULL))
	{
		sscanf(p,";GS=%f;A=%f;CP=&&DataTime=%04d%02d%02d%02d%02d%02d",&valf,&valf2,&year,&mon,&day,&hour,&min,&sec);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf2,INFOR_ARGUMENTS,&arg_n);

		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13107",t1,0,INFOR_ARGUMENTS,&arg_n);
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0065ST=32;CN=5011;PW=123456;MN=00000000000001;SC=;A=;CP=&&DataTime=&&EE01\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info2.2", 1);
	if(((p=strstr(com_rbuf,"CN=5011"))!=NULL) && ((p=strstr(p,";SC="))!=NULL))
	{
		sscanf(p,";SC=%f;A=%f;CP=&&DataTime=%04d%02d%02d%02d%02d%02d",&valf,&valf2,&year,&mon,&day,&hour,&min,&sec);
		acqdata_set_value_flag(acq_data,"i13109",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,valf2,INFOR_ARGUMENTS,&arg_n);

		t1=getTimeValue(year, mon, day, hour, min, sec);
		//acqdata_set_value_flag(acq_data,"i13107",t1,0,INFOR_ARGUMENTS,&arg_n);
	}
	

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
	
}

int protocol_ZONGLIN_ZeTian_GBhjt2005_info3(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val=0;
	float valf=0,valf2=0;
	char *p;
	struct tm timer;
	time_t t1,t2,t3;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_ZeTian_GBhjt2005_info3\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	acqdata_set_value_flag(acq_data,"w21011",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0068ST=32;CN=5033;PW=123456;MN=00000000000001;CFk=;CFb=;CP=&&DataTime=&&4100\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info3.1", 1);
	if(((p=strstr(com_rbuf,"CN=5033"))!=NULL) && ((p=strstr(p,";CFk="))!=NULL))
	{
		sscanf(p,";CFk=%f;CFb=%f",&valf,&valf2);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,valf2,INFOR_ARGUMENTS,&arg_n);
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0068ST=32;CN=5016;PW=123456;MN=00000000000001;OTH=;IST=;CP=&&DataTime=&&0140\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info3.2", 1);
	if(((p=strstr(com_rbuf,"CN=5016"))!=NULL) && ((p=strstr(p,";IST="))!=NULL))
	{
		sscanf(p,";IST=%d;",&val);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
	}
	

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
	
}

int protocol_ZONGLIN_ZeTian_GBhjt2005_info4(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val=0;
	float valf=0,valf2=0;
	char *p;
	struct tm timer;
	time_t t1,t2,t3;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_ZeTian_GBhjt2005_info4\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	acqdata_set_value_flag(acq_data,"w21011",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0063ST=32;CN=5020;PW=123456;MN=00000000000001;DTI=;CP=&&DataTime=&&4DC0\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info4.1", 1);
	if(((p=strstr(com_rbuf,"CN=5020"))!=NULL) && ((p=strstr(p,";DTI="))!=NULL))
	{
		sscanf(p,";DTI=%d",&val);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0063ST=32;CN=5021;PW=123456;MN=00000000000001;DTE=;CP=&&DataTime=&&7080\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info4.2", 1);
	if(((p=strstr(com_rbuf,"CN=5021"))!=NULL) && ((p=strstr(p,";DTE="))!=NULL))
	{
		sscanf(p,";DTE=%d;",&val);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);
	}
	

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
	
}

int protocol_ZONGLIN_ZeTian_GBhjt2005_info5(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val=0;
	float valf=0,valf2=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	char *p;
	struct tm timer;
	time_t t1,t2,t3;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_ZeTian_GBhjt2005_info5\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	acqdata_set_value_flag(acq_data,"w21011",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"##0090ST=32;CN=5051;PW=123456;MN=00000000000001;SCS=;CS=;A=;Dev=;Sign=;CP=&&DataTime=&&6FC0\r\n");
	size=strlen(com_tbuf);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian GBhjt2005 TP info5.1", 1);
	if(((p=strstr(com_rbuf,"CN=5051"))!=NULL) && ((p=strstr(p,";SCS="))!=NULL))
	{
		sscanf(p,";SCS=%f;CS=%*f;A=%*f;Dev=%f;Sign=%*d;CP=&&DataTime=%04d%02d%02d%02d%02d%02d",&valf,&valf2,&year,&mon,&day,&hour,&min,&sec);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,valf2,INFOR_ARGUMENTS,&arg_n);

		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13128",t1,0,INFOR_ARGUMENTS,&arg_n);
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
	
}

