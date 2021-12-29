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
int protocol_FLUX_BJhuanke(struct acquisition_data *acq_data)
{
	int status=0;
   char com_tbuf[255]; 
   char com_rbuf[2048]; 
   float *pf;
	int size=0;
   int arg_n=0;
	int i=0;
	char str[9]={0},str1[3]={0},str2[3]={0},str3[3]={0};
    
   if(!acq_data) return -1;
#if 1
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0xEB;
	com_tbuf[1]=0x4D;
	com_tbuf[2]=0x90;
	com_tbuf[3]=0x01;
	size=4;
	
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("BJhuanke FLUX protocol,FLUX : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG_HEX("BJhuanke FLUX",com_rbuf,size);
    
   	pf=acq_data->com_f;       
	if(size>=11 && com_rbuf[0]==0xEA && com_rbuf[1]==0x4D 
		&& com_rbuf[2]==0x90 && com_rbuf[3]==0x01)
	{
		memset(str, 0x00, sizeof(str));
      	if(com_rbuf[4] < 0x40)
		{
			sprintf(str, "%02x%02x", com_rbuf[5], com_rbuf[6]);
          pf[arg_n] = ((float)atoi(str) / 10000.0) * pow10((float)com_rbuf[4]);
          pf[arg_n] /= 3.6;
		}
     	else
    	{
       	sprintf(str, "%02x%02x", com_rbuf[5], com_rbuf[6]);
          pf[arg_n] = ((float)atoi(str) / 10000.0) / pow10((float)(0x80 - com_rbuf[4]));
          pf[arg_n] /= 3.60;
    	}
		strcpy(acq_data->polcode[arg_n],"w00000");
		acq_data->unit[arg_n]=UNIT_L_S;
		arg_n++;

		memset(str, 0x00, sizeof(str));
      	sprintf(str, "%02x", com_rbuf[7]);
       memset(str1, 0x00, sizeof(str1));
       sprintf(str1, "%02x", com_rbuf[8]);
       memset(str2, 0x00, sizeof(str2));
       sprintf(str2, "%02x", com_rbuf[9]);
       memset(str3, 0x00, sizeof(str3));
       sprintf(str3, "%02x", com_rbuf[10]);
       pf[arg_n] = (float)atoi(str) + (float)atoi(str1) * 100 + (float)atoi(str2) * 10000 + 
			 	(float)atoi(str3) * 1000000;

	   	acq_data->total=pf[arg_n];

		strcpy(acq_data->polcode[arg_n],"w00001");
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n++;

		//read_rtc_time(acq_data->acq_tm); 
		read_system_time(acq_data->acq_tm);
	}
	else
   {
   		pf[arg_n]=0.0;
      	strcpy(acq_data->polcode[arg_n],"w00000"); 
      	acq_data->unit[arg_n]=UNIT_L_S;
		arg_n++;
	 	pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"w00001"); 
      	acq_data->unit[arg_n]=UNIT_M3;
       //read_rtc_time(acq_data->acq_tm); 
       read_system_time(acq_data->acq_tm);
			 
       status = 1;
 	}
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
   		acq_data->acq_status = ACQ_ERR;
#endif

    return arg_n;
}



