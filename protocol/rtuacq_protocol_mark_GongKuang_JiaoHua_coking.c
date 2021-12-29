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
RX:01 03 88 3F 80 00 00 40 00 00 00 40 40 00 0040 80 00 00 40 A0 00 00 40 C0 00 00 40 E0 00 00 41 00 00 00 41 10 00 00 41 20 00 00 41 30 00 00 41 40 00 00 41 50 00 00 41 60 00 00 41 70 00 00 41 80 00 00 41 88 00 00 41 90 00 00 41 98 00 00 41 A0 00 00 41 A8 00 00 41 B0 00 00 41 B8 00 00 41 C0 00 00 41 C8 00 00 41 D0 00 00 41 D8 00 00 41 E0 00 00 41 E8 00 00 41 F0 00 00 41 F8 00 00 42 00 00 00 00 06 00 0F 00 01 00 1A 8E B0
*/

/*
RX2:01 03 5C 3F 80 00 00 07 E5 00 0A 00 09 00 17 00 25 00 18 07 E5 00 0A 00 09 00 17 00 1D 00 30 00 00 00 00 43 38 00 00 45 CC EC E1 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 43 7F 8F 5C 42 15 7A E1 07 E5 00 0A 00 0A 00 11 00 36 00 31 1f 17
*/
int protocol_mark_GongKuang_JiaoHua_coking(struct acquisition_data *acq_data)
{
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	/*
	char com_rbuf[2048]={
	0x01 ,0x03 ,0x5C ,0x3F ,0x80 ,0x00 ,0x00 ,0x07 ,0xE5 ,0x00 
	,0x0A ,0x00 ,0x09 ,0x00 ,0x17 ,0x00 ,0x25 ,0x00 ,0x18 ,0x07 
	,0xE5 ,0x00 ,0x0A ,0x00 ,0x09 ,0x00 ,0x17 ,0x00 ,0x1D ,0x00
	,0x30 ,0x00 ,0x00 ,0x00 ,0x00 ,0x43 ,0x38 ,0x00 ,0x00 ,0x45
	,0xCC ,0xEC ,0xE1 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x43 ,0x7F ,0x8F ,0x5C ,0x42
	,0x15 ,0x7A ,0xE1 ,0x07 ,0xE5 ,0x00 ,0x0A ,0x00 ,0x0A ,0x00
	,0x11 ,0x00 ,0x36 ,0x00 ,0x31 ,0x1f ,0x17};	
	*/
	char com_tbuf[100]={0}; 

	//int size=0;
	int size=97;
	int val;
	union uf f;
	int ret=0;
	unsigned int devaddr=0;
	unsigned int cmd;
	int i=0;

#define Coking_POLCODE_NUM   17
   char polcodestr[][20]={"SCXBH",
                                                                                       "JHGYDZMSJ","JHGYDTJSJ","JHGYDZMCDL","JHGYDTJCDL","JHGYDMQSYL",
											"CCSSBDCCQQHYC","CCSSDCCQRCDL","CCSSDCCQRCDY","TLSSXSJPHZ","TLSSGJLL",
											"TLSSGLLL","TLSSJYXHBYXZT", "TLSSSFYHFJDL","TXSSYQSD","TXSSTXHYJYL",
											"GXSJ"
											}
;
   UNIT_DECLARATION unitstr[Coking_POLCODE_NUM]={UNIT_NONE,
   	                                                                                               UNIT_NONE,UNIT_NONE,UNIT_A,UNIT_A,UNIT_NONE,
													UNIT_V,UNIT_A,UNIT_V,UNIT_NONE,UNIT_M3_H,
													UNIT_KG,UNIT_NONE,UNIT_A,UNIT_0C,UNIT_NONE,
													UNIT_NONE
													};


    int posnum[Coking_POLCODE_NUM]={
	0,
   	2,8,14,16,18,
		20,22,24,26,28,
		30,32,34,36,38,
		40
   	};
   

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int log_len=0;
	

	if(!acq_data) return -1;
	
	   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

	   acq_ctrl=ACQ_CTRL(acq_data);
	   modbusarg_running=&acq_ctrl->modbusarg_running;
	   mpolcodearg=modbusarg_running->polcode_arg;
	   devaddr=modbusarg_running->devaddr&0xffff;

	   SYSLOG_DBG("JiaoHua protocol modbusarg : %x, %x,%x,%x,%d\n",modbusarg_running->devaddr&0xffff,modbusarg_running->devfun&0xffff,
	   	modbusarg_running->devstaddr&0xffff,modbusarg_running->regcnt&0xffff,modbusarg_running->array_count);
	   for(i=0;i<modbusarg_running->array_count && i<MODBUS_POLCODE_MAX;i++)
	   {
	      SYSLOG_DBG("JiaoHua protocol modbusarg  polcode  %d, %s,%x,%d\n",i,
		  	    mpolcodearg[i].polcode,mpolcodearg[i].regaddr&0xffff,mpolcodearg[i].datatype);
	   }

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03,0x00, 46);
	SYSLOG_DBG_HEX("JiaoHua protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JiaoHua SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);

      	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("JiaoHua protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JiaoHua protocol recv data",com_rbuf,size);

        //size=97;
	for(log_len=size;log_len>=200;log_len-=200)
	{
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JiaoHua modbus RECV:",&com_rbuf[size-log_len],log_len+1);
	}
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JiaoHua modbus RECV:",&com_rbuf[size-log_len],log_len+1);

        if(size>10 && (com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
        {
		for(i=0;i<Coking_POLCODE_NUM;i++)
		{
			  mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			   if(mpolcodearg_temp && 
			   	mpolcodearg_temp->enableFlag==1)
			   {
				       float valf=0.0;
				      	int tm[TIME_ARRAY]={0};

				   
				       	if(
							!strcmp(polcodestr[i],"JHGYDZMSJ")||
							!strcmp(polcodestr[i],"JHGYDTJSJ") ||
							!strcmp(polcodestr[i],"GXSJ"))
					{
					        unsigned long tmlong=0;
					        ret=modbus_get_valf(com_rbuf,size,posnum[i],INT_AB,&valf);
						tm[5]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+1,INT_AB,&valf);
						tm[4]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+2,INT_AB,&valf);
						tm[3]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+3,INT_AB,&valf);
						tm[2]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+4,INT_AB,&valf);
						tm[1]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+5,INT_AB,&valf);
						tm[0]=(int)valf;	

                                                tmarray2tmlong(tm,&tmlong);
					   SYSLOG_DBG("JiaoHua protocol modbus_get_valf %s ,%f,%d,%d,tm=%04d%02d%02d%02d%02d%02d,tmlong=%d\n",polcodestr[i],valf,
					   	       posnum[i],mpolcodearg_temp->datatype,tm[5],tm[4],tm[3],tm[2],tm[1],tm[0],tmlong);	
					   
						ret=acqdata_set_value(acq_data,polcodestr[i],tmlong,0,&arg_n);
						continue;
					}
				   
			               //ret=modbus_get_valf(com_rbuf,size,mpolcodearg_temp->regaddr,mpolcodearg_temp->datatype,&valf);
			               ret=modbus_get_valf(com_rbuf,size,posnum[i],FLOAT_ABCD,&valf);
					   //SYSLOG_DBG("steel protocol modbus_get_valf %s ,%f,%d,%d,ret=%d\n",polcodestr[i],valf,
					   //	       mpolcodearg_temp->regaddr&0xffff,mpolcodearg_temp->datatype,ret);		
					   SYSLOG_DBG("JiaoHua protocol modbus_get_valf %s ,%f,%d,%d,ret=%d\n",polcodestr[i],valf,
					   	       posnum[i],mpolcodearg_temp->datatype,ret);						   
					ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
			   }
		}
        }
	else
	{
		status =1;
		for(i=0;i<Coking_POLCODE_NUM;i++)
		{
			  mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			   if(mpolcodearg_temp && 
			   	mpolcodearg_temp->enableFlag==1)
			   {		
			       	if(
						!strcmp(polcodestr[i],"JHGYDZMSJ")||
						!strcmp(polcodestr[i],"JHGYDTJSJ") ||
						!strcmp(polcodestr[i],"GXSJ"))
				{
				      ret=acqdata_set_value(acq_data,polcodestr[i],0,0,&arg_n);
				      continue;
			       	}
				ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
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

int protocol_mark_GongKuang_JiaoHua_coking_staddr(struct acquisition_data *acq_data)
{
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	/*
	char com_rbuf[2048]={
	0x01 ,0x03 ,0x5C ,0x3F ,0x80 ,0x00 ,0x00 ,0x07 ,0xE5 ,0x00 
	,0x0A ,0x00 ,0x09 ,0x00 ,0x17 ,0x00 ,0x25 ,0x00 ,0x18 ,0x07 
	,0xE5 ,0x00 ,0x0A ,0x00 ,0x09 ,0x00 ,0x17 ,0x00 ,0x1D ,0x00
	,0x30 ,0x00 ,0x00 ,0x00 ,0x00 ,0x43 ,0x38 ,0x00 ,0x00 ,0x45
	,0xCC ,0xEC ,0xE1 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x43 ,0x7F ,0x8F ,0x5C ,0x42
	,0x15 ,0x7A ,0xE1 ,0x07 ,0xE5 ,0x00 ,0x0A ,0x00 ,0x0A ,0x00
	,0x11 ,0x00 ,0x36 ,0x00 ,0x31 ,0x1f ,0x17};	
	*/
	char com_tbuf[100]={0}; 

	//int size=0;
	int size=97;
	int val;
	union uf f;
	int ret=0;
	unsigned int devaddr=0;
	unsigned int cmd;
	int i=0;

#define Coking_POLCODE_NUM   17
   char polcodestr[][20]={"SCXBH",
                                                                                       "JHGYDZMSJ","JHGYDTJSJ","JHGYDZMCDL","JHGYDTJCDL","JHGYDMQSYL",
											"CCSSBDCCQQHYC","CCSSDCCQRCDL","CCSSDCCQRCDY","TLSSXSJPHZ","TLSSGJLL",
											"TLSSGLLL","TLSSJYXHBYXZT", "TLSSSFYHFJDL","TXSSYQSD","TXSSTXHYJYL",
											"GXSJ"
											}
;
   UNIT_DECLARATION unitstr[Coking_POLCODE_NUM]={UNIT_NONE,
   	                                                                                               UNIT_NONE,UNIT_NONE,UNIT_A,UNIT_A,UNIT_NONE,
													UNIT_V,UNIT_A,UNIT_V,UNIT_NONE,UNIT_M3_H,
													UNIT_KG,UNIT_NONE,UNIT_A,UNIT_0C,UNIT_NONE,
													UNIT_NONE
													};


    int posnum[Coking_POLCODE_NUM]={
	0,
   	2,8,14,16,18,
		20,22,24,26,28,
		30,32,34,36,38,
		40
   	};
   

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int log_len=0;
	

	if(!acq_data) return -1;
	
	   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

	   acq_ctrl=ACQ_CTRL(acq_data);
	   modbusarg_running=&acq_ctrl->modbusarg_running;
	   mpolcodearg=modbusarg_running->polcode_arg;
	   devaddr=modbusarg_running->devaddr&0xffff;

	   SYSLOG_DBG("JiaoHua protocol modbusarg : %x, %x,%x,%x,%d\n",modbusarg_running->devaddr&0xffff,modbusarg_running->devfun&0xffff,
	   	modbusarg_running->devstaddr&0xffff,modbusarg_running->regcnt&0xffff,modbusarg_running->array_count);
	   for(i=0;i<modbusarg_running->array_count && i<MODBUS_POLCODE_MAX;i++)
	   {
	      SYSLOG_DBG("JiaoHua protocol modbusarg  polcode  %d, %s,%x,%d\n",i,
		  	    mpolcodearg[i].polcode,mpolcodearg[i].regaddr&0xffff,mpolcodearg[i].datatype);
	   }

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03,modbusarg_running->devstaddr&0xffff, 46);
	SYSLOG_DBG_HEX("JiaoHua protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JiaoHua SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);

      	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("JiaoHua protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JiaoHua protocol recv data",com_rbuf,size);

        //size=97;
	for(log_len=size;log_len>=200;log_len-=200)
	{
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JiaoHua modbus RECV:",&com_rbuf[size-log_len],log_len+1);
	}
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JiaoHua modbus RECV:",&com_rbuf[size-log_len],log_len+1);

        if(size>10 && (com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
        {
		for(i=0;i<Coking_POLCODE_NUM;i++)
		{
			  mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			   if(mpolcodearg_temp && 
			   	mpolcodearg_temp->enableFlag==1)
			   {
				       float valf=0.0;
				      	int tm[TIME_ARRAY]={0};

				   
				       	if(
							!strcmp(polcodestr[i],"JHGYDZMSJ")||
							!strcmp(polcodestr[i],"JHGYDTJSJ") ||
							!strcmp(polcodestr[i],"GXSJ"))
					{
					        unsigned long tmlong=0;
					        ret=modbus_get_valf(com_rbuf,size,posnum[i],INT_AB,&valf);
						tm[5]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+1,INT_AB,&valf);
						tm[4]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+2,INT_AB,&valf);
						tm[3]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+3,INT_AB,&valf);
						tm[2]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+4,INT_AB,&valf);
						tm[1]=(int)valf;
						ret=modbus_get_valf(com_rbuf,size,posnum[i]+5,INT_AB,&valf);
						tm[0]=(int)valf;	

                                                tmarray2tmlong(tm,&tmlong);
					   SYSLOG_DBG("JiaoHua protocol modbus_get_valf %s ,%f,%d,%d,tm=%04d%02d%02d%02d%02d%02d,tmlong=%d\n",polcodestr[i],valf,
					   	       posnum[i],mpolcodearg_temp->datatype,tm[5],tm[4],tm[3],tm[2],tm[1],tm[0],tmlong);	
					   
						ret=acqdata_set_value(acq_data,polcodestr[i],tmlong,0,&arg_n);
						continue;
					}
				   
			               //ret=modbus_get_valf(com_rbuf,size,mpolcodearg_temp->regaddr,mpolcodearg_temp->datatype,&valf);
			               ret=modbus_get_valf(com_rbuf,size,posnum[i],FLOAT_ABCD,&valf);
					   //SYSLOG_DBG("steel protocol modbus_get_valf %s ,%f,%d,%d,ret=%d\n",polcodestr[i],valf,
					   //	       mpolcodearg_temp->regaddr&0xffff,mpolcodearg_temp->datatype,ret);		
					   SYSLOG_DBG("JiaoHua protocol modbus_get_valf %s ,%f,%d,%d,ret=%d\n",polcodestr[i],valf,
					   	       posnum[i],mpolcodearg_temp->datatype,ret);						   
					ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
			   }
		}
        }
	else
	{
		status =1;
		for(i=0;i<Coking_POLCODE_NUM;i++)
		{
			  mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			   if(mpolcodearg_temp && 
			   	mpolcodearg_temp->enableFlag==1)
			   {		
			       	if(
						!strcmp(polcodestr[i],"JHGYDZMSJ")||
						!strcmp(polcodestr[i],"JHGYDTJSJ") ||
						!strcmp(polcodestr[i],"GXSJ"))
				{
				      ret=acqdata_set_value(acq_data,polcodestr[i],0,0,&arg_n);
				      continue;
			       	}
				ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
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



