/******************************************************************
*
*author:Ben
*Date:2018.7.12
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


int protocol_COD_WXChuangChen(struct acquisition_data *acq_data)
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
	com_tbuf[0]=0x01;
	com_tbuf[1]=0x04;
	com_tbuf[2]=0x00;
	com_tbuf[3]=0x02;
	com_tbuf[4]=0x00;
	com_tbuf[5]=0x09;
	com_tbuf[6]=0x91;
	com_tbuf[7]=0xcc;
	size=8;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf)-1);
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("WXChuangChen COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("WXChuangChen COD data",com_rbuf,size);
	
	if((com_rbuf[0]=0x01)&&(com_rbuf[1]=0x04)&&(size>=23))
		{
			pf.ch[0]=com_rbuf[7];
			pf.ch[1]=com_rbuf[8];
			pf.ch[2]=com_rbuf[9];
			pf.ch[3]=com_rbuf[10];
			total_cod=pf.f;
			printf("cod=%f\n",total_cod);
			status=0;

	}
	else {
		status=1;
		printf("error,recieved data is %s\n",com_rbuf);
		total_cod=0;
	}
	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
