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

int protocol_FLUX_KeSheng(struct acquisition_data *acq_data)
{
	int status=0;
	char *p;
	char com_rbuf[2048]={0}; 
	int size=0;
	float total_flux_M3=0,speed=0;
	int ret=0;
	int arg_n=0;
	int val=0;

	if(!acq_data) return -1;

	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("KeSheng flux protocol,flux : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"KeSheng flux RECV:",com_rbuf,size);
	p=strstr(com_rbuf,"#");
	if((size>=19)&&(p!=NULL))
	{
		sscanf(&p[3],"%d %f\n",&val,&speed);
		total_flux_M3=val;
		status=0;
	}
	else
	{
		total_flux_M3=0;
		speed=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



