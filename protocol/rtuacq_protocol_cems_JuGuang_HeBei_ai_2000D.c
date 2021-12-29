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

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.08.25 Wed.
Description:protocol_cems_JuGuang_HeBei_ai_2000D
TX:03 03 00 00 00 11 84 24
RX:03 03 22 00 00 40 80 00 00 40 80 95 dc 41 24 54 14 41 3f 33 34 40 d6 d9 2c 40 b2 00 00 40 80 00 00 40 80 00 ff c5 21

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00 = 1440.25
*/


int protocol_cems_JuGuang_HeBei_ai_2000D(struct acquisition_data *acq_data)
{

	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[500]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int devaddr=0;
	int devfun=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	float valf = 0;
	int val = 0;
	char *p=NULL;
	float ai[8]={0};
	float speed = 0,PTC = 0;
		
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;
	MODBUS_DATA_TYPE dataType;
	
	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;
	
	int i=0;
	int ret=0;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr&0xffff;
	devfun=modbusarg_running->devfun&0xffff;
	regpos = 0x00;
	regcnt = 0x11;
	dataType = FLOAT_ABCD;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, devfun,regpos,regcnt);
	SYSLOG_DBG_HEX("JuGuang protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang cems AI protocol : CEMS AI read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang cems AI protocol recv data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang cems AI RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		for(i=0;i<8;i++)
		{
			valf = getFloatValue(p, 3+i*4, dataType);
			ai[i]=valf;
		}
		status=0;
	}
	else
	{
		status=1;
	}
	
	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		int pos=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		
		//if((!strcmp(mpolcodearg->polcode,"a01011")) && isPolcodeEnable(modbusarg_running,"a01017"))
			//continue;
		
		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
				valf=(ai[pos]-4)/16;
				valf *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
				valf +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d %f pos=%d, ai 2000D %s valf:%f,max %f,min %f\n",i,ai[pos],pos,
					mpolcodearg->polcode,valf,mpolcodearg->alarmMax,mpolcodearg->alarmMin);

				#if 0
				if(!strcmp(mpolcodearg->polcode,"a01017"))
				{
					speed=PTC*sqrt(fabs(valf)*2/SAD);
					acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
				}
				#endif
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,valf,&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}

	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;
}
