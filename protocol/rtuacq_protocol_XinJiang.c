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

static char * XinJiang_pack_check(char *pack, int size, int *data_size)
{
	char *p;
	int i=0;
	int index=0;
	int len=0,data_len=0;
	int sum=0;

	if(!pack || !data_size || size<=0)
		return NULL;

	*data_size=0;
	
	index=0;
	for(i=0;i<size;i=index+1)
	{
		len=size-i;
		p=(char *)(memchr(&pack[i],0x24,len));
		if(p==NULL)
			return NULL;

		index=(int)(p-pack);
		len=size-index;
		if(len<12)
			return NULL;

		if(p[2]!=0x54)
			continue;

		data_len=p[1];
		if(data_len<10)
			continue;
		
		if(len<data_len+2)
			return NULL;

		sum=SUM_CHECK_HEX(&p[2], data_len-1);
		sum=sum&0xFF;

		if(sum!=p[data_len+1])
			continue;

		*data_size=data_len+2;
		return p;
	}

	return NULL;
}

static int getXinJiangValf(char *data,int len,char *polcode,float *valf)
{
	char *p=NULL;
	int index=0;
	
	if(!data || len<0 || !polcode || !valf)
		return -1;

	p=memstr(data,len,polcode);
	if(p==NULL)
		return -2;

	index=p-data;
	if((data-index)<7)
		return -3;

	*valf=getFloatValue(p, 3, FLOAT_ABCD);

	return 0;
}

int protocol_XinJiang(struct acquisition_data *acq_data,char *polcode05[],char *polcode17[],UNIT_DECLARATION *units,int polcode_num)
{
	int status=0;
   	char com_rbuf[2048]={0}; 
   	int size=0;
   	int arg_n=0;
   	int ret=0;
   	union uf f;
	int i=0;
	float valf = 0.0;
	char *p=NULL;
	int len=0;
	
	if(polcode_num<=0) return -1;
	if(!polcode05 || !polcode17 || !units) return -2;

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=getHexDataPack(DEV_NAME(acq_data),NULL, 0, com_rbuf, sizeof(com_rbuf), "XinJiang protocol", 3);
	p=XinJiang_pack_check(com_rbuf, size, &len);
	if((p!=NULL) && (len>=12))
	{
		for(i=0;i<polcode_num;i++)
		{
			ret=getXinJiangValf(p,len,polcode05[i],&valf);
			if(ret<0)
			{
				valf=0;
			}
			SYSLOG_DBG("XinJiang protocol : %d> %s valf=%f\n",i+1,polcode17[i],valf);
			acqdata_set_value(acq_data,polcode17[i],units[i],valf,&arg_n);
		}
		status = 0;
	}
	else
	{
		for(i=0;i<polcode_num;i++)
		{
			SYSLOG_DBG("XinJiang protocol : %d> %s 0 D\n",i+1,polcode17[i]);
			acqdata_set_value(acq_data,polcode17[i],units[i],0,&arg_n);
		}
		status = 1;
	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

