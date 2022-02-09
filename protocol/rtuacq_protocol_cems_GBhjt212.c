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




static int hjt212_packet_is_ok(char *pack,int size)
{
    char *pp;
	if(!pack || size==0) return 0;//not ok
    if( size>40 && (pp=strstr(pack,"CP=&&")) && strstr(&pp[4],"&&")) return 1; // packet is ok

	return 0; //packet is not ok
}


static int get_hjt212_packet(struct acquisition_data *acq_data,char *com_rbuf,int rbuff_len,int *size)
{
	int i=0;
	int ret=0;
	int len=0;
	char *p=NULL;

	#define LOOP_N 7

	if(!acq_data || !com_rbuf || !size) return -1;

	for(i=0;i<LOOP_N;i++)
	{
		sleep(3);
		ret=read_device(DEV_NAME(acq_data),&com_rbuf[len],rbuff_len-len);
		SYSLOG_DBG("%d>get_hjt212_packet,ret=%d,rbuff_len=%d,len=%d\n",i,ret,rbuff_len,len);
		if(ret<=0 || (p=strstr(com_rbuf,"CN=2011"))==NULL) 
		{//isnot rtdata
			len=0;
			continue;
		}


		len+=ret;

		if(hjt212_packet_is_ok(p,len)) 
			break;
		else
		{
		    sleep(3);
			ret=read_device(DEV_NAME(acq_data),&com_rbuf[len],rbuff_len-len);
			len+=ret;
			break;
		}
	}

	*size=len;

	return (i<LOOP_N) ? 0 : -2;
}

static int getHjt212PolcodeValf(char *valstr,char *cemspol,float *valf)
{
   int ret=0;
   char  polRtdstr[100]={0};
   char *polstr;
   char cems_polcode[20]={0};

   if(!valstr || !cemspol ||!valf) return -1;
   
   strcpy(cems_polcode,cemspol);
   memset(polRtdstr,0,sizeof(polRtdstr));
   if((strcmp(cems_polcode,"a00000z"))&&strstr(cems_polcode,"z"))
   {
       cems_polcode[strlen(cems_polcode)-1]='\0'; //del 'z'
   	   sprintf(polRtdstr,"%s-ZsRtd",cems_polcode);
   }
   else
   	   sprintf(polRtdstr,";%s-Rtd",cems_polcode);
   //sprintf(polRtdstr,"%s-Rtd",cems_polcode); //del by miles zhang for hjt2005 01 S01 02 S02 errors 
   
   polstr=strstr(valstr,polRtdstr);
   
   if(polstr && valf)
   {
	 strcat(polRtdstr,"=%f,");
     ret=sscanf(polstr,polRtdstr,valf);
	 if(ret!=1) return -2;

	 return 0;
   }

   return -1;
}

int protocol_CEMS_GBhjt212(struct acquisition_data *acq_data,int cemspol_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,char cemspol[][20])
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   int i=0,j=0;
   int ret=0;

   if(!acq_data) return -1;

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt212:size=%d,%s,%s\n",cemspol_num,polcodestr[0],cemspol[0]);

   	memset(com_rbuf,0,sizeof(com_rbuf));
    ret=get_hjt212_packet(acq_data,com_rbuf,sizeof(com_rbuf),&size);
	SYSLOG_DBG("protocol_CEMS_GBhjt212,ret=%d,size=%d\n",ret,size);
	SYSLOG_DBG("protocol_CEMS_GBhjt212:%s\n",com_rbuf);
	if(ret==0 && size >= 40 )
	{
	     //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"CEMS1:%s\n",&com_rbuf[0]);
		 //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"CEMS2:%s\n",&com_rbuf[200]);
	     for(i=0;i<cemspol_num;i++)
	     {
		       ret=getHjt212PolcodeValf(com_rbuf,cemspol[i],&f.f);
			   if(ret!=0)
			   {
			    	f.f=0;
			   }
			   if(strlen(polcodestr[i])>0)
			        acqdata_set_value(acq_data,polcodestr[i],unitstr[i],f.f,&arg_n);
	     }	
	}
    else
    {
        int i=0;
		read_system_time(acq_data->acq_tm);
		for(i=0;i<cemspol_num;i++)
		{
		   if(strlen(polcodestr[i])>0)
		       acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0.0,&arg_n);
		}		

		status = 1;
		//status = 0;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;	
}

static int get_hjt212_packet_once(struct acquisition_data *acq_data,char *com_rbuf,int rbuff_len,int *size)
{
	int i=0;
	int ret=0;
	int len=0;
	char *p=NULL;

	#define LOOP_ONCE 1

	if(!acq_data || !com_rbuf || !size) return -1;

	for(i=0;i<LOOP_ONCE;i++)
	{
		//sleep(3);
		sleep(1);
		ret=read_device(DEV_NAME(acq_data),&com_rbuf[len],rbuff_len-len);
		SYSLOG_DBG("%d>get_hjt212_packet,ret=%d,rbuff_len=%d,len=%d\n",i,ret,rbuff_len,len);
		if(ret<=0 || (p=strstr(com_rbuf,"CN=2011"))==NULL) 
		{//isnot rtdata
			if((p=strstr(com_rbuf,"CN=8013"))==NULL)
			{
				len=0;
				continue;
			}
		}

		len=ret;

		if(hjt212_packet_is_ok(p,len)) 
			break;
		else
		{
		    sleep(3);
			ret=read_device(DEV_NAME(acq_data),&com_rbuf[len],rbuff_len-len);
			len+=ret;
			break;
		}
	}

	*size=len;

	return (i<LOOP_ONCE) ? 0 : -2;
}


int protocol_CEMS_GBhjt212_once(struct acquisition_data *acq_data,int cemspol_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,char cemspol[][20])
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   int i=0,j=0;
   int ret=0;

   if(!acq_data) return -1;

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt212:size=%d,%s,%s\n",cemspol_num,polcodestr[0],cemspol[0]);

   	memset(com_rbuf,0,sizeof(com_rbuf));
    ret=get_hjt212_packet_once(acq_data,com_rbuf,sizeof(com_rbuf),&size);
	SYSLOG_DBG("protocol_CEMS_GBhjt212 once,ret=%d,size=%d\n",ret,size);
	SYSLOG_DBG("protocol_CEMS_GBhjt212 once:%s\n",com_rbuf);
	SYSLOG_DBG("protocol_CEMS_GBhjt212 once2:%s\n",&com_rbuf[194]);
	SYSLOG_DBG("protocol_CEMS_GBhjt212 once3:%s\n",&com_rbuf[388]);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"GBhjt212 RECV:",com_rbuf,size);
	if(ret==0 && size >= 40 )
	{
	     for(i=0;i<cemspol_num;i++)
	     {
		       ret=getHjt212PolcodeValf(com_rbuf,cemspol[i],&f.f);
			   if(ret!=0)
			   {
			    	f.f=0;
			   }
			   if(strlen(polcodestr[i])>0)
			   {
			   
/*	//del by miles zhang 20210525 for  error when a01013(pa)=-5, will be -5(Kpa)= -5000Pa				   
			   	 if(!strcmp(polcodestr[i],"a00000"))
			   	 {
			   	 	unitstr[i]=UNIT_M3_S;
					if(f.f>200 || f.f<-200)
					{
						f.f/=3600;
					}
				 }
				 else if(!strcmp(polcodestr[i],"a01013"))
				 {
				 	unitstr[i]=UNIT_KPA;
					if(f.f>10 || f.f<-10)
					{
						f.f/=1000;
					}
			    }
*/					 
			        acqdata_set_value(acq_data,polcodestr[i],unitstr[i],f.f,&arg_n);
			   }
	     }	
	}
    else
    {
        int i=0;
		read_system_time(acq_data->acq_tm);
		for(i=0;i<cemspol_num;i++)
		{
		   if(strlen(polcodestr[i])>0)
		       acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0.0,&arg_n);
		}		

		status = 1;
		//status = 0;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;	
}

int protocol_CEMS_GBhjt212_once_flag(struct acquisition_data *acq_data,int cemspol_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,char cemspol[][20])
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   int i=0,j=0;
   int ret=0;

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;   
   int devaddr=0;

   if(!acq_data) return -1;

	   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

	   acq_ctrl=ACQ_CTRL(acq_data);
	   modbusarg_running=&acq_ctrl->modbusarg_running;
	   mpolcodearg=modbusarg_running->polcode_arg;
	   devaddr=modbusarg_running->devaddr&0xffff;

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt212:size=%d,%s,%s\n",cemspol_num,polcodestr[0],cemspol[0]);

   	memset(com_rbuf,0,sizeof(com_rbuf));
    ret=get_hjt212_packet_once(acq_data,com_rbuf,sizeof(com_rbuf),&size);
	SYSLOG_DBG("protocol_CEMS_GBhjt212 once flag,ret=%d,size=%d\n",ret,size);
	SYSLOG_DBG("protocol_CEMS_GBhjt212 once flag:%s\n",com_rbuf);
	SYSLOG_DBG("protocol_CEMS_GBhjt212 once2 flag:%s\n",&com_rbuf[194]);
	SYSLOG_DBG("protocol_CEMS_GBhjt212 once3 flag:%s\n",&com_rbuf[388]);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"GBhjt212 RECV flag:",com_rbuf,size);
	if(ret==0 && size >= 40 )
	{
	     for(i=0;i<cemspol_num;i++)
	     {
			   if(isPolcodeEnable(modbusarg_running,polcodestr[i]))
			   {
			           ret=getHjt212PolcodeValf(com_rbuf,cemspol[i],&f.f);
				   if(ret!=0)
				   {
				    	f.f=0;
					if(strlen(polcodestr[i])>0)
					      acqdata_set_value_flag(acq_data,polcodestr[i],unitstr[i],f.f,'D',&arg_n);
				   }
				   else
				   {
					   if(strlen(polcodestr[i])>0)
					   {
					   					 
					        acqdata_set_value_flag(acq_data,polcodestr[i],unitstr[i],f.f,'N',&arg_n);
					   }
				   }
			   }
	     }	
		 
	   acq_data->dev_stat=0xAA;
	   acq_data->acq_status = ACQ_OK;		 
	}
    else
    {
                int i=0;
		read_system_time(acq_data->acq_tm);
		for(i=0;i<cemspol_num;i++)
		{
			   if(isPolcodeEnable(modbusarg_running,polcodestr[i]))
			   {		
				   if(strlen(polcodestr[i])>0)
				       acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0.0,&arg_n);
			   }
		}		

		status = 1;
		if(status == 0)
	   		acq_data->acq_status = ACQ_OK;
		else 
	 		acq_data->acq_status = ACQ_ERR;		
    }

    return arg_n;	
}


