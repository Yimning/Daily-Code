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


int protocol_FLUX_VOTEC_VT2000(struct acquisition_data *acq_data)
{
/*shen zhen long tai xin huan bao
TX:01 03 00 00  00 04 09
RX:01 03 08 40 e8 f5 c3 42 18 9e e4 16 10

40 E8 F5 C2 = float 7.28
[3][2][1][0] = float 7.28

*/
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float speed=0,total_flux_M3=0;
   int unit=0;
   UNIT_DECLARATION funit_s,funit_f;
   union uf f;
   
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x04);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("FLUX_VOTEC protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG_HEX("FLUX_VOTEC protocol",com_rbuf,size);
	
	if((size>=13)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		speed=f.f;

		
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		total_flux_M3+=f.f;
		
		acq_data->total=total_flux_M3;
		acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
		acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
		status=0;
	}
	else
	{
		status=1;
		total_flux_M3=0;
		speed=0;
		acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
		acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	}
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

