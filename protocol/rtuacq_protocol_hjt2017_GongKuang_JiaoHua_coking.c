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

/*
RX2:01 03 5C 3F 80 00 00 07 E5 00 0A 00 09 00 17 00 25 00 18 07 E5 00 0A 00 09 00 17 00 1D 00 30 00 00 00 00 43 38 00 00 45 CC EC E1 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 43 7F 8F 5C 42 15 7A E1 07 E5 00 0A 00 0A 00 11 00 36 00 31 1f 17
*/
int protocol_hjt2017_GongKuang_JiaoHua_coking(struct acquisition_data *acq_data)
{
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	/*
	char com_rbuf[2048]={
	0x01 ,0x03 ,0x5C ,0x3F ,0x80 ,0x00 ,0x00 ,0x07 ,0xE5 ,0x00 
	,0x0A ,0x00 ,0x09 ,0x00 ,0x17 ,0x00 ,0x25 ,0x00 ,0x18 ,0x07 
	,0xE5 ,0x00 ,0x0A ,0x00 ,0x09 ,0x00 ,0x17 ,0x00 ,0x1D ,0x00
	,0x30 ,0x00 ,0x00 ,0x00 ,0x00 ,0x43 ,0x38 ,0x00 ,0x00 ,0x45
	,0xCC ,0xEC ,0xE1 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
	,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x43 ,0x7F ,0x8F ,0x5C ,0x42
	,0x15 ,0x7A ,0xE1 ,0x07 ,0xE5 ,0x00 ,0x0A ,0x00 ,0x0A ,0x00
	,0x11 ,0x00 ,0x36 ,0x00 ,0x31 ,0x1f ,0x17};	
	*/
	char com_tbuf[100]={0}; 

	//int size=0;
	int size=97;
	int val;
	union uf f;
	int ret=0;
	unsigned int devaddr=0;
	unsigned int cmd;
	int i=0;

#define Coking_POLCODE_NUM   17
   char polcodestr[Coking_POLCODE_NUM][20]={"SCXBH",
                                                                                       "JHGYDZMSJ","JHGYDTJSJ","JHGYDZMCDL","JHGYDTJCDL","JHGYDMQSYL",
											"CCSSBDCCQQHYC","CCSSDCCQRCDL","CCSSDCCQRCDY","TLSSXSJPHZ","TLSSGJLL",
											"TLSSGLLL","TLSSJYXHBYXZT", "TLSSSFYHFJDL","TXSSYQSD","TXSSTXHYJYL",
											"GXSJ"
											};

   UNIT_DECLARATION unitstr[Coking_POLCODE_NUM]={UNIT_NONE,
   	                                                                                               UNIT_NONE,UNIT_NONE,UNIT_A,UNIT_A,UNIT_NONE,
													UNIT_V,UNIT_A,UNIT_V,UNIT_NONE,UNIT_M3_H,
													UNIT_KG,UNIT_NONE,UNIT_A,UNIT_0C,UNIT_NONE,
													UNIT_NONE
													};


   char cemspol[Coking_POLCODE_NUM][20]={"SCXBH",
                                                                                       "JHGYDZMSJ","JHGYDTJSJ","JHGYDZMCDL","JHGYDTJCDL","JHGYDMQSYL",
											"CCSSBDCCQQHYC","CCSSDCCQRCDL","CCSSDCCQRCDY","TLSSXSJPHZ","TLSSGJLL",
											"TLSSGLLL","TLSSJYXHBYXZT", "TLSSSFYHFJDL","TXSSYQSD","TXSSTXHYJYL",
											"GXSJ"
											};
   

	if(!acq_data) return -1;


	   SYSLOG_DBG("protocol_CEMS_GBhjt2017:size=%d,%s,%s\n",Coking_POLCODE_NUM,polcodestr[0],cemspol[0]);

        ret=protocol_CEMS_GBhjt212_once_flag(acq_data,Coking_POLCODE_NUM,polcodestr,unitstr,cemspol);	
		
	NEED_ERROR_CACHE(acq_data,120);

	return ret;


}

