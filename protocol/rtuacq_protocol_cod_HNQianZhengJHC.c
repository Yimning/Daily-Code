/******************************************************************
*
*author:Ben
*Date:2018.7.11
*debug_date:2018.7.13
*********************************************************************/
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


int protocol_COD_HNQianZhengJHC(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[20]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_cod;
   union uf pf;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x51;
	com_tbuf[1]=0xff;
	com_tbuf[2]=0xff;
	com_tbuf[3]=0xff;
	com_tbuf[4]=0xff;
	com_tbuf[5]=0xff;
	com_tbuf[6]=0xff;
	com_tbuf[7]=0x51;
	size=8;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	memset(com_rbuf,0,sizeof(com_rbuf));
	sleep(1);
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("HNQianZhengJHC COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HNQianZhengJHC COD data",com_rbuf,size);
	
	if((size>=11)&&(com_rbuf[0]==0x07)&&(com_rbuf[1]==0x07))
		{
		status=0;
		pf.ch[0]=com_rbuf[6];
		pf.ch[1]=com_rbuf[7];
		pf.ch[2]=com_rbuf[8];
		pf.ch[3]=com_rbuf[9];
		total_cod=pf.f;
	}
	else 
		{
		printf("recive data error\n");	
		total_cod=0;
		status=1;
	}
	
	printf("the cod is %f\n",total_cod);
	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
		{
 		acq_data->acq_status = ACQ_ERR;

		}

    return arg_n;
}