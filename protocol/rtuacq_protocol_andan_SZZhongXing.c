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
int protocol_ANDAN_SZZhongXing(struct acquisition_data *acq_data)
{
	int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   float andan=0;
   int ret=0;
   if(!acq_data) return -1;

   sleep(1);

	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"Z1GRESULT\r");
	size=strlen(com_tbuf);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"zhongxin  SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("SZZhongXing ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("SZZhongXing ANDAN data %s\n",com_rbuf);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zhongxin  RECV:",com_rbuf,size);
	
	if((size>=51)&&(com_rbuf[0]=='Z')&&(com_rbuf[1]=='1'))
		{
		ret=sscanf(com_rbuf,"Z1,%f,",&andan);
		printf("ret=%d\n",ret);
		printf("andan=%f\n",andan);
		status=0;
	}
	else
		{
		andan=0;
		status=1;
		printf("error of recieve data\n");
	}
	if(ret!=1)
		{
		andan=0;
		status=1;
		printf("process data error\n");
	}
	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
