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

static int hjt2005_packet_is_ok(char *pack,int size)
{
    char *pp;
	
	if(!pack||size==0) return 0;//not ok
	
    if( size>40 &&
		(pp=strstr(pack,"##")) &&
		(pp=strstr(pp,"CP=&&")) &&
		strstr(&pp[4],"&&")) return 1; // packet is ok

	return 0; //packet is not ok
}

static int getHjt2005PolcodeValf(char *valstr,char *cemspol,float *valf)
{
/*
"STX,15,20100602122233,ZA9,11.23,ZB0,22.23,ZB1,33.23,ZB2,44.23,ZB3,55.23,ZB4,66.23,ZD1,77.23,ZD3,88.23,ZD4,99.23,ZD5,105.23,ZD6,100.23,ZD7,101.23,ZD8,102.23,ZD9,103.23,ZB6,145.30,ETX
"
*/
   int ret=0;
   char  polRtdstr[100]={0};
   char *polstr;
   char cems_polcode[20]={0};

   if(!valstr || !cemspol ||!valf) return -1;
   
   strcpy(cems_polcode,cemspol);
   memset(polRtdstr,0,sizeof(polRtdstr));
   if(strstr(cems_polcode,"z"))
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


int protocol_CEMS_GBhjt2005_2(struct acquisition_data *acq_data)
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

//#define CEMSPOL_NUM (ARRAY_SIZE(polcodestr))

#if 1

   char *polcodestr[]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014"};

#define CEMSPOL_NUM (ARRAY_SIZE(polcodestr))

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_H,UNIT_PECENT};
   
	   char *cemspol[CEMSPOL_NUM]={"02","02z","03","03z",
   "01","01z",
   "S02","S01","S03","S08","B02","S05"};

       //float cemvalf[CEMSPOL_NUM]={0};
	   
	   //float cemvalf_num=0;
	   
   if(!acq_data) return -1;   


   	memset(com_rbuf,0,sizeof(com_rbuf));
	for(i=0;i<10;i++)
	{
	    sleep(2);
		ret=read_device(DEV_NAME(acq_data),&com_rbuf[size],sizeof(com_rbuf)-size);
        if(ret<0) 
		{
		    size=0;
			break;
        }

		size+=ret;
		
		if(hjt2005_packet_is_ok(com_rbuf,size)) break;
	}   

	

	if(i<10 && size >= 40 )
	{
	     for(i=0;i<CEMSPOL_NUM;i++)
	     {
		       ret=getHjt2005PolcodeValf(com_rbuf,cemspol[i],&f.f);
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
		for(i=0;i<CEMSPOL_NUM;i++)
		{
		   if(strlen(polcodestr[i])>0)
		       acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0.0,&arg_n);
		}		

		if(size == 0)// stop product
			status = 0;
		else
			status = 1;
		//status = 0;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;	
#endif

}


extern int protocol_CEMS_GBhjt212(struct acquisition_data *acq_data,int cemspol_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,char cemspol[][20]);
int protocol_CEMS_GBhjt2005(struct acquisition_data *acq_data)
{
	int ret=0;
//##0426ST=32;CN=2011;PW=123456;MN=888888800000020123456781;CP=&&DataTime=20190630105200;LD4-Rtd=99.23,LD4-Flag=N;S02-Rtd=11.73,S02-Flag=N;S01-Rtd=17.08,S01-Flag=N;S03-Rtd=118.52,S03-Flag=N;S08-Rtd=2.23,S08-Flag=N;B02-Rtd=29163.11,B02-Flag=N;LD3-Rtd=29163.11,LD3-Flag=N;02-Rtd=2.48,02-ZsRtd=10.96,02-Flag=N;LD5-Rtd=105.23,LD5-Flag=N;03-Rtd=11.58,03-ZsRtd=44.30,03-Flag=N;LD6-Rtd=100.23,LD6-Flag=N;01-Rtd=2.46,01-ZsRtd=8.99,01-Flag=N;S05-Rtd=18.08,S05-Flag=N&&9d41

#define CEMSPOL_NUM 17

   char polcodestr[][20]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014","a21005","a21005z",
   "a21024","a21024z","a00000z"};

//#define CEMSPOL_NUM (ARRAY_SIZE(polcodestr))

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_KPA,UNIT_M3_S,UNIT_PECENT,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3};
   
   char cemspol[][20]={"02","02z","03","03z",
     "01","01z",
     "S02","S01","S03","S08","B02","S05","04","04z",
     "08","08z","a00000z"};

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
    SYSLOG_DBG("protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
	
#if 0 //del by miles zhang 20191209	
    return protocol_CEMS_GBhjt212(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);	
#else
	ret=protocol_CEMS_GBhjt212_once(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);

	NEED_ERROR_CACHE(acq_data,20);

	return ret;
#endif
}


int protocol_CEMS_GBhjt2005_PaMh(struct acquisition_data *acq_data)
{
	int ret=0;
//##0426ST=32;CN=2011;PW=123456;MN=888888800000020123456781;CP=&&DataTime=20190630105200;LD4-Rtd=99.23,LD4-Flag=N;S02-Rtd=11.73,S02-Flag=N;S01-Rtd=17.08,S01-Flag=N;S03-Rtd=118.52,S03-Flag=N;S08-Rtd=2.23,S08-Flag=N;B02-Rtd=29163.11,B02-Flag=N;LD3-Rtd=29163.11,LD3-Flag=N;02-Rtd=2.48,02-ZsRtd=10.96,02-Flag=N;LD5-Rtd=105.23,LD5-Flag=N;03-Rtd=11.58,03-ZsRtd=44.30,03-Flag=N;LD6-Rtd=100.23,LD6-Flag=N;01-Rtd=2.46,01-ZsRtd=8.99,01-Flag=N;S05-Rtd=18.08,S05-Flag=N&&9d41

#define CEMSPOL_NUM 17

   char polcodestr[][20]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014","a21005","a21005z",
   "a21024","a21024z","a00000z"};

//#define CEMSPOL_NUM (ARRAY_SIZE(polcodestr))

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_H,UNIT_PECENT,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3};
   
   char cemspol[][20]={"02","02z","03","03z",
     "01","01z",
     "S02","S01","S03","S08","B02","S05","04","04z",
     "08","08z","a00000z"};

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
    SYSLOG_DBG("protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
	
#if 0 //del by miles zhang 20191209	
    return protocol_CEMS_GBhjt212(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);	
#else
	ret=protocol_CEMS_GBhjt212_once(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);

	NEED_ERROR_CACHE(acq_data,20);

	return ret;
#endif
}

int protocol_CEMS_GBhjt2005_PaMs(struct acquisition_data *acq_data)
{
	int ret=0;
//##0426ST=32;CN=2011;PW=123456;MN=888888800000020123456781;CP=&&DataTime=20190630105200;LD4-Rtd=99.23,LD4-Flag=N;S02-Rtd=11.73,S02-Flag=N;S01-Rtd=17.08,S01-Flag=N;S03-Rtd=118.52,S03-Flag=N;S08-Rtd=2.23,S08-Flag=N;B02-Rtd=29163.11,B02-Flag=N;LD3-Rtd=29163.11,LD3-Flag=N;02-Rtd=2.48,02-ZsRtd=10.96,02-Flag=N;LD5-Rtd=105.23,LD5-Flag=N;03-Rtd=11.58,03-ZsRtd=44.30,03-Flag=N;LD6-Rtd=100.23,LD6-Flag=N;01-Rtd=2.46,01-ZsRtd=8.99,01-Flag=N;S05-Rtd=18.08,S05-Flag=N&&9d41

#define CEMSPOL_NUM 17

   char polcodestr[][20]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014","a21005","a21005z",
   "a21024","a21024z","a00000z"};

//#define CEMSPOL_NUM (ARRAY_SIZE(polcodestr))

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_S,UNIT_PECENT,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3};
   
   char cemspol[][20]={"02","02z","03","03z",
     "01","01z",
     "S02","S01","S03","S08","B02","S05","04","04z",
     "08","08z","a00000z"};

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
    SYSLOG_DBG("protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
	
#if 0 //del by miles zhang 20191209	
    return protocol_CEMS_GBhjt212(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);	
#else
	ret=protocol_CEMS_GBhjt212_once(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);

	NEED_ERROR_CACHE(acq_data,20);

	return ret;
#endif
}

