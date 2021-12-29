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
int protocol_FLUX_WXqiuxin(struct acquisition_data *acq_data)
{
	int status=0;
   char com_tbuf[255]; 
   char com_rbuf[2048]; 
   float *pf;
	int size=0;
   int arg_n=0;
	char *p;
    
   if(!acq_data) return -1;
#if 1
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"#0101");
	com_tbuf[5]=0x0d;
	size=6;
	
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   memset(com_rbuf,0,sizeof(com_rbuf));
   size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("wuxiqiuxin FLUX protocol,FLUX speed : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG("data:%s\n",com_rbuf);
    
   	pf=acq_data->com_f;       
	if(size > 5)
	{
  		p=strchr(com_rbuf,'=');
		if(p!=NULL)
		{
			p++;
			pf[arg_n]=atof(p);
			strcpy(acq_data->polcode[arg_n],"w00000");
			acq_data->unit[arg_n]=UNIT_L_S;
			arg_n++;
		}
		else
		{
			pf[arg_n]=0.0;
			status=1;
			goto ACQ_RET;
		}
        
       memset(com_tbuf,0,sizeof(com_tbuf));
		strcpy(com_tbuf,"#01");
		com_tbuf[3]=0x0d;
		size=4;

		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
    	sleep(2);

    	memset(com_rbuf,0,sizeof(com_rbuf));
    	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
    	SYSLOG_DBG("wuxiqiuxin FLUX protocol,FLUX count: read device %s , size=%d\n",DEV_NAME(acq_data),size);
    	SYSLOG_DBG("data:%s\n",com_rbuf);
    
    	pf=acq_data->com_f;       
    	if(size > 5)
		{
			p=strchr(com_rbuf,'=');
			if(p!=NULL)
			{
				p++;
				pf[arg_n]=atof(p);
				strcpy(acq_data->polcode[arg_n],"w00001");
				acq_data->unit[arg_n]=UNIT_M3;
				arg_n++;

				acq_data->total=atof(p);
			}
			else
			{
				pf[arg_n]=0.0;
				status=1;
				goto ACQ_RET;
			}
    	}

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
       read_system_time(acq_data->acq_tm); 
			 
        status = 1;
    }
	
ACQ_RET:
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
   		acq_data->acq_status = ACQ_ERR;
#endif

    return arg_n;
}



