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
Create Time:2021.11.09 Tues.
Description:protocol_CEMS_WuXiChenChuang_PLC
TX:01 03 00 00 00 32 C4 1F
RX:01 03 28
47 ae 42 38 
80 00 C3 06 
00 00 41 84
00 00  41 84
66 66 41 a6 
14 7b 42 5a 
99 9A 41 93
70 A4 3f 9d
8b 5c 45 7f 
99 9a 41 93 
......
FF FF

DataType and Analysis:
	(FLOAT_CDAB) 08 00 44 B4   = 1440.25
*/

int protocol_CEMS_WuXiChenChuang_PLC(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 14
	char *polcode[POLCODE_NUM]={
	"a21026","a21026z","a21003","a21002","a21002z","a34013a","a34013","a34013z","a19001a",
		"a01012","a01013","a01011","a01014","a00000z"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_MG_M3,	UNIT_PECENT,
		UNIT_0C,	UNIT_KPA,	UNIT_M_S,	UNIT_PECENT,	UNIT_M3_H
	};
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int val=0;
	float valf = 0;
	int devaddr;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	float valfArray[POLCODE_NUM]={0};

	MODBUS_DATA_TYPE dataType;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x32;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"WuXiChenChuang PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("WuXiChenChuang PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("WuXiChenChuang PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"WuXiChenChuang PLC RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		for(i = 0; i < POLCODE_NUM; i++)
		{
			valfArray[i]  = getFloatValue(p, i*4+3, dataType);

			acqdata_set_value(acq_data,polcode[i],unit[i],valfArray[i],&arg_n);

		}
		valf =  getFloatValue(p, 99, dataType);
		val = (int)valf;
		
		switch(val){
			case 2: acq_data->dev_stat = 'N'; break;
			case 3: acq_data->dev_stat = 'C'; break;
			case 4: acq_data->dev_stat = 'M'; break;
			case 5: acq_data->dev_stat = 'N'; break;  //'T' int word file.
			case 6: acq_data->dev_stat = 'N'; break; //'T' int word file.
			case 7: acq_data->dev_stat = 'D'; break;
			case 8: acq_data->dev_stat = 'N'; break;
			default: acq_data->dev_stat = 'N'; break;
		}
		
		status=0;
	}
	else
	{
		for(i = 0; i < POLCODE_NUM; i++)
		{
			valfArray[i]  = 0;
			acqdata_set_value(acq_data,polcode[i],unit[i],valfArray[i],&arg_n);

		}
		status=1;
		//acq_data->dev_stat = 'V';
	}
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,valfArray[0],&arg_n);
	
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,valfArray[2],&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,valfArray[2],&arg_n);
	
	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,valfArray[3],&arg_n);
	
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,valfArray[8],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_H,valfArray[13],&arg_n);


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
