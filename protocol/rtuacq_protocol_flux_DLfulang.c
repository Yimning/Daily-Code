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
int protocol_FLUX_DLfulang(struct acquisition_data *acq_data)
{
    int status=0;
    float *pf;
    char com_rbuf[2048]; 
    char modpack[8];
    int size=0;
    union uf  f;
    int arg_n=0;
    
    if(!acq_data) return -1;
#if 1
    size=modbus_pack(modpack,0x01,0x04,0x1010,0x0A);
    size=write_device(DEV_NAME(acq_data),modpack,size);
    sleep(2);

    memset(com_rbuf,0,sizeof(com_rbuf));
    size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
    SYSLOG_DBG("DLfulang FLUX protocol,FLUX : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("DLfulang FLUX data",com_rbuf,size);
    
    pf=acq_data->com_f;       
    if( (size >= 25)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x04) )
    {
		f.ch[3] = com_rbuf[3];
      	f.ch[2] = com_rbuf[4];
      	f.ch[1] = com_rbuf[5];
      	f.ch[0] = com_rbuf[6];
                 
		pf[arg_n] = f.f;       
		strcpy(acq_data->polcode[arg_n],"w00000"); 
		acq_data->unit[arg_n]=UNIT_L_S;
		arg_n ++;

		f.ch[3] = com_rbuf[19];
      	f.ch[2] = com_rbuf[20];
      	f.ch[1] = com_rbuf[21];
      	f.ch[0] = com_rbuf[22];
                 
		pf[arg_n] = f.l;     
		strcpy(acq_data->polcode[arg_n],"w00001"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n ++;

		acq_data->total=f.l;
		
		read_system_time(acq_data->acq_tm);    
    }
    else
    {
		pf[arg_n] = 0.0;       
		strcpy(acq_data->polcode[arg_n],"w00000"); 
		acq_data->unit[arg_n]=UNIT_L_S;
		arg_n ++;
		pf[arg_n] = 0.0;       
		strcpy(acq_data->polcode[arg_n],"w00001"); 
		acq_data->unit[arg_n]=UNIT_M3;
		arg_n ++;		

		status = 1;
    }

	if(status == 0)
 		acq_data->acq_status = ACQ_OK;
	else 
   		acq_data->acq_status = ACQ_ERR;
#endif
 	return arg_n;
}


