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

#include "rtuacq_protocol_error_cache.h"


static int HeBeiXT_cems_get_valf(char *com_rbuf,unsigned int size,unsigned int regaddr,float *valf)
{
	union uf  f;
	unsigned int datalen=0;
	int datapos=0;

	if(!com_rbuf || !valf) return -1;

	if(regaddr<0) return -1;

	datapos=3+(regaddr);


	//f.ch[0]=com_rbuf[1+datapos];
	//f.ch[1]=com_rbuf[0+datapos];
	//f.ch[2]=com_rbuf[3+datapos];
	//f.ch[3]=com_rbuf[2+datapos];
	
	//f.ch[0]=com_rbuf[3+datapos];
	//f.ch[1]=com_rbuf[2+datapos];
	//f.ch[2]=com_rbuf[1+datapos];
	//f.ch[3]=com_rbuf[0+datapos];	
	
	//f.ch[0]=com_rbuf[2+datapos];
	//f.ch[1]=com_rbuf[3+datapos];
	//f.ch[2]=com_rbuf[0+datapos];
	//f.ch[3]=com_rbuf[1+datapos];	

	//f.ch[0]=com_rbuf[0+datapos];
	//f.ch[1]=com_rbuf[1+datapos];
	//f.ch[2]=com_rbuf[2+datapos];
	//f.ch[3]=com_rbuf[3+datapos];	
	
	f.ch[0]=com_rbuf[1+datapos];
	f.ch[1]=com_rbuf[0+datapos];
	f.ch[2]=com_rbuf[3+datapos];
	f.ch[3]=com_rbuf[2+datapos];
	*valf=f.f;
		
	return 0;
}

static struct modbus_polcode_arg * HeBeiXT_cems_find_arg_by_polcode(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode)
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


int protocol_CEMS_LaJi_pack_HeBeiXT(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float valf=0; 
   union uf f;

#define HeBeiXT_CEMS_POLCODE_NUM 15
	char *polcodestr[HeBeiXT_CEMS_POLCODE_NUM]={
		"a01901","a01902","i33332","i33331","i33330","i33312",
		"i33311","i33310","i33322","i33321","i33320",
		"i33342","i33341","i33300","p10201"
	};
	UNIT_DECLARATION unitstr[HeBeiXT_CEMS_POLCODE_NUM]={
		UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,
		UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,
		UNIT_0C,UNIT_0C,UNIT_0C,UNIT_T_H
	};

	int r_addr[HeBeiXT_CEMS_POLCODE_NUM]={
		0,	4,  8,	12,	16,20,
		24,	28,	32,	36,40,	
		44,	48,	52,	60
	};

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int devaddr=0,devfun=0;
   
   int i=0;

   if(!acq_data) return -1;

   acq_data->total=0;

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg; 
   devaddr=modbusarg_running->devaddr;
   devfun=modbusarg_running->devfun;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, devfun, modbusarg_running->devstaddr&0xff, modbusarg_running->regcnt&0xff);
	//size=modbus_pack(com_tbuf, devaddr, devfun, modbusarg_running->devstaddr&0xff,33);
	SYSLOG_DBG_HEX("HeBeiXT CEMS data send",com_tbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HeBeiXT SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("HeBeiXT CEMS protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HeBeiXT CEMS data RECV",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBeiXT RECV:",com_rbuf,size);

	if(size>=30 && com_rbuf[0]==com_tbuf[0] && com_rbuf[1]==com_tbuf[1])
	{
	
		for(i=0;i<HeBeiXT_CEMS_POLCODE_NUM;i++)
		{
			mpolcodearg_temp=HeBeiXT_cems_find_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			if(mpolcodearg_temp)
			{
				valf=0.0;
				ret=HeBeiXT_cems_get_valf(com_rbuf,size,(r_addr[i]),&valf);
				
				if(ret<0) 
				{
					valf=0.0;
				}
			}

			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
		}
	}
    else
    {
	    ret=acqdata_set_value(acq_data,"a01901",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a01902",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33332",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33331",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33330",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33312",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33311",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33310",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33322",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33321",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33320",UNIT_0C,0,&arg_n);

		ret=acqdata_set_value(acq_data,"i33342",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33341",UNIT_0C,0,&arg_n);	
		ret=acqdata_set_value(acq_data,"i33300",UNIT_0C,0,&arg_n);	
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}

