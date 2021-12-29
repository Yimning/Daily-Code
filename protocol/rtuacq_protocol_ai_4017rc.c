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

int protocol_ai_4017rc(struct acquisition_data *acq_data)
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
	int devaddr;

	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x08);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ai 4017rc CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ai 4017rc  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ai 4017rc CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ai 4017rc CEMS RECV:",com_rbuf,size);
	if((size>=21)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<8;i++)
		{
			val=com_rbuf[3+i*2];
			val<<=8;
			val+=com_rbuf[4+i*2];
			if(val>39321)
			 ai[i]=(val-39321)*16/26214.0+4;
			else 
			 ai[i]=4;
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
		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
				valf=(ai[pos]-4)/16;
				valf *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
				valf +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d %f pos=%d, 4017rc %s valf:%f,max %f,min %f\n",i,ai[pos],pos,
					mpolcodearg->polcode,valf,mpolcodearg->alarmMax,mpolcodearg->alarmMin);
				
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

	NEED_ERROR_CACHE(acq_data,20);

	return arg_n;
}
