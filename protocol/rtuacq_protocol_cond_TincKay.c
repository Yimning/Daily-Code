#include <unistd.h>
#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"



int protocol_TincKay_COND(struct acquisition_data *acq_data)
{
   int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[20]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
    
   if(!acq_data) return -1;

   read_system_time(acq_data->acq_tm); 
   pf=acq_data->com_f;       

   //=cond conductivity
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x4,0x03,0x1);
   printf("write size %d ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"write cond data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
 
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_TincKay_COND Electric conductivity : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   printf("size %d electric data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    size , com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   
   if( (size >= 7)&&(com_rbuf[0] == 0x01) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		pf[arg_n] = f.s;     

		printf("pf[arg_n] =%f,f.s=%.4x,%.2x\n",pf[arg_n] ,f.s,com_rbuf[2]);		
		if(com_rbuf[2]==0x3)
		    pf[arg_n] /=100;
		else if(com_rbuf[2]==0x2)
		    pf[arg_n] /=1000;		
		else pf[arg_n] /=100;
		
    //ms_cm ===> us_cm
		pf[arg_n] *=1000;
	       //pf[arg_n]=134.0;
		strcpy(acq_data->polcode[arg_n],"463"); 
		acq_data->unit[arg_n]=UNIT_US_CM;
		arg_n ++;


    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"463"); 
		acq_data->unit[arg_n]=UNIT_US_CM;
		arg_n++;
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"cond data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }
	
#if 0	
   //=temperature
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x4,0x04,0x1);
   //printf("size %d temperature data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water temperature : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d temperature data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   
   if( (size >= 7)&&(com_rbuf[0] == 0x01) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	       f.s += com_rbuf[4];
                 
		pf[arg_n] = f.s;     	
		pf[arg_n] /=100;
		
		strcpy(acq_data->polcode[arg_n],"301"); 
		acq_data->unit[arg_n]=UNIT_0C;
		arg_n ++;
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"301"); 
		acq_data->unit[arg_n]=UNIT_0C;
		arg_n++;
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"temp data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }
#endif

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;
}


