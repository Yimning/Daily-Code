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
Create Time:2021.09.15 Wed.
Description:protocol_CEMS_LaJi_pack_HBZhongJieNengDCS
TX:01 03 27 10 00 18 41 BE
RX:01 03 30 
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
08 00 44 B4
B3 BB

DataType and Analysis:
	(FLOAT_CDAB)  08 00 44 B4  = 1440.25
*/

int protocol_CEMS_LaJi_pack_HBZhongJieNengDCS(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 12
	char *polcode[MODBUS_POLCODE_NUM]={"a01902","a01901","i33311","i33310","i33312","i33321"
	,"i33320","i33322","i33331","i33330","i33332","i33300"};
   	UNIT_DECLARATION polcodeUnit[1]={UNIT_0C};
	int *adrrstr[MODBUS_POLCODE_NUM]={0x00,0x06,0x0C,0x12,0x18};

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int val=0; 
	float valfArray[12]={0};
	float valf=0;
	int ret=0;
	int devaddr;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	char flag;
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	cmd = 0x10;
	//regcnt = modbusarg_running->regcnt;
	regcnt=58;
	dataType = FLOAT_CDAB ;
	/*memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LaJi_pack_HBZhongJieNengDCS CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);*/
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBZhongJieNengDCS CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBZhongJieNeng DCS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LaJi_pack_HBZhongJieNengDCS CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check_write(com_rbuf,size, devaddr, cmd, 0x00, regcnt);
	if(p!=NULL)
	{
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			valfArray[i] = getFloatValue(p, 7+i*4, dataType);
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,valfArray[i],&arg_n);
		}
#if 0
		val=p[57];
		if(val == 0)
			flag='N';
		else if((val&0x80)==0x80)
			flag='F';
		else if((val&0x04)==0x04)
			flag='D';
		else if((val&0x08)==0x08)
			flag='g';
		else if((val&0x02)==0x02)
			flag='t';
		else if((val&0x20)==0x20)
			flag='b';
		else if((val&0x40)==0x40)
			flag='b';
		else if((val&0x10)==0x10)
			flag='a';
		else if((val&0x01)==0x01)
			flag='d';
		else
			flag='N';
#else
		valf=getFloatValue(p, 55, dataType);
		val=valf;
		if(val == 0)
			flag='N';
		else if((val&0x80)==0x80)
			flag='F';
		else if((val&0x04)==0x04)
			flag='D';
		else if((val&0x08)==0x08)
			flag='g';
		else if((val&0x02)==0x02)
			flag='t';
		else if((val&0x20)==0x20)
			flag='b';
		else if((val&0x40)==0x40)
			flag='b';
		else if((val&0x10)==0x10)
			flag='a';
		else if((val&0x01)==0x01)
			flag='d';
		else
			flag='N';
#endif

		status = 0;
	}
	else
	{
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,0,&arg_n);
		}
		flag=0;
		
		status = 1;
	}

	if(status == 0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);
#undef MODBUS_POLCODE_NUM
	return arg_n;
}

