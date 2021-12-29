#include <unistd.h>
#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"



int protocol_watermeter_SM_10H_in(struct acquisition_data *acq_data)
{// value=1234.56
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

   //=wmin
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x3,0x00,0x4);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("SM_10H in : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);

   if( (size >= 12)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x03)&&(com_rbuf[2] == 0x08)  )
   {
		f.f = BCD(com_rbuf[3]);
		f.f *=100;
    	f.f += BCD(com_rbuf[4]);
		pf[arg_n] = f.f;      
		
		f.f=BCD(com_rbuf[5]);
		f.f/=100;
		pf[arg_n] +=f.f;
		
		strcpy(acq_data->polcode[arg_n],"wmin"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n ++;
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"wmin"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n++;
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"wmin data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;
}

int protocol_watermeter_SM_10H_in_6integers(struct acquisition_data *acq_data)
{//  value=123456.78
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

   //=wmin
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x3,0x00,0x4);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("SM_10H in : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);

   if( (size >= 13)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x03)&&(com_rbuf[2] == 0x08)  )
   {
		f.f = BCD(com_rbuf[3]);
		f.f *=10000;
		pf[arg_n] = f.f; 
    	f.f = BCD(com_rbuf[4]);
		f.f*=100;
		pf[arg_n]+=f.f;
		f.f = BCD(com_rbuf[5]);
		pf[arg_n] += f.f;      
		
		f.f=BCD(com_rbuf[6]);
		f.f/=100;
		pf[arg_n] +=f.f;
		
		strcpy(acq_data->polcode[arg_n],"wmin"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n ++;
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"wmin"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n++;
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"wmin data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;
}


int protocol_watermeter_SM_10H_out(struct acquisition_data *acq_data)
{// 1234.56
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

   //=wmout
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x2,0x3,0x00,0x4);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("SM_10H in : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);

   if( (size >= 12)&&(com_rbuf[0] == 0x02)&&(com_rbuf[1] == 0x03)&&(com_rbuf[2] == 0x08)  )
   {
		f.f = BCD(com_rbuf[3]);
		f.f *=100;
    	f.f += BCD(com_rbuf[4]);
		pf[arg_n] = f.f;      
		
		f.f=BCD(com_rbuf[5]);
		f.f/=100;
		pf[arg_n] +=f.f;
		
		strcpy(acq_data->polcode[arg_n],"wmout"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n ++;
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"wmout"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n++;
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"wmout data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;
}

int protocol_watermeter_SM_10H_out_6integers(struct acquisition_data *acq_data)
{// 123456.78
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

   //=wmout
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x2,0x3,0x00,0x4);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("SM_10H in : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);

   if( (size >= 13)&&(com_rbuf[0] == 0x02)&&(com_rbuf[1] == 0x03)&&(com_rbuf[2] == 0x08)  )
   {
		f.f = BCD(com_rbuf[3]);
		f.f *=10000;
		pf[arg_n] = f.f; 
    	f.f = BCD(com_rbuf[4]);
		f.f *=100;
		pf[arg_n] += f.f;      
    	f.f = BCD(com_rbuf[5]);
		pf[arg_n] += f.f; 
		
		f.f=BCD(com_rbuf[6]);
		f.f/=100;
		pf[arg_n] +=f.f;
		
		strcpy(acq_data->polcode[arg_n],"wmout"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n ++;
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"wmout"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n++;
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"wmout data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;
}

