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
Time:2021.06.05 Sta
Description:protocol_others_YULV_LiDe
TX:01 03 00 00 00 04 44 09
RX:01 03 08  40 E8 F5 C3  42 18 9E E4  16 10
DataType and Analysis:
	(Float) 40 E8 F5 C3 = 7.28
	(Float) 42 18 9E E4 = 38.155
*/

int protocol_others_YULV_LiDe(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	float yulv=0;
	float temperature=0;
	int val=0;
	union uf f;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 0x00, 0x04);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG_HEX("LiDe yulv&temperature send data :",com_tbuf,size);	
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("LiDe yulv&temperature protocol,yulv&temperature : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LiDe yulv&temperature data ",com_rbuf,size);	

	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3] = com_rbuf[3];
		f.ch[2] = com_rbuf[4];
		f.ch[1] = com_rbuf[5];
		f.ch[0] = com_rbuf[6];
		yulv=f.f;

		f.ch[3] = com_rbuf[7];
		f.ch[2] = com_rbuf[8];
		f.ch[1] = com_rbuf[9];
		f.ch[0] = com_rbuf[10];
		temperature=f.f;
		status=0;
		
	}
	else
	{
		temperature=0;
		yulv=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w01010",UNIT_0C,temperature,&arg_n);
	acqdata_set_value(acq_data,"w21024",UNIT_MG_L,yulv,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

