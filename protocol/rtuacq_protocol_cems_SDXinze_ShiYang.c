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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.10.29 Fri.
Description:protocol_CEMS_HBHeFengCEMS
TX:01 03 00 02 00 02 45 C5 
RX:01 03 28
44 B4 08 00
A8 D7

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/

int protocol_CEMS_SDXinZe_ShiYang(struct acquisition_data *acq_data)
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
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	int val=0;
	float valf = 0;
	float flue_gas_temperature = 0,flue_gas_pressure = 0,flue_gas_speed = 0,flue_gas_humidity = 0, smoke = 0,O2 = 0;
	
	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x04;
	regpos = 0x02;
	regcnt = 0x02;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SDXinZe_ShiYang CEMS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SDXinZe_ShiYang CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SDXinZe_ShiYang CEMS data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SDXinZe_ShiYang CEMS RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		flue_gas_humidity = getFloatValue(p, 3, dataType);

		flue_gas_humidity=gas_ppm_to_pecent(flue_gas_humidity);
		
		status=0;
	}
	else
	{
		flue_gas_temperature =0 ;
		
		status=1;
	}

	
	sleep(1);
	cmd = 0x04;
	regpos = 0x06;
	regcnt = 0x02;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SDXinZe_ShiYang CEMS SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SDXinZe_ShiYang CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SDXinZe_ShiYang CEMS data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SDXinZe_ShiYang CEMS RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		O2 = getFloatValue(p, 3, dataType);
		O2=gas_ppm_to_pecent(O2);
		
		status=0;
	}
	else
	{
		O2 =0 ;
		
		status=1;
	}


	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,flue_gas_humidity,&arg_n);

	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,O2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,O2,&arg_n);


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
