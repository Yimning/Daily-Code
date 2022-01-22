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

int protocol_FLUX_XinJiang(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 2	
	static char *polcode2005[POLCODE_NUM] = {"B01","B00"};
	static char *polcode2017[POLCODE_NUM] = {"w00000","w00001"};
	static UNIT_DECLARATION units[POLCODE_NUM]={UNIT_M3_H,UNIT_M3};

	int arg_n=0;
	arg_n= protocol_XinJiang(acq_data,polcode2005,polcode2017,units,POLCODE_NUM);
	
	NEED_ERROR_CACHE(acq_data,60);
	return arg_n;
#undef POLCODE_NUM
}
