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


int protocol_modbus_ai_4017rc(struct acquisition_data *acq_data)
{
/*
 send : #01      send hex: 23 30 31 0d
 read : >+08.074+05.794+07.228+07.748+05.763+06.812+11.576+14.668
*/
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	char *pcom_rbuf,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float aivalf[9]={0};
	float aival[9]={0};
	union uf f;
	int ret=0;
	float hum=0,smoke=0,smokez=0,o2=0,temp=0,pres=0;

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int devaddr=0;

   if(!acq_data) return -1;

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;
   devaddr=modbusarg_running->devaddr;
	
   ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;
	

	memset(com_tbuf,0,sizeof(com_tbuf));
	//size=modbus_pack(com_tbuf,devaddr,modbusarg_running->devfun&0xff,modbusarg_running->devstaddr&0xff,modbusarg_running->regcnt&0xff);
	size=modbus_pack(com_tbuf,devaddr,modbusarg_running->devfun&0xff,0/*modbusarg_running->devstaddr&0xff*/,8/*modbusarg_running->regcnt&0xff*/); //mod by miles zhang 20210723
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"modbus 4017rc CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("modbus 4017rc  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("modbus 4017rc CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"modbus 4017rc CEMS RECV:",com_rbuf,size);

 	if(size>=20)
	{
        char substr[5]={0};
		int val=0;
		substr[0]=devaddr;
		substr[1]=modbusarg_running->devfun&0xff;
		substr[2]=(modbusarg_running->regcnt&0xff)*2;
	    pcom_rbuf=memstr(com_rbuf,size,substr);
		if(!pcom_rbuf) goto ERROR_PACK;

        for(i=0;i<size && i<modbusarg_running->regcnt && i<8;i++)
        {
			val=pcom_rbuf[3+i*2];
			val<<=8;
			val+=pcom_rbuf[4+i*2];
			aival[i]=val;
			//val[i]=aival[i]/65535*16.0+4;

			if(aival[i]>39321)
			 aival[i]=(aival[i]-39321)*16/26214.0+4;
			else 
			 aival[i]=4;
 
            aivalf[i]=(aival[i]-4)/(20-4);
			SYSLOG_DBG("modbus 4017rc %d,%d,%f,%f\n",i,val,aival[i],aivalf[i]);

        }
 	}
	else
	{

ERROR_PACK:
		status=1;
	}

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
	    int pos=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		if(!strcmp(mpolcodearg->polcode,"a34013z")) continue;
		if(!strcmp(mpolcodearg->polcode,"a34013")) continue;
		
		if(mpolcodearg->enableFlag==1)
		{
		    pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
			    aivalf[pos] *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			    aivalf[pos] +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d pos=%d, modbus 4017rc %s valf:%f,max %f,min %f\n",i,pos,
					mpolcodearg->polcode,aivalf[pos],mpolcodearg->alarmMax,mpolcodearg->alarmMin);
				if(!strcmp(mpolcodearg->polcode,"a01014"))
				{
					hum=aivalf[pos];
				}
			    else if(!strcmp(mpolcodearg->polcode,"a34013a"))
				{
					smoke=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a19001a"))
				{
					o2=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a01012"))
				{
					temp=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a01013"))
				{
					pres=aivalf[pos];
				}
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,aivalf[pos],&arg_n);


				if(!strcmp(mpolcodearg->polcode,"a19001a"))
				{
					acqdata_set_value(acq_data,"a19001",mpolcodearg->unit,aivalf[pos],&arg_n);
				}
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	    
	}
	
	if(pres>-101325 && temp>-273 && hum<100 && o2<19 && o2>0)
	{
		smokez=smoke*(101325/(101325+pres))*((273+temp)/273)*(100/(100-hum));
		smokez=smokez*((21-6)/(21-o2));
	}
	else
	{
		smokez=smoke;
	}

    acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,smokez,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,smokez,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}



int protocol_modbus_ai_4017rc_XinTianCompany(struct acquisition_data *acq_data)
{
/*新天铁热扎厂*/
//added by miles zhang 20210721
/*

TX:size=8,01 03 00 00 00 08 44 0c
RX:size=2047,77 a5 ff ff a6 65 48 9f ff ff 37 fd 01 03 00 00 00 08 44 0c 01 03 10 0c c8 23 4b 88 e8 78 61 ff ff a6 65 48 9f ff ff 65 08 01 03 00 00 00 08 44 0c 01 03 10 0c c8 23 4b 88 e8 78 61 ff ff a6 f1 4

*/
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	char *pcom_rbuf,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float aivalf[9]={0};
	float aival[9]={0};
	union uf f;
	int ret=0;
	float hum=0,smoke=0,smokez=0,o2=0,temp=0,pres=0;

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int devaddr=0;

   if(!acq_data) return -1;

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;
   devaddr=modbusarg_running->devaddr;
	
   ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;
	

	memset(com_tbuf,0,sizeof(com_tbuf));
	//size=modbus_pack(com_tbuf,devaddr,modbusarg_running->devfun&0xff,modbusarg_running->devstaddr&0xff,modbusarg_running->regcnt&0xff);
	size=modbus_pack(com_tbuf,devaddr,modbusarg_running->devfun&0xff,0/*modbusarg_running->devstaddr&0xff*/,8/*modbusarg_running->regcnt&0xff*/);//mod by miles zhang 20210723
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"modbus 4017rc CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("modbus 4017rc  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("modbus 4017rc CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"modbus 4017rc CEMS RECV:",com_rbuf,size);

 	if(size>=20)
	{
        char substr[5]={0};
		int val=0;
		substr[0]=devaddr;
		substr[1]=modbusarg_running->devfun&0xff;
		substr[2]=(modbusarg_running->regcnt&0xff)*2;
	    pcom_rbuf=memstr(com_rbuf,size,substr);
		if(!pcom_rbuf) goto ERROR_PACK;

        for(i=0;i<size && i<modbusarg_running->regcnt && i<8;i++)
        {
			val=pcom_rbuf[3+i*2];
			val<<=8;
			val+=pcom_rbuf[4+i*2];
			aival[i]=val;
			aivalf[i]=aival[i]/65535*16.0+4;
/*//del by miles zhang 20210721
			if(aival[i]>39321)
			 aival[i]=(aival[i]-39321)*16/26214.0+4;
			else 
			 aival[i]=4;
 */
            aivalf[i]=(aivalf[i]-4)/(20-4);
			SYSLOG_DBG("modbus 4017rc %d,%d,%f,%f\n",i,val,aival[i],aivalf[i]);

        }
 	}
	else
	{

ERROR_PACK:
		status=1;
	}

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
	    int pos=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		if(!strcmp(mpolcodearg->polcode,"a34013z")) continue;
		if(!strcmp(mpolcodearg->polcode,"a34013")) continue;
		
		if(mpolcodearg->enableFlag==1)
		{
		    pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
			    aivalf[pos] *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			    aivalf[pos] +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d pos=%d, modbus 4017rc %s valf:%f,max %f,min %f\n",i,pos,
					mpolcodearg->polcode,aivalf[pos],mpolcodearg->alarmMax,mpolcodearg->alarmMin);
				if(!strcmp(mpolcodearg->polcode,"a01014"))
				{
					hum=aivalf[pos];
				}
			else if(!strcmp(mpolcodearg->polcode,"a34013a"))
				{
					smoke=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a19001"))
				{
					o2=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a01012"))
				{
					temp=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a01013"))
				{
					pres=aivalf[pos];
				}
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,aivalf[pos],&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	    
	}
	if(pres>-101325 && temp>-273 && hum<100 && o2<19 && o2>0)
	{
		smokez=smoke*(101325/(101325+pres))*((273+temp)/273)*(100/(100-hum));
		smokez=smokez*((21-6)/(21-o2));
	}
	else
	{
		smokez=smoke;
	}

    acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,smokez,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,smokez,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}



