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

int protocol_ai_XingZe_CJ01(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int val=0; 
	float ai[8]={0};
	float valf=0;
	int ret=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char *p;

	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	
	devaddr=modbusarg_running->devaddr;
	cmd=0x03;
	cnt=0x08;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "XingZe CJ-01 CEMS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<8;i++)
		{
			val=getInt16Value(p, 3+i*2, INT_AB);
			
			ai[i]=val*0.1;
		}
		status=0;
	}
	else
	{
		status=1;
	}

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		int pos=0,nox=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		if(mpolcodearg->enableFlag==1)
		{
			if((!strncmp(mpolcodearg->polcode,"a21002",6)) && isPolcodeEnable(modbusarg_running, "a21003"))
				continue;
		
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
				valf=(ai[pos]-4)/16;
				valf *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
				valf +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai[%d]=%f pos=%d, XingZe CJ-01 %s valf:%f,max %f,min %f\n",
					i,ai[pos],pos,mpolcodearg->polcode,valf,mpolcodearg->alarmMax,mpolcodearg->alarmMin);

				if(isPolcodeEnable(modbusarg_running, "a21003a"))
				{
					if(!strcmp(mpolcodearg->polcode,"a21003a"))
					{
						nox=NO_to_NOx(valf);
						acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
						acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
						acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
					}
				}
				else
				{
					if(!strcmp(mpolcodearg->polcode,"a21003"))
					{
						nox=NO_to_NOx(valf);
						acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
						acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
						acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
					}
				}
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,valf,&arg_n);
			}
			else
			{
				if(isPolcodeEnable(modbusarg_running, "a21003a"))
				{
					if(!strcmp(mpolcodearg->polcode,"a21003a"))
					{
						nox=0;
						acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
						acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
						acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
					}
				}
				else
				{
					if(!strcmp(mpolcodearg->polcode,"a21003"))
					{
						nox=0;
						acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
						acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
						acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
					}
				}
			
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,valf,&arg_n);
			}
		}

	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

	return arg_n;
}
