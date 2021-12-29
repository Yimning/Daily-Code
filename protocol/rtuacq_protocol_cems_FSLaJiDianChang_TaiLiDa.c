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

int protocol_CEMS_FSLaJiDianChang_TaiLiDa(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 24

	char *polcode[POLCODE_NUM]={
		"a21018",		"a21018z",	"a21001",		"a21001z",	"a34013a",
		"a34013",		"a34013z",	"a01011",		"a00000",		"a00000z",
		"a01012",		"a01013",		"a21024",		"a21024z",	"a21005",	
		"a21005z",	"a21002",		"a21002z",	"a21026",		"a21026z",
		"a05001",		"a05001z",	"a01014",		"a19001"
	};
	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_M_S,		UNIT_M3_S,		UNIT_M3_S,
		UNIT_0C,			UNIT_KPA,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_PECENT,	UNIT_PECENT
	};
	int regaddr[POLCODE_NUM]={
		0,	0,	2,	2,	4,
		4,	4,	6,	6,	6,
		8,	10,	20,	20,	22,
		22,	24,	24,	26,	26,
		28,	28,	30,	34
	};

	int status=0;
	float valf=0;
	int val=0;
	int i=0,pos=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0,cnt=0;
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
	cnt=0x26;
	dataType=FLOAT_ABCD;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "TaiLiDa FSLaJiDianChang CEMS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			pos=regaddr[i]*2+3;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,polcode[i],unit[i],valf,&arg_n);
		}

		val=getUInt16Value(p, 75, INT_BA);
		if((val&0x0002)==0x0002)
			flag='D';
		else if((val&0x0001)==0x0001)
			flag='M';
		else if((val&0x0008)==0x0008)
			flag='M';
		else if((val&0x0020)==0x0020)
			flag='C';
		else if((val&0x0200)==0x0200)
			flag='C';
		else if((val&0x0004)==0x0004)
			flag='N';
		else
			flag='D';
		
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
	
	if(status == 0)
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

