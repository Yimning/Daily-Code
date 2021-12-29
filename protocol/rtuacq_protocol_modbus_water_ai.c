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
//#include "devices_mng_api.h"

//#include "rtuacq_protocol_error_cache.h"

#define MODBUS_RECV_DATA_POS 3

static int modbus_get_valf(char *com_rbuf,unsigned int size,unsigned int regaddr,MODBUS_DATA_TYPE datatype,float *valf)
{
    union uf  f;
	unsigned int datalen=0;
	int datapos=0;

	if(!com_rbuf || !valf) return -1;

	if(regaddr>=size) return -1;

    datalen=com_rbuf[2];
	if(regaddr>=datalen) return -2;

	//datapos=MODBUS_RECV_DATA_POS+regaddr;
	datapos=MODBUS_RECV_DATA_POS+regaddr*2;
	if(datapos>(size-4)) return -3;
	
    switch(datatype)
    {
        //case INT_AB:
		case INT_BA:
			f.ch[0]=com_rbuf[0+datapos];
			f.ch[1]=com_rbuf[1+datapos];
			*valf=f.s;
		break;
		//case INT_BA:
		case INT_AB:
			f.ch[0]=com_rbuf[1+datapos];
			f.ch[1]=com_rbuf[0+datapos];
			*valf=f.s;
		break;
        //case LONG_ABCD:
		case LONG_DCBA:
			f.ch[0]=com_rbuf[0+datapos];
			f.ch[1]=com_rbuf[1+datapos];
			f.ch[2]=com_rbuf[2+datapos];
			f.ch[3]=com_rbuf[3+datapos];
			*valf=f.l;
		break;
		//case LONG_CDAB:
		case LONG_BADC:
			f.ch[0]=com_rbuf[2+datapos];
			f.ch[1]=com_rbuf[3+datapos];
			f.ch[2]=com_rbuf[0+datapos];
			f.ch[3]=com_rbuf[1+datapos];	
			*valf=f.l;
		break;
        //case LONG_BADC:
		case LONG_CDAB:
			f.ch[0]=com_rbuf[1+datapos];
			f.ch[1]=com_rbuf[0+datapos];
			f.ch[2]=com_rbuf[3+datapos];
			f.ch[3]=com_rbuf[2+datapos];
			*valf=f.l;
		break;
		//case LONG_DCBA:
		case LONG_ABCD:
			f.ch[0]=com_rbuf[3+datapos];
			f.ch[1]=com_rbuf[2+datapos];
			f.ch[2]=com_rbuf[1+datapos];
			f.ch[3]=com_rbuf[0+datapos];	
			*valf=f.l;
		break;		
		//case FLOAT_ABCD:
		case FLOAT_DCBA:
			f.ch[0]=com_rbuf[0+datapos];
			f.ch[1]=com_rbuf[1+datapos];
			f.ch[2]=com_rbuf[2+datapos];
			f.ch[3]=com_rbuf[3+datapos];
			*valf=f.f;
		break;			
		//case FLOAT_CDAB:
		case FLOAT_BADC:
			f.ch[0]=com_rbuf[2+datapos];
			f.ch[1]=com_rbuf[3+datapos];
			f.ch[2]=com_rbuf[0+datapos];
			f.ch[3]=com_rbuf[1+datapos];	
			*valf=f.f;
		break;
        //case FLOAT_BADC:
        case FLOAT_CDAB:
			f.ch[0]=com_rbuf[1+datapos];
			f.ch[1]=com_rbuf[0+datapos];
			f.ch[2]=com_rbuf[3+datapos];
			f.ch[3]=com_rbuf[2+datapos];
			*valf=f.f;
		break;
		//case FLOAT_DCBA:
		case FLOAT_ABCD:
			f.ch[0]=com_rbuf[3+datapos];
			f.ch[1]=com_rbuf[2+datapos];
			f.ch[2]=com_rbuf[1+datapos];
			f.ch[3]=com_rbuf[0+datapos];
			//printf("datapos %d,%x,%x,%x,%x:%f\n",datapos,
			//	f.ch[0]&0xff,f.ch[1]&0xff,f.ch[2]&0xff,f.ch[3]&0xff,
			//	f.f);

			*valf=f.f;
		break;		

		default:
			return -10;
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

static float change_to_ai_value(float valf,struct modbus_polcode_arg *mpolcodearg)
{
	float modbusmax=0,modbusmin=0;
	float valuemax=0, valumin=0;

    if(!mpolcodearg) return 0;

    modbusmax=mpolcodearg->valueMax;
	modbusmin=mpolcodearg->valueMin;
	valuemax=mpolcodearg->alarmMax;
	valumin=mpolcodearg->alarmMin;

    if(modbusmax==modbusmin) return valf;
	if(modbusmax<modbusmin) return 0;
    if(valuemax==valumin) return 0;
	if(valuemax<valumin) return 0;
	if((modbusmax<0.0001 && modbusmax>-0.0001) && (modbusmin<0.0001 && modbusmin>-0.0001) ) return valf;

	valf =((valf-modbusmin) / (modbusmax-modbusmin) ) *(valuemax-valumin)+valumin;

	return valf;
}

/*
return : com_f arguments amount;
*/
int protocol_modbus_water_ai(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float speed=0,total_flux_M3=0,cod=0,andan=0,zlin=0,zdan=0;

#define MODBUS_POLCODE_NUM 19
   char *polcodestr[MODBUS_POLCODE_NUM]={"w00000","w00001","w01018","w21003","w21011",
                                           "w21001","w20117","w20121","w20116","w20119",
                                           "w20115","w20125","w20124","w20120","w20123",
                                           "w20122","w21016","w01001","w01010"};
   UNIT_DECLARATION unitstr[MODBUS_POLCODE_NUM]={UNIT_L_S,UNIT_M3,UNIT_MG_L,UNIT_MG_L,UNIT_MG_L,
   	                                        UNIT_MG_L,UNIT_MG_L,UNIT_MG_L,UNIT_MG_L,UNIT_MG_L,
   	                                        UNIT_MG_L,UNIT_MG_L,UNIT_MG_L,UNIT_MG_L,UNIT_MG_L,
   	                                        UNIT_MG_L,UNIT_MG_L,UNIT_PH,UNIT_0C};


   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   
   int i=0;
   int ret=0;

   if(!acq_data) return -1;

   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;

   SYSLOG_DBG("modbus protocol modbusarg : %x, %x,%x,%x,%d\n",modbusarg_running->devaddr&0xffff,modbusarg_running->devfun&0xffff,
   	modbusarg_running->devstaddr&0xffff,modbusarg_running->regcnt&0xffff,modbusarg_running->array_count);
   for(i=0;i<modbusarg_running->array_count && i<MODBUS_POLCODE_MAX;i++)
   {
      SYSLOG_DBG("modbus protocol modbusarg  polcode  %d, %s,%x,%d\n",i,
	  	    mpolcodearg[i].polcode,mpolcodearg[i].regaddr&0xffff,mpolcodearg[i].datatype);
   }

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, modbusarg_running->devaddr&0xffff, modbusarg_running->devfun&0xffff,
		               modbusarg_running->devstaddr&0xffff, modbusarg_running->regcnt&0xffff);
	SYSLOG_DBG_HEX("modbus protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"modbus SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("modbus protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("modbus protocol recv data",com_rbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"modbus RECV:",com_rbuf,size);
	
	//if((size>=(modbusarg_running->regcnt&0xffff+5))&&
	if(size>0 && modbusarg_running->regcnt>0 &&
		(size>=((modbusarg_running->regcnt*2)&0xffff+5))&&
	   (com_rbuf[0]==(modbusarg_running->devaddr&0xffff))&&
	   (com_rbuf[1]==(modbusarg_running->devfun&0xffff)))
	{
	    for(i=0;i<MODBUS_POLCODE_NUM;i++)
	    {
		   mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
		   if(mpolcodearg_temp)
		   {
		       float valf=0.0;
	           ret=modbus_get_valf(com_rbuf,size,mpolcodearg_temp->regaddr,mpolcodearg_temp->datatype,&valf);
			   SYSLOG_DBG("modbus protocol modbus_water_valf %s ,%f,%d,%d,%f,%f,%f,%f,ret=%d\n",polcodestr[i],valf,
			   	       mpolcodearg_temp->regaddr&0xffff,mpolcodearg_temp->datatype,
			   	       mpolcodearg_temp->alarmMax,mpolcodearg_temp->alarmMin,
			   	       mpolcodearg_temp->valueMax,mpolcodearg_temp->valueMin,ret);
			   if(ret<0) 
			   {
			   	  valf=0.0;
			   }

               valf=change_to_ai_value(valf,mpolcodearg_temp);
			   
			   acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);

			   if(!strcmp(polcodestr[i],"w00001"))
			   {//added by miles zhang 20191211
					total_flux_M3=valf;
					acq_data->total=total_flux_M3;
			   }
		   }
	    }
	}
	else
	{
		status=1;
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
		   acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
		}
	}



	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

    return arg_n;
}



