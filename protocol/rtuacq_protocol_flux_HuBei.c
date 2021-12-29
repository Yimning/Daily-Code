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

int protocol_FLUX_HuBei(struct acquisition_data *acq_data)
{
	int status=0;
	
	char com_tbuf[18]={0x02,0x30,0x30,0x30,0x32,
					0x32,0x31,0x30,0x35,
					0x34,0x31,0x30,0x36,
					0x42,0x30,0x42,0x32,0x03};
	
    char com_rbuf[2048];    
    int arg_n=0;
   	int size=0;
	int value = 0;
	float speed=0;
	float total_flux_M3=0;
	
    if(!acq_data) return -1;

	size=write_device(DEV_NAME(acq_data), com_tbuf, sizeof(com_tbuf));
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
    SYSLOG_DBG("HuBei protocol  : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("HuBei flux data",com_rbuf,size);

	if( size>=26 && com_rbuf[0]==0x02 && com_rbuf[size-1]==0x03 && 
		!strncmp(&com_rbuf[6],"105",3) && !strncmp(&com_rbuf[14],"106",3) )
	{
		sscanf(&com_rbuf[9],"%4x",&value);
		speed = value;
		sscanf(&com_rbuf[17],"%8x",&value);
		total_flux_M3 = value;
	}
	else
	{
		speed = 0;
		total_flux_M3 = 0;
		status =1;
	}

	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
