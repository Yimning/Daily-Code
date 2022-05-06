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

#define VOC_POLCODE_NUM 20

int protocol_VOCs_SDHaiHui(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float val=0;
	int ret=0;
	int arg_n=0;
	char *p=NULL;
	int i=0;
	char *polcode[VOC_POLCODE_NUM]={"a24088","a24087","a05002","","",
									"","","","a01012","a01013",
									"a01011","a01014","","a00000","a19001",
									"","","","","a01006"};
	UNIT_DECLARATION unitstr[VOC_POLCODE_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
												UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_0C,UNIT_KPA,
												UNIT_M_S,UNIT_PECENT,UNIT_PA,UNIT_M3_S,UNIT_PECENT,
												UNIT_DU,UNIT_M_S,UNIT_0C,UNIT_PECENT,UNIT_KPA};
	

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x01,0x03,0x00,0x28);

	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SDHaiHui VOCs SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SDHaiHui VOCs protocol,VOCs : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SDHaiHui VOCs data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SDHaiHui VOCs RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, 0x01, 0x03, 0x28);
	if(p!= NULL)
	{
		for(i=0;i<VOC_POLCODE_NUM;i++)
		{
			if(!strcmp(polcode[i],""))
				continue;
			
			f.ch[0]=com_rbuf[4*i+3];
			f.ch[1]=com_rbuf[4*i+4];
			f.ch[2]=com_rbuf[4*i+5];
			f.ch[3]=com_rbuf[4*i+6];
			val=f.f;

			ret=acqdata_set_value(acq_data,polcode[i],unitstr[i],val,&arg_n);
		}
		status=0;
	}
	else
	{
		for(i=0;i<VOC_POLCODE_NUM;i++)
		{
			if(!strcmp(polcode[i],""))
				continue;
			ret=acqdata_set_value(acq_data,polcode[i],unitstr[i],0,&arg_n);
		}
		status=1;
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data,20);
	return arg_n;
}


