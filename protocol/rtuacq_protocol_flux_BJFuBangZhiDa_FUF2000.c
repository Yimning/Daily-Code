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
Create Time:2022.03.02 Wed.
Description:protocol_FLUX_BJFuBangZhiDa_FUF2000
TX:01 04 00 06 00 04 11 C8
RX:01 04 08 41 43 00 00 41 43 00 00 76 DD

DataType and Analysis:
	(FLOAT_ABCD) 41 43 00  = 12.1875
*/
int protocol_FLUX_BJFuBangZhiDa_FUF2000(struct acquisition_data *acq_data)
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
	
	cmd = 0x04;
	regpos = 0x06;
	regcnt = 0x04;
	dataType = FLOAT_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJFuBangZhiDa_FUF2000 flux SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJFuBangZhiDa_FUF2000 flux protocol :flux read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJFuBangZhiDa_FUF2000 flux protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJFuBangZhiDa_FUF2000 flux RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		speed=valf;
		
		total_flux = getFloatValue(p, 7, dataType);

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




