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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

int protocol_CEMS_FSLuTangWenDu_TaiLiDa(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 13

	char *polcode[POLCODE_NUM]={
		"a01902",		"a01901",		"i33311",		"i33310",		"i33312",
		"i33321",		"i33320",		"i33322",		"i33331",		"i33330",
		"i33332",		"i33300",		"g90205"
	};

	int regaddr[POLCODE_NUM]={
		1,	3,	5,	7,	9,
		11,	13,	15,	17,	19,
		21,	27,	29
	};

	int status=0;
	float valf=0;
	int val=0;
	int i=0,pos=0,bit=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0,cnt=0,startreg=0;
	char *p=NULL;
	char flag=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
#if 1
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x20;
	startreg=1;
	dataType=FLOAT_CDAB;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "TaiLiDa FSLuTangWenDu CEMS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			pos=(regaddr[i]-startreg)*2+3;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,valf,&arg_n);
		}

		val=p[64];
		if(val==0)
			flag='N';
		else if((val&0x10)==0x10)
			flag='F';
		else if((val&0x20)==0x20)
			flag='D';
		else if((val&0x40)==0x40)
			flag='g';
		else if((val&0x02)==0x02)
			flag='t';
		else if((val&0x01)==0x01)
			flag='b';
		else if((val&0x80)==0x80)
			flag='b';
		else if((val&0x08)==0x08)
			flag='a';
		else if((val&0x04)==0x04)
			flag='d';
		else
			flag='N';
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,0,&arg_n);
		}
		status=1;
	}
#else
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x10;
	cnt=modbusarg->regcnt&0xffff;
	startreg=1;
	dataType=FLOAT_CDAB;

	size=getHexDataPack(DEV_NAME(acq_data), NULL, 0, com_rbuf, sizeof(com_rbuf), "TaiLiDa FSLuTangWenDu CEMS", 1);
	p=modbus_crc_check_write(com_rbuf, size, devaddr, cmd, startreg-1,cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			pos=(regaddr[i]-startreg)*2+7;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,valf,&arg_n);
		}

		val=p[68];
		if(val==0)
			flag='N';
		else if((val&0x10)==0x10)
			flag='F';
		else if((val&0x20)==0x20)
			flag='D';
		else if((val&0x40)==0x40)
			flag='g';
		else if((val&0x02)==0x02)
			flag='t';
		else if((val&0x01)==0x01)
			flag='b';
		else if((val&0x80)==0x80)
			flag='b';
		else if((val&0x08)==0x08)
			flag='a';
		else if((val&0x04)==0x04)
			flag='d';
		else
			flag='N';
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,0,&arg_n);
		}
		status=1;
	}
#endif
	if(status==0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
	
#undef POLCODE_NUM
}

