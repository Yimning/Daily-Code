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

int protocol_GongKuang_ChangRun(struct acquisition_data *acq_data)
{
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0}; 
	int size=0;
	int val=0,valflag=0;
	float valf=0;
	int year=0, mon=0, day=0, hour=0, min=0, sec=0;
	time_t t1=0;
	union uf f;
	int ret=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char *p=NULL;
	int i=0;

#define FirePower_POLCODE_NUM   17
	char polcodestr[][20]={
		"p10201",			"p10301",			"p10101",				"p10202",				"p10302",				"p10102",
		"GLCGLHYL01",	"TXTXHYJYL01",	"CCSSBDCTQQHYC01",	"TLSSXSTNJYPHZ01",	"TLSSJYXHBYXZT01",	
		"GLCGLHYL02",	"TXTXHYJYL02",	"CCSSBDCTQQHYC02",	"TLSSXSTNJYPHZ02",	"TLSSJYXHBYXZT02",	
		"GXSJ"
	};
	UNIT_DECLARATION unitstr[FirePower_POLCODE_NUM]={
		UNIT_T_H,		UNIT_MW,	UNIT_NONE,	UNIT_T_H,	UNIT_MW,	UNIT_NONE,
		UNIT_PECENT,	UNIT_NONE,	UNIT_PA,		UNIT_PH,		UNIT_NONE,
		UNIT_PECENT,	UNIT_NONE,	UNIT_PA,		UNIT_PH,		UNIT_NONE,
		0
	};

	int posnum[FirePower_POLCODE_NUM]={
		0x00,	0x02,	0x04,	0x05,	0x07,	0x09,
		0x0A,	0x0C,	0x0E,	0x10,	0x12,
		0x13,	0x15,	0x17,	0x19,	0x1B,
		0x1C
	};

	int datatype[FirePower_POLCODE_NUM]={		/*0:	FLOAT,	1:MARK,	2:STATUS,	3:TIME*/
		0,	0,	1,	0,	0,	1,
		0,	0,	0,	0,	2,
		0,	0,	0,	0,	2,
		3
	};

	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	

	if(!acq_data) return -1;
	
	acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr&0xffff;
	cmd=0x03;
	cnt=0x22;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ChangRun GongKuang", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<FirePower_POLCODE_NUM;i++)
		{
			switch(datatype[i])
			{
				case 0:
					valf=getFloatValue(p, posnum[i]*2+3, FLOAT_CDAB);
					acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
					break;

				case 1:
					val=getUInt16Value(p, posnum[i]*2+3, INT_AB);
					switch(val)
					{
						case 0:	valflag=MARK_N;		break;
						case 1:	valflag=MARK_DC_Sd;	break;
						case 2:	valflag=MARK_DC_Fa;	break;
						case 3:	valflag=MARK_DC_Fb;	break;
						case 4:	valflag=MARK_DC_Sta;	break;
						case 5:	valflag=MARK_DC_Stb;	break;
						case 6:	valflag=MARK_DC_Sr;	break;
						default:	valflag=MARK_N;	break;
					}
					acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valflag,&arg_n);
					break;

				case 2:
					val=getUInt16Value(p, posnum[i]*2+3, INT_AB);
					valflag=(val==0)?0:1;
					acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valflag,&arg_n);
					break;

				case 3:
					year=getUInt16Value(p, posnum[i]*2+3, INT_AB);
					mon=getUInt16Value(p, posnum[i]*2+5, INT_AB);
					day=getUInt16Value(p, posnum[i]*2+7, INT_AB);
					hour=getUInt16Value(p, posnum[i]*2+9, INT_AB);
					min=getUInt16Value(p, posnum[i]*2+11, INT_AB);
					sec=getUInt16Value(p, posnum[i]*2+13, INT_AB);
					t1=getTimeValue(year+2000, mon, day, hour, min, sec);
					acqdata_set_value(acq_data,polcodestr[i],t1,0,&arg_n);
			}
		}

		status = 0;
	}
	else
	{
		for(i=0;i<FirePower_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
		}
		status = 1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;

}


