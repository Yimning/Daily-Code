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


int protocol_VOCs_ZhuoZheng(struct acquisition_data *acq_data)
{
   int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   union uf  f;
   int arg_n=0;

   char *polcodestr[]={
    "","","","","","a24087",// 0~ 5
   	"a05002","a19001","a24088","a25002","a25003","a25005",// 11
   	"a01011","a01012","a25000","a01013","a00000","a00000z",// 17
   	"","","","","","",// 23
   	"","","","","","",// 29
   	"","a01014","","a25006","a25008","a25007",// 35
   	"","","","","","",// 41
   	"","","","","","",// 47
   	"a34013","","","","","",// 53
    "","","","",""//58
   };

   UNIT_DECLARATION unitstr[]={
   	UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_MG_M3,// 0~ 5
   	UNIT_MG_M3,UNIT_PECENT,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,// 11
   	UNIT_M_S,UNIT_0C,UNIT_MG_M3,UNIT_KPA,UNIT_M3_S,UNIT_M3_S, // 17
   	UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,// 23
   	UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,// 29
   	UNIT_NONE,UNIT_PECENT,UNIT_NONE,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,// 35
   	UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,// 41
   	UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,// 47
   	UNIT_MG_M3,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,// 53
    UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE// 58
	
   	};
   
   if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x3,0x00,0x3B);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("ZhuoZheng VOCs protocol,VOCs : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	//SYSLOG_DBG("data:%s\n",com_rbuf);
	SYSLOG_DBG_HEX(com_rbuf,size);
    
	pf=acq_data->com_f;       
   if( (size >= 123)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x03) )
   {	
        int i=0;
		read_system_time(acq_data->acq_tm);
		for(i=0;i<0x3b;i++) //0x3b=59
		{
		   unsigned short val=0;
		   val=com_rbuf[3+i*2];
		   val<<=8;
		   val+=com_rbuf[4+i*2];
		   if(strlen(polcodestr[i])>0)
		       acqdata_set_value(acq_data,polcodestr[i],unitstr[i],(float)(val),&arg_n);
		}
    
    }
    else
    {
        int i=0;
		read_system_time(acq_data->acq_tm);
		for(i=0;i<0x3b;i++)
		{
		   if(strlen(polcodestr[i])>0)
		       acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0.0,&arg_n);
		}		
		status = 1;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 30);
    return arg_n;
}


