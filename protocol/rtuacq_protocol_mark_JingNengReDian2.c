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

static int set_val2(struct acquisition_data *acq_data,float pt,float evaporation,CONDITION_MARK condition_mark,char status)
{
	int arg_n=0;
	int val;
	float mark;
	
	val=condition_mark;
	mark=(float)val;

	acqdata_set_value(acq_data,"p10101",UNIT_NONE,mark,&arg_n);
	acqdata_set_value(acq_data,"p10201",UNIT_T_H,evaporation,&arg_n);
	acqdata_set_value(acq_data,"p10301",UNIT_MW,pt,&arg_n);

	acq_data->dev_stat=status;

	acq_data->acq_status = ACQ_OK;

	return arg_n;
}

static int set_error_val2(struct acquisition_data *acq_data)
{
	return set_val2(acq_data,0,0,MARK_DC_Sr,'D');
}

int protocol_MARK_JingNengReDian2(struct acquisition_data *acq_data)
{
	char com_rbuf[2048]={0}; 
	int size=0;
	int val;
	union uf f;
	float pt=0,evaporation=0;
	CONDITION_MARK mark;
	int ret=0;
	unsigned int devaddr=0;
	char sel_str[10];
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	if(size<29)
	{
		sleep(1);
		size+=read_device(DEV_NAME(acq_data),&(com_rbuf[size]),sizeof(com_rbuf)-1-size);
	}
	SYSLOG_DBG("JingNengReDian mark2 protocol,mark : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JingNengReDian mark2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JingNengReDian mark2 RECV:",com_rbuf,size);
	if(size>=29)
	{
		sel_str[0]=devaddr;
		sel_str[1]=0x10;
		sel_str[2]=0x00;
		p=memstr(com_rbuf,size,sel_str);
		if(p!=NULL)
		{
			f.ch[3]=p[9];
			f.ch[2]=p[10];
			f.ch[1]=p[7];
			f.ch[0]=p[8];
			evaporation=f.f;

			f.ch[3]=p[13];
			f.ch[2]=p[14];
			f.ch[1]=p[11];
			f.ch[0]=p[12];
			pt=f.f;

			val=p[15];
			val<<=8;
			val+=p[16];

			if(val==0x00)
				mark=MARK_N;
			else if((val&0x01)==0x01)
				mark=MARK_DC_Sd;
			else if((val&0x02)==0x02)
				mark=MARK_DC_Fa;
			else if((val&0x04)==0x04)
				mark=MARK_DC_Fb;
			else if((val&0x08)==0x08)
				mark=MARK_DC_Sta;
			else if((val&0x10)==0x10)
				mark=MARK_DC_Stb;
			else if((val&0x20)==0x20)
				mark=MARK_DC_Sr;
			else if((val&0x40)==0x40)
				mark=MARK_N;
			else
				mark=MARK_DC_Sr;
		}
		else
		{
			ret=set_error_val2(acq_data);
			return ret;
		}
	}
	else
	{
		ret=set_error_val2(acq_data);
		return ret;
	}
	
	ret=set_val2(acq_data,pt,evaporation,mark,'N');
	return ret;
}

