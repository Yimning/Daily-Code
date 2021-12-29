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

int protocol_sampler_KeSheng(struct acquisition_data *acq_data)
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
	char check[5];
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0xAA;
	com_tbuf[1]=0x42;
	com_tbuf[2]=0xBB;
	size=3;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KeSheng sampler SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("KeSheng sampler protocol,sampler : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("KeSheng sampler data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KeSheng sampler RECV:",com_rbuf,size);
	check[0]=0xCC;
	check[1]=0xDD;
	check[2]=0xAA;
	check[3]=0x42;
	check[4]=0x00;
	p=memstr(com_rbuf, size, check);
	if((p!=NULL) && ((size-(p-com_rbuf))>=40) && (p[39]==0xBB))
	{
		val=p[10];
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

int protocol_sampler_KeSheng_info(struct acquisition_data *acq_data)
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
	char check[5];
	char *p;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_sampler_KeSheng_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"SAMPLER",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0xAA;
	com_tbuf[1]=0x42;
	com_tbuf[2]=0xBB;
	size=3;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KeSheng SAMPLER INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("KeSheng SAMPLER INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("KeSheng SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KeSheng SAMPLER INFO RECV1:",com_rbuf,size);
	check[0]=0xCC;
	check[1]=0xDD;
	check[2]=0xAA;
	check[3]=0x42;
	check[4]=0x00;
	p=memstr(com_rbuf, size, check);
	if((p!=NULL) && ((size-(p-com_rbuf))>=40) && (p[39]==0xBB))
	{
		val=p[10];
		acqdata_set_value_flag(acq_data,"i42001",UNIT_NONE,val,INFOR_STATUS,&arg_n);


		t3=getTimeValue(BCD(p[4])+2000, BCD(p[5]), BCD(p[6]), BCD(p[7]), BCD(p[8]), BCD(p[9]));
		acqdata_set_value_flag(acq_data,"i42002",t3,0.0,INFOR_STATUS,&arg_n);


		val=p[13];
		acqdata_set_value_flag(acq_data,"i42003",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		switch(p[11])
		{
			case 0:	val=0;break;
			case 1:	val=2;break;
			case 2:	val=3;break;
			case 3:	val=6;break;
			case 4:	val=8;break;
			default:	val=8;break;
		}
		acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		switch(p[12])
		{
			case 0:	val=1;break;
			case 1:	val=2;break;
			case 2:	val=3;break;
			case 3:	val=4;break;
			case 4:	val=7;break;
			case 5:	val=5;break;
			default:	val=7;break;
		}
		acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 37,  INT_AB);
		acqdata_set_value_flag(acq_data,"i43005",UNIT_NONE,BCD(val),INFOR_ARGUMENTS,&arg_n);

		val=((p[30]>=1) && (p[30]<=25))?p[30]:0;
		acqdata_set_value_flag(acq_data,"i43006",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_sampler_KeSheng_old(struct acquisition_data *acq_data)
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
	char check[5];
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0xAA;
	com_tbuf[1]=0x40;
	com_tbuf[2]=0xBB;
	size=3;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KeSheng old sampler SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("KeSheng old sampler protocol,sampler : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("KeSheng old sampler data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KeSheng old sampler RECV:",com_rbuf,size);
	check[0]=0xCC;
	check[1]=0xDD;
	check[2]=0xAA;
	check[3]=0x00;
	p=memstr(com_rbuf, size, check);
	if((p!=NULL) && ((size-(p-com_rbuf))>=28) && (p[27]==0xBB))
	{
		val=p[4];
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

int protocol_sampler_KeSheng_old_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0,val2=0;
	unsigned int devaddr,cmd;
	union uf f;
	char check[5];
	char *p;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_sampler_KeSheng_old_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"SAMPLER",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0xAA;
	com_tbuf[1]=0x40;
	com_tbuf[2]=0xBB;
	size=3;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"KeSheng old SAMPLER INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("KeSheng old SAMPLER INFO protocol,INFO: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("KeSheng old SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"KeSheng old SAMPLER INFO RECV:",com_rbuf,size);
	check[0]=0xCC;
	check[1]=0xDD;
	check[2]=0xAA;
	check[3]=0x00;
	p=memstr(com_rbuf, size, check);
	if((p!=NULL) && ((size-(p-com_rbuf))>=28) && (p[27]==0xBB))
	{
		val=p[4];
		acqdata_set_value_flag(acq_data,"i42001",UNIT_NONE,val,INFOR_STATUS,&arg_n);


		t3=getTimeValue(BCD(p[6])+2000, BCD(p[7]), BCD(p[8]), BCD(p[9]), BCD(p[10]), BCD(p[11]));
		acqdata_set_value_flag(acq_data,"i42002",t3,0.0,INFOR_STATUS,&arg_n);


		val=!((p[21]>>6)&0x01);
		acqdata_set_value_flag(acq_data,"i42003",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		switch(p[3])
		{
			case 0:	val=7;val2=6;break;
			case 1:	val=7;val2=6;break;
			case 2:	val=7;val2=6;break;
			case 3:	val=7;val2=6;break;
			case 4:	val=7;val2=1;break;
			case 5:	val=7;val2=2;break;
			case 6:	val=0;val2=6;break;
			case 7:	val=1;val2=6;break;
			case 8:	val=7;val2=6;break;
			case 9:	val=1;val2=6;break;
			case 10:	val=0;val2=6;break;
			case 11:	val=5;val2=6;break;
			case 12:	val=4;val2=6;break;
			default:	val=7;val2=6;break;
		}
		acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,val2,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

