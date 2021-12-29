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




static int hjt212_XJ_packet_is_ok(char *pack,int size)
{
    char *pp1,*pp2;
	unsigned long paddr0,paddr1;
	int packlen=0;

    if(!pack || size==0) return 0;//not ok
	
    if( size>12 &&
		(pp1=strstr(pack,"$")) &&
		(pp2=strstr(pp1,"T")))
		{
		   packlen=pp1[1];
		   paddr0=(unsigned long)pack;
		   paddr1=(unsigned long)pp1;
		   size -= (paddr1-paddr0);
		   if(size>packlen) return 1; // packet is ok
    	}

	return 0; //packet is not ok
}


static int get_hjt212_XJ_packet(struct acquisition_data *acq_data,char *com_rbuf,int rbuff_len,int *size)
{
    int i=0;
	int ret=0;
	int len=0;
	
#define LOOP_N 3 // 10

    if(!acq_data || !com_rbuf || !size) return -1;
	
	for(i=0;i<LOOP_N;i++)//need 20 seconds running
	{// ERROR , NOTE : for loop 20 seconds will make error , in rtu_statistics.c
	 //error log : "save_statistics_records , incompleted"
	 
	    sleep(2);
		ret=read_device(DEV_NAME(acq_data),&com_rbuf[len],rbuff_len-len);
		SYSLOG_DBG("%d>get_hjt212_packet,ret=%d,rbuff_len=%d,len=%d\n",i,ret,rbuff_len,len);
        if(ret<0) 
		{
		    len=0;
			break;
        }

		len+=ret;
		
		if(hjt212_XJ_packet_is_ok(com_rbuf,len)) break;
	}

	*size=len;

	return (i<LOOP_N) ? 0 : -2;
}

static int getHjt212XJPolcodeValf(char *valstr,int valstrlen,char *cemspol,float *valf)
{
   int ret=0;
   //char  polRtdstr[100]={0};
   char *polstr;
   union uf  f;
   
   //char cems_polcode[20]={0};

   if(!valstr || !cemspol ||!valf) return -1;

   f.f=0;
   polstr=memstr(valstr,valstrlen,cemspol);
   if(polstr)
   {
       if(strlen(cemspol)==2)
	   {// 2 char for polcode
	      f.ch[3]=polstr[2+0];
	      f.ch[2]=polstr[2+1];
	      f.ch[1]=polstr[2+2];
	      f.ch[0]=polstr[2+3];
	   }
	   else
	   {
	      //  3 char for polcode
	      f.ch[3]=polstr[3+0];
	      f.ch[2]=polstr[3+1];
	      f.ch[1]=polstr[3+2];
	      f.ch[0]=polstr[3+3];
	   }
   }

   *valf=f.f;
   
   return 0;
}





int protocol_CEMS_GBhjt212_XinJiang(struct acquisition_data *acq_data,int cemspol_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,char cemspol[][20])
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
    ret=get_hjt212_XJ_packet(acq_data,com_rbuf,sizeof(com_rbuf),&size);
	SYSLOG_DBG("protocol_CEMS_GBhjt212_XinJiang,ret=%d,size=%d\n",ret,size);
	SYSLOG_DBG_HEX("get_hjt212_packet_XinJiang1 ",com_rbuf,size);
	if(size>0)
	SYSLOG_DBG_HEX("get_hjt212_packet_XinJiang2 ",&com_rbuf[50],300);
	
	if(ret==0 && size >= 12 )
	{
	     for(i=0;i<cemspol_num;i++)
	     {
		       ret=getHjt212XJPolcodeValf(com_rbuf,size,cemspol[i],&f.f);
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

		//if(size == 0)// stop product
		//	status = 0;
		//else
			status = 1;
		//status = 0;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;	
}

