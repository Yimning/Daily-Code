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


int protocol_Lin_JSTianRui1(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[40]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   static float total_lin=0;
   int tem=0;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x01;
	com_tbuf[1]=0x03;
	com_tbuf[2]=0x00;
	com_tbuf[3]=0x00;
	com_tbuf[4]=0x03;
	com_tbuf[5]=0x05;
	com_tbuf[6]=0xcb;
	size=7;
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("JSTianRui1 ZONGLIN protocol,ZONGLIN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JSTianRui1 ZONGLIN data ",com_rbuf,size);
	
	if((size>=11)&&(com_rbuf[0]==0x01)&&(com_rbuf[3]==0)&&(com_rbuf[4]==0))
		{
			if(com_rbuf[5]==0&&com_rbuf[6]==1)
				{
				tem=(int)com_rbuf[7]<<8;
				tem=(tem&0xff00)+com_rbuf[8];
				total_lin=(float)tem;
				status=0;
				printf("the cod is %f\n",total_lin);
			}

	}
	else 
		{
			total_lin=0;
			status=1;
			printf("recieve data error\n");
	}
	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,total_lin,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

