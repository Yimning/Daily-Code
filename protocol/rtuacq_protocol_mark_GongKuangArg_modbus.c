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


static int datatype2valf(char *com_rbuf,MODBUS_DATA_TYPE datatype,float *valf)
{
    union uf f;
	
    switch(datatype)
    {
        case INT_AB:
		//case INT_BA:
			f.s=com_rbuf[0];
		    f.s<<=8;
			f.s+=com_rbuf[1];
			*valf=f.s;
		break;
		case INT_BA:
		//case INT_AB:
			//f.ch[0]=com_rbuf[1];
			//f.ch[1]=com_rbuf[0];
			f.s=com_rbuf[0];
		    f.s<<=8;
			f.s+=com_rbuf[1];
			*valf=f.s;
			*valf/=100;
		break;
        case LONG_ABCD:
		//case LONG_DCBA:
			//f.ch[0]=com_rbuf[0];
			//f.ch[1]=com_rbuf[1];
			//f.ch[2]=com_rbuf[2];
			//f.ch[3]=com_rbuf[3];
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];

			*valf=f.l;
		break;
		case LONG_CDAB:
		//case LONG_BADC:
			f.l=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];
		    f.l<<=8;
			f.l+=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];	
			*valf=f.l;
		break;
        case LONG_BADC:
		//case LONG_CDAB:
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];
			*valf=f.l/100;
		break;
		case LONG_DCBA:
		//case LONG_ABCD:
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];	
			*valf=f.l/1000;
		break;		
		//case FLOAT_ABCD:
		case FLOAT_DCBA:
			f.ch[0]=com_rbuf[0];
			f.ch[1]=com_rbuf[1];
			f.ch[2]=com_rbuf[2];
			f.ch[3]=com_rbuf[3];
			*valf=f.f;
		break;			
		//case FLOAT_CDAB:
		case FLOAT_BADC:
			f.ch[0]=com_rbuf[2];
			f.ch[1]=com_rbuf[3];
			f.ch[2]=com_rbuf[0];
			f.ch[3]=com_rbuf[1];	
			*valf=f.f;
		break;
        //case FLOAT_BADC:
        case FLOAT_CDAB:
			f.ch[0]=com_rbuf[1];
			f.ch[1]=com_rbuf[0];
			f.ch[2]=com_rbuf[3];
			f.ch[3]=com_rbuf[2];
			*valf=f.f;
		break;
		//case FLOAT_DCBA:
		case FLOAT_ABCD:
			f.ch[0]=com_rbuf[3];
			f.ch[1]=com_rbuf[2];
			f.ch[2]=com_rbuf[1];
			f.ch[3]=com_rbuf[0];
			//printf("datapos %d,%x,%x,%x,%x:%f\n",datapos,
			//	f.ch[0]&0xff,f.ch[1]&0xff,f.ch[2]&0xff,f.ch[3]&0xff,
			//	f.f);

			*valf=f.f;
		break;		

		default:
			return -10;
    }

}

static int mark_data_modbus(struct acquisition_data *acq_data,char *com_tbuf,int size,MODBUS_DATA_TYPE datatype,float *valf,char *polcode)
{
     int ret=0;
	 char com_rbuf[1000]={0};
	 union uf f;
	 char buff[100]={0};

	 if(!acq_data || !com_tbuf || size<=0 || !valf) return 1;

	 ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	 
     ret=write_device(DEV_NAME(acq_data),com_tbuf,size);
	 
	 sprintf(buff,"mark %s send:",polcode);
	 SYSLOG_DBG_HEX(buff, com_tbuf, size);
	 LOG_WRITE_HEX(DEV_NAME(acq_data),0,buff,com_tbuf,size);
     if(ret!=size)
     {
        *valf=0;
		return 1;
     }

     sleep(2);
	 
     ret=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	 SYSLOG_DBG("mark read %s %s size=%d\n",polcode, DEV_NAME(acq_data),ret);
	 
	 sprintf(buff,"mark %s recv:",polcode);
	 SYSLOG_DBG_HEX(buff, com_rbuf, ret);
	 LOG_WRITE_HEX(DEV_NAME(acq_data),1,buff,com_rbuf,size);
     if(ret>=7 && com_rbuf[0]==com_tbuf[0] && com_rbuf[1]==com_tbuf[1] )
     {
		datatype2valf(&com_rbuf[3],datatype,valf);
		SYSLOG_DBG("datatype=%d,%s valf:%f\n",datatype,polcode,*valf);
		     	      
     }
     else
     {
        *valf=0;
		return 1;
     }

	 return 0;
}

static int mark_data_modbus_flag(struct acquisition_data *acq_data,char *com_tbuf,int size,MODBUS_DATA_TYPE datatype,int *data,char *polcode)
{
     int ret=0;
	 char com_rbuf[1000]={0};
	 union uf f;

	 if(!acq_data || !com_tbuf || size<=0 || !data) return 1;

	 ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	 
     ret=write_device(DEV_NAME(acq_data),com_tbuf,size);
	 SYSLOG_DBG_HEX("markflag send", com_tbuf, size);
     if(ret!=size)
     {
        *data=0;
		return 1;
     }

     sleep(2);
	 
     ret=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	 SYSLOG_DBG("markflag read %s %s size=%d\n",polcode,DEV_NAME(acq_data),ret);
	 SYSLOG_DBG_HEX("markflag read", com_rbuf, ret);
     if(ret>=6 && com_rbuf[0]==com_tbuf[0] && com_rbuf[1]==com_tbuf[1] )
     {
        *data=com_rbuf[3];
		     	      
     }
     else
     {
        *data=0;
		return 1;
     }

	 return 0;
}



static struct modbus_polcode_arg * mark_find_modbus_polcode_arg_by_polcode(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode)
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

int protocol_MARK_GongKuangArg_modbus(struct acquisition_data *acq_data)
{
#define MARK_MODBUS_POLCODE_NUM 14
   char *polcodestr[MARK_MODBUS_POLCODE_NUM]={"p10201","p10301","p10202","p10302","p10203",
                                           "p10303","p10101","p20101","p30101","p30201",
                                           "p10102","p20102","p30102","p30202"};
   UNIT_DECLARATION unitstr[MARK_MODBUS_POLCODE_NUM]={UNIT_T_H,UNIT_MW,UNIT_T_H,UNIT_MW,UNIT_T_H,
   	                                        UNIT_MW,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,
   	                                        UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE};
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


     for(i=0;i<MARK_MODBUS_POLCODE_NUM;i++)
     {
        //printf("modmark1 :%d %s %d\n",i,polcodestr[i],modbusarg_running->array_count);  
        mpolcodearg_temp=mark_find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
		if(!mpolcodearg_temp) continue;

		//printf("modtcp2 :%d %s %d %d\n",i,polcodestr[i],modbusarg_running->array_count,mpolcodearg_temp->enableFlag);  

		if(mpolcodearg_temp->enableFlag==0) continue;
        valf=0;
		if((modbusarg_running->devfun&0xffff) == 0x3)
		{
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf, modbusarg_running->devaddr&0xffff, modbusarg_running->devfun&0xffff,
				               mpolcodearg_temp->regaddr&0xffff, modbusarg_running->regcnt&0xffff);

			SYSLOG_DBG_HEX("modmark hex",com_tbuf,size);
			
	     	ret=mark_data_modbus(acq_data,com_tbuf,size,mpolcodearg_temp->datatype,&valf,mpolcodearg_temp->polcode);
			if(ret!=0)
			{
				status=1;
				break;
			}

			if(strstr(mpolcodearg_temp->polcode,"p101"))
			{
			    int valflag=0;
				if(valf>=0.999 && valf<=1.0001 ) valflag=MARK_DC_Sd;
				if(valf>=1.999 && valf<=2.0001 ) valflag=MARK_DC_Fa;
				if(valf>=2.999 && valf<=3.0001 ) valflag=MARK_DC_Fb;
				if(valf>=3.999 && valf<=4.0001 ) valflag=MARK_DC_Sta;
				if(valf>=4.999 && valf<=5.0001 ) valflag=MARK_DC_Stb;
				if(valf>=5.999 && valf<=6.0001 ) valflag=MARK_DC_Sr;	
				valf=valflag;
			}
		}

		if((modbusarg_running->devfun&0xffff) == 0x1 &&
			strstr(mpolcodearg_temp->polcode,"p101"))
		{
		    for(j=0;j<6;j++)
		    {
		        int data=0;
				memset(com_tbuf,0,sizeof(com_tbuf));
				size=modbus_pack(com_tbuf, modbusarg_running->devaddr&0xffff, modbusarg_running->devfun&0xffff,
					               (mpolcodearg_temp->regaddr&0xffff) + j, 0x1);	
				SYSLOG_DBG_HEX("modmarkflag hex",com_tbuf,size);
				
		     	ret=mark_data_modbus_flag(acq_data,com_tbuf,size,mpolcodearg_temp->datatype,&data,mpolcodearg_temp->polcode);
				if(ret!=0)
				{
					status=1;
					break;
				}	

				if(j==0 && data==1 ) valf=MARK_DC_Sd;
				if(j==1 && data==1 ) valf=MARK_DC_Fa;
				if(j==2 && data==1 ) valf=MARK_DC_Fb;
				if(j==3 && data==1 ) valf=MARK_DC_Sta;
				if(j==4 && data==1 ) valf=MARK_DC_Stb;
				if(j==5 && data==1 ) valf=MARK_DC_Sr;
		    }
		}

		ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
     }

	 if(status!=0)
	 {
        for(i=0;i<MARK_MODBUS_POLCODE_NUM;i++)
        {
			ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
        }
	 }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    NEED_ERROR_CACHE(acq_data, 10);

     return arg_n;
}




