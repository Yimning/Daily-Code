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


int protocol_gas_thermo450i(struct acquisition_data *acq_data)
{
/*
TX:01 03 00 00 00 18 14 0f 
RX:01 03 30 ec f6 3b 5a 59 61 3c a4 35 84 3d 17 36 ea 3c ce c3 6a 3c 5b 73 7b 3c 8e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ce 3d
*/
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float valf=0; 
   union uf f;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x18);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("thermo 450i gas protocol,gas : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("thermo 450i gas data ",com_rbuf,size);
	
	if((size>=29)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]=0x03))
	{
	    int pos=0;
/*		
		f.ch[3]=com_rbuf[pos+3];
		f.ch[2]=com_rbuf[pos+4];
		f.ch[1]=com_rbuf[pos+5];
		f.ch[0]=com_rbuf[pos+6];
*/		
		f.ch[0]=com_rbuf[4+pos];
		f.ch[1]=com_rbuf[3+pos];
		f.ch[2]=com_rbuf[6+pos];
		f.ch[3]=com_rbuf[5+pos];		
		valf=f.f;
		acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,valf,&arg_n);

        pos=4;
		f.ch[0]=com_rbuf[4+pos];
		f.ch[1]=com_rbuf[3+pos];
		f.ch[2]=com_rbuf[6+pos];
		f.ch[3]=com_rbuf[5+pos];
		valf=f.f;
		acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,valf,&arg_n);

        pos=8;
		f.ch[0]=com_rbuf[4+pos];
		f.ch[1]=com_rbuf[3+pos];
		f.ch[2]=com_rbuf[6+pos];
		f.ch[3]=com_rbuf[5+pos];
		valf=f.f;
		//valf*=1000000;//an gas
		acqdata_set_value(acq_data,"a21001",UNIT_NG_M3,valf,&arg_n);

        pos=12;
		f.ch[0]=com_rbuf[4+pos];
		f.ch[1]=com_rbuf[3+pos];
		f.ch[2]=com_rbuf[6+pos];
		f.ch[3]=com_rbuf[5+pos];
		valf=f.f;		
		acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,valf,&arg_n);

        pos=16;
		f.ch[0]=com_rbuf[4+pos];
		f.ch[1]=com_rbuf[3+pos];
		f.ch[2]=com_rbuf[6+pos];
		f.ch[3]=com_rbuf[5+pos];
		valf=f.f;		
		acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,valf,&arg_n);

        pos=20;
		f.ch[0]=com_rbuf[4+pos];
		f.ch[1]=com_rbuf[3+pos];
		f.ch[2]=com_rbuf[6+pos];
		f.ch[3]=com_rbuf[5+pos];
		valf=f.f;		
		acqdata_set_value(acq_data,"a21028",UNIT_MG_M3,valf,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
		valf=0;
	    acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21001",UNIT_NG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21028",UNIT_MG_M3,0,&arg_n);
	}
	
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}






