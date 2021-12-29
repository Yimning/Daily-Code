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


int protocol_Lin_SXZhongLv(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[40]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   char data_buff[20]={0};
   int tem=0;
   float total_lin=0; 
      if(!acq_data) return -1;
	 memset(com_tbuf,0,sizeof(com_tbuf));
	 strcpy(com_tbuf,"%01#RDD0152101525**\r\n");
	 size=strlen(com_tbuf);
	size =write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("SXZhongLv ZONGLIN protocol,ZONGLIN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("SXZhongLv ZONGLIN data %s\n",com_rbuf);
	
	if((size>=30)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0'))
		{
			strncpy(data_buff,&com_rbuf[18],8);
			tem=atoi(data_buff);
			total_lin=((float)tem)/10;
			printf("the zonglin is %f\n",total_lin);

	}
	else
		{
		status=1;
		total_lin=0;
	}
	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,total_lin,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

