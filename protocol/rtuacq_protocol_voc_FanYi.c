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

int protocol_VOCs_FanYi(struct acquisition_data *acq_data)
{
	int status=0;
	int val=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	status=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x20,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV1:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x21,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV2:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"g28001",UNIT_0C,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x22,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV3:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"g28201",UNIT_KPA,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x23,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV4:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"g20201",UNIT_M_S,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x24,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV5:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"g09001",UNIT_MG_M3,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x26,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND6:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 6: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV6:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"e70201",UNIT_A,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x28,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND7:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 7: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV7:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"e70202",UNIT_A,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x29,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND8:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 8: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV8:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"g19501",UNIT_PECENT,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x2A,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND9:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 9: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV9:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"g20101",UNIT_M3_S,valf,&arg_n);


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x2B,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"FanYi VOC SEND10:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("FanYi VOC protocol,VOC 10: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("FanYi VOC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"FanYi VOC RECV10:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val/100.0;
	}
	else
	{
		valf=0;
		status=1;
	}
	ret=acqdata_set_value(acq_data,"g20301",UNIT_KG_H,valf,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



