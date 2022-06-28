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
Create Time:2021.12.23 Thur.
Description:protocol_PLC_AnRomhXin_APT2000
DataType and Analysis:
	(FLOAT_DCBA) 
*/ 

int protocol_PLC_AnRomhXin_APT2000(struct acquisition_data *acq_data)
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
	char *p = NULL;
	float tsp=0,temp=0,press=0,speed=0,humid=0;
	unsigned int crc;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	dataType = FLOAT_DCBA ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=devaddr;
	com_tbuf[1]=0x66;
	com_tbuf[2]=0x00;
	com_tbuf[3]=0x9D;
	crc = CRC16_modbus(com_tbuf, 4);
	com_tbuf[4]=crc&0xFF;
	com_tbuf[5]=(crc>>8)&0xFF;

	size=6;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"AnRomhXin APT2000 PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnRomhXin APT2000 PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnRomhXin APT2000 PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"AnRomhXin APT2000 PLC RECV:",com_rbuf,size);
	if((size>=50)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x66)&&(com_rbuf[2]==0x2C))
	{
		temp = getFloatValue(com_rbuf, 4 , dataType);
		press = getFloatValue(com_rbuf, 8 , dataType);
		speed = getFloatValue(com_rbuf, 12 , dataType);
		status=0;
	}
	else
	{
		temp = 0;
		press = 0;
		speed = 0;
		status=1;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x04;
	regpos = 0x1F82;
	regcnt = 0x02;
	dataType = FLOAT_DCBA;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"AnRomhXin APT2000 PLC SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnRomhXin APT2000 PLC protocol,PLC1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnRomhXin APT2000 PLC data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"AnRomhXin APT2000 PLC RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		humid = getFloatValue(p, 3 , dataType);
		status=0;
	}
	else
	{
		humid = 0;
		status=1;
	}

	acqdata_set_value(acq_data,"a01012",UNIT_0C,temp,&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_KPA,press,&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,humid,&arg_n);
	
	acqdata_set_value(acq_data,"a00000",UNIT_M3_H,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_H,0,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

