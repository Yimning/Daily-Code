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


int protocol_Lin_ANDAN_GermanWTW(struct acquisition_data *acq_data)
{
/*
T1RCONZ(1 为仪表地址,仪表地址为 0 时,发送 TXRCONZ)

T1RCONZ T1=devaddr 1 ,, TXRCONZ TX=devaddr 0

*/
    int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[20]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float n=0;
   float total_lin=0,andan=0;
   char *p;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"TXRCONZ");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	//printf("size=%d\n",size);
	if((size>=32)&&(com_rbuf[2]=='.')&&(strstr(com_rbuf,"mg/l")!=NULL))
		{
		total_lin=atof(&com_rbuf[18]);
		printf("total_lin=%f\n",total_lin);
	}
	else goto ERROR;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"T1RCONZ");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	
	sleep(5);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	if((size>=32)&&(com_rbuf[2]=='.')&&(strstr(com_rbuf,"mg/l")!=NULL))
		{
		andan=atof(&com_rbuf[18]);
		printf("andan=%f\n",andan);
	}
	else 
		{
		ERROR:
			total_lin=0;
			andan=0;
			status=1;
			printf("error\n");
	}
	
	ret=acqdata_set_value(acq_data,"101",UNIT_MG_L,total_lin,&arg_n);
	ret=acqdata_set_value(acq_data,"060",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}