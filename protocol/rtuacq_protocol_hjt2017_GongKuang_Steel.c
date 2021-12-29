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

/*


*/
int protocol_hjt2017_GongKuang_Steel(struct acquisition_data *acq_data)
{
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0}; 
	int size=0;
	int val;
	union uf f;
	int ret=0;
	unsigned int devaddr=0;
	unsigned int cmd;
	int i=0;

#define STEEL_POLCODE_NUM   32
   char polcodestr[STEEL_POLCODE_NUM][20]={  "SCXBH",
                                                                                      "SJSJJGK","SJSCFH","SJZCFJSD","SJFMKBD","QTZCFJDL",
											"QTSCBLQXLL","QTBTPDYXXH","LTDY","LTFY","LTFYL",
											"LTDLGFJFL","LGLS", "LGCYL","LGYQGD","LGLL",
											"BHYRQLL","BHYYL","BHYYTWD","BHYYWWD","CCBDCCQQHYC",
											"CCDCCQECDL","CCDCCQDY","CCDCCQECDY","TLXSJPHZ","TLGJLL",
											"TLGLLL","TLJYXHGYXZT","TLSFYHFJDL","TXHYJYL","HXTYL",
											"GXSJ"
											};

   UNIT_DECLARATION unitstr[STEEL_POLCODE_NUM]={UNIT_NONE,
   	                                                                                               UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_A,
													UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_PECENT,
													UNIT_NONE,UNIT_NONE,UNIT_PECENT,UNIT_NONE,UNIT_NONE,
													UNIT_NONE,UNIT_KPA,UNIT_0C,UNIT_0C,UNIT_V,
													UNIT_A,UNIT_V,UNIT_V,UNIT_NONE,UNIT_M3_H,
													UNIT_KG,UNIT_NONE,UNIT_A,UNIT_NONE,UNIT_NONE,
													UNIT_NONE
													};
   char cemspol[STEEL_POLCODE_NUM][20]={  "SCXBH",
                                                                                      "SJSJJGK","SJSCFH","SJZCFJSD","SJFMKBD","QTZCFJDL",
											"QTSCBLQXLL","QTBTPDYXXH","LTDY","LTFY","LTFYL",
											"LTDLGFJFL","LGLS", "LGCYL","LGYQGD","LGLL",
											"BHYRQLL","BHYYL","BHYYTWD","BHYYWWD","CCBDCCQQHYC",
											"CCDCCQECDL","CCDCCQDY","CCDCCQECDY","TLXSJPHZ","TLGJLL",
											"TLGLLL","TLJYXHGYXZT","TLSFYHFJDL","TXHYJYL","HXTYL",
											"GXSJ"
											};
   

	if(!acq_data) return -1;


	   SYSLOG_DBG("protocol_CEMS_GBhjt2017:size=%d,%s,%s\n",STEEL_POLCODE_NUM,polcodestr[0],cemspol[0]);

        ret=protocol_CEMS_GBhjt212_once_flag(acq_data,STEEL_POLCODE_NUM,polcodestr,unitstr,cemspol);	
		
	NEED_ERROR_CACHE(acq_data,120);

	return ret;
}


