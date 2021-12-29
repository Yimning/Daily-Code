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

int protocol_COD_GBhjt2017(struct acquisition_data *acq_data)
{
//##0428QN=20190630144452444;ST=32;CN=2011;PW=123456;MN=888888800000020123456781;Flag=4;CP=&&DataTime=20190630141200;a21026-Rtd=1.0000,a21026-ZsRtd=2.0000,a21026-Flag=N;a21002-Rtd=3.0000,a21002-ZsRtd=4.0000,a21002-Flag=N;a34013-Rtd=5.0000,a34013-ZsRtd=6.0000,a34013-Flag=N;a01011-Rtd=7,a01011-Flag=N;a19001-Rtd=9,a19001-Flag=N;a01012-Rtd=10,a01012-Flag=N;a01013-Rtd=11,a01013-Flag=N;a00000-Rtd=12,a00000-Flag=N;a01014-Rtd=8,a01014-Flag=N&&0b40

	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   int i=0,j=0;
   int ret=0;

#define WATERPOL_NUM 17

   char polcodestr[WATERPOL_NUM][20]={"w01018"};

   UNIT_DECLARATION unitstr[WATERPOL_NUM]={UNIT_MG_L};
   
	   char waterpol[WATERPOL_NUM][20]={"w01018"};


    SYSLOG_DBG("protocol_CEMS_GBhjt2017:size=%d,%s,%s\n",WATERPOL_NUM,polcodestr[0],waterpol[0]);


    if(!acq_data) return -1; 

	
#if 0 //del by miles zhang 20191209
	return protocol_CEMS_GBhjt212(acq_data,CEMSPOL_NUM,polcodestr,unitstr,cemspol);
#else	
    ret=protocol_CEMS_GBhjt212_once(acq_data,WATERPOL_NUM,polcodestr,unitstr,waterpol);

	NEED_ERROR_CACHE(acq_data,10);

	return ret;
#endif

}

