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

#include "rtuacq_protocol_error_cache.h"

/*
return : com_f arguments amount;
*/
int protocol_CEMS_BJxuedilong(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
    
   if(!acq_data) return -1;
#if 1
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0] = 0x02;
	com_tbuf[1] = 0x47;
	com_tbuf[2] = 0x00;
	com_tbuf[3] = 0x47;
	com_tbuf[4] = 0x03;
	size=5;
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   sleep(2);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("BJxuedilong CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJxuedilong CEMS data",com_rbuf,size);
    
	pf=acq_data->com_f;       
   if(size >= 66 && com_rbuf[0]==0x02 && com_rbuf[1]== 0x44
	 	&& com_rbuf[63]==0x00 && com_rbuf[65]==0x03)
   {               
		pf[arg_n] = *((float *)(&(com_rbuf[3])));
		strcpy(acq_data->polcode[arg_n],"a01012");//"S03");
		acq_data->unit[arg_n]=UNIT_0C;
		arg_n++;		
		pf[arg_n] = *((float *)(&(com_rbuf[7])));
		strcpy(acq_data->polcode[arg_n],"a01013");//"S08");
		acq_data->unit[arg_n]=UNIT_KPA;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[11])));
		strcpy(acq_data->polcode[arg_n],"a01014");//"S05");
		acq_data->unit[arg_n]=UNIT_PECENT;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[15])));
		strcpy(acq_data->polcode[arg_n],"a19001");//"S01");
		acq_data->unit[arg_n]=UNIT_PECENT;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[19])));
		strcpy(acq_data->polcode[arg_n],"a01011");//"S02");
		acq_data->unit[arg_n]=UNIT_M_S;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[23])));
		strcpy(acq_data->polcode[arg_n],"a34013");//"01");
		acq_data->unit[arg_n]=UNIT_MG_M3;
		arg_n++;
		acq_data->unit[arg_n]=UNIT_0C;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[27])));
		strcpy(acq_data->polcode[arg_n],"a34013z");//"01z");
		acq_data->unit[arg_n]=UNIT_MG_M3;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[31])));
		strcpy(acq_data->polcode[arg_n],"a21026");//"02");
		acq_data->unit[arg_n]=UNIT_MG_M3;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[35])));
		strcpy(acq_data->polcode[arg_n],"a21026z");//"02z");
		acq_data->unit[arg_n]=UNIT_MG_M3;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[39])));
		strcpy(acq_data->polcode[arg_n],"a21002");//"03");
		acq_data->unit[arg_n]=UNIT_MG_M3;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[43])));
		strcpy(acq_data->polcode[arg_n],"a21002z");//"03z");
		acq_data->unit[arg_n]=UNIT_MG_M3;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[47])));
		strcpy(acq_data->polcode[arg_n],"a21005");//"04");
		acq_data->unit[arg_n]=UNIT_MG_M3;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[51])));
		strcpy(acq_data->polcode[arg_n],"a21005z");//"04z");
		acq_data->unit[arg_n]=UNIT_MG_M3;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[55])));
		strcpy(acq_data->polcode[arg_n],"a00000");//"B02");
		acq_data->unit[arg_n]=UNIT_M3_S;
		arg_n++;
		pf[arg_n] = *((float *)(&(com_rbuf[59])));
		strcpy(acq_data->polcode[arg_n],"a00000z");//"B02z");
		acq_data->unit[arg_n]=UNIT_M3_S;
		arg_n++;

		read_system_time(acq_data->acq_tm); 
    }
    else
    {
		pf[arg_n]=0.0;
		strcpy(acq_data->polcode[arg_n],"a01012");//"S03"); 
		acq_data->unit[arg_n]=UNIT_MG_L;
		arg_n++;
		read_system_time(acq_data->acq_tm); 
		status = 1;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
#endif
    return arg_n;
}

extern int protocol_CEMS_GBhjt212(struct acquisition_data *acq_data,int cemspol_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,char cemspol[][20]);
int protocol_CEMS_BJxuedilong_fujian(struct acquisition_data *acq_data)
{
	int ret=0;
/*NOTE: test packet
##0259ST=32;CN=2011;PW=123456;MN=88888880000001;CP=&&DataTime=20191226182116;01-ZsRtd=0;02-ZsRtd=0;03-ZsRtd=0;01-Rtd=0;02-Rtd=0;03-Rtd=0;S01-Rtd=20.9595;S02-Rtd=2.40558;B02-Rtd=321518;S03-Rtd=18.4833;S05-Rtd=0.78372;S08-Rtd=-271.755;Load-Rtd=0;Nh3-scr-Rtd=4.12656&&B541 
*/

#define CEMSPOL_NUM 14

   char polcodestr[][20]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014","Nh3-scr","Load"};

//#define CEMSPOL_NUM (ARRAY_SIZE(polcodestr))

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_KPA,UNIT_M3_S,UNIT_PECENT,UNIT_KG_H,UNIT_PECENT};
   
   char cemspol[][20]={"02","02z","03","03z",
     "01","01z",
     "S02","S01","S03","S08","B02","S05","Nh3-scr","Load"};

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
    SYSLOG_DBG("protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
	
#if 0 //del by miles zhang 20191209	
    return protocol_CEMS_GBhjt212(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);	
#else
	ret=protocol_CEMS_GBhjt212_once(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);

	NEED_ERROR_CACHE(acq_data,20);

	return ret;
#endif
}

