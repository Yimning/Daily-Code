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


int protocol_MN_SongXiaSEr(struct acquisition_data *acq_data)
{
    int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[100]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float mn=0;
   char pch[50]={0};
   int tem=0;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0152101525**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=0;
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	//printf("size=%d\n",size);
	ret=songxia(com_rbuf, size, &mn, 2,&status);
	if(ret!=0)
		{
		mn=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"w20124",UNIT_MG_L,mn,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}