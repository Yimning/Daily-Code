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
static struct modbus_polcode_arg * find_modbus_polcode_arg_by_polcode(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode)
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
*/

int protocol_MARK_8DI(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int val;
	float mark=0;
	int dival=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   
   int i=0;

   if(!acq_data) return -1;

   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;

	mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,"p10101");
	if(mpolcodearg_temp && mpolcodearg_temp->enableFlag==1)
	{
	    dival=1;
		mark=MARK_N;
	    ret=read_device(DEV_DI0, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_DC_Sd;
	    ret=read_device(DEV_DI1, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_DC_Fa;
	    ret=read_device(DEV_DI2, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_DC_Fb;
	    ret=read_device(DEV_DI3, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_DC_Sta;
	    ret=read_device(DEV_DI4, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_DC_Stb;
	    ret=read_device(DEV_DI5, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_DC_Sr;		
		
		acqdata_set_value(acq_data,"p10101",UNIT_NONE,mark,&arg_n);
	}

	mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,"p20101");
	if(mpolcodearg_temp && mpolcodearg_temp->enableFlag==1)
	{
	    dival=1;
		mark=MARK_N;
	    ret=read_device(DEV_DI0, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_NY_Sda;
	    ret=read_device(DEV_DI1, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_NY_Sdb;
	    ret=read_device(DEV_DI2, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_NY_F;
	    ret=read_device(DEV_DI3, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_NY_Sta;
	    ret=read_device(DEV_DI4, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_NY_Stb;
	    ret=read_device(DEV_DI5, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_NY_Sr;		
		
		acqdata_set_value(acq_data,"p20101",UNIT_NONE,mark,&arg_n);
	}

	mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,"p30101");
	if(mpolcodearg_temp && mpolcodearg_temp->enableFlag==1)
	{
	    dival=1;
		mark=MARK_N;
	    ret=read_device(DEV_DI0, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_ZZ_F;
	    ret=read_device(DEV_DI1, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_ZZ_Sr;
	    ret=read_device(DEV_DI2, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_ZZ_R;	
		
		acqdata_set_value(acq_data,"p30101",UNIT_NONE,mark,&arg_n);
	}

	mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,"p30201");
	if(mpolcodearg_temp && mpolcodearg_temp->enableFlag==1)
	{
	    dival=1;
		mark=MARK_N;
	    ret=read_device(DEV_DI3, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_ZZ_F;
	    ret=read_device(DEV_DI4, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_ZZ_Sr;
	    ret=read_device(DEV_DI5, &dival, sizeof(int));
		if(ret>0 && dival==0) mark=MARK_ZZ_R;	
		
		acqdata_set_value(acq_data,"p30201",UNIT_NONE,mark,&arg_n);
	}

	status = 0;
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

