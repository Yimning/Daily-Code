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
Create Time:2021.10.19 Tues.
Description:protocol_cems_XiKeMaiHaKe_PLC_MCS100FT
TX:01 03 00 96 00 08 A4 20
RX:01 03 10 20 23 20 24 20 25 20 26 20 27 20 28 20 29 20 30 B7 84

DataType and Analysis:
	(INT_AB)  00 10 = 16
*/

extern char CEMS_XiKeMaiHaKe_MCS100FT_STATUS;
int protocol_cems_XiKeMaiHaKe_PLC_MCS100FT_New(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 4
	char *polcode[MODBUS_POLCODE_NUM]={"a34013a","a01011","a01012","a01013"};
	UNIT_DECLARATION  polcodeUnit[MODBUS_POLCODE_NUM]={
		UNIT_MG_M3,	UNIT_M_S,	UNIT_0C,	UNIT_KPA	};	
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[500]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int devaddr=0;
	int devfun=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	float valf = 0;
	int val = 0;
	char *p=NULL;
	float valfArray[MODBUS_POLCODE_NUM]={0};
	float speed = 0,PTC = 0;
		
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;
	MODBUS_DATA_TYPE dataType;
	
	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;
	
	int i=0;
	int ret=0;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x04;
	regcnt = 0x08;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd,regpos,regcnt);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT PLC protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"XiKeMaiHaKe MCS100FT cems PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("XiKeMaiHaKe MCS100FT cems PLC protocol : CEMS PLC read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("XiKeMaiHaKe MCS100FT cems PLC protocol recv data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"XiKeMaiHaKe MCS100FT cems PLC RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
 
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			valfArray[i]=getFloatValue(p, 3+i*4, dataType);
			acqdata_set_value_flag(acq_data,polcode[i],polcodeUnit[i],valfArray[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);

			if(!strcmp(polcode[i],"a34013a"))
				acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valfArray[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);

		}

		status = 0;
	}
	else
	{
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			valfArray[i]=0;
			acqdata_set_value_flag(acq_data,polcode[i],polcodeUnit[i],valfArray[i],CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);	
		}
		status=1;
	}
 
	acqdata_set_value_flag(acq_data,"a34013z",UNIT_MG_M3,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);	
	acqdata_set_value_flag(acq_data,"a00000",UNIT_M3_S,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"a00000z",UNIT_M3_S,0,CEMS_XiKeMaiHaKe_MCS100FT_STATUS,&arg_n);

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;
}
