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

#define DLT645_2007_NUM 10

#if 0
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

static int get_DLT645_val(char *buff,float format,int negative,double *value,int buff_len)
{
	int len=0;
	int temp=0;
	int i=0;
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

	if(negative == 1)
	{
		p[len-1] = p[len-1] & 0x7F;
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
	
	if(negative == 1)
	{
		(*value) *= -1; 
	}
	return 0;
}

int protocol_DLT645_2007(struct acquisition_data *acq_data)
{
	char * pcodes[DLT645_2007_NUM] = {"Qw","Ua","Ub","Uc","Ia",
                                           "Ib","Ic","Pw","PW","Te"};
	
	int DI_2007[DLT645_2007_NUM] = {0x00010000,0x02010100,0x02010200,0x02010300,0x02020100,
							0x02020200,0x02020300,0x02030000,0x02040000,0x02060000};
	
	float data_format[DLT645_2007_NUM] = {100,10,10,10,1000,
                                           1000,1000,10000,10000,1000};
	
	UNIT_DECLARATION units[DLT645_2007_NUM]={UNIT_KWH,UNIT_V,UNIT_V,UNIT_V,UNIT_A,
   	                                        UNIT_A,UNIT_A,UNIT_KW,UNIT_KVAR,UNIT_NONE};

	int negative[DLT645_2007_NUM]={1,0,0,0,1,1,1,1,1,1};
	
	char polcode[DLT645_2007_NUM][10];
	char polcode_prefix[10]={0};
	char polcode_suffix[10]={0};
	char elmeter_addr[20]={0};


	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg;

	char com_rbuf[2048]={0}; 
	int status=0;
	int size=0;
	double val=0;
	char *p;
	int arg_n=0;
	int ret=0;
	int i=0;
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;

	memset(elmeter_addr,0,sizeof(elmeter_addr));
	ret=get_addr(modbusarg_running, elmeter_addr);
	if(ret!=0)
	{
		SYSLOG_DBG("device %s get elmeter_addr error\n",DEV_NAME(acq_data));
		memset(elmeter_addr,0,sizeof(elmeter_addr));
	}
	
	strcpy(polcode_prefix,getDevCode(modbusarg_running->devstaddr));
	sprintf(polcode_suffix,"%.2x",modbusarg_running->regcnt);

	SYSLOG_DBG("electric_meter %s,%s,%s",elmeter_addr,polcode_prefix,polcode_suffix);

	for(i=0;i<DLT645_2007_NUM;i++)
	{
		  sprintf(polcode[i],"%s%s%s",polcode_prefix,pcodes[i],polcode_suffix);
	}
	
	status=0;
	for(i=0;i<DLT645_2007_NUM;i++)
	{
		if(!polcode_is_enable(mpolcodearg,polcode[i]))
		{
			acqdata_set_value(acq_data,polcode[i],units[i],0,&arg_n);
			continue;
		}
	
		ret=send_DLT645_2007(acq_data,elmeter_addr,DI_2007[i],i+1);
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
		
		if(size > 12 && is_pack_ok(p,elmeter_addr,size))
		{
			ret=get_DLT645_val(p,data_format[i],negative[i],&val,size);
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

	return arg_n;
}
#endif

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

int get_DLT2007_Qw(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x00 + 0x33;		//encrypt
	com_tbuf[12]=0x01 + 0x33;		//encrypt
	com_tbuf[13]=0x00 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 20 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<4;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format = 100;
		if((p[3]&0x80) == 0x80)
		{
			format = -format;
			p[3]=p[3] & 0x7F;
		}
		
		for(i=3;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_Ua(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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
	
	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x01 + 0x33;		//encrypt
	com_tbuf[12]=0x01 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 18 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<2;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format=10;
		
		for(i=1;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_Ub(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x02 + 0x33;		//encrypt
	com_tbuf[12]=0x01 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 18 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<2;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format=10;
		
		for(i=1;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_Uc(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x03 + 0x33;		//encrypt
	com_tbuf[12]=0x01 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 18 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<2;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format=10;
		
		for(i=1;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_Ia(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x01 + 0x33;		//encrypt
	com_tbuf[12]=0x02 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 19 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<3;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format = 1000;
		if((p[2]&0x80) == 0x80)
		{
			format = -format;
			p[2]=p[2] & 0x7F;
		}
		
		for(i=2;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_Ib(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x02 + 0x33;		//encrypt
	com_tbuf[12]=0x02 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 19 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<3;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format = 1000;
		if((p[2]&0x80) == 0x80)
		{
			format = -format;
			p[2]=p[2] & 0x7F;
		}
		
		for(i=2;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_Ic(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x03 + 0x33;		//encrypt
	com_tbuf[12]=0x02 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 19 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<3;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format = 1000;
		if((p[2]&0x80) == 0x80)
		{
			format = -format;
			p[2]=p[2] & 0x7F;
		}
		
		for(i=2;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_Pw(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x00 + 0x33;		//encrypt
	com_tbuf[12]=0x03 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 19 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<3;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format = 10000;
		if((p[2]&0x80) == 0x80)
		{
			format = format;
			p[2]=p[2] & 0x7F;
		}
		
		for(i=2;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_PW(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x00 + 0x33;		//encrypt
	com_tbuf[12]=0x04 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 19 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<3;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}

		format = 10000;
		if((p[2]&0x80) == 0x80)
		{
			format = -format;
			p[2]=p[2] & 0x7F;
		}
		
		for(i=2;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}
int get_DLT2007_Te(struct acquisition_data *acq_data,char *addr,char *polcode,float *val)
{
	int cs = 0;
	int i=0;
	int size=0;
	int buff_len=0;
	int temp=0;
	int format;
	char *p;
	char com_tbuf[20];
	char com_rbuf[1024];

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

	com_tbuf[10]=0x00 + 0x33;		//encrypt
	com_tbuf[11]=0x00 + 0x33;		//encrypt
	com_tbuf[12]=0x06 + 0x33;		//encrypt
	com_tbuf[13]=0x02 + 0x33;		//encrypt
	
	for(i=0;i<14;i++)
	{
		cs+=com_tbuf[i];
	}
	cs&=0xFF;

	com_tbuf[14]=cs;
	com_tbuf[15]=0x16;

	size=16;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("[%s] DLT645 2007 send device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 TX data ",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("[%s] DLT645 2007 read device %s , size=%d\n",polcode,DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DLT645 2007 RX data ",com_rbuf,size);
	buff_len=size;
	
	for(p=com_rbuf;p<&com_rbuf[size];p++,buff_len--) 	//delete 0xFE
	{
		if(p[0] != 0xFE)
			break;
	}

	if(buff_len >= 18 && is_pack_ok(p,addr,buff_len))
	{
		p=&p[14];
		
		for(i=0;i<2;i++)	//un-encrypt
		{
			p[i]-=0x33;
		}
		
		format = 1000;
		if((p[1]&0x80) == 0x80)
		{
			format = -format;
			p[1]=p[1] & 0x7F;
		}
		
		for(i=1;i>=0;i--)
		{
			temp*=10;
			temp+=((p[i]>>4)&0x0F);
			temp*=10;
			temp+=((p[i]>>0)&0x0F);
		}
		*val = temp;
		(*val) /=format;

		return 0;
	}
	else
	{
		*val = 0;
		return 1;
	}
}

static int polcode_is_enable(struct modbus_polcode_arg *mpolcodearg,char *pcode)
{
	int i=0;
	for(i=0;i<DLT645_2007_NUM;i++)
	{
		if(!strcmp(mpolcodearg[i].polcode,pcode) && mpolcodearg[i].enableFlag==1)
		{
			return 1;
		}
	}
	return 0;
}

static int get_addr(struct modbus_arg *modbusarg_running,char *addr)
{
	if(!modbusarg_running || !addr) return -1;

	addr[5]=((modbusarg_running->devaddr)>>16)&0xFF;
	addr[4]=((modbusarg_running->devaddr)>>8)&0xFF;
	addr[3]=((modbusarg_running->devaddr)>>0)&0xFF;
	addr[2]=((modbusarg_running->devfun)>>16)&0xFF;
	addr[1]=((modbusarg_running->devfun)>>8)&0xFF;
	addr[0]=((modbusarg_running->devfun)>>0)&0xFF;

	return 0;
}

int protocol_DLT645_2007(struct acquisition_data *acq_data)
{
	char * pcodes[DLT645_2007_NUM] = {"Qw","Ua","Ub","Uc","Ia",
                                           "Ib","Ic","Pw","PW","Te"};

	UNIT_DECLARATION units[DLT645_2007_NUM]={UNIT_KWH,UNIT_V,UNIT_V,UNIT_V,UNIT_A,
   	                                        UNIT_A,UNIT_A,UNIT_KW,UNIT_KVAR,UNIT_NONE};

	int (*DLT645_2007_func[DLT645_2007_NUM])(struct acquisition_data *,char *,char *,float *)={
		get_DLT2007_Qw,get_DLT2007_Ua,get_DLT2007_Ub,get_DLT2007_Uc,get_DLT2007_Ia,
		get_DLT2007_Ib,get_DLT2007_Ic,get_DLT2007_Pw,get_DLT2007_PW,get_DLT2007_Te};
	
	//char polcode[DLT645_2007_NUM][10];
	char polcode[DLT645_2007_NUM][CODE_LEN]; //20210308
	//char polcode_prefix[10]={0};
	//char polcode_suffix[10]={0};
	char polcode_prefix[CODE_LEN]={0}; //20210308
	char polcode_suffix[CODE_LEN]={0}; //20210308
	char elmeter_addr[20]={0};


	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg;

	char com_rbuf[2048]={0}; 
	int status=0;
	float val=0;
	int size=0;
	int arg_n=0;
	int ret=0;
	int i=0;
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;

	memset(elmeter_addr,0,sizeof(elmeter_addr));
	ret=get_addr(modbusarg_running, elmeter_addr);
	if(ret!=0)
	{
		SYSLOG_DBG("device %s get elmeter_addr error\n",DEV_NAME(acq_data));
		memset(elmeter_addr,0,sizeof(elmeter_addr));
	}
	
	strcpy(polcode_prefix,getDevCode(modbusarg_running->devstaddr));
	sprintf(polcode_suffix,"%.2x",modbusarg_running->regcnt);

	SYSLOG_DBG("electric_meter %s,%s,%s",elmeter_addr,polcode_prefix,polcode_suffix);

	for(i=0;i<DLT645_2007_NUM;i++)
	{
		  sprintf(polcode[i],"%s%s%s",polcode_prefix,pcodes[i],polcode_suffix);
	}
	
	status=0;
	for(i=0;i<DLT645_2007_NUM;i++)
	{
		if(polcode_is_enable(mpolcodearg,polcode[i]))
		{
			ret=(*DLT645_2007_func[i])(acq_data,elmeter_addr,polcode[i],&val);
			acqdata_set_value(acq_data,polcode[i],units[i],val,&arg_n);
			if(ret != 0)
			{
				status=1;
			}
		}
		else
		{
			acqdata_set_value(acq_data,polcode[i],units[i],0,&arg_n);
		}
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}