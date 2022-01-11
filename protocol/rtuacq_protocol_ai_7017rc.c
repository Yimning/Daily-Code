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

static int minus_to_plus(char *buff)
{
	int size;
	int i;
	
	if(!buff)
		return -1;

	size=strlen(buff);

	for(i=0;i<size;i++)
	{
		if(buff[i]=='-')
			buff[i]='+';
	}

	return 0;
}

int protocol_ai_7017rc(struct acquisition_data *acq_data)
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
	char *p,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float aivalf[9]={0};
	union uf f;
	int ret=0;
	float hum=0,smoke=0,smokez=0,o2=0,temp=0,pres=0;

	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;

	ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	//com_tbuf[1]=0x30;
	//com_tbuf[2]=0x31;
	sprintf(&com_tbuf[1],"%02X",modbusarg_running->devaddr);
	com_tbuf[3]=0x0d;
	size=4;
	SYSLOG_DBG_HEX("ai 7017rc CEMS ",com_tbuf,4);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"ai 7017rc CEMS SEND:",com_rbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ai 7017rc  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("ai 7017rc CEMS %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"ai 7017rc CEMS RECV:",com_rbuf,size);
 	if(size>=10)
	{
		minus_to_plus(com_rbuf);
	
		pq=strstr(com_rbuf,">+");
		if(!pq) goto ERROR_PACK;
		else
		{
		   p=pq;
		}
		
		pq=strstr(pq++,">+");
		if(pq) p=pq;
		
		ret=sscanf(p,">+%f+%f+%f+%f+%f+%f+%f+%f",&ai1,&ai2,&ai3,&ai4,&ai5,&ai6,&ai7,&ai8);	
		if(ret!=8) goto ERROR_PACK;
		
		if(ai1>4)aivalf[0]=(ai1-4)/16;
		if(ai2>4)aivalf[1]=(ai2-4)/16; 
		if(ai3>4)aivalf[2]=(ai3-4)/16;
		if(ai4>4)aivalf[3]=(ai4-4)/16; 
		if(ai5>4)aivalf[4]=(ai5-4)/16; 
		if(ai6>4)aivalf[5]=(ai6-4)/16; 
		if(ai7>4)aivalf[6]=(ai7-4)/16; 
		if(ai8>4)aivalf[7]=(ai8-4)/16;


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
		
		if(mpolcodearg->enableFlag==1)
		{
		
		    pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
			    aivalf[pos] *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			    aivalf[pos] +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d pos=%d, 7017rc %s valf:%f,max %f,min %f\n",i,pos,
					mpolcodearg->polcode,aivalf[pos],mpolcodearg->alarmMax,mpolcodearg->alarmMin);
				if(!strcmp(mpolcodearg->polcode,"a01014"))
				{
					hum=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a34013"))
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

	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,smokez,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}

int protocol_ai_7017rc_vol(struct acquisition_data *acq_data)
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
	char *p,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float aivalf[9]={0};
	union uf f;
	int ret=0;
	float hum=0,smoke=0,smokez=0,o2=0,temp=0,pres=0,a01017=0;
	float speed = 0,PTC = 0;
	
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;
	
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;

	rdtuinfo=get_parent_rdtu_info(acq_data);

	PTC=rdtuinfo->PTC;

	ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	//com_tbuf[1]=0x30;
	//com_tbuf[2]=0x31;
	sprintf(&com_tbuf[1],"%02X",modbusarg_running->devaddr);
	com_tbuf[3]=0x0d;
	size=4;
	SYSLOG_DBG_HEX("ai vol 7017rc CEMS ",com_tbuf,4);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"ai vol 7017rc CEMS SEND:",com_rbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ai vol 7017rc  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("ai vol 7017rc CEMS %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"ai vol 7017rc CEMS RECV:",com_rbuf,size);
 	if(size>=10)
	{
		minus_to_plus(com_rbuf);
	
		pq=strstr(com_rbuf,">+");
		if(!pq) goto ERROR_PACK;
		else
		{
		   p=pq;
		}
		
		pq=strstr(pq++,">+");
		if(pq) p=pq;
		
		ret=sscanf(p,">+%f+%f+%f+%f+%f+%f+%f+%f",&ai1,&ai2,&ai3,&ai4,&ai5,&ai6,&ai7,&ai8);	
		if(ret!=8) goto ERROR_PACK;
		
		if(ai1>1)aivalf[0]=(ai1-1)/4;
		if(ai2>1)aivalf[1]=(ai2-1)/4; 
		if(ai3>1)aivalf[2]=(ai3-1)/4;
		if(ai4>1)aivalf[3]=(ai4-1)/4; 
		if(ai5>1)aivalf[4]=(ai5-1)/4; 
		if(ai6>1)aivalf[5]=(ai6-1)/4; 
		if(ai7>1)aivalf[6]=(ai7-1)/4; 
		if(ai8>1)aivalf[7]=(ai8-1)/4;


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
		//if(!strcmp(mpolcodearg->polcode,"a34013z")) continue;
 
		if((!strcmp(mpolcodearg->polcode,"a01011")) && isPolcodeEnable(modbusarg_running,"a01017"))
			continue;

		if(mpolcodearg->enableFlag==1)
		{
		
		    pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
			    aivalf[pos] *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			    aivalf[pos] +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d pos=%d, 7017rc %s valf:%f,max %f,min %f\n",i,pos,
					mpolcodearg->polcode,aivalf[pos],mpolcodearg->alarmMax,mpolcodearg->alarmMin);

				if(!strcmp(mpolcodearg->polcode,"a01012"))
				{
					temp=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a01013"))
				{
					pres=aivalf[pos];
					if(mpolcodearg->unit == UNIT_KPA)
						pres*=1000;
				}

				if(!strcmp(mpolcodearg->polcode,"a01017"))
				{
					//speed=PTC*sqrt(fabs(aivalf[pos])*2/SAD);
					a01017=aivalf[pos];
				}
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,aivalf[pos],&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	    
	}

	if((pres>0) && (a01017>=0) && ((273+temp)>=0))
		speed=1.414*PTC*sqrt(a01017*8312.0*(273.0+temp)/30.0/pres);
	else
		speed=0;

	SYSLOG_DBG("speed = %f,aivalf[pos]= %f,temp = %f,pressure = %f,PTC=%f\n",speed,a01017,temp,pres,PTC);
	if(isPolcodeEnable(modbusarg_running,"a01017"))
		acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}


int protocol_ai_7017rc_vol1(struct acquisition_data *acq_data)
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
	char *p,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float aivalf[9]={0};
	union uf f;
	int ret=0;
	float hum=0,smoke=0,smokez=0,o2=0,temp=0,pres=0,a01017=0;
	float speed = 0,PTC = 0,atmos = 0 ;
	
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;
	
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;

	rdtuinfo=get_parent_rdtu_info(acq_data);

	PTC=rdtuinfo->PTC;

	atmos = rdtuinfo->atm_press;

	ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	//com_tbuf[1]=0x30;
	//com_tbuf[2]=0x31;
	sprintf(&com_tbuf[1],"%02X",modbusarg_running->devaddr);
	com_tbuf[3]=0x0d;
	size=4;
	SYSLOG_DBG_HEX("ai vol 7017rc CEMS ",com_tbuf,4);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"ai vol 7017rc CEMS SEND:",com_rbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ai vol 7017rc  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("ai vol 7017rc CEMS %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"ai vol 7017rc CEMS RECV:",com_rbuf,size);
 	if(size>=10)
	{
		minus_to_plus(com_rbuf);
	
		pq=strstr(com_rbuf,">+");
		if(!pq) goto ERROR_PACK;
		else
		{
		   p=pq;
		}
		
		pq=strstr(pq++,">+");
		if(pq) p=pq;
		
		ret=sscanf(p,">+%f+%f+%f+%f+%f+%f+%f+%f",&ai1,&ai2,&ai3,&ai4,&ai5,&ai6,&ai7,&ai8);	
		if(ret!=8) goto ERROR_PACK;
		
		if(ai1>1)aivalf[0]=(ai1-1)/4;
		if(ai2>1)aivalf[1]=(ai2-1)/4; 
		if(ai3>1)aivalf[2]=(ai3-1)/4;
		if(ai4>1)aivalf[3]=(ai4-1)/4; 
		if(ai5>1)aivalf[4]=(ai5-1)/4; 
		if(ai6>1)aivalf[5]=(ai6-1)/4; 
		if(ai7>1)aivalf[6]=(ai7-1)/4; 
		if(ai8>1)aivalf[7]=(ai8-1)/4;


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
		//if(!strcmp(mpolcodearg->polcode,"a34013z")) continue;
 
		if((!strcmp(mpolcodearg->polcode,"a01011")) && isPolcodeEnable(modbusarg_running,"a01017"))
			continue;

		if(mpolcodearg->enableFlag==1)
		{
		
		    pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
			    aivalf[pos] *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			    aivalf[pos] +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d pos=%d, 7017rc %s valf:%f,max %f,min %f\n",i,pos,
					mpolcodearg->polcode,aivalf[pos],mpolcodearg->alarmMax,mpolcodearg->alarmMin);

				if(!strcmp(mpolcodearg->polcode,"a01012"))
				{
					temp=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a01013"))
				{
					pres=aivalf[pos];
					if(mpolcodearg->unit == UNIT_KPA)
						pres*=1000;
				}

				if(!strcmp(mpolcodearg->polcode,"a01017"))
				{
					//speed=PTC*sqrt(fabs(aivalf[pos])*2/SAD);
					a01017=aivalf[pos];
				}
				
				if(!strcmp(mpolcodearg->polcode,"a01013"))
				{
					aivalf[pos] -= atmos;
					SYSLOG_DBG("==aivalf[pos]= %f,atmos =%f",aivalf[pos],atmos);
				}
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,aivalf[pos],&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	    
	}

	if((pres>0) && (a01017>=0) && ((273+temp)>=0))
		speed=1.414*PTC*sqrt(a01017*8312.0*(273.0+temp)/30.0/pres);
	else
		speed=0;

	SYSLOG_DBG("speed = %f,aivalf[pos]= %f,temp = %f,pressure = %f,PTC=%f,atmos=%f\n",speed,a01017,temp,pres,PTC,atmos);
	if(isPolcodeEnable(modbusarg_running,"a01017"))
		acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}




#if 0
{9880,9600,0,8,1,7017RC@ 8\u8def\u6a21\u62df\u91cf\u8f6c485\u6a21\u57571},
{9880,a01011,0,1,UNIT_M_S,1000,0,1000,0,0},
{9880,a00000,0,1,UNIT_M3_H,10000000,0,10000000,0,2},
{9880,a00000z,0,1,UNIT_M3_H,10000000,0,10000000,0,2},
{9880,a21024z,0,1,UNIT_MG_M3,50,0,10000,0,2},
{9880,a21024,0,1,UNIT_MG_M3,50,0,1000,0,0},
{9880,a21005z,0,1,UNIT_MG_M3,200,0,10000,0,2},
{9880,a21005,0,1,UNIT_MG_M3,200,0,1000,0,0},
{9880,a21002z,0,1,UNIT_MG_M3,600,0,10000,0,2},
{9880,a21002,0,1,UNIT_MG_M3,600,0,1000,0,0},
{9880,a21026z,0,1,UNIT_MG_M3,200,0,10000,0,2},
{9880,a21026,0,1,UNIT_MG_M3,200,0,1000,0,0},
{9880,a34013z,0,1,UNIT_MG_M3,15,0,10000,0,2},
{9880,a34013,0,1,UNIT_MG_M3,15,0,1000,0,0},
{9880,a01012,0,1,UNIT_0C,1000,0,1000,0,0},
{9880,a01013,0,1,UNIT_PA,1000000,-1000,1000000,-1000,0},
{9880,a01014,0,1,UNIT_PECENT,40,0,100,0,0},
{9880,a19001,0,1,UNIT_PECENT,21,0,100,0,0},
{9880,w01001,0,1,UNIT_PH,14,0,14,0,0},
{9880,w01010,0,1,UNIT_0C,100,0,100,0,0},
{9880,w00000,0,1,UNIT_L_S,1000,0,1000,0,0},
{9880,w21011,0,1,UNIT_MG_L,1000,0,1000,0,0},
{9880,w21001,0,1,UNIT_MG_L,1000,0,1000,0,0},

int protocol_ai_7017rc_vol1(struct acquisition_data *acq_data);
	{9880,protocol_ai_7017rc_vol1},              


int protocol_ai_7017rc_vol1(struct acquisition_data *acq_data)
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
	char *p,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float aivalf[9]={0};
	union uf f;
	int ret=0;
	float hum=0,smoke=0,smokez=0,o2=0,temp=0,pres=0;
	float speed = 0,PTC = 0;
	
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;
	
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;

	ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	//com_tbuf[1]=0x30;
	//com_tbuf[2]=0x31;
	sprintf(&com_tbuf[1],"%02X",modbusarg_running->devaddr);
	com_tbuf[3]=0x0d;
	size=4;
	SYSLOG_DBG_HEX("ai vol 7017rc CEMS ",com_tbuf,4);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"ai vol 7017rc CEMS SEND:",com_rbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ai vol 7017rc  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("ai vol 7017rc CEMS %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"ai vol 7017rc CEMS RECV:",com_rbuf,size);
 	if(size>=10)
	{
		minus_to_plus(com_rbuf);
	
		pq=strstr(com_rbuf,">+");
		if(!pq) goto ERROR_PACK;
		else
		{
		   p=pq;
		}
		
		pq=strstr(pq++,">+");
		if(pq) p=pq;
		
		ret=sscanf(p,">+%f+%f+%f+%f+%f+%f+%f+%f",&ai1,&ai2,&ai3,&ai4,&ai5,&ai6,&ai7,&ai8);	
		if(ret!=8) goto ERROR_PACK;
		
		if(ai1>1)aivalf[0]=(ai1-1)/4;
		if(ai2>1)aivalf[1]=(ai2-1)/4; 
		if(ai3>1)aivalf[2]=(ai3-1)/4;
		if(ai4>1)aivalf[3]=(ai4-1)/4; 
		if(ai5>1)aivalf[4]=(ai5-1)/4; 
		if(ai6>1)aivalf[5]=(ai6-1)/4; 
		if(ai7>1)aivalf[6]=(ai7-1)/4; 
		if(ai8>1)aivalf[7]=(ai8-1)/4;


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
		//if(!strcmp(mpolcodearg->polcode,"a34013z")) continue;
 
		if((!strcmp(mpolcodearg->polcode,"a01011")) && isPolcodeEnable(modbusarg_running,"a01017"))
			continue;

		if(mpolcodearg->enableFlag==1)
		{
		
		    pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
			    aivalf[pos] *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			    aivalf[pos] +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d pos=%d, 7017rc %s valf:%f,max %f,min %f\n",i,pos,
					mpolcodearg->polcode,aivalf[pos],mpolcodearg->alarmMax,mpolcodearg->alarmMin);
				/*
				if(!strcmp(mpolcodearg->polcode,"a01014"))
				{
					hum=aivalf[pos];
				}
				else if(!strcmp(mpolcodearg->polcode,"a34013"))
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
				*/

				if(!strcmp(mpolcodearg->polcode,"a01017"))
				{
					//speed=PTC*sqrt(fabs(aivalf[pos])*2/SAD);
					speed=PTC*sqrt(fabs(aivalf[pos])*2/SAD);
					acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
				}
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,aivalf[pos],&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	    
	}
	/*
	if(pres>-101325 && temp>-273 && hum<100 && o2<19 && o2>0)
	{
		smokez=smoke*(101325/(101325+pres))*((273+temp)/273)*(100/(100-hum));
		smokez=smokez*((21-6)/(21-o2));
	}
	else
	{
		smokez=smoke;
	}
	*/

	//acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,smokez,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}
#endif


int protocol_CEMS_7017rc_RO(struct acquisition_data *acq_data)
{
/*
	read : >+08.074+05.794+07.228+07.748+05.763+06.812+11.576+14.668
*/
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	char *p,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float aivalf[9]={0};
	union uf f;
	int ret=0;
	char flag=0;
	int di[4]={0};

	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;

	ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;
#if 0
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	sprintf(&com_tbuf[1],"%02X",modbusarg_running->devaddr);
	//com_tbuf[1]=0x30+modbusarg_running->devaddr/10;
	//com_tbuf[2]=0x30+modbusarg_running->devaddr%10;	
	com_tbuf[3]=0x0d;
	size=4;
	SYSLOG_DBG_HEX("ai 7017rc CEMS ",com_tbuf,4);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"ai 7017rc CEMS SEND:",com_rbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
#endif
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("7017rc  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("7017rc CEMS %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"7017rc RO CEMS RECV:",com_rbuf,size);
	if(size>=10)
	{
		minus_to_plus(com_rbuf);

		pq=strstr(com_rbuf,">+");
		if(!pq)
			goto ERROR_PACK;
		else
			p=pq;

		pq=strstr(pq++,">+");
		if(pq)
			p=pq;

		ret=sscanf(p,">+%f+%f+%f+%f+%f+%f+%f+%f",&ai1,&ai2,&ai3,&ai4,&ai5,&ai6,&ai7,&ai8);	
		if(ret!=8)
			goto ERROR_PACK;

		if(ai1>4)
			aivalf[0]=(ai1-4)/16;
		if(ai2>4)
			aivalf[1]=(ai2-4)/16; 
		if(ai3>4)
			aivalf[2]=(ai3-4)/16;
		if(ai4>4)
			aivalf[3]=(ai4-4)/16; 
		if(ai5>4)
			aivalf[4]=(ai5-4)/16; 
		if(ai6>4)
			aivalf[5]=(ai6-4)/16; 
		if(ai7>4)
			aivalf[6]=(ai7-4)/16; 
		if(ai8>4)
			aivalf[7]=(ai8-4)/16;

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

		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
				aivalf[pos] *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
				aivalf[pos] +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d pos=%d, 7017rc %s valf:%f,max %f,min %f\n",i,pos,
					mpolcodearg->polcode,aivalf[pos],mpolcodearg->alarmMax,mpolcodearg->alarmMin);

				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,aivalf[pos],&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	}

	di[0]=1;
	di[1]=1;
	di[2]=1;
	di[3]=1;

	read_device(DEV_DI0,&di[0],sizeof(int));
	read_device(DEV_DI1,&di[1],sizeof(int));
	read_device(DEV_DI2,&di[2],sizeof(int));
	read_device(DEV_DI3,&di[3],sizeof(int));

	if(di[0]==0)
		flag='D';
	else if(di[1]==0)
		flag='z';
	else if(di[2]==0)
		flag='M';
	else if(di[3]==0)
		flag='C';
	else
		flag='N';

	if(status == 0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

	return arg_n;
}

