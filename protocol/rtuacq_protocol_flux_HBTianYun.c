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

int protocol_FLUX_HBTianYun(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	long val=0;
	float valf=0;
	float total=0;
	float speed=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,startaddr=0,cnt=0;
	char flag=0;
	char *p=NULL;
	int pos=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;

	
	devaddr = modbusarg->devaddr&0xffff;
	cmd=0x04;
	cnt=0x02;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1010,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "HBTianYun speed", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		speed=getFloatValue(p, 3, FLOAT_ABCD);
		
		flag='N';
	}
	else
	{
		speed=0;
		flag='D';
	}
	acqdata_set_value_flag(acq_data,"w00000",UNIT_M3_H,speed,flag,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1018,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "HBTianYun total", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		total=getInt32Value(p, 3, LONG_ABCD);
		
		flag='N';
	}
	else
	{
		total=0;
		flag='D';
	}
	acqdata_set_value_flag(acq_data,"w00001",UNIT_M3,total,flag,&arg_n);
	
	
	acq_data->dev_stat = 0xAA;
	acq_data->acq_status = ACQ_OK;
	
	NEED_ERROR_CACHE_ONE_POLCODE_VALUE(acq_data, arg_n, 10);
	return arg_n;
}



