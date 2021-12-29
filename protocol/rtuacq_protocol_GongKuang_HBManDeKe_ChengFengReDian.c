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

int protocol_GongKuang_HBManDeKe_ChengFengReDian1(struct acquisition_data *acq_data)
{
#define POLCODE_NUM_AI 18
#define POLCODE_NUM_DI 6

	char *ai_polcode[POLCODE_NUM_AI]={
		"FJCGLZFL01",			"p10201",				"FJCGLZFL02",			"p10202",				"GLCGLHYL01",
		"TXTXHYJYL01",		"TXSSYQSD01",		"CCSSBDCTQQHYC01",	"GLCGLHYL02",		"TXTXHYJYL02",
		"TXSSYQSD02",		"CCSSBDCTQQHYC02",	"CCSSJDCCQRCDL01",	"CCSSJDCCQRCDY01",	"CCSSJDCCQRCDL02",
		"CCSSJDCCQRCDY02",	"TLSSXSTNJYPHZ",		"TLSSGJLL"
	};
	UNIT_DECLARATION ai_unit[POLCODE_NUM_AI]={
		UNIT_T_H,	UNIT_T_H,	UNIT_T_H,	UNIT_T_H,		UNIT_PECENT,
		UNIT_NONE,	UNIT_0C,		UNIT_PA,		UNIT_PECENT,	UNIT_NONE,
		UNIT_0C,		UNIT_PA,		UNIT_A,		UNIT_V,			UNIT_A,
		UNIT_V,		UNIT_PH,		UNIT_M3_H
	};
	int ai_regaddr[POLCODE_NUM_AI]={
		1,		1,		3,		3,		11,
		15,		17,		23,		25,		29,
		31,		37,		81,		83,		85,
		87,		115,		117
	};

	char *di_polcode[POLCODE_NUM_DI]={
		"p10101",				"p10102",		"TLSSJYXHBYXZT01",	"TLSSJYXHBYXZT02",	"TLSSJYXHBYXZT03",
		"TLSSJYXHBYXZT04"
	};
	int di_regaddr[POLCODE_NUM_DI]={
		1,	7,	31,	32,	33,
		34
	};
	int di_type[POLCODE_NUM_DI]={
		0,	0,	1,	1,	1,
		1
	};

	int status=0;
	float valf=0;
	int val=0;
	int i=0,j=0;
	int pos=0,bit=0;
	int p101[6]={0};
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

	if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],0,&arg_n);
		}

		for(i=0;i<POLCODE_NUM_DI;i++)
		{
			acqdata_set_value(acq_data,di_polcode[i],UNIT_NONE,0,&arg_n);
		}
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
	}

	status=0;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	startreg=1;
	cnt=0x76;
	dataType=FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack_tcp(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "HBManDeKe ChengFengReDian1 GongKuang AI", 1);
	p=modbus_tcp_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			pos=(ai_regaddr[i]-startreg)*2+9;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],valf,&arg_n);
		}
	}
	else
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],0,&arg_n);
		}
		status=1;
	}

	sleep(1);
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x01;
	startreg=1;
	cnt=0x26;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack_tcp(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "HBManDeKe ChengFengReDian1 GongKuang DI", 1);
	p=modbus_tcp_crc_check_coil(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM_DI;i++)
		{
			if(di_type[i]==0)
			{
				for(j=0;j<6;j++)
				{
					pos=(di_regaddr[i]-1+j)/8+9;
					bit=(di_regaddr[i]-1+j)%8;
					p101[j]=p[pos]>>bit;
					p101[j]&=0x01;
				}
				if(p101[0]==1)
					valf=MARK_DC_Fb;
				else if(p101[1]==1)
					valf=MARK_DC_Sta;
				else if(p101[2]==1)
					valf=MARK_DC_Stb;
				else if(p101[3]==1)
					valf=MARK_DC_Sd;
				else if(p101[4]==1)
					valf=MARK_DC_Fa;
				else if(p101[5]==1)
					valf=MARK_DC_Sr;
				else
					valf=MARK_N;
				acqdata_set_value(acq_data,di_polcode[i],UNIT_NONE,valf,&arg_n);
			}
			else
			{
				pos=(di_regaddr[i]-1)/8+9;
				bit=(di_regaddr[i]-1)%8;
				val=p[pos]>>bit;
				val&=0x01;
				val=!val;
				acqdata_set_value(acq_data,di_polcode[i],UNIT_NONE,val,&arg_n);
			}
		}
	}
	else
	{
		for(i=0;i<POLCODE_NUM_DI;i++)
		{
			acqdata_set_value(acq_data,di_polcode[i],UNIT_NONE,0,&arg_n);
		}
		status=1;
	}
	
	if(status==0)
		acq_data->acq_status = ACQ_OK;
	else
	{
		acq_data->acq_status = ACQ_ERR;
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	}
	
	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
	
#undef POLCODE_NUM_AI
#undef POLCODE_NUM_DI
}

int protocol_GongKuang_HBManDeKe_ChengFengReDian2(struct acquisition_data *acq_data)
{
#define POLCODE_NUM_AI 18
#define POLCODE_NUM_DI 6

	char *ai_polcode[POLCODE_NUM_AI]={
		"FJCGLZFL01",			"p10201",				"FJCGLZFL02",			"p10202",				"GLCGLHYL01",
		"TXTXHYJYL01",		"TXSSYQSD01",		"CCSSBDCTQQHYC01",	"GLCGLHYL02",		"TXTXHYJYL02",
		"TXSSYQSD02",		"CCSSBDCTQQHYC02",	"CCSSJDCCQRCDL01",	"CCSSJDCCQRCDY01",	"CCSSJDCCQRCDL02",
		"CCSSJDCCQRCDY02",	"TLSSXSTNJYPHZ",		"TLSSGJLL"
	};
	UNIT_DECLARATION ai_unit[POLCODE_NUM_AI]={
		UNIT_T_H,	UNIT_T_H,	UNIT_T_H,	UNIT_T_H,		UNIT_PECENT,
		UNIT_NONE,	UNIT_0C,		UNIT_PA,		UNIT_PECENT,	UNIT_NONE,
		UNIT_0C,		UNIT_PA,		UNIT_A,		UNIT_V,			UNIT_A,
		UNIT_V,		UNIT_PH,		UNIT_M3_H
	};
	int ai_regaddr[POLCODE_NUM_AI]={
		5,		5,		7,		7,		39,
		43,		45,		51,		53,		57,
		59,		65,		89,		91,		93,
		95,		119,		121
	};

	char *di_polcode[POLCODE_NUM_DI]={
		"p10101",				"p10102",		"TLSSJYXHBYXZT01",	"TLSSJYXHBYXZT02",	"TLSSJYXHBYXZT03",
		"TLSSJYXHBYXZT04"
	};
	int di_regaddr[POLCODE_NUM_DI]={
		13,	19,	35,	36,	37,
		38
	};
	int di_type[POLCODE_NUM_DI]={
		0,	0,	1,	1,	1,
		1
	};

	int status=0;
	float valf=0;
	int val=0;
	int i=0,j=0;
	int pos=0,bit=0;
	int p101[6]={0};
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

	if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],0,&arg_n);
		}

		for(i=0;i<POLCODE_NUM_DI;i++)
		{
			acqdata_set_value(acq_data,di_polcode[i],UNIT_NONE,0,&arg_n);
		}
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
	}

	status=0;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	startreg=5;
	cnt=0x76;
	dataType=FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack_tcp(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "HBManDeKe ChengFengReDian2 GongKuang AI", 1);
	p=modbus_tcp_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			pos=(ai_regaddr[i]-startreg)*2+9;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],valf,&arg_n);
		}
	}
	else
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],0,&arg_n);
		}
		status=1;
	}

	sleep(1);
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x01;
	startreg=1;
	cnt=0x26;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack_tcp(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "HBManDeKe ChengFengReDian2 GongKuang DI", 1);
	p=modbus_tcp_crc_check_coil(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM_DI;i++)
		{
			if(di_type[i]==0)
			{
				for(j=0;j<6;j++)
				{
					pos=(di_regaddr[i]-1+j)/8+9;
					bit=(di_regaddr[i]-1+j)%8;
					p101[j]=p[pos]>>bit;
					p101[j]&=0x01;
				}
				if(p101[0]==1)
					valf=MARK_DC_Fb;
				else if(p101[1]==1)
					valf=MARK_DC_Sta;
				else if(p101[2]==1)
					valf=MARK_DC_Stb;
				else if(p101[3]==1)
					valf=MARK_DC_Sd;
				else if(p101[4]==1)
					valf=MARK_DC_Fa;
				else if(p101[5]==1)
					valf=MARK_DC_Sr;
				else
					valf=MARK_N;
				acqdata_set_value(acq_data,di_polcode[i],UNIT_NONE,valf,&arg_n);
			}
			else
			{
				pos=(di_regaddr[i]-1)/8+9;
				bit=(di_regaddr[i]-1)%8;
				val=p[pos]>>bit;
				val&=0x01;
				val=!val;
				acqdata_set_value(acq_data,di_polcode[i],UNIT_NONE,val,&arg_n);
			}
		}
	}
	else
	{
		for(i=0;i<POLCODE_NUM_DI;i++)
		{
			acqdata_set_value(acq_data,di_polcode[i],UNIT_NONE,0,&arg_n);
		}
		status=1;
	}
	
	if(status==0)
		acq_data->acq_status = ACQ_OK;
	else
	{
		acq_data->acq_status = ACQ_ERR;
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	}
	
	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
	
#undef POLCODE_NUM_AI
#undef POLCODE_NUM_DI
}

int protocol_GongKuang_HBManDeKe_ChengFengReDian3(struct acquisition_data *acq_data)
{
#define POLCODE_NUM_AI 7

	char *ai_polcode[POLCODE_NUM_AI]={
		"FJCGLZFL",		"p10201",			"TXTXHYJYL",		"TXSSYQSD",		"CCSSBDCTQQHYC",
		"TLSSGLLL01",		"TLSSGLLL02"
	};
	UNIT_DECLARATION ai_unit[POLCODE_NUM_AI]={
		UNIT_T_H,	UNIT_T_H,	UNIT_NONE,	UNIT_0C,		UNIT_PA,	
		UNIT_KG,		UNIT_KG
	};
	int ai_regaddr[POLCODE_NUM_AI]={
		9,		9,		71,		73,		79,
		123,		125,	
	};

	int status=0;
	float valf=0;
	int val=0;
	int i=0,j=0;
	int pos=0,bit=0;
	int p101[6]={0};
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

	if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],0,&arg_n);
		}
		acqdata_set_value(acq_data,"p10101",UNIT_NONE,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
	}

	status=0;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	startreg=9;
	cnt=0x76;
	dataType=FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack_tcp(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "HBManDeKe ChengFengReDian3 GongKuang AI", 1);
	p=modbus_tcp_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			pos=(ai_regaddr[i]-startreg)*2+9;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],valf,&arg_n);
		}
	}
	else
	{
		for(i=0;i<POLCODE_NUM_AI;i++)
		{
			acqdata_set_value(acq_data,ai_polcode[i],ai_unit[i],0,&arg_n);
		}
		status=1;
	}

	sleep(1);
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x01;
	startreg=1;
	cnt=0x26;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack_tcp(com_tbuf,devaddr,cmd,startreg-1,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "HBManDeKe ChengFengReDian3 GongKuang DI", 1);
	p=modbus_tcp_crc_check_coil(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(j=0;j<6;j++)
		{
			pos=(24+j)/8+9;
			bit=(24+j)%8;
			p101[j]=p[pos]>>bit;
			p101[j]&=0x01;
		}
		if(p101[0]==1)
			valf=MARK_DC_Fb;
		else if(p101[1]==1)
			valf=MARK_DC_Sta;
		else if(p101[2]==1)
			valf=MARK_DC_Stb;
		else if(p101[3]==1)
			valf=MARK_DC_Sd;
		else if(p101[4]==1)
			valf=MARK_DC_Fa;
		else if(p101[5]==1)
			valf=MARK_DC_Sr;
		else
			valf=MARK_N;
		acqdata_set_value(acq_data,"p10101",UNIT_NONE,valf,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"p10101",UNIT_NONE,0,&arg_n);
		status=1;
	}
	
	if(status==0)
		acq_data->acq_status = ACQ_OK;
	else
	{
		acq_data->acq_status = ACQ_ERR;
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	}
	
	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
	
#undef POLCODE_NUM_AI
}

