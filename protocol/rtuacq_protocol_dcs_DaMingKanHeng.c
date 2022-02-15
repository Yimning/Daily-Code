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

int protocol_DCS_DaMingKanHeng(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 12

	char polcode[POLCODE_NUM][20]={
		"a01901",		"a01902",		"i33310",		"i33311",		"i33312",
		"i33320",		"i33321",		"i33322",		"i33330",		"i33331",
		"i33332",		"i33300"
	};

	int regaddr[POLCODE_NUM]={
		1,	3,	5,	7,	9,
		11,	13,	15,	17,	19,
		21,	23
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
	cnt=modbusarg->regcnt&0xffff;
	dataType=FLOAT_CDAB;
	startreg=1;

	size=getHexDataPack(DEV_NAME(acq_data), NULL, 0, com_rbuf, sizeof(com_rbuf), "TaiLiDa GuangHuanTou DCS", 1);
	p=modbus_crc_check_write(com_rbuf, size, devaddr, cmd, startreg-1, cnt);
	printf("p=%x\n",p);
	if(p!=NULL)
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			pos=(regaddr[i]-startreg)*2+7;
			valf=getFloatValue(p, pos, dataType);
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,valf,&arg_n);
		}

		pos=(25-startreg)*2+7;
		val=getInt16Value(p, pos, INT_AB);
		switch(val)
		{
			case 1:	flag='d';break;
			case 2:	flag='t';break;
			case 3:	flag='D';break;
			case 4:	flag='g';break;
			case 5:	flag='a';break;
			case 6:	flag='b';break;
			case 7:	flag='b';break;
			case 8:	flag='F';break;
			default:	flag='N';break;
		}
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,0,&arg_n);
		}
		status=1;
		printf("p=NULL\n");
	}
	printf("status:%d\n",status);
	
	if(status==0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	printf("arg_n=%d\n",arg_n);
	return arg_n;
	
#undef POLCODE_NUM
}

