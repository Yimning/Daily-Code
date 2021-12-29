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


int protocol_ANDAN_ZJHZWeiLan(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_cod=0;
   union uf f;
	if(!acq_data) return -1;
   	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x00, 0x02);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HZWeiLan andan protocol,andan : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HZWeiLan andan data",com_rbuf,size);
	
	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
		{
#if 0		
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
#else
		f.ch[0]=com_rbuf[4]; //66
		f.ch[1]=com_rbuf[3]; //c6
		f.ch[2]=com_rbuf[6]; //45  791.1=66 c6 45 44
		f.ch[3]=com_rbuf[5]; //44		
#endif
		total_cod=f.f;
		status=0;
		//printf("cod=%f\n",total_cod);
	}
	else
		{
		total_cod=0;
		status=1;
		//printf("ppack:%s\n",com_rbuf);
	}
	ret=acqdata_set_value(acq_data,"w21003",UNIT_MG_L,total_cod,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

