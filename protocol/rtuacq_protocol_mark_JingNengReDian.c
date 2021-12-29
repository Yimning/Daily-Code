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

static int set_error_val(struct acquisition_data *acq_data)
{
	int arg_n=0;
	
	acqdata_set_value(acq_data,"p10101",UNIT_NONE,MARK_DC_Sr,&arg_n);
	acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);
	acqdata_set_value(acq_data,"p10301",UNIT_MW,0,&arg_n);
	acqdata_set_value(acq_data,"FJCGLZFL",UNIT_T_H,0,&arg_n);
	acqdata_set_value(acq_data,"FJCRQLUJGL",UNIT_MW,0,&arg_n);
	acqdata_set_value(acq_data,"GLCGLHYL",UNIT_PECENT,0,&arg_n);
	acqdata_set_value(acq_data,"TXTXHYJYL",UNIT_NONE,0,&arg_n);
	acqdata_set_value(acq_data,"CCSSBDCTQQHYC",UNIT_PA,0,&arg_n);
	acqdata_set_value(acq_data,"CCSSJDCCQRCDL",UNIT_A,0,&arg_n);
	acqdata_set_value(acq_data,"CCSSJDCCQRCDY",UNIT_V,0,&arg_n);
	acqdata_set_value(acq_data,"TLSSXSTNJYPHZ",UNIT_PH,0,&arg_n);
	acqdata_set_value(acq_data,"TLSSGJLL",UNIT_M3_H,0,&arg_n);
	acqdata_set_value(acq_data,"TLSSGLLL",UNIT_KG,0,&arg_n);
	acqdata_set_value(acq_data,"TLSSJYXHBYXZT",UNIT_NONE,0,&arg_n);
	acqdata_set_value(acq_data,"SCXBH",UNIT_NONE,0,&arg_n);

	acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_MARK_JingNengReDian(struct acquisition_data *acq_data)
{
	char com_rbuf[2048]={0}; 
	int size=0;
	int val=0;
	float pt=0,evaporation=0;
	int status=0;
	float valf[9]={0};
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

	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	sleep(2);
	size+=read_device(DEV_NAME(acq_data),&(com_rbuf[size]),sizeof(com_rbuf)-1-size);
	SYSLOG_DBG("JingNengReDian mark protocol,mark : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JingNengReDian mark data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JingNengReDian mark RECV:",com_rbuf,size);
	if(size>=40)
	{
		sel_str[0]=devaddr;
		sel_str[1]=0x10;
		sel_str[2]=0x00;
		sel_str[3]=0x00;
		sel_str[4]=0x00;
		sel_str[5]=0x0A;
		p=memstrhex(com_rbuf,size,sel_str,6);
		if(p!=NULL)
		{
			val=p[7];
			val<<=8;
			val+=p[8];
			evaporation=val*0.1;

			val=p[9];
			val<<=8;
			val+=p[10];
			pt=val*0.01;

			val=getUInt16Value(p, 11, INT_AB);
			valf[0]=val*0.01;

			val=getUInt16Value(p, 13, INT_AB);
			valf[1]=val*0.01;

			val=getUInt16Value(p, 15, INT_AB);
			valf[2]=val;

			val=getUInt16Value(p, 17, INT_AB);
			valf[3]=val*0.001;

			val=getUInt16Value(p, 19, INT_AB);
			valf[4]=val;

			val=getUInt16Value(p, 21, INT_AB);
			valf[5]=val*0.01;

			val=getUInt16Value(p, 23, INT_AB);
			valf[6]=val*0.01;

			val=getUInt16Value(p, 25, INT_AB);
			valf[7]=val;
		}
		else
		{
			return set_error_val(acq_data);
		}

		sel_str[0]=devaddr;
		sel_str[1]=0x10;
		sel_str[2]=0x00;
		sel_str[3]=0x0A;
		sel_str[4]=0x00;
		sel_str[5]=0x0A;
		p=memstrhex(com_rbuf,size,sel_str,6);
		if(p!=NULL)
		{
			val=getUInt16Value(p, 7, INT_AB);
			valf[8]=val*0.01;
		}
		else
		{
			return set_error_val(acq_data);
		}

		sel_str[0]=devaddr;
		sel_str[1]=0x0F;
		sel_str[2]=0x00;
		sel_str[3]=0x00;
		sel_str[4]=0x00;
		sel_str[5]=0x0A;
		p=memstrhex(com_rbuf,size,sel_str,6);
		if(p!=NULL)
		{
			val=p[7];
			val<<=8;
			val+=p[8];

			if((val&0x0100)==0x0100)
				mark=MARK_DC_Sd;
			else if((val&0x0200)==0x0200)
				mark=MARK_DC_Fa;
			else if((val&0x0400)==0x0400)
				mark=MARK_DC_Fb;
			else if((val&0x0800)==0x0800)
				mark=MARK_DC_Sta;
			else if((val&0x1000)==0x1000)
				mark=MARK_DC_Stb;
			else if((val&0x2000)==0x2000)
				mark=MARK_DC_Sr;
			else if((val&0x4000)==0x4000)
				mark=MARK_N;
			else
				mark=MARK_N;

			status=(p[7]>>7)&0x01;
		}
		else
		{
			return set_error_val(acq_data);
		}
	}
	else
	{
		return set_error_val(acq_data);
	}
	
	acqdata_set_value(acq_data,"p10101",UNIT_NONE,mark,&ret);
	acqdata_set_value(acq_data,"p10201",UNIT_T_H,evaporation,&ret);
	acqdata_set_value(acq_data,"p10301",UNIT_MW,pt,&ret);
	acqdata_set_value(acq_data,"FJCGLZFL",UNIT_T_H,evaporation,&ret);
	acqdata_set_value(acq_data,"FJCRQLUJGL",UNIT_MW,pt,&ret);
	acqdata_set_value(acq_data,"GLCGLHYL",UNIT_PECENT,valf[0],&ret);
	acqdata_set_value(acq_data,"TXTXHYJYL",UNIT_NONE,valf[1],&ret);
	acqdata_set_value(acq_data,"CCSSBDCTQQHYC",UNIT_PA,valf[2],&ret);
	acqdata_set_value(acq_data,"CCSSJDCCQRCDL",UNIT_A,valf[3],&ret);
	acqdata_set_value(acq_data,"CCSSJDCCQRCDY",UNIT_V,valf[4],&ret);
	acqdata_set_value(acq_data,"TLSSXSTNJYPHZ",UNIT_PH,valf[5],&ret);
	acqdata_set_value(acq_data,"TLSSGJLL",UNIT_M3_H,valf[6],&ret);
	acqdata_set_value(acq_data,"TLSSGLLL",UNIT_KG,valf[7],&ret);
	acqdata_set_value(acq_data,"TLSSJYXHBYXZT",UNIT_NONE,status,&ret);
	acqdata_set_value(acq_data,"SCXBH",UNIT_NONE,valf[8],&ret);

	acq_data->acq_status = ACQ_OK;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return ret;
}

