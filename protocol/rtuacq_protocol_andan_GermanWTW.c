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
int protocol_ANDAN_GermanWTW(struct acquisition_data *acq_data)
{
	int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
  float andan=0;
  char databuf[50]={0};
  char *p;
  int ret=0;
   if(!acq_data) return -1;
   	/*memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"T1RCONZ");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	if((size>=32)&&(com_rbuf[2]=='.')&&(com_rbuf[5]=='.'))
		{
		p=strchr(com_rbuf,':');
		if(p!=NULL)
			{
			strncpy(databuf,p,30);
			printf("%s\n",databuf);
			p=strchr(databuf,' ');
			if(p!=NULL)
				{
				ret=sscanf(p,"%f",&andan);
				status=0;
				printf("%s\n",p);
				printf("andan=%f\n",andan);
			}
		}
	}
	else
		{
		andan=0;
		status=1;
		printf("recieve data error\n");
	}
	if((p==NULL)||(ret!=1))
		{
		andan=0;
		status=1;
		printf("process data error\n");
	}
	acqdata_set_value(acq_data,"060",UNIT_MG_L,andan,&arg_n);*/
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"T1RCONZ");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	if(strstr(com_rbuf,"TresCon>")==NULL)
	{
		sleep(5);
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	}
	
	SYSLOG_DBG("GermanWTW ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("GermanWTW ANDAN data %s\n",com_rbuf);	
	
	if(strstr(com_rbuf,"TresCon>")==NULL) goto ERROR;
	if(((p=strstr(com_rbuf,"NH4-N"))==NULL)||(strstr(p,"NH4-N")==NULL)) goto ERROR;
	p=strchr(p,':');
	if(p==NULL) goto ERROR;
	if((p=strchr(p,'-'))!=NULL) 
		{
		while((p!=NULL)&&(*p=='-')) p++;
		andan=atof(p);
		status=0;
		printf("%s\n",p);
		printf("andan=%f\n",andan);
		}
	else
		{
		ERROR:
		andan=0;
		status=1;
		printf("recieve data error\n");
	}
	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
	
}
