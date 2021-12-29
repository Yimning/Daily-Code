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


int protocol_COD_BJYongXing(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   int size=0;
   int arg_n=0;
   int ret=0;
   int tem=0;
   float total_cod=0; 
   char *ppack;
   
    if(!acq_data) return -1;
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJYongXing COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("BJYongXing COD data %s\n",com_rbuf);
	
	if((size>=32)&&((ppack=strstr(com_rbuf,"M-01 C"))!=NULL)&&(strstr(ppack,"mg/L")!=NULL))
	{
			ret=sscanf(ppack,"M-01 C=%f",&total_cod);
			if(ret!=1) goto ERROR;
	}
	else
	{
		ERROR:
		status=1;
		total_cod=0;
		
	}
	//printf("ppack:%s\n",com_rbuf);
	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
			
    return arg_n;
}
