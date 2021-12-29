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

int protocol_FLUX_dongguan(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	long val;
	float total=0;
	float speed=0;
	int ret=0;
	int arg_n=0;
	unsigned int addr=1;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;
	addr = modbusarg->devaddr&0xffff;

	sleep(2);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,addr,0x03,0x1010,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"dongguan flux SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("dongguan flux protocol,flux : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("dongguan flux data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"dongguan flux RECV:",com_rbuf,size);
	if((size>=17)&&(com_rbuf[0]==addr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		speed=f.f;

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		val<<=8;
		val+=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		f.ch[0]=com_rbuf[14];
		f.ch[1]=com_rbuf[13];
		f.ch[2]=com_rbuf[12];
		f.ch[3]=com_rbuf[11];
		total=val+f.f;
		
		status=0;
	}
	else
	{
		speed=0;
		total=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w10101",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w10001",UNIT_M3,total,&arg_n);
	acqdata_set_value(acq_data,"w10102",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w10002",UNIT_M3,total,&arg_n);
	acqdata_set_value(acq_data,"w10103",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w10003",UNIT_M3,total,&arg_n);
	acqdata_set_value(acq_data,"w10104",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w10004",UNIT_M3,total,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



