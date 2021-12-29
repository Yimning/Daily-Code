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

static int send_DLT645_2007(struct acquisition_data *acq_data,char *addr,int DI,int num)
{
	int cs = 0;
	int i=0;
	int size=0;
	char com_tbuf[20];

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x68;
	
	com_tbuf[1]=addr[0];
	com_tbuf[2]=addr[1];
	com_tbuf[3]=addr[2];
	com_tbuf[4]=addr[3];
	com_tbuf[5]=addr[4];
	com_tbuf[6]=addr[5];
	
	com_tbuf[7]=0x68;	

	com_tbuf[8]=0x11;
	com_tbuf[9]=0x04;

	com_tbuf[10]= ((DI>>0)&0x000000FF);
	com_tbuf[11]= ((DI>>8)&0x000000FF);
	com_tbuf[12]= ((DI>>16)&0x000000FF);
	com_tbuf[13]= ((DI>>24)&0x000000FF);
	com_tbuf[10]+=0x33;		//encrypt
	com_tbuf[11]+=0x33;		//encrypt
	com_tbuf[12]+=0x33;		//encrypt
	com_tbuf[13]+=0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%d] DLT645 2007 send device %s , size=%d\n",num,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	return size;
}

static int is_pack_ok(char *com_rbuf,char *addr,int size)
{
	if(com_rbuf[0]!=0x68 || com_rbuf[7]!=0x68 || com_rbuf[8]!=0x91 || com_rbuf[size-1]!=0x16)	//pack is error
	{
		return 0;
	}
	if(addr[0]!=0xAA || addr[1]!=0xAA || addr[2]!=0xAA ||
		addr[3]!=0xAA || addr[4]!=0xAA || addr[5]!=0xAA)		//address != AAAAAAAAAAAA
	{
		if(com_rbuf[1]!=addr[0] || com_rbuf[2]!=addr[1] || com_rbuf[3]!=addr[2] || 
			com_rbuf[4]!=addr[3] || com_rbuf[5]!=addr[4] || com_rbuf[6]!=addr[5])		//address error
		{
			return 0;
		}
	}
	
	return 1;
}

static int get_DLT645_val(char *buff,float format,double *value,int buff_len)
{
	int len=0;
	int temp=0;
	int i=0;
	int flag=0;
	char *p;
	
	if(!buff) return -1;
	
	len=buff[9];
	if(len+10 >= buff_len)
	{
		return -2;
	}
		
	p=&buff[10];
	temp=0;
	
	for(i=4;i<len;i++)	//un-encrypt
	{
		p[i]-=0x33;
	}

	if((p[len-1] & 0x80)==0x80)
	{
		p[len-1]=p[len-1] & 0x7F;
		flag=1;
	}
	
	for(i=len-1;i>3;i--)
	{
		temp*=10;
		temp+=((p[i]>>4)&0x0F);
		temp*=10;
		temp+=((p[i]>>0)&0x0F);
	}
	
	*value=temp;
	*value/=format;
	if(flag==1)
	{
		*value=0 - *value;
	}
	return 0;
}

int protocol_DLT645_GDDG1(struct acquisition_data *acq_data)
{
	#define DLT645_GDDG1_NUM 2
	const char * const polcode[DLT645_GDDG1_NUM] = {"ea2001","ea1901"};
	
	const int const DI[DLT645_GDDG1_NUM] = {0x00010000,0x02030000};
	
	const float const data_format[DLT645_GDDG1_NUM] = {100,10000};
	
	const UNIT_DECLARATION const units[DLT645_GDDG1_NUM]={UNIT_KWH,UNIT_KW};
	
	char addr[6]={0x54,0x98,0x00,0x11,0x06,0x19};//{0x53,0x08,0x15,0x80,0x10,0x91};
	char com_rbuf[2048]={0}; 
	int status=0;
	int size=0;
	double val=0;
	char *p;
	int arg_n=0;
	int ret=0;
	int i=0;

	status=0;
	for(i=0;i<DLT645_GDDG1_NUM;i++)
	{	
		sleep(1);
		ret=send_DLT645_2007(acq_data,addr,DI[i],i+1);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
		SYSLOG_DBG("[%d] DLT645 2007 read device %s , size=%d\n",i+1,DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);

		for(p=com_rbuf;p<&com_rbuf[size];p++,size--) 	//delete 0xFE
		{
			if(p[0] != 0xFE)
				break;
		}
		
		if(size > 12 && is_pack_ok(p,addr,size))
		{
			ret=get_DLT645_val(p,data_format[i],&val,size);
			if(ret != 0)
			{
				val=0;
				status=1;
			}
		}
		else
		{
			val=0;
			status=1;
		}
		acqdata_set_value(acq_data,polcode[i],units[i],val,&arg_n);
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	//return arg_n;

	
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

