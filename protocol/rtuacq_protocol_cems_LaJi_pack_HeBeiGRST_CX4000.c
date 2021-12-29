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

#include "rtuacq_protocol_error_cache.h"

static int HeBeiGRST_cems_get_valf_CX4000(char *com_rbuf,unsigned int size,unsigned int regaddr,float *valf)
{
	union uf  f;
	unsigned int datalen=0;
	int datapos=0;

	if(!com_rbuf || !valf) return -1;

	if(regaddr<0) return -1;

	datapos=3+(regaddr/2)*4;

	f.ch[0]=com_rbuf[1+datapos];
	f.ch[1]=com_rbuf[0+datapos];
	f.ch[2]=com_rbuf[3+datapos];
	f.ch[3]=com_rbuf[2+datapos];
	
	*valf=f.f;

	SYSLOG_DBG("HeBeiGRST_cems_get_valf %d,%x,%f\n",regaddr,f.l&0xffffffff,f.f);
		
	return 0;
}


int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   //char *pcomrbuff;
   int size=0;
   int arg_n=0;
   int ret=0;
   float valf=0; 
   union uf f;
   char devstat=0;

#define HeBeiGRST_CX4000_POLCODE_NUM 9
	char *polcodestr[HeBeiGRST_CX4000_POLCODE_NUM]={
		"a21026","a21002","a21004","a21001","a21005",
		"a05001","a21024","a21018","a01014"
	};
	UNIT_DECLARATION unitstr[HeBeiGRST_CX4000_POLCODE_NUM]={
		UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
		UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_PECENT
	};

	int r_addr[HeBeiGRST_CX4000_POLCODE_NUM]={
		0,	2,	4,	6,  8,
		10,	12,	14,	16
	};

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int devaddr=0,devfun=0;
   
   int i=0;

   if(!acq_data) return -1;

   acq_data->total=0;

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg; 
   devaddr=modbusarg_running->devaddr;
   devfun=modbusarg_running->devfun;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 0x00,0x12);
	SYSLOG_DBG_HEX("HeBeiGRST CX4000 data send",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000 SEND:",com_tbuf,size);
	
	sleep(2);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HeBeiGRST CX4000 protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HeBeiGRST CX4000 data RECV",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000 RECV:",com_rbuf,size);

	if(modbus_crc_check_ok(com_rbuf, size, devaddr, 0x03,0x12)!=0) goto ERROR_PACK;

	if(size>=41 && com_rbuf[0]==devaddr && com_rbuf[1]==0x03 && com_rbuf[2]==(0x12*2))
	{

		for(i=0;i<HeBeiGRST_CX4000_POLCODE_NUM;i++)
		{
			mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			if(mpolcodearg_temp)
			{
				valf=0.0;
				ret=HeBeiGRST_cems_get_valf_CX4000(com_rbuf,size,(r_addr[i]),&valf);
				
				if(ret<0) 
				{
					valf=0.0;
				}
			}


			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
		}
	}
    else
    {
    
ERROR_PACK:    
	    ret=acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a21001",UNIT_MG_M3,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a05001",UNIT_MG_M3,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a21024",UNIT_MG_M3,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a21018",UNIT_MG_M3,0,&arg_n);	
		ret=acqdata_set_value(acq_data,"a01014",UNIT_PECENT,0,&arg_n);

		status=1;
	}

    ret=acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	ret=acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	ret=acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);
	ret=acqdata_set_value(acq_data,"a21001z",UNIT_MG_M3,0,&arg_n);
	ret=acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);
	//ret=acqdata_set_value(acq_data,"a05001z",UNIT_MG_M3,0,&arg_n);
	ret=acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,0,&arg_n);
	//ret=acqdata_set_value(acq_data,"a21018z",UNIT_MG_M3,0,&arg_n);	


	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 20,2);
	SYSLOG_DBG_HEX("HeBeiGRST CX4000 stat data send",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000 stat SEND:",com_tbuf,size);
	
	sleep(2);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HeBeiGRST CX4000 stat protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HeBeiGRST CX4000 stat data RECV",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000 stat  RECV:",com_rbuf,size);

	if(size>=7 && com_rbuf[0]==devaddr && com_rbuf[1]==0x03)
	{
		int stat=0;
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];

		stat=(int) f.f;

		if(stat==0)
			devstat='z';
		if(stat==1)
			devstat='C';	
		if(stat==2)
			devstat='C';
		if(stat==3)
			devstat='C';		
	}

	acq_data->dev_stat=devstat;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}

#define SO2 0
#define NOx 1
#define CO 2
#define HCL 3

struct cx4000_info{	
	float i13013[4];
	float i13025[4];
	float i13010[4];
	float i13026[4];
	
	time_t i13021;
	time_t i13027;
};

static struct cx4000_info cx4000_data;

int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x12,0x0E);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000 STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CX4000 protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CX4000 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000 STATUS RECV:",com_rbuf,size);
	if((size>=33)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=getFloatValue(com_rbuf, 3, FLOAT_CDAB);
		if(val!=0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		}
		else
		{
			val=getFloatValue(com_rbuf, 11, FLOAT_CDAB);
			if(val!=0)
			{
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			}
			else
			{
				val=getFloatValue(com_rbuf, 15, FLOAT_CDAB);
				if(val!=0)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				}
				else
				{
					val=getFloatValue(com_rbuf, 19, FLOAT_CDAB);
					if(val!=0)
					{
						acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
						acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
						acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
					}
					else
					{
						val=getFloatValue(com_rbuf, 23, FLOAT_CDAB);
						if(val!=0)
						{
							acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
							acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
							acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
						}
						else
						{
							val=getFloatValue(com_rbuf, 27, FLOAT_CDAB);
							if(val!=0)
							{
								acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
								acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
								acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,4,INFOR_STATUS,&arg_n);
							}
							else
							{
								val=getFloatValue(com_rbuf, 7, FLOAT_CDAB);
								switch(val)
								{
									case 0:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
									case 1:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
									case 2:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
									case 3:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
									case 4:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
									default:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
								}
								acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
								acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
							}
						}
					}
				}
			}
		}
	}


	sleep(1);
	cmd=0x42;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x09);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CX4000 protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CX4000 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000 INFO RECV1:",com_rbuf,size);
	if((size>=23)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		cx4000_data.i13013[SO2] = getInt16Value(com_rbuf, 3, INT_AB);
		cx4000_data.i13013[NOx] = getInt16Value(com_rbuf, 5, INT_AB);
		cx4000_data.i13013[CO] = getInt16Value(com_rbuf, 11, INT_AB);
		cx4000_data.i13013[HCL] = getInt16Value(com_rbuf, 15, INT_AB);
		//SYSLOG_DBG("CX4000 protocol,INFO data i13013: %.2f,%.2f,%.2f,%.2f\n",data.i13013[SO2],data.i13013[NOx],data.i13013[CO],data.i13013[HCL]);
	}


	sleep(1);
	cmd=0x43;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CX4000 protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CX4000 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000 INFO RECV2:",com_rbuf,size);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getInt16Value(com_rbuf, 3, INT_AB);
		cx4000_data.i13025[SO2] = valf/32767*cx4000_data.i13013[SO2];

		valf = getInt16Value(com_rbuf, 5, INT_AB);
		cx4000_data.i13025[NOx] = valf/32767*cx4000_data.i13013[NOx];

		valf = getInt16Value(com_rbuf, 11, INT_AB);
		cx4000_data.i13025[CO] = valf/32767*cx4000_data.i13013[CO];

		valf = getInt16Value(com_rbuf, 15, INT_AB);
		cx4000_data.i13025[HCL] = valf/32767*cx4000_data.i13013[HCL];

		val=com_rbuf[61]+1900;
		cx4000_data.i13021 = getTimeValue(val, com_rbuf[62], com_rbuf[63], com_rbuf[64], com_rbuf[65], com_rbuf[66]);

		/*SYSLOG_DBG("CX4000 protocol,INFO data i13025: %.2f,%.2f,%.2f,%.2f; i13021:%d\n",
			data.i13025[SO2],data.i13025[NOx],data.i13025[CO],data.i13025[HCL],data.i13021);*/
	}

	sleep(1);
	cmd=0x44;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000 INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CX4000 protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CX4000 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000 INFO RECV3:",com_rbuf,size);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getInt16Value(com_rbuf, 3, INT_AB);
		cx4000_data.i13010[SO2] = valf/32767*cx4000_data.i13013[SO2];

		valf = getInt16Value(com_rbuf, 5, INT_AB);
		cx4000_data.i13010[NOx] = valf/32767*cx4000_data.i13013[NOx];

		valf = getInt16Value(com_rbuf, 11, INT_AB);
		cx4000_data.i13010[CO] = valf/32767*cx4000_data.i13013[CO];

		valf = getInt16Value(com_rbuf, 15, INT_AB);
		cx4000_data.i13010[HCL] = valf/32767*cx4000_data.i13013[HCL];

		val=com_rbuf[61]+1900;
		cx4000_data.i13027 = getTimeValue(val, com_rbuf[62], com_rbuf[63], com_rbuf[64], com_rbuf[65], com_rbuf[66]);

		/*SYSLOG_DBG("CX4000 protocol,INFO data i13010: %.2f,%.2f,%.2f,%.2f; i13027:%d\n",
			data.i13010[SO2],data.i13010[NOx],data.i13010[CO],data.i13010[HCL],data.i13027);*/
	}


	sleep(1);
	cmd=0x46;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x09);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000 INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("CX4000 protocol,INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CX4000 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000 INFO RECV4:",com_rbuf,size);
	if((size>=23)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getUInt16Value(com_rbuf, 3, INT_AB);
		cx4000_data.i13026[SO2] = valf/65535*cx4000_data.i13013[SO2];

		valf = getUInt16Value(com_rbuf, 5, INT_AB);
		cx4000_data.i13026[NOx] = valf/65535*cx4000_data.i13013[NOx];

		valf = getUInt16Value(com_rbuf, 11, INT_AB);
		cx4000_data.i13026[CO] = valf/65535*cx4000_data.i13013[CO];

		valf = getUInt16Value(com_rbuf, 15, INT_AB);
		cx4000_data.i13026[HCL] = valf/65535*cx4000_data.i13013[HCL];
		

		//SYSLOG_DBG("CX4000 protocol,INFO data i13026: %.2f,%.2f,%.2f,%.2f\n",data.i13026[SO2],data.i13026[NOx],data.i13026[CO],data.i13026[HCL]);
	}


	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,cx4000_data.i13013[SO2],INFOR_ARGUMENTS,&arg_n);
#if 0	
	acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,data.i13025[SO2],INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,data.i13010[SO2],INFOR_ARGUMENTS,&arg_n);
#else	
	if(cx4000_data.i13013[SO2]>0.0001) //
	{//data.i13025[SO2] is value not percent, miles zhang 20210601
	 //data.i13010[SO2] is value not percent
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,cx4000_data.i13025[SO2]/cx4000_data.i13013[SO2]*100,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,cx4000_data.i13010[SO2]/cx4000_data.i13013[SO2]*100,INFOR_ARGUMENTS,&arg_n);
	}
	else
	{
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);	
	}
#endif	
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,cx4000_data.i13026[SO2],INFOR_ARGUMENTS,&arg_n);

	acqdata_set_value_flag(acq_data,"i13021",cx4000_data.i13021,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13027",cx4000_data.i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	
	read_system_time(acq_data->acq_tm);

	acq_data->acq_status = ACQ_OK;
	

	return arg_n;
}

int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_NOx_info(struct acquisition_data *acq_data)
{
	int arg_n=0;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_NOx_info\n");
	

	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);


	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,cx4000_data.i13013[NOx],INFOR_ARGUMENTS,&arg_n);
	if(cx4000_data.i13013[NOx]>0.0001) //
	{//data.i13025[NOx] is value not percent, miles zhang 20210601
	 //data.i13010[NOx] is value not percent
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,cx4000_data.i13025[NOx]/cx4000_data.i13013[NOx]*100,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,cx4000_data.i13010[NOx]/cx4000_data.i13013[NOx]*100,INFOR_ARGUMENTS,&arg_n);
	}
	else
	{
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);	
	}	
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,cx4000_data.i13026[NOx],INFOR_ARGUMENTS,&arg_n);

	acqdata_set_value_flag(acq_data,"i13021",cx4000_data.i13021,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13027",cx4000_data.i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	
	read_system_time(acq_data->acq_tm);


	acq_data->acq_status = ACQ_OK;

	return arg_n;
}

int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_CO_info(struct acquisition_data *acq_data)
{
	int arg_n=0;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_CO_info\n");
	

	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);


	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,cx4000_data.i13013[CO],INFOR_ARGUMENTS,&arg_n);
#if 0	
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,data.i13025[CO],INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,data.i13010[CO],INFOR_ARGUMENTS,&arg_n);
#else
	if(cx4000_data.i13013[CO]>0.0001) //
	{//data.i13025[CO] is value not percent, miles zhang 20210601
	 //data.i13010[CO] is value not percent
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,cx4000_data.i13025[CO]/cx4000_data.i13013[CO]*100,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,cx4000_data.i13010[CO]/cx4000_data.i13013[CO]*100,INFOR_ARGUMENTS,&arg_n);
	}
	else
	{
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);	
	}	
#endif	
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,cx4000_data.i13026[CO],INFOR_ARGUMENTS,&arg_n);

	acqdata_set_value_flag(acq_data,"i13021",cx4000_data.i13021,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13027",cx4000_data.i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	
	read_system_time(acq_data->acq_tm);


	acq_data->acq_status = ACQ_OK;

	return arg_n;
}

int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_HCL_info(struct acquisition_data *acq_data)
{
	int arg_n=0;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_HCL_info\n");
	

	acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);


	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,cx4000_data.i13013[HCL],INFOR_ARGUMENTS,&arg_n);
#if 0
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,data.i13025[HCL],INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,data.i13010[HCL],INFOR_ARGUMENTS,&arg_n);
#else
	if(cx4000_data.i13013[HCL]>0.0001) //
	{//data.i13025[HCL] is value not percent, miles zhang 20210601
	 //data.i13010[HCL] is value not percent	
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,cx4000_data.i13025[HCL]/cx4000_data.i13013[HCL]*100,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,cx4000_data.i13010[HCL]/cx4000_data.i13013[HCL]*100,INFOR_ARGUMENTS,&arg_n);
	}
	else
	{
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);	
	}	
#endif	
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,cx4000_data.i13026[HCL],INFOR_ARGUMENTS,&arg_n);

	acqdata_set_value_flag(acq_data,"i13021",cx4000_data.i13021,0.0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13027",cx4000_data.i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	
	read_system_time(acq_data->acq_tm);


	acq_data->acq_status = ACQ_OK;

	return arg_n;
}

#undef SO2
#undef NOx
#undef CO
#undef HCL

