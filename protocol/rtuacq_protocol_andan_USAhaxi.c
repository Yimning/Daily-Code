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

#include "rtuacq_protocol_error_cache.h"

int protocol_ANDAN_USAhaxi(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float andan=0;
    
   if(!acq_data) return -1;
#if 0
	memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x3,0x00,0x2);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("haxi ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
    
	pf=acq_data->com_f;       
   if( (size >= 9)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x03) )
   {
		f.ch[1] = com_rbuf[3];
    	f.ch[0] = com_rbuf[4];
		f.ch[3] = com_rbuf[5];
		f.ch[2] = com_rbuf[6];
                 
		pf[arg_n] = f.f;       
		strcpy(acq_data->polcode[arg_n],"w21003"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n ++;

		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,0x01, 0x03, 0x02, 0x04);
    	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
  	 	sleep(2); 

		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("haxi ANDAN protocol,time : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG("data:%s\n",com_rbuf);      

		if( (size >= 9)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x03) )
		{
			acq_data->acq_tm[5]= hex2dec(com_rbuf[3]) * 100 + hex2dec(com_rbuf[4]) -1900;
			acq_data->acq_tm[4]= hex2dec(com_rbuf[5]);
			acq_data->acq_tm[3]= hex2dec(com_rbuf[6]);
			acq_data->acq_tm[2]= hex2dec(com_rbuf[7]);
			acq_data->acq_tm[1]= hex2dec(com_rbuf[8]);
			acq_data->acq_tm[0]= 0;        
		}
		else
		{
			read_rtc_time(acq_data->acq_tm); 
		}
    
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"w21003"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n++;
		read_rtc_time(acq_data->acq_tm); 
		status = 1;
    }
#endif
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x06);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("haxi ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("haxi ANDAN data",com_rbuf,size);	
	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
/*		
		f.ch[3] = com_rbuf[3];
        f.ch[2] = com_rbuf[4];
		f.ch[1] = com_rbuf[5];
		f.ch[0] = com_rbuf[6];
*/		
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];		
		andan=f.f;
		status=0;
		printf("andan=%f\n",andan);
	}
	else
		{
		andan=0;
		status=1;
		printf("error of recieve data\n");
	}
	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

NEED_ERROR_CACHE(acq_data,10);//added by miles zhang

    return arg_n;
}



