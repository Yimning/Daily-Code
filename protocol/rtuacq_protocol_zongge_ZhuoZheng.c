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


int protocol_ZONGGE_ZhuoZheng(struct acquisition_data *acq_data)
{
   int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x3,0x00,0x2);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("ZhuoZheng ZONGGE protocol,ZONGGE : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	//SYSLOG_DBG("data:%s\n",com_rbuf);
	print_hex(com_rbuf,size);
    
	pf=acq_data->com_f;       
   if( (size >= 9)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x03) )
   {
/*   
		f.ch[1] = com_rbuf[3];
    	f.ch[0] = com_rbuf[4];
		f.ch[3] = com_rbuf[5];
		f.ch[2] = com_rbuf[6];
*/
		f.ch[1] = com_rbuf[5];
    	f.ch[0] = com_rbuf[6];
		f.ch[3] = com_rbuf[3];
		f.ch[2] = com_rbuf[4];
	SYSLOG_DBG("ZONGGE data:%.2x,%.2x,%.2x,%.2x,%.2x,%.2x,%.2x,%.2x,%.2x,\n",
	com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],
	com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
		
		read_system_time(acq_data->acq_tm);
		acqdata_set_value(acq_data,"w20116",UNIT_MG_L,(float)(f.f),&arg_n);
    
    }
    else
    {
		read_system_time(acq_data->acq_tm);
		acqdata_set_value(acq_data,"w20116",UNIT_MG_L,(float)(0.0),&arg_n);
		status = 1;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

int protocol_ZONGGE_ZhuoZheng2(struct acquisition_data *acq_data)
{
   int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x3,0x02,0x05);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("ZhuoZheng ZONGGE protocol,ZONGGE : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	//SYSLOG_DBG("data:%s\n",com_rbuf);
	print_hex(com_rbuf,size);
    
	pf=acq_data->com_f;       
   if( (size >= 15)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x03) )
   {
/*   
		f.ch[1] = com_rbuf[3];
    	f.ch[0] = com_rbuf[4];
		f.ch[3] = com_rbuf[5];
		f.ch[2] = com_rbuf[6];
*/
		f.ch[1] = com_rbuf[5];
    	f.ch[0] = com_rbuf[6];
		f.ch[3] = com_rbuf[3];
		f.ch[2] = com_rbuf[4];

		acq_data->acq_tm[5] = 2000+com_rbuf[7];
		acq_data->acq_tm[4] = com_rbuf[8];
		acq_data->acq_tm[3] = com_rbuf[9];
		acq_data->acq_tm[2] = com_rbuf[10];
		acq_data->acq_tm[1] = com_rbuf[11];
		acq_data->acq_tm[0] = com_rbuf[12];
	SYSLOG_DBG("ZONGGE data:%.2x,%.2x,%.2x,%.2x,%.2x,%.2x,%.2x,%.2x,%.2x,\n",
	com_rbuf[0],com_rbuf[1],com_rbuf[2],com_rbuf[3],
	com_rbuf[4],com_rbuf[5],com_rbuf[6],com_rbuf[7],com_rbuf[8]);
		
		//read_system_time(acq_data->acq_tm);
		acqdata_set_value(acq_data,"w20116",UNIT_MG_L,(float)(f.f),&arg_n);
    
    }
    else
    {
		//read_system_time(acq_data->acq_tm);
		acqdata_set_value(acq_data,"w20116",UNIT_MG_L,(float)(0.0),&arg_n);
		status = 1;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}


