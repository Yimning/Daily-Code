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


static const  char * const polcode2005HB[] = {"B01","B00"};
static const  char * const polcode2017HB[] = {"w00000","w00001"};
static const UNIT_DECLARATION const units[]={UNIT_L_S,UNIT_M3};
/*
static char* memstr(char* buffer, int buff_len, char* substr)
{
    int sublen,bufflen;
	int i=0;

    if (buffer == NULL || buff_len <= 0 || substr == NULL) {
        return NULL;
    }

    if (*substr == '\0') {
        return NULL;
    }

   	sublen = strlen(substr);
    bufflen = buff_len-sublen + 1;
	
    for (i = 0; i < bufflen; i++) 
	{
        if (buffer[i] == substr[0] && 
			!memcmp(&buffer[i], substr, sublen)) 
                return &buffer[i];
    }

    return NULL;
}
*/
static int comrbuf_get_valf_HeBei(char *rbuf,int rbuf_len,char *polcode,float *valf,char *flag)
{
   char polstr[100]={0};
   char *polindex;
   union uf f;
   char vflag;
   char *valindex=NULL;

   if(!rbuf || !polcode || !valf) return -1;

#if 0 //mod by miles zhang 20190729
   //sprintf(polstr,"%s-RN",polcode);
   if(strlen(polcode)==3)sprintf(polstr,"%s-RN",polcode);
   if(strlen(polcode)==2)sprintf(polstr,"%s -RN",polcode);  

   if(strstr(polcode,"z"))
   {
        char polcodeTmp[20]={0};
        strcpy(polcodeTmp,polcode);
		polcodeTmp[strlen(polcodeTmp)-1]='\0';
        if(strlen(polcodeTmp)==3)sprintf(polstr,"%s-ZN",polcodeTmp);
        if(strlen(polcodeTmp)==2)sprintf(polstr,"%s -ZN",polcodeTmp);
   }
   else
   {
        if(strlen(polcode)==3)sprintf(polstr,"%s-RN",polcode);
        if(strlen(polcode)==2)sprintf(polstr,"%s -RN",polcode);
   }

   if((polindex=memstr(rbuf,rbuf_len,polstr))==NULL) 
   {
	   if(strstr(polcode,"z"))
	   {
	        char polcodeTmp[20]={0};
	        strcpy(polcodeTmp,polcode);
			polcodeTmp[strlen(polcodeTmp)-1]='\0';
	        if(strlen(polcodeTmp)==3)sprintf(polstr,"%s-ZT",polcodeTmp);
	        if(strlen(polcodeTmp)==2)sprintf(polstr,"%s -ZT",polcodeTmp);
	   }
	   else
	   {
	        if(strlen(polcode)==3)sprintf(polstr,"%s-RT",polcode);
	        if(strlen(polcode)==2)sprintf(polstr,"%s -RT",polcode);
	   }
	   if((polindex=memstr(rbuf,rbuf_len,polstr))==NULL) return -2;

	   valindex=memchr(polindex,'T',strlen(polstr));
		if(valindex==NULL) return -3;
	  	f.ch[3]=valindex[1];
		f.ch[2]=valindex[2];
		f.ch[1]=valindex[3];
		f.ch[0]=valindex[4];
		*valf=f.f;
		*flag='T';
		
	    return 0;
   }


   	valindex=memchr(polindex,'N',strlen(polstr));
	if(valindex==NULL) return -3;
  	f.ch[3]=valindex[1];
	f.ch[2]=valindex[2];
	f.ch[1]=valindex[3];
	f.ch[0]=valindex[4];
	*valf=f.f;
	*flag='N';
	
    return 0;
#endif

	if(strstr(polcode,"z"))
	{
		char polcodeTmp[100]={0};
		strcpy(polcodeTmp,polcode);
		polcodeTmp[strlen(polcodeTmp)-1]='\0';
		if(strlen(polcodeTmp)==3)sprintf(polstr,"%s-Z",polcodeTmp);
		if(strlen(polcodeTmp)==2)sprintf(polstr,"%s -Z",polcodeTmp);
	}
	else
	{
		if(strlen(polcode)==3)sprintf(polstr,"%s-R",polcode);
		if(strlen(polcode)==2)sprintf(polstr,"%s -R",polcode);
	}
	if((polindex=memstr(rbuf,rbuf_len,polstr))==NULL) return -2;
	vflag=polindex[5];
	switch(vflag)
	{
		case 'N' : *flag='N';break;
		case 'M' : *flag='M';break;
		case 'T' : *flag='T';break;
		case 'D' : *flag='D';break;
		case 'C' : *flag='C';break;
		default : 
		{
			if(vflag>='A' && vflag<='Z') { *flag=vflag; break;}
			else
			return -3;break;
		}
	}
	

	f.ch[3]=polindex[6];
	f.ch[2]=polindex[7];
	f.ch[1]=polindex[8];
	f.ch[0]=polindex[9];
	*valf=f.f;

	return 0;
}


static int comrbuf_get_valf_HeBei2(char *rbuf,int rbuf_len,char *polcode,float *valf,char *flag)
{
   char polstr[100]={0};
   char *polindex;
   union uf f;
   char vflag;
   char *valindex=NULL;

   if(!rbuf || !polcode || !valf) return -10;

	if(strstr(polcode,"z"))
	{
		char polcodeTmp[100]={0};
		strcpy(polcodeTmp,polcode);
		polcodeTmp[strlen(polcodeTmp)-1]='\0';
		//if(strlen(polcodeTmp)==3)sprintf(polstr,"%s-Z",polcodeTmp);
		//if(strlen(polcodeTmp)==2)sprintf(polstr,"%s -Z",polcodeTmp);
		if(strlen(polcodeTmp)==2)
			sprintf(polstr,"%s-Z",polcodeTmp);
		else return -11;
	}
	else
	{
		//if(strlen(polcode)==3)sprintf(polstr,"%s-R",polcode);
		//if(strlen(polcode)==2)sprintf(polstr,"%s -R",polcode);
		if(strlen(polcode)==2)
			sprintf(polstr,"%s-R",polcode);
		else return -12;
	}
	if((polindex=memstr(rbuf,rbuf_len,polstr))==NULL) return -21;
	vflag=polindex[4];
	switch(vflag)
	{
		case 'N' : *flag='N';break;
		case 'M' : *flag='M';break;
		case 'T' : *flag='T';break;
		case 'D' : *flag='D';break;
		case 'C' : *flag='C';break;
		
		default : 
		{
			if(vflag>='A' && vflag<='Z') { *flag=vflag; break;}
			else
			return -3;break;
		}
	}
	

	f.ch[3]=polindex[5];
	f.ch[2]=polindex[6];
	f.ch[1]=polindex[7];
	f.ch[0]=polindex[8];
	*valf=f.f;

	return 0;
}


int protocol_HeBei(struct acquisition_data *acq_data,char *polcode2005HB[100],int polcode2005HB_len,
	                       char * polcode2017HB[100],int polcode2017HB_len,UNIT_DECLARATION units[100],int units_len)
{
	int status=0;
	int size=0;
	int arg_n=0;
	int ret=0;
	float valf=0;
	int i=0;
	char flag;
	char strcache[2048]={0};
	char com_rbuf[1024]={0}; 
	char com_tbuf[30]={0x23,0x23,0x01,
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		0x11,0x22,0x26,0x26};// for HZWeiLan
	//0xff,0xff,0x26,0x26};// for HBCangZhou
	//0x7e,0xc0,0x26,0x26};   
	size=17;	

	if(polcode2005HB_len<=0 ||
	polcode2005HB_len!=polcode2017HB_len ||
	polcode2005HB_len!=units_len) return -1;
	if(!polcode2005HB || !polcode2017HB || !units) return -2;  

	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	memset(strcache,0,sizeof(strcache));
	for(i=0;(i<size) && (i<500);i++)
	sprintf(&strcache[strlen(strcache)],"%.2x ",com_tbuf[i]);  
	SYSLOG_DBG("HeBei protocol : write device %s , size=%d,%s\n",DEV_NAME(acq_data),size,strcache);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HeBei SEND:",com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HeBei protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HeBei protocol read",com_rbuf,size);

	//printf("pack:%s\n",&com_rbuf[13]);
	SYSLOG_DBG("polcode2005HB_len %d pack:%s\n",polcode2005HB_len,&com_rbuf[13]); 
	memset(strcache,0,sizeof(strcache));
	for(i=0;(i<size) && (i<500);i++)
		sprintf(&strcache[strlen(strcache)],"%.2x ",com_rbuf[i]);
	SYSLOG_DBG("HeBei size=%d,%s=%s\n",size,polcode2005HB[0],strcache);
	//write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"HeBei size=%d,%s=%s\n",size,polcode2005HB[0],strcache);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBei RECV:",com_rbuf,size);

	if(size<26 || size > 1000) goto ERR_DATA;
	if(memstr(com_rbuf,size,"##")==NULL) goto ERR_DATA;
	if(memstr(com_rbuf,size,"&&")==NULL) goto ERR_DATA;


	status=-1;
	for(i=0;i<polcode2005HB_len;i++)
	{
		ret=comrbuf_get_valf_HeBei(com_rbuf,size,polcode2005HB[i],&valf,&flag);
		if(ret<0) ret=comrbuf_get_valf_HeBei2(com_rbuf,size,polcode2005HB[i],&valf,&flag); //added by miles zhang 20210508
		
		SYSLOG_DBG("polcode %s,ret=%d,valf=%f\n",polcode2005HB[i],ret,valf); 
		if(ret<0)
		{
			if(!strcmp(polcode2017HB[i],"a01017"))
			{
				struct rdtu_info *rdtuinfo;
				rdtuinfo=get_parent_rdtu_info(acq_data);
				acqdata_set_value_flag(acq_data,polcode2017HB[i],units[i],rdtuinfo->gas_acreage,'N',&arg_n);
			}
			else
			{
				acqdata_set_value_flag(acq_data,polcode2017HB[i],units[i],0,'D',&arg_n);
			}
		}
		else 
		{
			int unit_tmp=units[i];
/*	//del by miles zhang 20210525 for  error when a01013(pa)=-5, will be -5(Kpa)= -5000Pa		
			if(!strcmp(polcode2017HB[i],"a00000"))
			{
				unit_tmp=UNIT_M3_S;
				if(valf>200 || valf<-200)
				{
					valf/=3600;
				}
			}
			
			else if(!strcmp(polcode2017HB[i],"a01013"))
			{
				unit_tmp=UNIT_KPA;
				if(valf>10 || valf<-10)
				{
					valf/=1000;
				}
			}
*/			

			acqdata_set_value_flag(acq_data,polcode2017HB[i],unit_tmp,valf,flag,&arg_n);
			status=0;
		}
	}

	acq_data->dev_stat=0xAA;
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;

ERR_DATA:
	for(i=0;i<polcode2005HB_len;i++)
	{
		acqdata_set_value(acq_data,polcode2017HB[i],units[i],0,&arg_n);
	}
	acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}


static int comrbuf_get_datatime_HeBei(char *rbuf,int rbuf_len,int *tm)
{
	if(!rbuf || rbuf_len<=12 || !tm) return -1;

	tm[5]=2000+BCD(rbuf[7]);
	tm[4]=BCD(rbuf[8]);
	tm[3]=BCD(rbuf[9]);
	tm[2]=BCD(rbuf[10]);
	tm[1]=BCD(rbuf[11]);
	tm[0]=BCD(rbuf[12]);

	return 0;
}


static int comrbuf_get_valf_flag_HeBei(char *rbuf,int rbuf_len,char *polcode,float *valf,char *flag)
{
	char polstr[100]={0};
	char *polindex,*valindex;
	union uf f;
	char fflag='D';

	if(!rbuf || rbuf_len<=0 || !polcode || !valf || !flag) return -1;

	//SYSLOG_DBG("comrbuf_get_valf_flag_HeBei 1 %s\n",polcode);
	if(strstr(polcode,"z"))
	{
		char polcodeTmp[20]={0};
		strcpy(polcodeTmp,polcode);
		polcodeTmp[strlen(polcodeTmp)-1]='\0';
		if(strlen(polcodeTmp)==3) sprintf(polstr,"%s-Z",polcodeTmp);
		if(strlen(polcodeTmp)==2) sprintf(polstr,"%s -Z",polcodeTmp);
		//SYSLOG_DBG("comrbuf_get_valf_flag_HeBei 2 %s,%s: %s\n",polcode,polcodeTmp,polstr);
	}
	else
	{
		if(strlen(polcode)==3)sprintf(polstr,"%s-R",polcode);
		if(strlen(polcode)==2)sprintf(polstr,"%s -R",polcode);
	}


	if((polindex=memstr(rbuf,rbuf_len,polstr))==NULL) return -2;
	//"B01-RN" for flag=N
	if(strstr(polcode,"z"))
		valindex=memchr(polindex,'Z',strlen(polstr));
	else
		valindex=memchr(polindex,'R',strlen(polstr));

	if(valindex==NULL) return -3;

	fflag=valindex[1];
	f.ch[3]=valindex[2];
	f.ch[2]=valindex[3];
	f.ch[1]=valindex[4];
	f.ch[0]=valindex[5];

	*flag=fflag;
	*valf=f.f;

	return 0;
}


int HeBei_rtdata(struct acquisition_data *acq_data,char polcode2005HB[][20],int polcode2005HB_len,
	                       char polcode2017HB[][20],int polcode2017HB_len,UNIT_DECLARATION units[100],int units_len)
{
	int status=0;
	int size=0;
	int arg_n=0;
	int ret=0;
	float valf=0;
	char flag='D';
	int i=0;
	//char strcache[2048]={0};
	char com_rbuf[1024]={0}; 
	char com_tbuf[30]={0x23,0x23,0x01, // 0x01 rtdate, 0x02 minute data, 0x03 hour data, 0x04 day data
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		0x11,0x22,0x26,0x26};

	size=17;

	if(polcode2005HB_len<=0 ||
	polcode2005HB_len!=polcode2017HB_len ||
	polcode2005HB_len!=units_len) return -1;
	if(!polcode2005HB || !polcode2017HB || !units) return -2;

#if 0
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG_HEX("HeBei rtdata write ",com_tbuf,size);

	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HeBei protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HeBei protocol read",com_rbuf,size);
#else
	for(i=0;i<3;i++)
	{// for gongkong pc
		size=write_device(DEV_NAME(acq_data), com_tbuf, size);
		SYSLOG_DBG_HEX("HeBei rtdata write ",com_tbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HeBei rtdata SEND:",com_tbuf,size);

		sleep(2);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data), com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("HeBei rtdata protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("HeBei rtdata protocol read",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBei rtdata RECV:",com_rbuf,size);

		if(size<26 || size > 1000) continue;

		if(memstr(com_rbuf,size,"##")==NULL) continue;
		if(memstr(com_rbuf,size,"&&")==NULL) continue;

		break;
	}
#endif


	if(size<26 || size > 1000) goto ERR_DATA;

	if(memstr(com_rbuf,size,"##")==NULL) goto ERR_DATA;
	if(memstr(com_rbuf,size,"&&")==NULL) goto ERR_DATA;

	if(com_rbuf[5]!=0x52) goto ERR_DATA;

	status=-1;
	for(i=0;i<polcode2005HB_len;i++)
	{
		ret=comrbuf_get_valf_flag_HeBei(com_rbuf,size,polcode2005HB[i],&valf,&flag);
		SYSLOG_DBG("HeBei rtdata polcode %s,ret=%d,valf=%f\n",polcode2005HB[i],ret,valf); 
		if(ret<0)
			//acqdata_set_value(acq_data,polcode2017HB[i],units[i],0,&arg_n);
			acqdata_set_rtdata(acq_data,polcode2017HB[i],units[i],0,'D',&arg_n);
		else 
		{
			//acqdata_set_value(acq_data,polcode2017HB[i],units[i],valf,&arg_n);
			acqdata_set_rtdata(acq_data,polcode2017HB[i],units[i],valf,flag,&arg_n);
			status=0;
		}
	}

	comrbuf_get_datatime_HeBei(com_rbuf,size,acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;

	ERR_DATA:
	for(i=0;i<polcode2005HB_len;i++)
	{
		//acqdata_set_value(acq_data,polcode2017HB[i],units[i],0,&arg_n);
		acqdata_set_rtdata(acq_data,polcode2017HB[i],units[i],0,'D',&arg_n);
	}
	comrbuf_get_datatime_HeBei(com_rbuf,size,acq_data->acq_tm);
	acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

static int comrbuf_get_statt_valf_flag_HeBei(char *rbuf,int rbuf_len,char *polcode,
	                                         float *avg,float *min,float *max,float *cou,char *flag)
{
	char polstr[100]={0};
	char *polindex,*valindex;
	union uf f;
	char fflag='D';

	if(!rbuf || rbuf_len<=0 || !polcode || !avg ||!min||!max||!cou|| !flag) return -1;

	//SYSLOG_DBG("comrbuf_get_statt_valf_flag_HeBei 1 %s\n",polcode);
	if(strstr(polcode,"z"))
	{
		char polcodeTmp[20]={0};
		strcpy(polcodeTmp,polcode);
		polcodeTmp[strlen(polcodeTmp)-1]='\0';
		if(strlen(polcodeTmp)==3) sprintf(polstr,"%s-Z",polcodeTmp);
		if(strlen(polcodeTmp)==2) sprintf(polstr,"%s -Z",polcodeTmp);
		//SYSLOG_DBG("comrbuf_get_statt_valf_flag_HeBei 2 %s,%s: %s\n",polcode,polcodeTmp,polstr);
	}
	else
	{
		if(strlen(polcode)==3) sprintf(polstr,"%s-R",polcode);
		if(strlen(polcode)==2) sprintf(polstr,"%s -R",polcode);
	}

	if((polindex=memstr(rbuf,rbuf_len,polstr))==NULL) return -2;

	//"B01-RN" for flag=N
	if(strstr(polcode,"z"))
		valindex=memchr(polindex,'Z',strlen(polstr));
	else
		valindex=memchr(polindex,'R',strlen(polstr));

	if(valindex==NULL) return -3;

	fflag=valindex[1];
	*flag=fflag;

	f.ch[3]=valindex[2];
	f.ch[2]=valindex[3];
	f.ch[1]=valindex[4];
	f.ch[0]=valindex[5];
	*cou=f.f;
	f.ch[3]=valindex[6];
	f.ch[2]=valindex[7];
	f.ch[1]=valindex[8];
	f.ch[0]=valindex[9];
	*min=f.f;
	f.ch[3]=valindex[10];
	f.ch[2]=valindex[11];
	f.ch[1]=valindex[12];
	f.ch[0]=valindex[13];
	*avg=f.f;
	f.ch[3]=valindex[14];
	f.ch[2]=valindex[15];
	f.ch[1]=valindex[16];
	f.ch[0]=valindex[17];
	*max=f.f;	

	return 0;
}


int HeBei_stattdata(int cn,struct acquisition_data *acq_data,char polcode2005HB[][20],int polcode2005HB_len,
	                       char polcode2017HB[][20],int polcode2017HB_len,UNIT_DECLARATION units[100],int units_len)
{
	int status=0;
	int size=0;
	int arg_n=0;
	int ret=0;
	//float valf=0;
	float avg,min,max,cou;
	char flag='D';
	int i=0;
	//char strcache[2048]={0};
	char com_rbuf[1024]={0}; 
	char com_tbuf[30]={0x23,0x23,0x02, // 0x01 rtdate, 0x02 minute data, 0x03 hour data, 0x04 day data
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		0x11,0x22,0x26,0x26};

	if(cn==2051)com_tbuf[2]=0x02; //minute
	if(cn==2061)com_tbuf[2]=0x03; //minute
	if(cn==2031)com_tbuf[2]=0x04; //minute

	size=17;

	if(polcode2005HB_len<=0 ||
		polcode2005HB_len!=polcode2017HB_len ||
		polcode2005HB_len!=units_len) return -1;
	if(!polcode2005HB || !polcode2017HB || !units) return -2;


	for(i=0;i<3;i++)
	{// for gongkong pc
		size=write_device(DEV_NAME(acq_data), com_tbuf, size);
		SYSLOG_DBG_HEX("HeBei stattdata write ",com_tbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HeBei stattdata SEND:",com_tbuf,size);

		sleep(2);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data), com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("HeBei protocol cn=%d: read device %s , size=%d\n",cn,DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("HeBei stattdata protocol read",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBei stattdata RECV:",com_rbuf,size);

		if(size<26 || size > 1000) continue;

		if(memstr(com_rbuf,size,"##")==NULL) continue;
		if(memstr(com_rbuf,size,"&&")==NULL) continue;

		break;
	}


	if(size<26 || size > 1000) goto ERR_DATA;

	if(memstr(com_rbuf,size,"##")==NULL) goto ERR_DATA;
	if(memstr(com_rbuf,size,"&&")==NULL) goto ERR_DATA;

	if(cn==2051 && com_rbuf[5]!=0x4D) goto ERR_DATA;
	if(cn==2061 && com_rbuf[5]!=0x48) goto ERR_DATA;
	if(cn==2031 && com_rbuf[5]!=0x44) goto ERR_DATA;

	status=-1;
	for(i=0;i<polcode2005HB_len;i++)
	{
		ret=comrbuf_get_statt_valf_flag_HeBei(com_rbuf,size,polcode2005HB[i],&avg,&min,&max,&cou,&flag);
		SYSLOG_DBG("HeBei stattdata cn=%d polcode %s,ret=%d,avg=%f,min=%f,max=%f,cou=%f,flag=%c\n",
			cn,polcode2005HB[i],ret,avg,min,max,cou,flag); 
		if(ret<0)
			//acqdata_set_value(acq_data,polcode2017HB[i],units[i],0,&arg_n);
			acqdata_set_stattdata(acq_data,polcode2017HB[i],units[i],0,0,0,0,'D',&arg_n);
		else 
		{
			//acqdata_set_value(acq_data,polcode2017HB[i],units[i],valf,&arg_n);
			acqdata_set_stattdata(acq_data,polcode2017HB[i],units[i],avg,min,max,cou,flag,&arg_n);
			status=0;
		}
	}

	acq_data->cn=cn;

	comrbuf_get_datatime_HeBei(com_rbuf,size,acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;

ERR_DATA:
	for(i=0;i<polcode2005HB_len;i++)
	{
		//acqdata_set_value(acq_data,polcode2017HB[i],units[i],0,&arg_n);
		acqdata_set_stattdata(acq_data,polcode2017HB[i],units[i],0,0,0,0,'D',&arg_n);
	}
	acq_data->cn=cn;
	comrbuf_get_datatime_HeBei(com_rbuf,size,acq_data->acq_tm);
	acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



