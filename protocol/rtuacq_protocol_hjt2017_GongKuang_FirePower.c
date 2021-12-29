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

/*
RX:01 03 88 3F 80 00 00 40 00 00 00 40 40 00 0040 80 00 00 40 A0 00 00 40 C0 00 00 40 E0 00 00 41 00 00 00 41 10 00 00 41 20 00 00 41 30 00 00 41 40 00 00 41 50 00 00 41 60 00 00 41 70 00 00 41 80 00 00 41 88 00 00 41 90 00 00 41 98 00 00 41 A0 00 00 41 A8 00 00 41 B0 00 00 41 B8 00 00 41 C0 00 00 41 C8 00 00 41 D0 00 00 41 D8 00 00 41 E0 00 00 41 E8 00 00 41 F0 00 00 41 F8 00 00 42 00 00 00 00 06 00 0F 00 01 00 1A 8E B0
*/
int protocol_hjt2017_GongKuang_FirePower(struct acquisition_data *acq_data)
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

#define FirePower_POLCODE_NUM   13
   char polcodestr[FirePower_POLCODE_NUM][20]={"SCXBH",
                                                                                        "GLCGLHYL","FJCGLZFL","FJCRQLUJGL","TXTXHYJYL","CCSSBDCTQQHYC",
											"CCSSJDCCQRCDL","CCSSJDCCQRCDY","TLSSXSTNJYPHZ","TLSSGJLL","TLSSGLLL",
											"TLSSJYXHBYXZT","GXSJ"
											};

   UNIT_DECLARATION unitstr[FirePower_POLCODE_NUM]={UNIT_NONE,
   	                                                                                               UNIT_PECENT,UNIT_NONE,UNIT_KW,UNIT_NONE,UNIT_V,
													UNIT_A,UNIT_V,UNIT_NONE,UNIT_M3_H,UNIT_KG,
													UNIT_NONE,UNIT_NONE
													};


   char cemspol[FirePower_POLCODE_NUM][20]={"SCXBH",
                                                                                        "GLCGLHYL","FJCGLZFL","FJCRQLUJGL","TXTXHYJYL","CCSSBDCTQQHYC",
											"CCSSJDCCQRCDL","CCSSJDCCQRCDY","TLSSXSTNJYPHZ","TLSSGJLL","TLSSGLLL",
											"TLSSJYXHBYXZT","GXSJ"
											};
   

	if(!acq_data) return -1;


	   SYSLOG_DBG("protocol_CEMS_GBhjt2017:size=%d,%s,%s\n",FirePower_POLCODE_NUM,polcodestr[0],cemspol[0]);

        ret=protocol_CEMS_GBhjt212_once_flag(acq_data,FirePower_POLCODE_NUM,polcodestr,unitstr,cemspol);	
		
	NEED_ERROR_CACHE(acq_data,120);

	return ret;
}

