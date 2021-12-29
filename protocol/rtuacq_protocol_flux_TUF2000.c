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

int protocol_FLUX_TUF2000(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float total_flux_M3=0,speed=0;
	union uf f;
	int unitn=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	sleep(2);
	memset(com_tbuf,0,sizeof(com_tbuf));
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03,0x00 ,0x0C);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "TUF-2000 flux", 1);
	if((size>=29)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		speed=getFloatValue(com_rbuf, 3, FLOAT_CDAB);
		
		total_flux_M3=getInt32Value(com_rbuf, 19, LONG_CDAB);
		total_flux_M3+=getFloatValue(com_rbuf, 23, FLOAT_CDAB);

		acq_data->total=total_flux_M3;
		status=0;
	}
	else
	{
		status=1;
		total_flux_M3=0;
		speed=0;
	}

	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

