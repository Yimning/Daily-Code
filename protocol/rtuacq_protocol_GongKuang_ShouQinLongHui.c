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

int protocol_GongKuang_ShouQinLongHui(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 18

	char *polcode[POLCODE_NUM]={
		"QTZCFJDL",		"QTSCBLQXLL",	"CCDCCQDY01",	"CCDCCQDY02",	"CCDCCQECDL01",
		"CCDCCQECDL02",	"TLSFYHFJDL01",	"TLSFYHFJDL02",	"TLGJLL01",		"TLGJLL02",
		"TXHYJYL01",		"TXHYJYL02",		"QTBTPDYXXH",	"TLJYXHGYXZT01",	"TLJYXHGYXZT02",
		"TLJYXHGYXZT03",	"TLJYXHGYXZT04",	"TLJYXHGYXZT05"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_A,		UNIT_T,	UNIT_V,		UNIT_V,		UNIT_A,
		UNIT_A,		UNIT_A,		UNIT_A,		UNIT_M3_H,	UNIT_M3_H,
		UNIT_MG_L,	UNIT_MG_L,	UNIT_NONE,	UNIT_NONE,	UNIT_NONE,
		UNIT_NONE,	UNIT_NONE,	UNIT_NONE,
	};

	int regaddr[POLCODE_NUM]={
		40002,	40003,	40004,	40005,	40006,
		40007,	40008,	40009,	40010,	40011,
		40012,	40013,	40018,	40019,	40020,
		40021,	40022,	40023
	};
	int datatype[POLCODE_NUM]={
		0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,
		0,	0,	1,	1,	1,
		1,	1,	1
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

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	//cmd=0x10;
	cmd=0x03;
	cnt=0x19;
	startreg=40001;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, startreg-40001, cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "TaiLiDa FSLuTangWenDu CEMS", 1);
	//size=getHexDataPack(DEV_NAME(acq_data), NULL, 0, com_rbuf, sizeof(com_rbuf), "TaiLiDa FSLuTangWenDu CEMS", 1);
	//p=modbus_crc_check_write(com_rbuf, size, devaddr, cmd, startreg-40001, cnt);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			//pos=(regaddr[i]-startreg)*2+7;
			pos=(regaddr[i]-startreg)*2+3;
			val=getInt16Value(p, pos, INT_AB);
			if(datatype[i]==0)
				valf=val*0.1;
			else
				valf=(!val);
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

