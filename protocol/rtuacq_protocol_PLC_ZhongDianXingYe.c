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

int protocol_PLC_ZhongDianXingYe(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	float valf[4];
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct modbus_polcode_arg *mpolcodearg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x2C;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZhongDianXingYe PLC", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf[0]=getFloatValue(p, 3, dataType);
		valf[1]=getFloatValue(p, 23, dataType);
		valf[2]=getFloatValue(p, 43, dataType);
		valf[3]=getFloatValue(p, 63, dataType);
		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		valf[2]=0;
		valf[3]=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[1],&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,valf[2],&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[3],&arg_n);
	
	acqdata_set_value(acq_data,"a00000",UNIT_M3_H,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_H,0,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else
		acq_data->acq_status = ACQ_ERR;

	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


