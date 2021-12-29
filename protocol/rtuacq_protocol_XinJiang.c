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

static int comrbuf_cmp_XinJiang(char *com_rbuf,char **polcode,int len)
{
	int i;
	for(i=0;i<len;i++)
		if(!strncmp(com_rbuf,polcode[i],3)) return i;
	return -1;
}

int protocol_XinJiang(struct acquisition_data *acq_data,char *polcode2005HB[100],int polcode2005HB_len,
	                       char * polcode2017HB[100],int polcode2017HB_len,UNIT_DECLARATION units[100],int units_len)
{
	int status=0;
   	char com_rbuf[2048]={0}; 
   	int size=0;
   	int arg_n=0;
   	int ret=0;
   	union uf f;
	int i=0;
	float valf = 0.0;
	
	if(polcode2005HB_len<=0 ||
   	  polcode2005HB_len!=polcode2017HB_len ||
   	  polcode2005HB_len!=units_len) return -1;
	if(!polcode2005HB || !polcode2017HB || !units) return -2;

	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("XinJiang flux protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XinJiang flux data",com_rbuf,size);
	
	if(size>=12 && com_rbuf[0]==0x24 && com_rbuf[2]==0x54)
	{
		for(i=3;i<size-1;i+=8)
		{
			valf = 0.0;
			ret=comrbuf_cmp_XinJiang(&com_rbuf[i+1],polcode2005HB,polcode2005HB_len);
			if(ret != -1)
			{
				f.ch[3] = com_rbuf[i+4];
				f.ch[2] = com_rbuf[i+5];
				f.ch[1] = com_rbuf[i+6];
				f.ch[0] = com_rbuf[i+7];
				valf = f.f;
	
				acqdata_set_value(acq_data,polcode2017HB[ret],units[ret],valf,&arg_n);
			}
		}
		status = 0;
	}
	else
	{
		status = 1;
	}


	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

