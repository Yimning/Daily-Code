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


int MARK_zz_3DI(struct acquisition_data *acq_data,int devaddr,int devfun,int devstaddr,int regcnt,float *zzstat)
{
/*
TX:11 02 00 20 00 04 7A 93
RX:11 02 01 02 24 89 
DI2 is input of voltage
*/
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

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, devfun, devstaddr, regcnt);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG_HEX("zz protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"zz",com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("zz protocol,zz : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("zz data ",com_rbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zz",com_rbuf,size);
	
	if((size>0)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==devfun))
	{
        int di_stat=0;
		float zz_stat=0;

		di_stat=com_rbuf[3];

        if(di_stat & 0x01)  zz_stat=MARK_ZZ_F;
		if(di_stat & 0x02)  zz_stat=MARK_ZZ_Sr;
		if(di_stat & 0x04)  zz_stat=MARK_ZZ_R;

		if(zz_stat==0)
          if((di_stat & 0x08)==0) zz_stat=MARK_ZZ_D;
		
		*zzstat=zz_stat;
	}
	else
	{
		*zzstat=0;
		return -1;
	}

	return 0;
}



int protocol_MARK_zz_3DI(struct acquisition_data *acq_data)
{
/*
TX:11 02 00 20 00 04 7A 93
RX:11 02 01 02 24 89 
DI2 is input of voltage
*/
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
	float zzstat=0;

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int i=0;

   if(!acq_data) return -1;

   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;

   for(i=0;i<modbusarg_running->array_count;i++)
   {
       //char buff[100]={0};
	   if(mpolcodearg[i].enableFlag==0)
	   {	
	   		continue;
	   }

	   ret=MARK_zz_3DI(acq_data,mpolcodearg[i].regaddr&0xff,modbusarg_running->devfun&0xffff,modbusarg_running->devstaddr&0xffff, modbusarg_running->regcnt&0xffff,&zzstat);

	   if(ret==0)
	   {
	        int zzstatInt=(int)zzstat;
			
	        if(zzstatInt==MARK_ZZ_D)
				acqdata_set_value_flag(acq_data,mpolcodearg[i].polcode,mpolcodearg[i].unit,0,'D',&arg_n);
			else
	   		    acqdata_set_value_flag(acq_data,mpolcodearg[i].polcode,mpolcodearg[i].unit,zzstat,'N',&arg_n);
	   }
	   else
	   {
	   	    acqdata_set_value_flag(acq_data,mpolcodearg[i].polcode,mpolcodearg[i].unit,0,'D',&arg_n);
	   }  

	   	SYSLOG_DBG("protocol_MARK_zz_3DI %s,%d:zzstat=%f,ret=%d\n",mpolcodearg[i].polcode,mpolcodearg[i].unit,zzstat,ret);

	}

   acq_data->dev_stat=0xaa;

   	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	//NEED_ERROR_CACHE(acq_data, 10);

	NEED_ERROR_CACHE_ONE_POLCODE_VALUE(acq_data, arg_n, 10);
	
	return arg_n;
}



#if 0
int protocol_MARK_zz_3DI(struct acquisition_data *acq_data)
{
/*
TX:11 02 00 20 00 04 7A 93
RX:11 02 01 02 24 89 
DI2 is input of voltage
*/
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

   SYSLOG_DBG("zz modbusarg : %x, %x,%x,%x,%d\n",modbusarg_running->devaddr&0xffff,modbusarg_running->devfun&0xffff,
   	modbusarg_running->devstaddr&0xffff,modbusarg_running->regcnt&0xffff,modbusarg_running->array_count);
   for(i=0;i<modbusarg_running->array_count && i<MODBUS_POLCODE_MAX;i++)
   {
      SYSLOG_DBG("zz modbusarg  polcode  %d, %s,%x,%d\n",i,
	  	    mpolcodearg[i].polcode,mpolcodearg[i].regaddr&0xffff,mpolcodearg[i].datatype);
   }

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, modbusarg_running->devaddr&0xffff, modbusarg_running->devfun&0xffff,
		               modbusarg_running->devstaddr&0xffff, modbusarg_running->regcnt&0xffff);
	SYSLOG_DBG_HEX("zz protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"zz SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("zz protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("zz protocol recv data",com_rbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zz RECV:",com_rbuf,size);
	
	if(size>0 && 
	   (com_rbuf[0]==(modbusarg_running->devaddr&0xffff))&&
	   (com_rbuf[1]==(modbusarg_running->devfun&0xffff)))
	{
        int di_stat=0;
		float zz_stat=0;

		di_stat=com_rbuf[3];

        if(di_stat | 0x01)zz_stat=MARK_ZZ_F;
		if(di_stat | 0x02)zz_stat=MARK_ZZ_Sr;
		if(di_stat | 0x04)zz_stat=MARK_ZZ_R;

		for(i=0;i<modbusarg_running->array_count;i++)
		{
		   if(mpolcodearg[i].enableFlag==0)
		   {	
		   		continue;
		   }
		   
		   acqdata_set_value(acq_data,mpolcodearg[i].polcode,mpolcodearg[i].unit,zz_stat,&arg_n);
		}
		
	}
	else
	{
		status=1;
		for(i=0;i<modbusarg_running->array_count;i++)
		{
		   acqdata_set_value(acq_data,mpolcodearg[i].polcode,mpolcodearg[i].unit,0,&arg_n);
		}
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
#endif



