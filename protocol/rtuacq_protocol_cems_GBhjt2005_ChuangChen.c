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


int protocol_CEMS_GBhjt2005_ChuangChen(struct acquisition_data *acq_data)
{
//##0318ST=31;CN=2011;PW=123456;MN=44011600300021;CP=&&DataTime=20190821113000;S01-Rtd=20.70,S01-Flag=N;S02-Rtd=11,S02-Flag=N;S03-Rtd=28.36,S03-Flag=N;S05-Rtd=2.30,S05-Flag=N;S08-Rtd=0.3,S08-Flag=N;01-Rtd=1,01-ZsRtd=12,01-Flag=N;02-Rtd=2,02-ZsRtd=20,02-Flag=N;03-Rtd=1.75,03-ZsRtd=69.91,03-Flag=N;B02-Rtd=508166.63,B02-Flag=N&&59C0

   int ret=0;
   int arg_n=0;
   
#define CEMSPOL_NUM 12

   char polcodestr[][20]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014"};

//#define CEMSPOL_NUM (ARRAY_SIZE(polcodestr))

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_KPA,UNIT_M3_S,UNIT_PECENT};
   
   char cemspol[][20]={"02","02z","03","03z",
     "01","01z",
     "S02","S01","S03","S08","B02","S05"};

    if(!acq_data) return -1;

    //write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"protocol_CEMS_GBhjt2005:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
    SYSLOG_DBG("protocol_CEMS_GBhjt2005_ChuangChen:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
		
    arg_n=protocol_CEMS_GBhjt212_once(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);	 

	NEED_ERROR_CACHE(acq_data,20);

	return arg_n;
}



