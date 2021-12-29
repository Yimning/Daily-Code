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
Create Time:2021.08.25 Wed.
Description:protocol_cems_JuGuang_HeBei_PLC_2000D
TX:01 03 00 00 00 1E C5 C2
RX:01 03 3C
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
00 00 00 00
1C D5 3F 8E
1C D5 3F 8E
20 21 01 09 09 09
00 00 00 00
20 21 01 09 00 00
20 21 01 09 00 00 00 00
79 3F
DataType and Analysis:
	(FLOAT_CDAB) 1C D5 3F 8E = 1.110
*/


int protocol_cems_JuGuang_HeBei_PLC_2000D(struct acquisition_data *acq_data)
{

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
	float ai[8]={0};
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
	devfun=modbusarg_running->devfun&0xffff;
	regpos = 0x00;
	regcnt = 0x1E;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, devfun,regpos,regcnt);
	SYSLOG_DBG_HEX("JuGuang protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang cems PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang cems PLC protocol : CEMS PLC read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang cems PLC protocol recv data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang cems PLC RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		#if 0
		for(i=0;i<8;i++)
		{
			valf = getFloatValue(p, 3+i*4, dataType);
			ai[i]=valf;
		}
		#endif
		
		valf = getFloatValue(p, 3, dataType);

		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf,&arg_n);
	
		status=0;
	}
	else
	{
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;
}
