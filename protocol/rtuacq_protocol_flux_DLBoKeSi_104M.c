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
Create Time:2021.10.26 Tues.
Description:protocol_ZONGDAN_ZJHZLiQi_V2
TX:01 03 00 00 00 11 85 C6
RX:01 03 22 00 02 00 00 06 D6 00 00 00 02 22 7E 04 92 00 00 01
05 00 10 00 0B 00 14 00 01 00 01 00 13 00 42 00 09 D5 01 
DataType and Analysis:
	(LONG_ABCD) 00 00 06 D6 = 1750
*/

int protocol_FLUX_DLBoKeSi_104M(struct acquisition_data *acq_data)
{

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	int value=0;
	float speed=0;
	int total_flux_M3=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	int val = 0;
	float valf = 0;
	float org = 0;
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;
	devaddr = modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x11;
	dataType = LONG_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DLBoKeSi 104M flux SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("flux DLBoKeSi 104M protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("flux DLBoKeSi 104M protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DLBoKeSi 104M flux RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		value=getInt32Value(com_rbuf,5, LONG_ABCD);
		speed=value*0.001;
		
		value=getInt32Value(com_rbuf,9, LONG_ABCD);
		total_flux_M3=value;
		
		status=0;

		acq_data->total=total_flux_M3;
	}
	else
	{
		speed=0;
		total_flux_M3=0;
		status = 1;
	}
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

