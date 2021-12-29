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

#if 0
#define SLEEP_TIME 1

char SDXinZe_S7_200CN_flag;
int SDXinZe_S7_200CN_status;
int SDXinZe_S7_200CN_alarm;
int SDXinZe_S7_200CN_isEnable;

static int create_SDXinZe_S7_200CN_response(char *pack)
{
	if(!pack)
		return 0;

	pack[0]=0x10;
	pack[1]=0x02;
	pack[2]=0x00;
	pack[3]=0x5C;
	pack[4]=0x5E;
	pack[5]=0x16;

	return 6;
}

static int create_SDXinZe_S7_200CN_cmd(char *pack,char cmd)
{
	int sum=0;

	if(!pack)
		return 0;

	pack[0]=0x68;
	pack[1]=0x1B;
	pack[2]=0x1B;
	pack[3]=0x68;
	pack[4]=0x02;
	pack[5]=0x00;
	pack[6]=0x6C;
	pack[7]=0x32;
	pack[8]=0x01;
	pack[9]=0x00;
	pack[10]=0x00;
	pack[11]=0x00;
	pack[12]=0x00;
	pack[13]=0x00;
	pack[14]=0x0E;
	pack[15]=0x00;
	pack[16]=0x00;
	pack[17]=0x04;
	pack[18]=0x01;
	pack[19]=0x12;
	pack[20]=0x0A;
	pack[21]=0x10;
	pack[22]=0x01;
	pack[23]=0x00;
	pack[24]=0x01;
	pack[25]=0x00;
	pack[26]=0x00;
	pack[27]=0x83;
	pack[28]=0x00;
	pack[29]=0x00;
	pack[30]=cmd&0xFF;
	pack[31]=SUM_CHECK_HEX(&pack[4], 27);
	pack[32]=0x16;

	return 33;
}

static int SDXinZe_S7_200CN_init(char *dev_name)
{
	char com_rbuf[50]={0}; 
	char com_tbuf[50]={0};
	int size=0;

	if(!dev_name)
		return -1;


	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x10;
	com_tbuf[1]=0x02;
	com_tbuf[2]=0x00;
	com_tbuf[3]=0x49;
	com_tbuf[4]=0x4B;
	com_tbuf[5]=0x16;
	size=6;
	size=getHexDataPack(dev_name, com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SDXinZe S7-200CN init 1", SLEEP_TIME);
	if((com_rbuf[0]==0x10) && (com_rbuf[0]==0x00) && (com_rbuf[0]==0x02) &&
		(com_rbuf[0]==0x00) && (com_rbuf[0]==0x02) && (com_rbuf[0]==0x16))
	{
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=create_SDXinZe_S7_200CN_response(com_tbuf);
		size=getHexDataPack(dev_name, com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SDXinZe S7-200CN init 2", SLEEP_TIME);
		if(com_rbuf[0]==0xE5)
			return 0;
		else
			return -3;
	}
	else
	{
		return -2;
	}
	
	
}

static int get_SDXinZe_S7_200CN_status(char *dev_name, char cmd, char *label)
{
	char str[50]={0};
	char com_rbuf[1024]={0}; 
	char com_tbuf[50]={0};
	char check[5];
	char *p;
	int sumCheck=0;
	int size=0;
	int status=0;

	if(!dev_name)
		return 0;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=create_SDXinZe_S7_200CN_cmd(com_tbuf, cmd);
	strcpy(str,label);
	strcat(str," cmd");
	size=getHexDataPack(dev_name, com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, SLEEP_TIME);
	if(com_rbuf[0]!=0xE5)
		return 0;

	check[0]=0x68;
	check[1]=0x16;
	check[2]=0x16;
	check[3]=0x68;
	check[4]=0x00;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=create_SDXinZe_S7_200CN_response(com_tbuf);
	strcpy(str,label);
	strcat(str," reponse");
	size=getHexDataPack(dev_name, com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, SLEEP_TIME);
	p=memstr(com_rbuf, size, check);
	if((size<28) || (p==NULL))
		return 0;

	sumCheck=SUM_CHECK_HEX(&p[4], 22);
	if((p[26]==sumCheck) && (p[27]==0x16))
		status=(p[25]==0x01)?1:0;
	else
		status=0;
	
	return status;
	
}

int protocol_PLC_SDXinZe_S7_200CN(struct acquisition_data *acq_data)
{
	acq_data->acq_status = ACQ_OK;
	
	return 0;
}


int protocol_PLC_SDXinZe_S7_200CN_STATUS(struct acquisition_data *acq_data)
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

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_PLC_SDXinZe_S7_200CN_STATUS\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	if(SDXinZe_S7_200CN_init(DEV_NAME(acq_data))!=0)
	{
		SDXinZe_S7_200CN_isEnable=0;
		return 0;
	}

	if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0x5E, "SDXinZe_S7_200CN status 1"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='D';
		SDXinZe_S7_200CN_status=2;
		SDXinZe_S7_200CN_alarm=7;
	}
	else if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0x5F, "SDXinZe_S7_200CN status 2"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='D';
		SDXinZe_S7_200CN_status=2;
		SDXinZe_S7_200CN_alarm=9;
	}
	else if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0xBD, "SDXinZe_S7_200CN status 3"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='D';
		SDXinZe_S7_200CN_status=2;
		SDXinZe_S7_200CN_alarm=4;
	}
	else if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0xBA, "SDXinZe_S7_200CN status 4"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='D';
		SDXinZe_S7_200CN_status=2;
		SDXinZe_S7_200CN_alarm=1;
	}
	else if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0x67, "SDXinZe_S7_200CN status 5"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='z';
		SDXinZe_S7_200CN_status=5;
		SDXinZe_S7_200CN_alarm=0;
	}
	else if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0x6A, "SDXinZe_S7_200CN status 6"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='z';
		SDXinZe_S7_200CN_status=5;
		SDXinZe_S7_200CN_alarm=0;
	}
	else if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0x5A, "SDXinZe_S7_200CN status 7"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='M';
		SDXinZe_S7_200CN_status=1;
		SDXinZe_S7_200CN_alarm=0;
	}
	else if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0x5D, "SDXinZe_S7_200CN status 8"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='C';
		SDXinZe_S7_200CN_status=3;
		SDXinZe_S7_200CN_alarm=0;
	}
	else if(get_SDXinZe_S7_200CN_status(DEV_NAME(acq_data), 0x70, "SDXinZe_S7_200CN status 9"))
	{
		SDXinZe_S7_200CN_isEnable=1;
		SDXinZe_S7_200CN_flag='N';
		SDXinZe_S7_200CN_status=0;
		SDXinZe_S7_200CN_alarm=0;
	}
	else
	{
		SDXinZe_S7_200CN_isEnable=0;
	}

	acq_data->acq_status = ACQ_OK;

	return 0;
}
#undef SLEEP_TIME
#endif

