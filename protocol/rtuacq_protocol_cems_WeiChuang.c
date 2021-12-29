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



static int WeiChuang_hjt212_packet_is_ok(char *pack,int size)
{
    char *pp;
	if(!pack || size==0) return 0;//not ok
    if( size>40 && (pp=strstr(pack,"CP=&&")) && strstr(&pp[4],"&&")) return 1; // packet is ok

	return 0; //packet is not ok
}

static int getWeiChuangPolcodeValf(char *valstr,char *cemspol,float *valf)
{
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
   if(!polstr)
   {
   	memset(polRtdstr,0,sizeof(polRtdstr));
	sprintf(polRtdstr,",%s-Rtd",cems_polcode);
	polstr=strstr(valstr,polRtdstr);
   }
   
   if(polstr && valf)
   {
	 strcat(polRtdstr,"=%f,");
     ret=sscanf(polstr,polRtdstr,valf);
	 if(ret!=1) return -2;

	 return 0;
   }

   return -1;
}

static int get_WeiChuang_hjt212_packet_once(struct acquisition_data *acq_data,char *com_rbuf,int rbuff_len,int *size)
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

		if(WeiChuang_hjt212_packet_is_ok(p,len)) 
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


static int protocol_CEMS_WeiChuang_once(struct acquisition_data *acq_data,int cemspol_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,char cemspol[][20])
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
    ret=get_WeiChuang_hjt212_packet_once(acq_data,com_rbuf,sizeof(com_rbuf),&size);
	SYSLOG_DBG("protocol_CEMS_WeiChuang once,ret=%d,size=%d\n",ret,size);
	SYSLOG_DBG("protocol_CEMS_WeiChuang once:%s\n",com_rbuf);
	SYSLOG_DBG("protocol_CEMS_WeiChuang once2:%s\n",&com_rbuf[194]);
	SYSLOG_DBG("protocol_CEMS_WeiChuang once3:%s\n",&com_rbuf[388]);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"WeiChuang RECV:",com_rbuf,size);
	if(ret==0 && size >= 40 )
	{
	     for(i=0;i<cemspol_num;i++)
	     {
		       ret=getWeiChuangPolcodeValf(com_rbuf,cemspol[i],&f.f);
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

int protocol_CEMS_WeiChuang(struct acquisition_data *acq_data)
{	
//##0316ST=31;CN=2011;PW=;MN=;CP=&&DataTime=20200718123600;01-ZsRtd=2.47,01-Rtd=1.48,01-Flag=N;02-ZsRtd=5.08,02-Rtd=3.05,02-Flag=N;03-ZsRtd=0.61,03-Rtd=0.37,03-Flag=N;S01-Rtd=20.7,S01-Flag=N;S02-Rtd=3.27,S02-Flag=N;S03-Rtd=40.35,S03-Flag=N;S05-Rtd=0,S05-Flag=N;S08-Rtd=0.1,S08-Flag=N;B02-ZsRtd=6.46,B02-Rtd=7.43,B02-Flag=N&&7640

#define CEMSPOL_NUM 12

	char polcodestr[][20]={"a21026","a21026z","a21002","a21002z",
		"a34013","a34013z",
		"a01011","a19001","a01012","a01013","a00000","a01014"};



	UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
		UNIT_MG_M3,UNIT_MG_M3,
		UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_H,UNIT_PECENT};

	char cemspol[][20]={"02","02z","03","03z",
		"01","01z",
		"S02","S01","S03","S08","B02","S05"};

	SYSLOG_DBG("protocol_CEMS_WeiChuang:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
	int ret=0;

	ret=protocol_CEMS_WeiChuang_once(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);

	NEED_ERROR_CACHE(acq_data,20);

	return ret;

}


