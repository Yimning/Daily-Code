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
int protocol_COD_BJhoutian(struct acquisition_data *acq_data)
{
	int status=0;
   float total_cod=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
	char data_buf[20]={0};
    
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0] = 0x23;
	com_tbuf[1] = 0x23;
	com_tbuf[2] = 0x01;
	memset(&com_tbuf[3],0xff,10);
	com_tbuf[13] = 0x11;
	com_tbuf[14] = 0x22;
	com_tbuf[15] = 0x26;
	com_tbuf[16] = 0x26;
	size=17;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(1);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("BJhoutian COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);
    
	       
   if(size>=26 && com_rbuf[0]==0x23 && com_rbuf[1]==0x23)
   {
   		f.ch[3] = com_rbuf[19];
		f.ch[2] = com_rbuf[20];
		f.ch[1] = com_rbuf[21];
		f.ch[0] = com_rbuf[22];
		total_cod = f.f;
		 
    }
    else
    {
		total_cod = 0; 
		status = 1;
    }

	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}



