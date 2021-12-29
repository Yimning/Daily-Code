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

#include "rtuacq_protocol_error_cache.h"


extern int protocol_CEMS_GBhjt212_XinJiang(struct acquisition_data *acq_data,int cemspol_num,char polcodestr[][20],UNIT_DECLARATION *unitstr,char cemspol[][20]);
int protocol_CEMS_GBhjt2005_XinJiang(struct acquisition_data *acq_data)
{
//24 1A 54 01 00 30 31 42 87 24 00 01 30 31 7A 40 E9 99 9A 01 00 30 32 43 52 4C CD 01 30 32 7A 40 E9 99 9A 01 00 30 33 43 52 4C CD 01 30 33 7A 40 E9 99 9A 01 53 30 31 40 E9 99 9A 01 53 30 32 43 52 4C CD 01 53 30 33 40 E9 99 9A 01 53 30 35 40 E9 99 9A 01 53 30 38 43 52 4C CD 01 42 30 32 43 52 4C CD 14
   int arg_n=0;

#define CEMSPOL_NUM 12

   char polcodestr[][20]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014"};

//#define CEMSPOL_NUM (ARRAY_SIZE(polcodestr))

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_H,UNIT_PECENT};
   
   char cemspol[][20]={" 02","02z"," 03","03z",
     " 01","01z",
     "S02","S01","S03","S08","B02","S05"};

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
    SYSLOG_DBG("protocol_CEMS_GBhjt2005 xinjiang:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
	
     arg_n=protocol_CEMS_GBhjt212_XinJiang(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);

     NEED_ERROR_CACHE(acq_data,10);//added by miles zhang

	 return arg_n;
}
