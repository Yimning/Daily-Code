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



static int modtcp_data_modbus(struct acquisition_data *acq_data,char *com_tbuf,int size,float *valf)
{
     int ret=0;
	 char com_rbuf[1000]={0};
	 union uf f;

	 if(!acq_data || !com_tbuf || size<=0 || !valf) return 1;
	 
     ret=write_device(DEV_NAME(acq_data),com_tbuf,size);
	 SYSLOG_DBG_HEX("modbusTcp send", com_tbuf, size);
     if(ret!=size)
     {
        *valf=0;
		return 1;
     }

     sleep(2);
	 
     ret=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	 SYSLOG_DBG("modbusTcp read %s size=%d\n",DEV_NAME(acq_data),ret);
	 SYSLOG_DBG_HEX("modbusTcp read", com_rbuf, ret);
     if(ret>=13 && com_rbuf[0]==0 && com_rbuf[6]==0x1 )
     {
		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[12];
		f.ch[3]=com_rbuf[11];		
		*valf=f.f;      	      
     }
     else
     {
        *valf=0;
		return 1;
     }

	 return 0;
}

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

int protocol_modtcp_DCS_GongKuang_modbus(struct acquisition_data *acq_data)
{
#define DCS_MODTCP_MODBUS_POLCODE_NUM 6
   char *polcodestr[DCS_MODTCP_MODBUS_POLCODE_NUM]={"p10201","p10301","p10202","p10302","p10203",
                                           "p10303"};
   UNIT_DECLARATION unitstr[DCS_MODTCP_MODBUS_POLCODE_NUM]={UNIT_T_H,UNIT_MW,UNIT_T_H,UNIT_MW,UNIT_T_H,
   	                                        UNIT_MW};
	/*
    char com_tbuf[4][12]={
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x28,0x9E,0x0,0x04},// 10398
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x4B,0x1A,0x0,0x04},// 19226
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x32,0xDC,0x0,0x04},// 13020
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x4B,0x22,0x0,0x04},// 19234
	};
	float valf[4]={0};
	*/
    char com_tbuf[500]={0};
	float valf=0;
    int arg_n=0;
    int size=0;
    int status=0;	
    int ret=0;
    int i=0,j=0;
   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   
   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
        for(i=0;i<DCS_MODTCP_MODBUS_POLCODE_NUM;i++)
        {
			ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
        }
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
     }

     for(i=0;i<DCS_MODTCP_MODBUS_POLCODE_NUM;i++)
     {
        printf("modtcp1 :%d %s %d\n",i,polcodestr[i],modbusarg_running->array_count);  
        mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
		if(!mpolcodearg_temp) continue;

		printf("modtcp2 :%d %s %d %d\n",i,polcodestr[i],modbusarg_running->array_count,mpolcodearg_temp->enableFlag);  

		if(mpolcodearg_temp->enableFlag==0) continue;
		
        memset(com_tbuf,0,sizeof(com_tbuf));
		com_tbuf[0]=0x0;com_tbuf[1]=0x0;com_tbuf[2]=0x0;com_tbuf[3]=0x0;com_tbuf[4]=0x0;com_tbuf[5]=0x6;
		com_tbuf[6]=modbusarg_running->devaddr&0xffff;
		com_tbuf[7]=modbusarg_running->devfun&0xffff;
		com_tbuf[8]=(mpolcodearg_temp->regaddr>>8)&0xffff;
		com_tbuf[9]=mpolcodearg_temp->regaddr&0xffff;
		com_tbuf[10]=(modbusarg_running->regcnt>>8)&0xffff;
		com_tbuf[11]=modbusarg_running->regcnt&0xffff;

		size=12;

		SYSLOG_DBG_HEX("modtcp hex",com_tbuf,size);
		
     	ret=modtcp_data_modbus(acq_data,com_tbuf,size,&valf);
		if(ret!=0)
		{
			status=1;
			break;
		}

		ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
     }

	 if(status!=0)
	 {
        for(i=0;i<DCS_MODTCP_MODBUS_POLCODE_NUM;i++)
        {
			ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
        }
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    NEED_ERROR_CACHE(acq_data, 10);

     return arg_n;
}




