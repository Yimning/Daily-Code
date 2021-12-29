/******************************************************************
*
*author:Ben
*Date:2018.7.14
*debug_date:2018.7.14
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


int protocol_COD_GermanWTW3520(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[50]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_cod=0;
   union uf pf;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"%01#RDD0250402508**\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("GermanWTW3520 COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("GermanWTW3520 COD data %s",com_rbuf);
	
	if((size>=28)&&(com_rbuf[0]=='%')&&(com_rbuf[1]=='0'))
		{
			pf.ch[0]=HexCharToDec(com_rbuf[18])*16+HexCharToDec(com_rbuf[19]);
			pf.ch[1]=HexCharToDec(com_rbuf[20])*16+HexCharToDec(com_rbuf[21]);
			pf.ch[2]=HexCharToDec(com_rbuf[22])*16+HexCharToDec(com_rbuf[23]);
			pf.ch[3]=HexCharToDec(com_rbuf[24])*16+HexCharToDec(com_rbuf[25]);
			total_cod=pf.f;
			status=0;
			printf("the cod is %f\n",total_cod);
	}
	else
		{
			status=1;
			total_cod=0;
			printf("error,recieve data is %s\n",com_rbuf);

	}
	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}