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

#include "rtuacq_protocol_error_cache.h"

int protocol_FLUX_TDS100(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int val=0;
	float valf=0;
	float total=0;
	float speed=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;

	dataType=FLOAT_CDAB;
	
	devaddr = modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0C;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, 0x00, cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "TDS-100 flux", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		speed=getFloatValue(p, 3, dataType);
		
		val=getInt32Value(p, 19, LONG_CDAB);
		valf=getFloatValue(p, 23, dataType);
		total=val+valf;
			
		status=0;
	}
	else
	{
		speed=0;
		total=0;
		
		status=1;

	}
	
	acqdata_set_value(acq_data,"w00000",UNIT_M3_H,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

