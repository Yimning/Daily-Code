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
int protocol_COD_GZyiwen(struct acquisition_data *acq_data)
{
    int status=0;
    char com_tbuf[255]; 
    char com_rbuf[2048]; 
    float *pf;
	int size=0;
    int arg_n=0;
	int i=0;
	unsigned int v=0;
	float total_cod=0;
    
    if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x02;
	com_tbuf[1]=0x11;
	com_tbuf[2]=0x11;
	com_tbuf[3]=0x03;
	size=4;
    size=write_device(DEV_NAME(acq_data),com_tbuf,size);
    sleep(1);

    memset(com_rbuf,0,sizeof(com_rbuf));
    size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
    SYSLOG_DBG("yiwen COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("yiwen COD data",com_rbuf,size);
    
    //pf=acq_data->com_f;       
    if((size >= 36)&&(com_rbuf[0]==0x02)
	&&(com_rbuf[1] == 0x11)&&(com_rbuf[35]==0x03))
 	{
  		for(i=2;i<27;i+=8)     
        {
           if((com_rbuf[i+1] > 0)&&(com_rbuf[i+2] > 0)&&(com_rbuf[i+3] > 0))
           {
               acq_data->acq_tm[5] = hex2dec(com_rbuf[i+1]);
               acq_data->acq_tm[4] = hex2dec(com_rbuf[i+2]);
               acq_data->acq_tm[3] = hex2dec(com_rbuf[i+3]);
               acq_data->acq_tm[2] = hex2dec(com_rbuf[i+4]);
               acq_data->acq_tm[1] = hex2dec(com_rbuf[i+5]);
               acq_data->acq_tm[0] = 0;

              v = com_rbuf[i+6]*0x100 + com_rbuf[i+7];
              if( (v == 0x8888)||(v == 0xcccc)||(v == 0xeeee)||(v == 0xdddd)||(v == 0xbbbb)
                 ||((v >= 0xaaa0)&&(v <= 0xaaa4)) )
              {
                 total_cod = 0.0;
                 status = 2;
              }
              else
              {
                 total_cod = v;
                 status = 0;
              }
              break;
           }
        }
     }
	else
    {
        total_cod=0.0;
        status = 2;
    }

	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
   		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}



