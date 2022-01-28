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


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.09.13 Mon.
Description:protocol_FLUX_BuLuKeSi
TX:01 03 05 0F 00 06 15 06 
RX:01 03 0C 41 43 00 00 00 00 00 02 00 00 19 64 46 B2

DataType and Analysis:
	(INT_AB) 19 64  = 6500
*/
int protocol_FLUX_BuLuKeSi(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float total_flux=0,speed=0;
	union uf f;
	unsigned long val=0;
	float valf=0;
	int devaddr;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	cmd = 0x03;
	regpos = 0x050F;
	regcnt = 0x06;
	dataType = FLOAT_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BuLuKeSi flux SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("BuLuKeSi flux protocol :flux read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BuLuKeSi flux protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BuLuKeSi flux RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		speed=valf;
		
		f.ch[3]=p[7];
		f.ch[2]=p[8];
		f.ch[1]=p[9];
		f.ch[0]=p[10];
		val=f.l*10000000;

		f.ch[3]=p[11];
		f.ch[2]=p[12];
		f.ch[1]=p[13];
		f.ch[0]=p[14];
		val+=f.l;

		//total_flux=val;
		total_flux=val*0.001;

		status=0;
	}
	else
	{
		status=1;
		total_flux=0;
		speed=0;
	}

	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);
	return arg_n;
}



