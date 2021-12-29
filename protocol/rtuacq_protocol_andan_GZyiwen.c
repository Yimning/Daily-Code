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
*return : com_f arguments amount;
*/

int protocol_ANDAN_GZyiwen(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
	float andan=0;
	unsigned int value=0;
	int i;


#if 0 
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%06#RDD1600016015**");
	size=strlen(com_tbuf);
	com_tbuf[size]=0x0d;
	size++;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("GZyiwen ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("GZyiwen ANDAN data:%s\n",com_rbuf);
    
	pf=acq_data->com_f;       
  if( (size >= 40)&&(strncmp(com_rbuf,"%06$RD",6)==0))
   {
		f.f = ((com_rbuf[33]-'0')*10)+(com_rbuf[34]-'0');
		f.f = (f.f*10)+(com_rbuf[35]-'0');
		f.f = (f.f*10)+(com_rbuf[36]-'0');
		f.f = (f.f*10)+(com_rbuf[37]-'0');
		pf[arg_n] = f.f/100.0;
		strcpy(acq_data->polcode[arg_n],"w21003"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n ++;

		acq_data->acq_tm[5] = ((com_rbuf[18]-'0')*10)+(com_rbuf[19]-'0')+2000;
		acq_data->acq_tm[4] = ((com_rbuf[20]-'0')*10)+(com_rbuf[21]-'0');
		acq_data->acq_tm[3] = ((com_rbuf[22]-'0')*10)+(com_rbuf[23]-'0');
		acq_data->acq_tm[2] = ((com_rbuf[24]-'0')*10)+(com_rbuf[25]-'0');
		acq_data->acq_tm[1] = ((com_rbuf[26]-'0')*10)+(com_rbuf[27]-'0');
		acq_data->acq_tm[0] = ((com_rbuf[28]-'0')*10)+(com_rbuf[29]-'0');		
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"w21003"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n++;
		read_system_time(acq_data->acq_tm); 
		status = 1;
    }
#endif

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0] = 0x02;
	com_tbuf[1] = 0x11;
	com_tbuf[2] = 0x11;
	com_tbuf[3] = 0x03;
	size=strlen(com_tbuf);
   	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
  	sleep(1);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("GZyiwen ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("GZyiwen ANDAN data:%s\n",com_rbuf);
    
	       
 	if( (size >= 36) && (com_rbuf[0] == 0x02) && 
		(com_rbuf[1] == 0x11) && (com_rbuf[35] == 0x03) )
  	{
		for(i=2;i<27;i+=8)
		{
			if((com_rbuf[i+1]>0)&&(com_rbuf[i+2]>0)&&(com_rbuf[i+3]>0))
			{
				acq_data->acq_tm[5] = hex2dec(com_rbuf[i+1]);
				acq_data->acq_tm[4] = hex2dec(com_rbuf[i+2]);
				acq_data->acq_tm[3] = hex2dec(com_rbuf[i+3]);
				acq_data->acq_tm[2] = hex2dec(com_rbuf[i+4]);
				acq_data->acq_tm[1] = hex2dec(com_rbuf[i+5]);
				acq_data->acq_tm[0] = 0;


				value = (com_rbuf[i+6]<<8) | com_rbuf[i+7];

				if( (value == 0x8888) || (value == 0xCCCC) || (value == 0xEEEE) || (value == 0xDDDD) ||
					(value == 0xBBBB) || ((value >= 0xAAA0) && (value <= 0xAAA4)) )
				{
					andan = 0;
					status = 2;
				}
				else
				{
					andan = value;
					status = 0;
				}
				break;
			}
		}
		
	}
	else
    	{
		andan = 0; 
		status = 1;
    	}

	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;

}
