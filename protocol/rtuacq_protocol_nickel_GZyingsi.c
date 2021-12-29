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
int protocol_NICKEL_GZyingsi(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
    
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%04#RDD0048100484**");
	size=strlen(com_tbuf);
	com_tbuf[size]=0x0d;
	size++;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("GZyingsi NICKEL protocol,NICKEL : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
    
	pf=acq_data->com_f;       
   if( (size >= 24)&&(strncmp(com_rbuf,"%04$RD",6)==0))
   {               
		pf[arg_n] = (HexCharToDec(com_rbuf[20])*0x1000)+(HexCharToDec(com_rbuf[21]) * 0x100)+
			(HexCharToDec(com_rbuf[18])*0x10)+(HexCharToDec(com_rbuf[19]));
		pf[arg_n] /= 1000.0;
		strcpy(acq_data->polcode[arg_n],"028"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n ++;

		acq_data->acq_tm[0] = HexCharToDec(com_rbuf[6]) * 10 + HexCharToDec(com_rbuf[7]);
		acq_data->acq_tm[1] = HexCharToDec(com_rbuf[8]) * 10 + HexCharToDec(com_rbuf[9]);
		acq_data->acq_tm[2] = HexCharToDec(com_rbuf[10]) * 10 + HexCharToDec(com_rbuf[11]);
		acq_data->acq_tm[3] = HexCharToDec(com_rbuf[12]) * 10 + HexCharToDec(com_rbuf[13]);
		acq_data->acq_tm[4] = HexCharToDec(com_rbuf[14]) * 10 + HexCharToDec(com_rbuf[15]);
		acq_data->acq_tm[5] = HexCharToDec(com_rbuf[16]) * 10 + HexCharToDec(com_rbuf[17]) + 2000;       
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"028"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n++;
		read_rtc_time(acq_data->acq_tm); 
		status = 1;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}



