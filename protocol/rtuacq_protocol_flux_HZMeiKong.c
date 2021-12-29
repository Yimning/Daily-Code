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

int protocol_FLUX_HZMeiKong(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float total_flux_M3=0,speed=0;
	union uf f;
	int unitn=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x04,0x63 ,0x0C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HZMeiKong flux SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HZMeiKong flux protocol :flux read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HZMeiKong flux protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HZMeiKong flux RECV:",com_rbuf,size);
	if((size>=29)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		speed=f.f;
		f.f=0;
		f.ch[3]=com_rbuf[21];
		f.ch[2]=com_rbuf[22];
		f.ch[1]=com_rbuf[19];
		f.ch[0]=com_rbuf[20];
		total_flux_M3=f.l;
		f.l=0;
		f.ch[3]=com_rbuf[25];
		f.ch[2]=com_rbuf[26];
		f.ch[1]=com_rbuf[23];
		f.ch[0]=com_rbuf[24];
		total_flux_M3+=f.l/1000.0;

		acq_data->total=total_flux_M3;
		status=0;
	}
	else
	{
		status=1;
		total_flux_M3=0;
		speed=0;
	}

	acqdata_set_value(acq_data,"w10101",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w10001",UNIT_M3,total_flux_M3,&arg_n);
	acqdata_set_value(acq_data,"w10102",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w10002",UNIT_M3,total_flux_M3,&arg_n);
	acqdata_set_value(acq_data,"w60101",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w60001",UNIT_M3,total_flux_M3,&arg_n);
	acqdata_set_value(acq_data,"w60102",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w60002",UNIT_M3,total_flux_M3,&arg_n);
	acqdata_set_value(acq_data,"w90101",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w90001",UNIT_M3,total_flux_M3,&arg_n);
	acqdata_set_value(acq_data,"w90102",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w90002",UNIT_M3,total_flux_M3,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,30);
	return arg_n;
}



