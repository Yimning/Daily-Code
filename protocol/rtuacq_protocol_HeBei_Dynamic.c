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

static int HeBei_Dynamic_packet_is_ok(char *pack,int size)
{
	char *pp;
	if(!pack || size==0) return 0;//not ok
	if( size>40 && (pp=strstr(pack,"CP=&&")) && strstr(&pp[4],"&&")) return 1; // packet is ok

	return 0; //packet is not ok
}

static int getHeBeiDynamicPolcodeValf(char *valstr,char *cemspol,float *valf)
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

	if(polstr && valf)
	{
		strcat(polRtdstr,"=%f,");
		ret=sscanf(polstr,polRtdstr,valf);
		if(ret!=1) return -2;

		return 0;
	}

	return -1;
}

static int getHeBeiDynamicInfopolValf(char *com_rbuf,char *info_polcode,void *val)
{
	int ret=0;
	char polInfostr[100]={0};
	char timestr[20];
	char *polstr;
	float *valf;
	time_t *valt;
	struct tm timer;
	int t[6]={0};

	if(!com_rbuf || !info_polcode ||!val) return -1;

	memset(polInfostr,0,sizeof(polInfostr));

	sprintf(polInfostr,";%s-Info",info_polcode);

	polstr=strstr(com_rbuf,polInfostr);

	if(polstr)
	{
		if(!strcmp(info_polcode,"i13021") ||
			!strcmp(info_polcode,"i13027") ||

			!strcmp(info_polcode,"i13101") ||
			!strcmp(info_polcode,"i13107") ||
			!strcmp(info_polcode,"i13128"))
		{
			valt=(time_t *)val;
		
			strcat(polInfostr,"=%14[^;];");
			ret=sscanf(polstr,polInfostr,timestr);
			if(ret!=1) return -2;

			ret=sscanf(timestr,"%04d%02d%02d%02d%02d%02d",&(t[0]),&(t[1]),&(t[2]),&(t[3]),&(t[4]),&(t[5]));
			if(ret!=6) return -3;

			timer.tm_year=t[0]-1900;
			timer.tm_mon=t[1]-1;
			timer.tm_mday=t[2];
			timer.tm_hour=t[3];
			timer.tm_min=t[4];
			timer.tm_sec=t[5];
			*valt=mktime(&timer);
		}
		else
		{
			valf=(float *)val;
			strcat(polInfostr,"=%f;");
			ret=sscanf(polstr,polInfostr,valf);
			if(ret!=1) return -2;
		}

		return 0;
	}

	return -1;
}

static int get_HeBei_Dynamic_packet(struct acquisition_data *acq_data,char *com_rbuf,int rbuff_len,int *size)
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
		SYSLOG_DBG("%d>get_HeBei_Dynamic_packet,ret=%d,rbuff_len=%d,len=%d\n",i,ret,rbuff_len,len);
		if(ret<=0 || (p=memstr(com_rbuf,ret,"CN=2011"))==NULL) 
		{//isnot rtdata
			if((p=memstr(com_rbuf,ret,"CN=8013"))==NULL || (p=memstr(com_rbuf,ret,"CN=3020"))==NULL)
			{
				len=0;
				continue;
			}
		}

		len=ret;

		if(HeBei_Dynamic_packet_is_ok(p,len)) 
			break;
		else
		{
			sleep(1);
			ret=read_device(DEV_NAME(acq_data),&com_rbuf[len],rbuff_len-len);
			len+=ret;
			break;
		}
	}

	*size=len;

	return (i<LOOP_ONCE) ? 0 : -2;
}


int protocol_HeBei_Dynamic(struct acquisition_data *acq_data,int polcode_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,int infopol_num,char infopol[][20],void **infoval,int *infoStatus)
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
	char str[30]={0};
	int len;

	if(!acq_data || !polcodestr || !unitstr || !infopol || !infoStatus) return -1;

	for(i=0;i<infopol_num;i++)
	{
		if(infopol[i]==NULL)
			return -1;
	}


	memset(com_rbuf,0,sizeof(com_rbuf));
	ret=get_HeBei_Dynamic_packet(acq_data,com_rbuf,sizeof(com_rbuf),&size);
	SYSLOG_DBG("protocol_HeBei_Dynamic once,ret=%d,size=%d\n",ret,size);
	SYSLOG_DBG("protocol_HeBei_Dynamic once:%s\n",com_rbuf);
	SYSLOG_DBG("protocol_HeBei_Dynamic once2:%s\n",&com_rbuf[194]);
	SYSLOG_DBG("protocol_HeBei_Dynamic once3:%s\n",&com_rbuf[388]);
	for(i=0,len=0;len<size;len+=200)
	{
		i++;
		sprintf(str,"HeBei Dynamic RECV%d:",i);
		LOG_WRITE_STR(DEV_NAME(acq_data),1,str,&com_rbuf[len],200);
	}
	
	if(ret==0 && size >= 40 )
	{
		for(i=0;i<polcode_num;i++)
		{
			ret=getHeBeiDynamicPolcodeValf(com_rbuf,polcodestr[i],&f.f);
			if(ret!=0)
			{
				f.f=0;
				status = 1;
			}
			if(strlen(polcodestr[i])>0)
			{
				acqdata_set_value(acq_data,polcodestr[i],unitstr[i],f.f,&arg_n);
			}
		}
		
		for(i=0;i<infopol_num;i++)
		{
			ret=getHeBeiDynamicInfopolValf(com_rbuf,infopol[i],infoval[i]);
		}
	}
	else
	{
		int i=0;
		read_system_time(acq_data->acq_tm);
		for(i=0;i<polcode_num;i++)
		{
			if(strlen(polcodestr[i])>0)
				acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0.0,&arg_n);
		}		

		status = 1;
		*infoStatus = 1;
		//status = 0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;	
}



