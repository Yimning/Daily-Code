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
//#include "devices_mng_api.h"

/*
return : com_f arguments amount;
*/
int protocol_sampler_HuaHouTianCheng_WS_A(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int val=0;
	unsigned int devaddr,cmd;
	union uf f;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HuaHouTianCheng WS-A sampler SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaHouTianCheng WS-A sampler protocol,sampler : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaHouTianCheng WS-A sampler data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HuaHouTianCheng WS-A sampler RECV:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val = (val==0)?1:0;
		status=0;
	}
	else
	{
		val=0;
		status=1;
	}

	acqdata_set_value(acq_data,"i42001",UNIT_NONE,val,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_sampler_HuaHouTianCheng_WS_A_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr,cmd;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_sampler_HuaHouTianCheng_WS_A_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"SAMPLER",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x21);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HuaHouTianCheng WS-A  SAMPLER INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaHouTianCheng WS-A  SAMPLER INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaHouTianCheng WS-A  SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HuaHouTianCheng WS-A  SAMPLER INFO RECV1:",com_rbuf,size);
	if((size>=71)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val = (val==0)?1:0;
		acqdata_set_value_flag(acq_data,"i42001",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		timer.tm_year=com_rbuf[5]+100;
		timer.tm_mon=com_rbuf[6]-1;
		timer.tm_mday=com_rbuf[7];
		timer.tm_hour=com_rbuf[8];
		timer.tm_min=com_rbuf[9];
		timer.tm_sec=com_rbuf[10];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):0;
		acqdata_set_value_flag(acq_data,"i42002",t3,0.0,INFOR_STATUS,&arg_n);

		val=com_rbuf[17];
		val<<=8;
		val+=com_rbuf[18];
		val = (val==0)?1:0;
		acqdata_set_value_flag(acq_data,"i42003",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,12,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,11,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
		}
		
		val=com_rbuf[13];
		val<<=8;
		val+=com_rbuf[14];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,12,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,11,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
		}

		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 12:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 13:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
		}

		val=com_rbuf[65];
		val<<=8;
		val+=com_rbuf[66];
		if(val>=1 && val<=24)
			acqdata_set_value_flag(acq_data,"i43006",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i43006",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[67];
		val<<=8;
		val+=com_rbuf[68];
		acqdata_set_value_flag(acq_data,"i43005",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x64,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HuaHouTianCheng WS-A  SAMPLER INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaHouTianCheng WS-A  SAMPLER INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaHouTianCheng WS-A  SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HuaHouTianCheng WS-A  SAMPLER INFO RECV2:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		acqdata_set_value_flag(acq_data,"i43003",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		acqdata_set_value_flag(acq_data,"i43004",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

