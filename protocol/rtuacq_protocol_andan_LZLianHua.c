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
int protocol_ANDAN_LZLianHua(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[30]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float andan=0;
   union uf pf;
   int temp=0;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x05;
	com_tbuf[1]=0xa0;
	com_tbuf[2]=0xc8;
	com_tbuf[3]=0x11;
	com_tbuf[4]=0x00;
	com_tbuf[5]=0x03;
	com_tbuf[6]=0x00;
	com_tbuf[7]=0x00;
	com_tbuf[8]=0x00;
	com_tbuf[9]=0x81;
	com_tbuf[10]=0x50;
	com_tbuf[11]=0x0a;
	size=12;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("LZLianHua ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LZLianHua ANDAN data ",com_rbuf,size);
	
	if((size>=12)&&(com_rbuf[0]==0x05)&&(com_rbuf[1]==0xa0))
		{
			pf.ch[0]=com_rbuf[8];
			pf.ch[1]=com_rbuf[7];
			pf.ch[2]=com_rbuf[6];
			pf.ch[3]=com_rbuf[5];
			temp=pf.l;
			andan=(float)temp;
			status=0;
			printf("the cod=%f,data=%d\n",andan,pf.i);
	}
	else
		{
			status=1;
			andan=0;
			printf("data error is %s\n",com_rbuf);
	}
	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
