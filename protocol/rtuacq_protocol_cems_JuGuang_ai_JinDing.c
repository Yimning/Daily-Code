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

static int JuGuang_cems_get_valf_JinDing(char *com_rbuf,unsigned int size,unsigned int regaddr,float *valf)
{
	union uf  f;
	unsigned int datalen=0;
	int datapos=0;

	if(!com_rbuf || !valf) return -1;

	if(regaddr<1 || regaddr>8) return -1;

	datapos=3+(regaddr-1)*4;


	f.ch[0]=com_rbuf[1+datapos];
	f.ch[1]=com_rbuf[0+datapos];
	f.ch[2]=com_rbuf[3+datapos];
	f.ch[3]=com_rbuf[2+datapos];
	*valf=f.f;
		
	return 0;
}

static struct modbus_polcode_arg * JuGuang_cems_find_arg_by_polcode_JinDing(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode)
{
	int i=0;

	if(!mpolcodearg ||num<=0 || !polcode) return NULL;

	for(i=0;i<num;i++)
	{
		if(!strcmp(mpolcodearg[i].polcode,polcode)) 
			return &mpolcodearg[i];
	}

	return NULL;
}

static float JuGuang_cems_change_to_ai_value_JinDing(float valf,struct modbus_polcode_arg *mpolcodearg)
{
	float valuemax=0, valuemin=0;

	if(!mpolcodearg) return 0;

	valuemax=mpolcodearg->alarmMax;
	valuemin=mpolcodearg->alarmMin;

	if(valf<=4.0) return valuemin;
	if(valuemax==valuemin) return 0;
	if(valuemax<valuemin) return 0;

	valf =((valf-4) / (20-4) ) *(valuemax-valuemin)+valuemin;

	return valf;
}


extern char CEMS_JuGuang_HeBei_JinDing_STATUS;

int protocol_cems_JuGuang_ai_JinDing(struct acquisition_data *acq_data)
{
/*
TX:03 03 00 00 00 11 84 24
RX:03 03 22 00 00 40 80 00 00 40 80 95 dc 41 24 54 14 41 3f 33 34 40 d6 d9 2c 40 b2 00 00 40 80 00 00 40 80 00 ff c5 21

*/
#define CEMS_POLCODE_NUM 7

	char *polcodestr[CEMS_POLCODE_NUM]={
		"a01017","a01012","a01013","a21005a","a21001a","a34013a","a01011"
	};
	UNIT_DECLARATION unitstr[CEMS_POLCODE_NUM]={
		UNIT_PA,	UNIT_0C,		UNIT_PA,    UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_M_S
	};

	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[500]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int devaddr=0;
	int devfun=0;

	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

	int i=0;
	int ret=0;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;


	memset(com_tbuf,0,sizeof(com_tbuf));
	devaddr=modbusarg_running->devaddr&0xffff;
	devfun=modbusarg_running->devfun&0xffff;
	size=modbus_pack(com_tbuf, devaddr, devfun,0x00, 0x11);
	SYSLOG_DBG_HEX("JinDing protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JinDing SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang cems AI protocol : CEMS AI read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang cems AI protocol recv data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang cems AI RECV:",com_rbuf,size);
	if((size>=39) && (com_rbuf[0]==devaddr) && (com_rbuf[1]==devfun))
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			mpolcodearg_temp=JuGuang_cems_find_arg_by_polcode_JinDing(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			if(mpolcodearg_temp)
			{
				float valf=0.0;
				ret=JuGuang_cems_get_valf_JinDing(com_rbuf,size,mpolcodearg_temp->regaddr,&valf);
				
				if(ret<0) 
				{
					valf=0.0;
				}

				valf=JuGuang_cems_change_to_ai_value_JinDing(valf,mpolcodearg_temp);
				if(mpolcodearg_temp->enableFlag==1)
				{
					acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
					
			        if(!strcmp(polcodestr[i],"a34013a"))
			        {
						acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf,&arg_n);
					    acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,valf,&arg_n);
				    }
			  }

			}
		}

		status=0;
	}
	else
	{
		status=1;
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
		}
		//acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
	}

	//acqdata_set_value(acq_data,"a01011",UNIT_M_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	//acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
	//acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21001",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21001z",UNIT_MG_M3,0,&arg_n);


    acq_data->dev_stat=CEMS_JuGuang_HeBei_JinDing_STATUS;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;
}

