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

int protocol_CEMS_YaoJiBoLi(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 6

	char *polcode[POLCODE_NUM]={
		"CCSSBDCCQQHYC",	"CCSSDCCQRCDL",	"CCSSDCCQRCDY",	"TLSSGLLL",	"TXSSYQSD",
		"TXSSTXHYJYL"
	};
	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_PA,		UNIT_A,	UNIT_V,	UNIT_M3_S,	UNIT_0C,
		UNIT_MG_L
	};
	int regaddr[POLCODE_NUM]={
		1,	3,	5,	7,	9,
		11
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
	cmd=0x10;
	startreg=0x01;
	cnt=0x0C;
	dataType=FLOAT_CDAB;

	//memset(com_tbuf,0,sizeof(com_tbuf));
	//size=modbus_pack(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), NULL, 0, com_rbuf, sizeof(com_rbuf), "YaoJiBoLi CEMS", 1);
	p=modbus_crc_check_write(com_rbuf, size, devaddr, cmd, startreg-1, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			pos=(regaddr[i]-startreg)*2+7;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,polcode[i],unit[i],valf,&arg_n);
		}
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}
		status=1;
	}
	
	if(status==0)
		acq_data->acq_status = ACQ_OK;
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
	
#undef POLCODE_NUM
}

