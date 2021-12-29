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
int protocol_COD_SZkete(struct acquisition_data *acq_data)
{
	int status=0;
   float total_cod=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
	char data_buf[20]={0};
    
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0152101525**\r\n");
	size=strlen(com_tbuf);
	com_tbuf[size]=0x0d;
	size++;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("SZkete COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
    
	       
   if( (size >= 22)&&(strncmp(com_rbuf,"%01$RD",6)==0))
   {
   		memset(data_buf,0,sizeof(data_buf));
		strncpy(data_buf,&com_rbuf[18],8);
		total_cod = atof(data_buf)/10;

		acq_data->acq_tm[5] = HexCharToDec(com_rbuf[6]) * 10 + HexCharToDec(com_rbuf[7])+100;
		acq_data->acq_tm[4] = HexCharToDec(com_rbuf[8]) * 10 + HexCharToDec(com_rbuf[9]);
		acq_data->acq_tm[3] = HexCharToDec(com_rbuf[10]) * 10 + HexCharToDec(com_rbuf[11]);
		acq_data->acq_tm[2] = HexCharToDec(com_rbuf[12]) * 10 + HexCharToDec(com_rbuf[13]);
		acq_data->acq_tm[1] = HexCharToDec(com_rbuf[14]) * 10 + HexCharToDec(com_rbuf[15]);
		acq_data->acq_tm[0] = HexCharToDec(com_rbuf[16]) * 10 + HexCharToDec(com_rbuf[17]);       

		read_system_time(acq_data->acq_tm); 
    }
    else
    {
		total_cod = 0;
		read_system_time(acq_data->acq_tm); 
		status = 1;
    }

	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}



