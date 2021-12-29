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
int protocol_COD_NJhongkai(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float total_cod=0;
    
   if(!acq_data) return -1;

/*
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0062100624**");
	size=strlen(com_tbuf);
	com_tbuf[size]=0x0d;
	size++;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("NJhongkai COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
    
	pf=acq_data->com_f;       
   if( (size >= 22)&&(strncmp(com_rbuf,"%01$RD",6)==0))
   {               
		pf[arg_n] = HexCharToDec(com_rbuf[18]) * 16 + HexCharToDec(com_rbuf[19]);
      	pf[arg_n] += (HexCharToDec(com_rbuf[20]) * 16 + HexCharToDec(com_rbuf[21])) * 0x100;
		strcpy(acq_data->polcode[arg_n],"011"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n ++;

		acq_data->acq_tm[0] = HexCharToDec(com_rbuf[6]) * 10 + HexCharToDec(com_rbuf[7]);
		acq_data->acq_tm[1] = HexCharToDec(com_rbuf[8]) * 10 + HexCharToDec(com_rbuf[9]);
		acq_data->acq_tm[2] = HexCharToDec(com_rbuf[10]) * 10 + HexCharToDec(com_rbuf[11]);
		acq_data->acq_tm[3] = HexCharToDec(com_rbuf[12]) * 10 + HexCharToDec(com_rbuf[13]);
		acq_data->acq_tm[4] = HexCharToDec(com_rbuf[14]) * 10 + HexCharToDec(com_rbuf[15]);
		acq_data->acq_tm[5] = HexCharToDec(com_rbuf[16]) * 10 + HexCharToDec(com_rbuf[17]) + 100;       
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"011"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n++;
		read_rtc_time(acq_data->acq_tm); 
		status = 1;
    }
*/
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0250402508**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"NJHongKai COD SEND:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJhongkai COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"NJHongKai COD RECV:",com_rbuf,size);
	
	if((size>=28)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0'))
		{
		f.ch[0]=HexCharToDec(com_rbuf[18])*16+HexCharToDec(com_rbuf[19]);
		f.ch[1]=HexCharToDec(com_rbuf[20])*16+HexCharToDec(com_rbuf[21]);
		f.ch[2]=HexCharToDec(com_rbuf[22])*16+HexCharToDec(com_rbuf[23]);
		f.ch[3]=HexCharToDec(com_rbuf[24])*16+HexCharToDec(com_rbuf[25]);
		total_cod=f.f;
		status=0;
		printf("the cod is %f\n",total_cod);
	}
	else
		{
		status=1;
		total_cod=0;
		printf("error of recieve data\n");
	}
	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,10);
    return arg_n;
}



