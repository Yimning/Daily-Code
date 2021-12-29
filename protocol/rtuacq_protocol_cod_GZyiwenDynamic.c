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

/*
return : com_f arguments amount;
*/

static int COD_GZyiwen_to_hjt2017_status(int status)
{
       int workstat=0;
	   
	   switch(status)
	   {
	      	case 0: 
		case 4:
		  	workstat=9; //hjt2017 doc i12001
		  	break;
		case 1:
		case 5:
		  	workstat=7; //hjt2017 doc i12001
		  	break;
		case 2:
		case 6:
		  	workstat=8; //hjt2017 doc i12001
		  	break;
		case 3:
		case 7:
		  	workstat=11; //hjt2017 doc i12001
		  	break;
		case 8:
		  	workstat=2; //hjt2017 doc i12001
		  	break;
		  default:
		   	workstat=2;
	   }
	   
	   return workstat;
}


static int read_GZyiwenDynamic_data(struct acquisition_data *acq_data,const char *t_data,char *com_rbuf,int len)
{
	char com_tbuf[255];
	int size=0;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,t_data);
	size=strlen(com_tbuf);
	SYSLOG_DBG("yiwen Dynamic COD data write size=%d:%s\n",size,com_tbuf);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,len);
	size=read_device(DEV_NAME(acq_data),com_rbuf,len-1);
	SYSLOG_DBG("yiwen Dynamic COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("yiwen Dynamic COD data : %s\n",com_rbuf);

	return size;
}

int protocol_COD_GZyiwenDynamic(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]; 
	int size=0;
	int arg_n=0;
	char *pf;
	int ret=0;
	char data[10]={0};
	char *pstr=NULL;

	if(!acq_data) return -1;

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_GZyiwenDynamic_data(acq_data,"##GET=01**",com_rbuf,sizeof(com_rbuf));	   
	if((size >= 40) && strstr(com_rbuf,"##") && strstr(com_rbuf,"**"))
	{
		float total_cod=0;
		int valid=0;
		float voltage=0;
		int  tm[TIME_ARRAY]={0};

		
        ret=sscanf(com_rbuf,"##VAL=%d;DAT=%f;DATATIME=%d-%d-%d %d:%d:%d;VOL=%f**",
			&valid,&total_cod,&tm[5],&tm[4],&tm[3],&tm[2],&tm[1],&tm[0],&voltage);

		if(ret!=9){
			acqdata_set_value(acq_data,"w01018",UNIT_MG_L,0,&arg_n);
			memset(acq_data->acq_tm,0,TIME_LEN);
			acq_data->voltage=0;			
			status=1;
			goto ERROR_ACQ;
		}

		acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
		memcpy(acq_data->acq_tm,tm,TIME_LEN);
		acq_data->voltage=voltage;
	}
	else
	{
			acqdata_set_value(acq_data,"w01018",UNIT_MG_L,0,&arg_n);
			memset(acq_data->acq_tm,0,TIME_LEN);
			acq_data->voltage=0;			
			status=1;
	}

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_GZyiwenDynamic_data(acq_data,"##GET=02**",com_rbuf,sizeof(com_rbuf));	
	if((size >= 10) && strstr(com_rbuf,"##") && strstr(com_rbuf,"**"))
	{
	    int stat=0;
		ret=sscanf(com_rbuf,"##DATA=%d**",&stat);
		if(ret!=1)
		{
		}

		acq_data->instrstat=COD_GZyiwen_to_hjt2017_status(stat);

	}
	
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_GZyiwenDynamic_data(acq_data,"##GET=03**",com_rbuf,sizeof(com_rbuf));	
	if((size >= 10) && strstr(com_rbuf,"##") && strstr(com_rbuf,"**"))
	{
        int valid=0;
		int alarm=0;
	    ret=sscanf(com_rbuf,"##VAL=%d;DATA=%d**",&valid,&alarm);
		if(ret!=2)
		{
		}
		if(valid==0) alarm=0;
		else alarm=1;
		acq_data->alarm=alarm;

	}
	
ERROR_ACQ:
	if(status == 0)
	acq_data->acq_status = ACQ_OK;
	else 
	acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



