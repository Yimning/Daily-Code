/******************************************************************
*
*author:Ben
*Date:2018.7.13
*debugdate:
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


int protocol_COD_JSTianRui2(struct acquisition_data *acq_data)
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
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x02, 0x02);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("JSTianRui2 COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JSTianRui2 COD data",com_rbuf,size);
	
	if((size>=10)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
			pf.ch[0]=com_rbuf[5];
			pf.ch[1]=com_rbuf[4];
			pf.ch[2]=com_rbuf[7];
			pf.ch[3]=com_rbuf[6];
			total_cod=pf.f;
			printf("the cod is %f,%f\n",total_cod,pf.f);
			status=0;
	}
	else{
		total_cod=0;
		status=1;
		printf("error,recieve data\n");
	}
	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
