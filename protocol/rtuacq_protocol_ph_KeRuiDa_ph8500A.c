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

int protocol_PH_KeRuiDa_ph8500A(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float ph=0,temp=0;
	int val,i;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0,cnt=0;
	char *p=NULL;
	char ph_flag=0,temp_flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x02,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KeRuiDa PH8500A PH ", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		ph=getInt32Value(p, 3, LONG_ABCD);
		//ph*=0.001;

		val=p[7];
		switch(val)
		{
			case 0x01:	ph=ph*0.1;	break;
			case 0x02:	ph=ph*0.01;	break;
			case 0x03:	ph=ph*0.001;	break;
		}
		ph_flag='N';
	}
	else
	{
		ph=0;
		ph_flag='D';
	}

	sleep(1);
	cmd=0x03;
	cnt=0x01;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0E,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KeRuiDa PH8500A temperature ", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		temp=getUInt16Value(p, 3, INT_AB);
		temp*=0.1;

		temp_flag='N';
	}
	else
	{
		temp=0;
		temp_flag='D';
	}

	acqdata_set_value_flag(acq_data,"w01001",UNIT_PH,ph,ph_flag,&arg_n);
	acqdata_set_value_flag(acq_data,"w01010",UNIT_0C,temp,temp_flag,&arg_n);

	acq_data->dev_stat = 0xAA;
	acq_data->acq_status = ACQ_OK;
	
	NEED_ERROR_CACHE_ONE_POLCODE_VALUE(acq_data, arg_n,10);
	return arg_n;
}



