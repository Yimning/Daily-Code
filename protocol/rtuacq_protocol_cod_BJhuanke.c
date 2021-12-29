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
int protocol_COD_BJhuanke(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float total_cod=0;
    
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"*#GET");
	size=strlen(com_tbuf);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("BJhuanke COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
	if((size>=13)&&(com_rbuf[0]=='*')&&(com_rbuf[1]=='#'))
	{
			f.ch[0]=com_rbuf[9];
			f.ch[1]=com_rbuf[10];
			f.ch[2]=com_rbuf[11];
			f.ch[3]=com_rbuf[12];
			total_cod=f.f;
			status=0;
			printf("the cod =%f\n",total_cod);
			
	}
	else
		{
			status=1;
			total_cod=0;
			printf("error of recieve data\n");
	}
	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	
/*    
	pf=acq_data->com_f;       
   if( (size >= 12)&&(com_rbuf[0]=='*')&&(com_rbuf[1]=='#') )
   {               
		pf[arg_n] = *((float *)(&com_rbuf[8]));       
		strcpy(acq_data->polcode[arg_n],"011"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n ++;

		acq_data->acq_tm[5]= hex2dec(com_rbuf[3]) * 100 + hex2dec(com_rbuf[4]) -1900;
		acq_data->acq_tm[4]= hex2dec(com_rbuf[5]);
		acq_data->acq_tm[3]= hex2dec(com_rbuf[6]);
		acq_data->acq_tm[2]= hex2dec(com_rbuf[7]);
		acq_data->acq_tm[1]= hex2dec(com_rbuf[8]);
		acq_data->acq_tm[0]= 0;        
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"w01018"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n++;
		read_rtc_time(acq_data->acq_tm); 
		status = 1;
    }
*/
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}



