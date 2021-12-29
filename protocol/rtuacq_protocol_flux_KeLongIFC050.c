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

//static float total_flux;

int protocol_FLUX_KeLongIFC050(struct acquisition_data *acq_data)
{
    int status=0;
    int size=0;
    int arg_n=0;
	int i=0,j=0;
	union	uf64 f64;
	union	uf f;
	char txspeed[8]={0x01,0x04,0x75,0x32,0x00,0x02,0xCA,0x08};
	char txtotal[8]={0x01,0x04,0x75,0x38,0x00,0x04,0x6A,0x08};
	char strcache[2048]={0};
    char com_rbuf[2048]; 
	char com_tbuf[100];
	
	
    if(!acq_data) return -1;
#if 1
//===========flux speed======================
   memcpy(com_tbuf,txspeed,8);
   size=8;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);
   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   if(size<8 || size > 30) goto ERR_DATA;

   SYSLOG_DBG("KeLong IFC050 FLUX protocol,FLUX speed: read device %s , size=%d\n",DEV_NAME(acq_data),size);
   for(i=0;(i<size) && (i<500);i++)
   	 sprintf(&strcache[strlen(strcache)],"%.2x ",com_rbuf[i]);
   write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"KeLong IFC050 size=%d,%s=%s\n",size,"w00001",strcache);
    
    if( (size >= 9)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x04))
    {
        for(i=0,j=3;i<4;i++,j--)
			f.ch[j]=com_rbuf[3+i];
		
		acqdata_set_value(acq_data,"w00000",UNIT_M3_S,(float)f.f,&arg_n);
		read_system_time(acq_data->acq_tm);    
    }
    else
    {
		acqdata_set_value(acq_data,"w00000",UNIT_M3_S,(float)(0.0),&arg_n);
		status++;
    }
#endif	
	
#if 1
//===========flux total======================	
   memcpy(com_tbuf,txtotal,8);
   size=8;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);
   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   if(size<8 || size > 30) goto ERR_DATA;

   SYSLOG_DBG("KeLong IFC050 FLUX protocol,FLUX total: read device %s , size=%d\n",DEV_NAME(acq_data),size);
   for(i=0;(i<size) && (i<500);i++)
   	 sprintf(&strcache[strlen(strcache)],"%.2x ",com_rbuf[i]);
   write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"KeLong IFC050 size=%d,%s=%s\n",size,"w00001",strcache);
    
    if( (size >= 13)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x04))
    {
        for(i=0,j=7;i<8;i++,j--)
			f64.ch[j]=com_rbuf[3+i];
		
		acqdata_set_value(acq_data,"w00001",UNIT_M3,(float)f64.d,&arg_n);
		//total_flux = (float)f64.d;		
		acq_data->total=(float)f64.d;//-total_flux;
		//total_flux = (float)f64.d;	
		read_system_time(acq_data->acq_tm);    
    }
    else
    {
		acqdata_set_value(acq_data,"w00001",UNIT_M3,(float)(0.0),&arg_n);
		status++;
    }
#endif

    if(status<2 ) status=0;

	if(status == 0)
 		acq_data->acq_status = ACQ_OK;
	else 
   		acq_data->acq_status = ACQ_ERR;


 	return arg_n;

ERR_DATA:
   acqdata_set_value(acq_data,"w00000",UNIT_M3_S,(float)(0.0),&arg_n);	
   acqdata_set_value(acq_data,"w00001",UNIT_M3,(float)(0.0),&arg_n);
   acq_data->acq_status = ACQ_ERR;

	return arg_n;	
}



int protocol_FLUX_KeLongIFC050_ZhuoZheng(struct acquisition_data *acq_data)
{
    int status=0;
    int size=0;
    int arg_n=0;
	int i=0,j=0;
	union	uf64 f64;
	union	uf f;
	char txspeed[8]={0x01,0x04,0x75,0x32,0x00,0x02,0xCA,0x08};
	char txtotal[8]={0x01,0x04,0x75,0x38,0x00,0x04,0x6A,0x08};
	char strcache[2048]={0};
    char com_rbuf[2048]; 
	char com_tbuf[100];
	
	
    if(!acq_data) return -1;
#if 1
//===========flux speed======================
   memcpy(com_tbuf,txspeed,8);
   size=8;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);
   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   if(size<8 || size > 30) goto ERR_DATA;

   SYSLOG_DBG("KeLong IFC050 FLUX protocol,FLUX speed: read device %s , size=%d\n",DEV_NAME(acq_data),size);
   for(i=0;(i<size) && (i<500);i++)
   	 sprintf(&strcache[strlen(strcache)],"%.2x ",com_rbuf[i]);
   write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"KeLong IFC050 size=%d,%s=%s\n",size,"w00001",strcache);
    
    if( (size >= 9)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x04))
    {
        for(i=0,j=3;i<4;i++,j--)
			f.ch[j]=com_rbuf[3+i];

		f.f=(f.f<0)?0:f.f;//added by miles zhang for zhuozheng 20190411
		
		acqdata_set_value(acq_data,"w00000",UNIT_M3_S,(float)f.f,&arg_n);
		read_system_time(acq_data->acq_tm);    
    }
    else
    {
		acqdata_set_value(acq_data,"w00000",UNIT_M3_S,(float)(0.0),&arg_n);
		status++;
    }
#endif	
	
#if 1
//===========flux total======================	
   memcpy(com_tbuf,txtotal,8);
   size=8;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);
   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   if(size<8 || size > 30) goto ERR_DATA;

   SYSLOG_DBG("KeLong IFC050 FLUX protocol,FLUX total: read device %s , size=%d\n",DEV_NAME(acq_data),size);
   for(i=0;(i<size) && (i<500);i++)
   	 sprintf(&strcache[strlen(strcache)],"%.2x ",com_rbuf[i]);
   write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"KeLong IFC050 size=%d,%s=%s\n",size,"w00001",strcache);
    
    if( (size >= 13)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x04))
    {
        for(i=0,j=7;i<8;i++,j--)
			f64.ch[j]=com_rbuf[3+i];
		
		acqdata_set_value(acq_data,"w00001",UNIT_M3,(float)f64.d,&arg_n);
		//total_flux = (float)f64.d;		
		acq_data->total=(float)f64.d;//-total_flux;
		//total_flux = (float)f64.d;	
		read_system_time(acq_data->acq_tm);    
    }
    else
    {
		acqdata_set_value(acq_data,"w00001",UNIT_M3,(float)(0.0),&arg_n);
		status++;
    }
#endif

    if(status<2 ) status=0;

	if(status == 0)
 		acq_data->acq_status = ACQ_OK;
	else 
   		acq_data->acq_status = ACQ_ERR;


 	return arg_n;

ERR_DATA:
   acqdata_set_value(acq_data,"w00000",UNIT_M3_S,(float)(0.0),&arg_n);	
   acqdata_set_value(acq_data,"w00001",UNIT_M3,(float)(0.0),&arg_n);
   acq_data->acq_status = ACQ_ERR;

	return arg_n;	
}


