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


static unsigned  CalGBCRC16(char *pData, int Length) //added by miles zhang 20211006
{
 unsigned char i; // loop counter
 unsigned CRC_acc = 0xffff;
 #define POLY 0xA001
	 while(Length--)
	 {
		  CRC_acc = CRC_acc ^ (*pData);
		  for (i = 0; i < 8; i++)
		  {
			   if (CRC_acc & 0x0001)
			   {
			    CRC_acc = (CRC_acc >> 1);
			    CRC_acc ^= POLY;
			   }
			   else
			   {
			    CRC_acc = CRC_acc >> 1;
			   }
		  }
	  pData++;
	 }
         return CRC_acc;
}

static unsigned CRC16_ChuoMei_hjt2005_HeBei(char* buf, int len)
{
	unsigned int r;
	unsigned char hi;
	char flag;
	int i, j;

	r = 0xffff;
	for(j=0;j<len;j++)
	{
		hi = r >> 8;
		hi ^= buf[j];
		r = hi;

		for(i=0;i<8;i++)
		{
			flag = r & 0x0001;
			r = r >> 1;
			if(flag == 1)
				r ^= 0xa001;
		}
	}
	return r;
}

static char *get_now_time_string()
{
	struct timeval tm;
	int systm[TIME_ARRAY]={0};
	static char qntm[100];
	
	memset(qntm,0,sizeof(qntm));

	gettimeofday(&tm,NULL);
	read_system_time(systm);

	sprintf(qntm,"%04d%02d%02d%02d%02d%02d%03d",
	systm[5],systm[4],systm[3],systm[2],systm[1],systm[0],tm.tv_usec/1000);

	return qntm;
}

static int create_pack_head_and_tail_ChuoMei_hjt2005_HeBei(char *packet, int *pack_size, unsigned int devaddr)
{
	int len=0;
	unsigned int CRC=0;
	char str[20]={0};

	len = strlen(&packet[10]);
	//CRC = CRC16_ChuoMei_hjt2005_HeBei(&packet[10], len);
	CRC =CalGBCRC16(&packet[10], len);
	memset(str, 0, sizeof(str));
	sprintf(str, "##%04d%04d",devaddr,len);
	memcpy(packet, str, 10);
	memset(str, 0, sizeof(str));
	sprintf(str,"%02X%02X\r\n",CRC&0xff,(CRC>>8)&0xff); //mod by miles zhang 20211006

	memcpy(&packet[len+10], str, 6);

	*pack_size = len+16;

	return 0;
}

static int create_cmd_pack_ChuoMei_hjt2005_HeBei(char *com_tbuf, unsigned int devaddr, char *cmd)
{
	int size=0;

	if(!com_tbuf || !cmd)
		return 0;

	sprintf(&com_tbuf[10],"QN=%s;ST=31;CN=%s;PW=123456;MN=88888880000001;Flag=3;CP=&&&&",get_now_time_string(),cmd);

	create_pack_head_and_tail_ChuoMei_hjt2005_HeBei(com_tbuf,&size,devaddr);

	return size;
}

static char *check_ChuoMei_hjt2005_HeBei(char *pack, char *cmd, unsigned int devaddr)
{
	char *p;
	char str[20]={0};

	if(!pack || !cmd)
		return NULL;

	memset(str,0,sizeof(str));
	sprintf(str,"##%04d",devaddr);
	p=strstr(pack, str);
	if(p==NULL)
		return NULL;

	memset(str,0,sizeof(str));
	sprintf(str,";CN=%s;",cmd);
	p=strstr(p, str);
	if(p==NULL)
		return NULL;

	p=strstr(p, "CP=&&");
	if(p==NULL)
		return NULL;

	p+=5;

	if(!strstr(p,"&&"))
		return NULL;

	return p;
	
}

static int get_val_for_ChuoMei_hjt2005_HeBei_pack(char *data, char *polcode, char *name, void *value, int type)
{
	char str[50]={0};
	char *p;
	int ret=0;
	int *val;
	float *valf;
	int year=0, mon=0, day=0, hour=0, min=0, sec=0;
	time_t *t1;

	if(!data || !polcode || !name || !value)
		return -1;

	memset(str,0,sizeof(str));
	sprintf(str,";%s-%s",polcode,name);
	
	p=strstr(data,str);
	if(p==NULL)
	{
#if 0
		memset(str,0,sizeof(str));
		sprintf(str,";%s-%s");
#else
		str[0]=',';
#endif
		p=strstr(data,str);
		if(p==NULL)
			return -2;
	}

	switch(type)
	{
		case 0:
			strcat(str,"=%f");
			valf=(float *)value;
			ret=sscanf(p,str,valf);
			break;

		case 1:
			val=(int *)value;
			strcat(str,"=%d");
			ret=sscanf(p,str,val);
			break;
			
		case 2:
			t1 = (time_t *)value;
			strcat(str,"=%04d%02d%02d%02d%02d%02d");
			ret=sscanf(p,str,&year,&mon,&day,&hour,&min,&sec);
			*t1=getTimeValue(year,mon,day,hour,min,sec);
			break;

		default:
			ret=0;
			break;
	}
		
	if(ret!=1 && ret!=6)
		return -3;

	return 0;
}

static int isZsVal(char *polcode)
{
	int len=0;

	if(!polcode)
		return 0;

	len=strlen(polcode);

	if(polcode[len-1]=='z')
		return 1; /*true*/

	return 0; /*false*/
}

static char ChuoMei_hjt2005_HeBei_status;
static int ChuoMei_hjt2005_HeBei_flag;

int protocol_CEMS_ChuoMei_hjt2005_HeBei(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 19

	char polcode17[POLCODE_NUM][10]={
		"a19001a",	"a19001",		"a01011",		"a01012",		"a01017",
		"a01014",		"a01015",		"a01013",		"a34013a",	"a34013",
		"a34013z",	"a21026a",	"a21026",		"a21026z",	"a21005a",
		"a21005",		"a21005z",	"a00000",		"a00000z"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_PECENT,	UNIT_PECENT,	UNIT_M_S,		UNIT_0C,			UNIT_PA,
		UNIT_PECENT,	UNIT_0C,			UNIT_PA,			UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_M3_H,		UNIT_M3_H
	};

	char polcodeChuoMei[POLCODE_NUM][10]={
		"S01",	"S01",	"S02",	"S03",	"S04",
		"S05",	"S06",	"S08",	"01",		"01",
		"01",		"02",		"02",		"02",		"04",	
		"04",		"04",		"100",	"100"
	};

	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[512]={0};
	char str[20]={0};
	int size=0;
	float valf=0;
	float nox=0,nox_zs=0,no=0,no_zs=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int i=0;
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=create_cmd_pack_ChuoMei_hjt2005_HeBei(com_tbuf,devaddr,"2013");
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ChuoMei hjt2005 HeBei CEMS", 2);
	p=check_ChuoMei_hjt2005_HeBei(com_rbuf, "2011", devaddr);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			memset(str,0,sizeof(str));
			if(isZsVal(polcode17[i]))
				strcpy(str,"ZsRtd");
			else
				strcpy(str,"Rtd");
			ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcodeChuoMei[i], str, &valf, 0);

			SYSLOG_DBG("CEMS_ChuoMei:%s,%s,valf=%f,ret=%d\n",polcodeChuoMei[i], str,valf,ret);
			
			if(ret<0)
				valf=0;
			acqdata_set_value(acq_data,polcode17[i],unit[i],valf,&arg_n);
		}

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, "03", "Rtd", &no, 0);
		SYSLOG_DBG("CEMS_ChuoMei:03,Rtd,valf=%f,ret=%d\n",no,ret);
		if(ret<0)
			no=0;

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, "03", "ZsRtd", &no_zs, 0);
		SYSLOG_DBG("CEMS_ChuoMei:03,ZsRtd,valf=%f,ret=%d\n",no_zs,ret);
		if(ret<0)
			no_zs=0;

		if(isPolcodeEnable(modbusarg, "a21003"))
		{
			nox=NO_to_NOx(no);
			nox_zs=NO_to_NOx(no_zs);
		
			acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
			acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
			acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,no_zs,&arg_n);
		}
		else
		{
			nox=no;
			nox_zs=no_zs;
		}

		acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
		acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
		acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox_zs,&arg_n);
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode17[i],unit[i],0,&arg_n);
		}
		acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
		
		status=1;
	}

	if(status == 0)
	{
		if(ChuoMei_hjt2005_HeBei_flag==1)
		{
			acq_data->dev_stat = ChuoMei_hjt2005_HeBei_status;
		}
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,20);

#undef POLCODE_NUM

	return arg_n;
}

int protocol_CEMS_ChuoMei_hjt2005_HeBei_SO2(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[512]={0};
	char polcode[10]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int flag=0;
	union uf f;
	char *p;
	int dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ChuoMei_hjt2005_HeBei_SO2\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=create_cmd_pack_ChuoMei_hjt2005_HeBei(com_tbuf,devaddr,"2023");
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ChuoMei hjt2005 HeBei CEMS STATUS", 2);
	p=check_ChuoMei_hjt2005_HeBei(com_rbuf, "2021", devaddr);
	if(p!=NULL)
	{
		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, "SB1", "InstrState", &val, 1);
		if(ret<0)
		{
			val=-1;
			ChuoMei_hjt2005_HeBei_flag=0;
		}
		else
		{
			ChuoMei_hjt2005_HeBei_flag=1;
		}
		
		switch(val)
		{
			case 0:	flag=0; ChuoMei_hjt2005_HeBei_status='N';break;
			case 1:	flag=1; ChuoMei_hjt2005_HeBei_status='M';break;
			case 2:	flag=3; ChuoMei_hjt2005_HeBei_status='C';break;
			case 3:	flag=5; ChuoMei_hjt2005_HeBei_status='z';break;
			case 4:	flag=3; ChuoMei_hjt2005_HeBei_status='C';break;
			case 5:	flag=3; ChuoMei_hjt2005_HeBei_status='C';break;
			case 6:	flag=3; ChuoMei_hjt2005_HeBei_status='C';break;
			case 7:	flag=3; ChuoMei_hjt2005_HeBei_status='C';break;
			default:	flag=99; ChuoMei_hjt2005_HeBei_status='N';break;
		}
		acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,flag,INFOR_STATUS,&arg_n);


		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, "SB1", "Ala", &val, 1);
		if(ret<0)
			val=-1;
		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			if((val&0x0001)==0x0001)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((val&0x0002)==0x0002)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x0004)==0x0004)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			else if((val&0x0008)==0x0008)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x0010)==0x0010)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,8,INFOR_STATUS,&arg_n);
			else if((val&0x0020)==0x0020)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x0040)==0x0040)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x0080)==0x0080)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x0100)==0x0100)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x0200)==0x0200)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x0400)==0x0400)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x0800)==0x0800)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x1000)==0x1000)
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);	
		}
		
		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=create_cmd_pack_ChuoMei_hjt2005_HeBei(com_tbuf,devaddr,"1073");
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ChuoMei hjt2005 HeBei SO2 INFO", 2);
	p=check_ChuoMei_hjt2005_HeBei(com_rbuf, "1073", devaddr);
	if(p!=NULL)
	{
		memset(polcode,0,sizeof(polcode));
		strcpy(polcode,"02");

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ScaleGasNd", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailDate", &t1, 2);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "FullRange", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ZeroDev", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailDev", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ZeroOrigin", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailOrigin", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "RealOrigin", &valf, 0);
		if(ret==0)
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

int protocol_CEMS_ChuoMei_hjt2005_HeBei_NOx(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[512]={0};
	char polcode[10]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int flag=0;
	union uf f;
	char *p;
	int dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ChuoMei_hjt2005_HeBei_NOx\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(isPolcodeEnable(modbusarg, "a21003"))
		acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=create_cmd_pack_ChuoMei_hjt2005_HeBei(com_tbuf,devaddr,"1073");
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ChuoMei hjt2005 HeBei NOx INFO", 2);
	p=check_ChuoMei_hjt2005_HeBei(com_rbuf, "1073", devaddr);
	if(p!=NULL)
	{
		memset(polcode,0,sizeof(polcode));
		strcpy(polcode,"03");

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ScaleGasNd", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailDate", &t1, 2);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "FullRange", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ZeroDev", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailDev", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ZeroOrigin", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailOrigin", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "RealOrigin", &valf, 0);
		if(ret==0)
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

int protocol_CEMS_ChuoMei_hjt2005_HeBei_O2(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[512]={0};
	char polcode[10]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int flag=0;
	union uf f;
	char *p;
	int dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ChuoMei_hjt2005_HeBei_O2\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=create_cmd_pack_ChuoMei_hjt2005_HeBei(com_tbuf,devaddr,"1073");
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ChuoMei hjt2005 HeBei O2 INFO", 2);
	p=check_ChuoMei_hjt2005_HeBei(com_rbuf, "1073", devaddr);
	if(p!=NULL)
	{
		memset(polcode,0,sizeof(polcode));
		strcpy(polcode,"S01");

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ScaleGasNd", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailDate", &t1, 2);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "FullRange", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ZeroDev", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailDev", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "ZeroOrigin", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "CailOrigin", &valf, 0);
		if(ret==0)
			acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		ret=get_val_for_ChuoMei_hjt2005_HeBei_pack(p, polcode, "RealOrigin", &valf, 0);
		if(ret==0)
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

