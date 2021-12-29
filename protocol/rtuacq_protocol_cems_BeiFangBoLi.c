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

int protocol_CEMS_BeiFangBoLi(struct acquisition_data *acq_data)
{
#define POLCODE_NUM_03 2
#define POLCODE_NUM_01 3

	char *polcode[POLCODE_NUM_03]={
		"TXSSTXHYJYL",	"TLSSGLLL"
	};
	UNIT_DECLARATION unit[POLCODE_NUM_03]={
		UNIT_MG_L,		UNIT_M3_H
	};
	int regaddr[POLCODE_NUM_03]={
		63,	68
	};
	float point[POLCODE_NUM_03]={
		0.1,	0.001
	};

	char *polcode2[POLCODE_NUM_01]={
		"g30401",		"g30402",		"TLSSJYXHBYXZT"
	};
	int regaddr2[POLCODE_NUM_01]={
		97,	98,	99
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

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	startreg=0x3E;
	cnt=0x07;
	dataType=INT_AB;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BeiFangBoLi CEMS 1", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM_03;i++)
		{
			pos=(regaddr[i]-startreg)*2+3;
			val=getUInt16Value(p, pos, dataType);
			valf=val*point[i];
			acqdata_set_value_flag(acq_data,polcode[i],unit[i],valf,'N',&arg_n);
		}
	}
	else
	{
		for(i=0;i<POLCODE_NUM_03;i++)
		{
			acqdata_set_value_flag(acq_data,polcode[i],unit[i],0,'D',&arg_n);
		}
	}

	sleep(1);
	cmd=0x01;
	startreg=0x61;
	cnt=0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BeiFangBoLi CEMS 2", 1);
	p=modbus_crc_check_coil(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM_01;i++)
		{
			pos=(regaddr2[i]-startreg)/8+3;
			bit=(regaddr2[i]-startreg)%8;
			val=p[pos];
			val=(val>>bit)&0x01;
			acqdata_set_value_flag(acq_data,polcode2[i],UNIT_NONE,val,'N',&arg_n);
		}
	}
	else
	{
		for(i=0;i<POLCODE_NUM_01;i++)
		{
			acqdata_set_value_flag(acq_data,polcode2[i],UNIT_NONE,0,'D',&arg_n);
		}
	}
	

	acq_data->dev_stat = 0xAA;
	acq_data->acq_status = ACQ_OK;
	
	NEED_ERROR_CACHE_ONE_POLCODE_VALUE(acq_data, arg_n, 10);
	return arg_n;
	
#undef POLCODE_NUM_01
#undef POLCODE_NUM_03
}

