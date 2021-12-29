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



int protocol_SHjinji_water(struct acquisition_data *acq_data)
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

   //=ph
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x4,0x01,0x1);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);

   if( (size >= 7)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x04) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	       f.s += com_rbuf[4];
                 
		pf[arg_n] = f.s;      
		pf[arg_n] /=100;
		//pf[arg_n]=6.96;
		strcpy(acq_data->polcode[arg_n],"302"); 
		acq_data->unit[arg_n]=UNIT_PH;
		arg_n ++;
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"302"); 
		acq_data->unit[arg_n]=UNIT_PH;
		arg_n++;
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=Electric conductivity
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x4,0x03,0x1);
   //printf("size %d ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Electric conductivity : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d electric data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
  // 	    size , com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   
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
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }


    
    

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
		//pf[arg_n]=30.43;
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



   //=Dissolved Oxygen : DO
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x4,0x02,0x1);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Dissolved Oxygen : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   
   if( (size >= 7)&&(com_rbuf[0] == 0x01) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	       f.s += com_rbuf[4];
                 
		pf[arg_n] = f.s;     	
		pf[arg_n] /=100;
		//pf[arg_n]=3.3;
		strcpy(acq_data->polcode[arg_n],"315"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n ++;
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"315"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n++;
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"DO data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }


	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;


    return arg_n;
}


int protocol_SHjinji_water2(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[20]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float valuef=0;
		
   if(!acq_data) return -1;

   read_system_time(acq_data->acq_tm); 
   //pf=acq_data->com_f;       

   //=ph
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x2,0x4,0x01,0x1);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);

   if( (size >= 7)&&(com_rbuf[0] == 0x02)&&(com_rbuf[1] == 0x04) )
   {
   		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
	   acqdata_set_value(acq_data,"302b",UNIT_PH,valuef,&arg_n);
   }
   else
   {
   		acqdata_set_value(acq_data,"302b",UNIT_PH,0,&arg_n);
   		status = 1;
   }



   //=Electric conductivity
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x2,0x4,0x03,0x1);
   //printf("size %d ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Electric conductivity : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d electric data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
  // 	    size , com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   if( (size >= 7)&&(com_rbuf[0] == 0x02) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     

		printf("pf[arg_n] =%f,f.s=%.4x,%.2x\n",valuef ,f.s,com_rbuf[2]);
		if(com_rbuf[2]==0x3)
		    valuef /=100;
		else if(com_rbuf[2]==0x2)
		    valuef /=1000;		
		else valuef /=100;
    //ms_cm ===> us_cm
		valuef *=1000;

		acqdata_set_value(acq_data,"463b",UNIT_US_CM,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"463b",UNIT_US_CM,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=temperature
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x2,0x4,0x04,0x1);
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
   
   if( (size >= 7)&&(com_rbuf[0] == 0x02) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
		acqdata_set_value(acq_data,"301b",UNIT_0C,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"301b",UNIT_0C,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"temp data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=Dissolved Oxygen : DO
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x2,0x4,0x02,0x1);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Dissolved Oxygen : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   
   if( (size >= 7)&&(com_rbuf[0] == 0x02) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
		acqdata_set_value(acq_data,"315b",UNIT_MG_L,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"315b",UNIT_MG_L,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"DO data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }


	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;   
}


int protocol_SHjinji_water3(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[20]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float valuef=0;
		
   if(!acq_data) return -1;

   read_system_time(acq_data->acq_tm); 
   //pf=acq_data->com_f;       

   //=ph
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x3,0x4,0x01,0x1);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);

   if( (size >= 7)&&(com_rbuf[0] == 0x03)&&(com_rbuf[1] == 0x04) )
   {
   		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
	   acqdata_set_value(acq_data,"302c",UNIT_PH,valuef,&arg_n);
   }
   else
   {
   		acqdata_set_value(acq_data,"302c",UNIT_PH,0,&arg_n);
   		status = 1;
   }



   //=Electric conductivity
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x3,0x4,0x03,0x1);
   //printf("size %d ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Electric conductivity : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d electric data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
  // 	    size , com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   if( (size >= 7)&&(com_rbuf[0] == 0x03) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     

		printf("pf[arg_n] =%f,f.s=%.4x,%.2x\n",valuef ,f.s,com_rbuf[2]);
		if(com_rbuf[2]==0x3)
		    valuef /=100;
		else if(com_rbuf[2]==0x2)
		    valuef /=1000;		
		else valuef /=100;
    //ms_cm ===> us_cm
		valuef *=1000;

		acqdata_set_value(acq_data,"463c",UNIT_US_CM,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"463c",UNIT_US_CM,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=temperature
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x3,0x4,0x04,0x1);
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
   
   if( (size >= 7)&&(com_rbuf[0] == 0x03) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
		acqdata_set_value(acq_data,"301c",UNIT_0C,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"301c",UNIT_0C,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"temp data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=Dissolved Oxygen : DO
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x3,0x4,0x02,0x1);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Dissolved Oxygen : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   
   if( (size >= 7)&&(com_rbuf[0] == 0x03) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
		acqdata_set_value(acq_data,"315c",UNIT_MG_L,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"315c",UNIT_MG_L,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"DO data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }


	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;   
}

int protocol_SHjinji_water4(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[20]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float valuef=0;
		
   if(!acq_data) return -1;

   read_system_time(acq_data->acq_tm); 
   //pf=acq_data->com_f;       

   //=ph
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x4,0x4,0x01,0x1);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);

   if( (size >= 7)&&(com_rbuf[0] == 0x04)&&(com_rbuf[1] == 0x04) )
   {
   		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
	   acqdata_set_value(acq_data,"302d",UNIT_PH,valuef,&arg_n);
   }
   else
   {
   		acqdata_set_value(acq_data,"302d",UNIT_PH,0,&arg_n);
   		status = 1;
   }



   //=Electric conductivity
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x4,0x4,0x03,0x1);
   //printf("size %d ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Electric conductivity : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d electric data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
  // 	    size , com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   if( (size >= 7)&&(com_rbuf[0] == 0x04) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     

		printf("pf[arg_n] =%f,f.s=%.4x,%.2x\n",valuef ,f.s,com_rbuf[2]);
		if(com_rbuf[2]==0x3)
		    valuef /=100;
		else if(com_rbuf[2]==0x2)
		    valuef /=1000;		
		else valuef /=100;
    //ms_cm ===> us_cm
		valuef *=1000;

		acqdata_set_value(acq_data,"463d",UNIT_US_CM,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"463d",UNIT_US_CM,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=temperature
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x4,0x4,0x04,0x1);
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
   
   if( (size >= 7)&&(com_rbuf[0] == 0x04) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
		acqdata_set_value(acq_data,"301d",UNIT_0C,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"301d",UNIT_0C,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"temp data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=Dissolved Oxygen : DO
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x4,0x4,0x02,0x1);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Dissolved Oxygen : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   
   if( (size >= 7)&&(com_rbuf[0] == 0x04) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
		acqdata_set_value(acq_data,"315d",UNIT_MG_L,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"315d",UNIT_MG_L,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"DO data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }


	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;   
}

int protocol_SHjinji_water5(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[20]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float valuef=0;
		
   if(!acq_data) return -1;

   read_system_time(acq_data->acq_tm); 
   //pf=acq_data->com_f;       

   //=ph
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x5,0x4,0x01,0x1);
   //printf("size %d ph data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water ph : read device %s , size=%d\n",DEV_NAME(acq_data),size);

   if( (size >= 7)&&(com_rbuf[0] == 0x05)&&(com_rbuf[1] == 0x04) )
   {
   		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
	   acqdata_set_value(acq_data,"302e",UNIT_PH,valuef,&arg_n);
   }
   else
   {
   		acqdata_set_value(acq_data,"302e",UNIT_PH,0,&arg_n);
   		status = 1;
   }



   //=Electric conductivity
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x5,0x4,0x03,0x1);
   //printf("size %d ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size , com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Electric conductivity : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d electric data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
  // 	    size , com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   if( (size >= 7)&&(com_rbuf[0] == 0x05) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     

		printf("pf[arg_n] =%f,f.s=%.4x,%.2x\n",valuef ,f.s,com_rbuf[2]);
		if(com_rbuf[2]==0x3)
		    valuef /=100;
		else if(com_rbuf[2]==0x2)
		    valuef /=1000;		
		else valuef /=100;
    //ms_cm ===> us_cm
		valuef *=1000;

		acqdata_set_value(acq_data,"463e",UNIT_US_CM,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"463e",UNIT_US_CM,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"ec data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=temperature
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x5,0x4,0x04,0x1);
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
   
   if( (size >= 7)&&(com_rbuf[0] == 0x05) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
		acqdata_set_value(acq_data,"301e",UNIT_0C,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"301e",UNIT_0C,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"temp data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }

   //=Dissolved Oxygen : DO
   memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x5,0x4,0x02,0x1);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_tbuf[0],com_tbuf[1],com_tbuf[2],com_tbuf[3],com_tbuf[4],com_tbuf[5],com_tbuf[6],com_tbuf[7],com_tbuf[8]);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("protocol_SHjinji_water Dissolved Oxygen : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
   SYSLOG_DBG("data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   //printf("size %d do data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   //	    size,com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
   
   if( (size >= 7)&&(com_rbuf[0] == 0x05) )
   {
		f.s = com_rbuf[3];
		f.s <<=8;
    	f.s += com_rbuf[4];
                 
		valuef = f.s;     	
		valuef /=100;
		acqdata_set_value(acq_data,"315e",UNIT_MG_L,valuef,&arg_n);
    }
    else
    {
		acqdata_set_value(acq_data,"315e",UNIT_MG_L,0,&arg_n);
		status = 1;
		write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"DO data=%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
   	    com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
    }


	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
 		//acq_data->acq_status = ACQ_OK;
 		
    return arg_n;   
}



