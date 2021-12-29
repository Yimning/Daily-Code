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

int protocol_NH3_LG1100(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nh3=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int val;
	char flag=0;
	char p[1024];
	char *q=NULL;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

  	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x04;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x02,cnt);
	size=getStrDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "LG1100 NH3", 1);
	q=modbus_ascii_lrc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(q==NULL)
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(q,size,p);
		
		nh3=getFloatValue(p, 3, FLOAT_CDAB);
		nh3=gas_ppm_to_mg_m3(nh3, M_NH3);
		
		status=0;	
	}
	else
	{
		nh3=0;
		status=1;
	}
	
	acqdata_set_value(acq_data,"a21001a",UNIT_MG_M3,nh3,&arg_n);
	acqdata_set_value(acq_data,"a21001",UNIT_MG_M3,nh3,&arg_n);
	acqdata_set_value(acq_data,"a21001z",UNIT_MG_M3,nh3,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

