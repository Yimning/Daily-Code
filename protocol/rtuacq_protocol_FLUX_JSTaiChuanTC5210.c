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


int protocol_FLUX_JSTaiChuanTC5210(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_flux_M3=0,speed=0;
   union uf f;
   
   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int devaddr=1;

   if(!acq_data) return -1;

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;
   devaddr=modbusarg_running->devaddr;
   
	 memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 0x12, 0x04);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("flux TC5210 protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("flux TC5210 protocol",com_rbuf,size);
	  
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
		{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		speed=f.f;

        f.l=0;
		f.l=com_rbuf[7];
		f.l <<= 8;
		f.l += com_rbuf[8];
		f.l <<= 8;
		f.l += com_rbuf[9];
		f.l <<= 8;
		f.l += com_rbuf[10];
		
		total_flux_M3=f.l;
		status=0;
		printf("speed=%f,total=%f\n",speed,total_flux_M3);

		acq_data->total=total_flux_M3;
	}
	else 
		{
		total_flux_M3=0;
		speed=0;
		status=1;
		printf("error flux TC5210 recieve data\n");
	}
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

    return arg_n;
}

