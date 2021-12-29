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

int protocol_CEMS_GBhjt2017_ChenduLePan(struct acquisition_data *acq_data)
{
//##0428QN=20190630144452444;ST=32;CN=2011;PW=123456;MN=888888800000020123456781;Flag=4;CP=&&DataTime=20190630141200;a21026-Rtd=1.0000,a21026-ZsRtd=2.0000,a21026-Flag=N;a21002-Rtd=3.0000,a21002-ZsRtd=4.0000,a21002-Flag=N;a34013-Rtd=5.0000,a34013-ZsRtd=6.0000,a34013-Flag=N;a01011-Rtd=7,a01011-Flag=N;a19001-Rtd=9,a19001-Flag=N;a01012-Rtd=10,a01012-Flag=N;a01013-Rtd=11,a01013-Flag=N;a00000-Rtd=12,a00000-Flag=N;a01014-Rtd=8,a01014-Flag=N&&0b40
   //static struct acquisition_data acq_data_bak;
   //static unsigned long cnt=0;
   
   int ret=0;
   int arg_n=0;


#define CEMSPOL_NUM 12

   char polcodestr[][20]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014"};

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_MG_M3,UNIT_MG_M3,
   	UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_KPA,UNIT_M3_S,UNIT_PECENT};
   
	   char cemspol[][20]={"a21026","a21026z","a21002","a21002z",
   "a34013","a34013z",
   "a01011","a19001","a01012","a01013","a00000","a01014"};

	if(!acq_data) return -1;   


    SYSLOG_DBG("protocol_CEMS_GBhjt2017:size=%d,%s,%s\n",CEMSPOL_NUM,polcodestr[0],cemspol[0]);
	
    arg_n=protocol_CEMS_GBhjt212_once(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);	  

    NEED_ERROR_CACHE(acq_data,20);

	return arg_n;
}



