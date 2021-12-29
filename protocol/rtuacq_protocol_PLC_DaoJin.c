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

char SHIMADZU_STATUS;

static int SHIMADZU_PLC_pack(char *buff, char *cmd, unsigned int reg, unsigned int cnt)
{
	unsigned int ascii,sum_check;
	char sreg[6]={0};
	
	if(!buff || !cmd || (reg>=10000) || sizeof(cmd)<2)
		return 0;
	
	sprintf(sreg,"D%04d",reg);

	buff[0]=0x05;
	buff[1]='0';
	buff[2]='0';
	buff[3]='F';
	buff[4]='F';
	buff[5]=cmd[0];
	buff[6]=cmd[1];
	buff[7]='0';
	memcpy(&(buff[8]),sreg,5);
	ascii=hex2ascii(cnt);
	buff[13]=(char)((ascii>>8)&0xFF);
	buff[14]=(char)(ascii&0xFF);
	sum_check=SUM_CHECK(buff,15);
	buff[15]=(char)((sum_check>>8)&0xFF);
	buff[16]=(char)(sum_check&0xFF);
	buff[17]=0x0A;
	buff[18]=0x0D;
	
	return 19;
	
}

int protocol_PLC_DaoJin(struct acquisition_data *acq_data)
{
#define CHANNEL_NUM 16

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	char str[50];
	char check[3];
	int size=0,len=0,size2=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	float valf[CHANNEL_NUM];
	int val=0;
	int pos=0;
	char *p;
	float minVal,maxVal;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;

	len=8*4+9;
	check[0]=0x0D;
	check[1]=0x0A;
	check[2]=0x00;

	status=0;
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	if(size<300)
	{
		sleep(2);
		size+=read_device(DEV_NAME(acq_data),&com_rbuf[size],sizeof(com_rbuf)-1-size);
	}

	for(i=0,size2=0;i<10;i++)
	{
		memset(str,0,sizeof(str));
		sprintf(str,"HEX DaoJin PLC RECV%d:",i+1);
		if(size-size2>200)
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,str,&com_rbuf[size2],200);
		else
		{
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,str,&com_rbuf[size2],size-size2);
			break;
		}

		size2=(i+1)*200;
	}

	for(i=0,size2=0;i<10;i++)
	{
		memset(str,0,sizeof(str));
		sprintf(str,"STR DaoJin PLC RECV%d:",i+1);
		if(size-size2>200)
			LOG_WRITE_STR(DEV_NAME(acq_data),1,str,&com_rbuf[size2],200);
		else
		{
			LOG_WRITE_STR(DEV_NAME(acq_data),1,str,&com_rbuf[size2],size-size2);
			break;
		}

		size2=(i+1)*200;
	}

	status=0;
	if(((p=memstr(com_rbuf, size, "00FFWR0D014008"))!=NULL)&&((p=memstr(&p[4], size, "00FF"))!=NULL)&&(!strncmp(&p[len-2],check,2)))
	{
		for(i=0;i<CHANNEL_NUM/2;i++)
		{
			sscanf(&p[4+i*4],"%04X",&val);
			valf[i]=val/4000.0;
		}
	}
	else
	{
		SYSLOG_DBG("DaoJin recv D0140:error\n");
		for(i=0;i<CHANNEL_NUM/2;i++)
		{
			valf[i]=0;
		}

		status=1;
	}
	
	if(((p=memstr(com_rbuf, size, "00FFWR0D015008"))!=NULL)&&((p=memstr(&p[4], size, "00FF"))!=NULL)&&(!strncmp(&p[len-2],check,2)))
	{
		for(i=CHANNEL_NUM/2;i<CHANNEL_NUM;i++)
		{
			sscanf(&p[4+(i-CHANNEL_NUM/2)*4],"%04X",&val);
			valf[i]=val/4000.0;
		}
	}
	else
	{
		SYSLOG_DBG("DaoJin recv D0150:error\n");
		for(i=CHANNEL_NUM/2;i<CHANNEL_NUM;i++)
		{
			valf[i]=0;
		}

		status=1;
	}

	if(((p=memstr(com_rbuf, size, "00FFWR0D005701"))!=NULL)&&((p=memstr(&p[4], size, "00FF"))!=NULL)&&(!strncmp(&p[11],check,2)))
	{
		sscanf(&p[4],"%04X",&val);
		val&=0x0F;

		if(val==0)
			SHIMADZU_STATUS='N';
		else
			SHIMADZU_STATUS='z';
	}
	else
	{
		SYSLOG_DBG("DaoJin recv D0057:error\n");
		//SHIMADZU_STATUS=0;
	}



	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=CHANNEL_NUM-1 && mpolcodearg->algorithm!=1)
			{
				maxVal=mpolcodearg->alarmMax;
				minVal=mpolcodearg->alarmMin;

				valf[pos]=valf[pos]*(maxVal-minVal)+minVal;
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,valf[pos],&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	}


	if(status == 0)
	{
		acq_data->dev_stat = SHIMADZU_STATUS;
		acq_data->acq_status = ACQ_OK;
	}
		
	else 
		acq_data->acq_status = ACQ_ERR;
	
	
	NEED_ERROR_CACHE(acq_data, 20);
	return arg_n;
}

