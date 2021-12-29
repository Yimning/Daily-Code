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


int protocol_NI_SZZhongXing(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[50]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float ni=0;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	strcpy(com_tbuf,"Z1GRESULT\r");
	size=strlen(com_tbuf);
	size=write_device(DEV_NAME(acq_data), com_tbuf,size);
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	if((size>=51)&&(com_rbuf[0]=='Z')&&(com_rbuf[1]=='1'))
		{
		ret=sscanf(com_rbuf,"Z1,%f,",&ni);
		if(ret!=1)
			{
		acq_data->acq_status = ACQ_ERR;
		acqdata_set_value(acq_data,"w20121",UNIT_MG_L,0,&arg_n);
		return -1;
		}
		printf("ni=%f\n",ni);
		status=0;
	}
	else
		{
		ni=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"w20121",UNIT_MG_L,ni,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
