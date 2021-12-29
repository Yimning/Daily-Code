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

int protocol_FLUX_yiwen_LS(struct acquisition_data *acq_data)
{/*TX: 0x11 
   RX: 06m02d02:05' Q=54.42L/S  M=00022561  07A1*/
    int status=0;
    char cmd=0x11;
    char com_rbuf[2048];    
    int arg_n=0;
    int size=0;
	char *ppack;
	int ret=0;
	float speed=0,total_flux_M3=0;
	int i=0;
    if(!acq_data) return -1;
/*
	size=write_device(DEV_NAME(acq_data), &cmd, sizeof(cmd));
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
    SYSLOG_DBG("yiwen protocol LS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG("data:%s\n",com_rbuf);
*/
	size=getHexDataPack(DEV_NAME(acq_data), &cmd, sizeof(cmd), com_rbuf, sizeof(com_rbuf), "YiWen flux", 1);
	
	ppack=memstr(com_rbuf,size,"Q=");
	if(ppack==NULL) goto ERROR_PROTOTOCL;
	ret=sscanf(ppack,"Q=%fL",&speed);
	if(ret != 1) goto ERROR_PROTOTOCL;

	ppack=memstr(com_rbuf,size,"M=");
	if(ppack==NULL) goto ERROR_PROTOTOCL;
	ret=sscanf(ppack,"M=%f",&total_flux_M3);
	if(ret != 1) goto ERROR_PROTOTOCL;

	status=0;
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	acq_data->total=total_flux_M3;
	if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;

ERROR_PROTOTOCL:
		acqdata_set_value(acq_data,"w00000",UNIT_L_S,0,&arg_n);
		acqdata_set_value(acq_data,"w00001",UNIT_M3,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
}




