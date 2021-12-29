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
int protocol_ANDAN_XiSiDi(struct acquisition_data *acq_data)
{
	int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   float andan=0;
    int ret=0;
   char *p;
   char databuf[40]={0};
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"#r\r\n");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiSiDi ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("XiSiDi ANDAN data %s\n",com_rbuf);
	
	if((size>=21)&&(com_rbuf[2]=='/')&&(com_rbuf[5]=='/'))
		{
		p=strchr(com_rbuf,':');
		if(p!=NULL)
			{
			strncpy(databuf,p,20);
			printf("%s\n",databuf);
			p=strchr(databuf,' ');
			if(p!=NULL)
				{
				ret=sscanf(p,"%f",&andan);
				status=0;
				printf("andan=%f\n",andan);
			}
		}
	}
	else
		{
		status=1;
		andan=0;
		printf("error of recieve data\n");	
	}
	if((p==NULL)||(ret!=1))
		{
		status=1;
		andan=0;
		printf("process error\n");
	}
	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
