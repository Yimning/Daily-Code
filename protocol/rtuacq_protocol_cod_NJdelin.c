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
int protocol_COD_NJdelin(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
	char data_buf[20]={0};
   int arg_n=0;
   float total_cod=0;
    
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x25;
	com_tbuf[1]=0x43;
	com_tbuf[2]=0x32;
	com_tbuf[3]=0x39;
	com_tbuf[4]=0x41;
	com_tbuf[5]=0x0d;
	size=6;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);

    memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJdelin COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJdelin data",com_rbuf,size);
	
	if((size>=24)&&(com_rbuf[0]==0x25)&&(com_rbuf[3]==0x79))
	{
	        int temp=0;
			memset(data_buf,0,sizeof(data_buf));
			strncpy(data_buf,&com_rbuf[13],13);
			temp=atoi(data_buf);
			total_cod=((float)temp)/10000;
			status=0;
			printf("cod=%d,%f\n",temp,total_cod);
	}
	else
		{
			status=1;
			total_cod=0;
			printf("error of recieve data\n");
	}
	acqdata_set_value(acq_data, "w01018",UNIT_MG_L, total_cod, &arg_n);
/*
   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("NJdelin COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
    
	pf=acq_data->com_f;       
   if(size>=24 && com_rbuf[0]==0x25 && com_rbuf[3]==0x79
	 	&& com_rbuf[4]==0x43 && com_rbuf[23]==0x0d)
   { 
   		memset(data_buf,0,sizeof(data_buf));
		memcpy(data_buf,&com_rbuf[13],6);
		pf[arg_n] = atof(data_buf);
		memset(data_buf,0,sizeof(data_buf));
		memcpy(data_buf,&com_rbuf[19],2);
		pf[arg_n] += (atof(data_buf)/100.0);
		
		strcpy(acq_data->polcode[arg_n],"011"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n ++;

		read_rtc_time(acq_data->acq_tm);       
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
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}



