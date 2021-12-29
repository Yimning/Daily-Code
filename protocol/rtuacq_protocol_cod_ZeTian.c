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

struct COD_ZeTian_info_data{
	int i12101;
	int i12102;
	int i12103;
	
	int i13003;
	int i13004;
	int i13005;
	int i13116;

	float i13007;
	float i13008;
	float i13104;
	float i13105;
	float i13106;
	float i13108;
	float i13110;
	float i13112;
	float i13119;
	float i13120;
	float i13127;
	float i13129;
	float i13130;

	time_t i13101;
	time_t i13107;
	time_t i13128;
};

static int protocol_COD_ZeTian_info_func1(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func2(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func3(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func4(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func5(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func6(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func7(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func8(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func9(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func10(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func11(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func12(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func13(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func14(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func15(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func16(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func17(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func18(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func19(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func20(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func21(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func22(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);
static int protocol_COD_ZeTian_info_func23(struct acquisition_data *acq_data,unsigned int devaddr,void *arg);


//TX:01 03 02 00 01 79 84
int protocol_COD_ZeTian(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float cod=0;
	int val=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	unsigned int crc;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD RECV:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		cod=f.f;
		
		status=0;
	}
	else
	{
		cod=0;
		status=1;
	}

#if 0
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x05,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD protocol,COD 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD RECV2:",com_rbuf,size);
	if(size>=7)
	{
		crc=CRC16_modbus(com_rbuf, size-2);
		if((com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
		{
			val=com_rbuf[3];
			val<<=8;
			val+=com_rbuf[4];
			switch(val)
			{
				case 0x0000:	acq_data->dev_stat='N';break;
				case 0x004E:	acq_data->dev_stat='N';break;
				case 0x004D:	acq_data->dev_stat='M';break;
				case 0x0053:	acq_data->dev_stat='C';break; //Update this line by Yimning 20210831
				case 0x004B:	acq_data->dev_stat='K';break;
				case 0x0054:	acq_data->dev_stat='T';break;
				case 0x0045:	acq_data->dev_stat='D';break;
				default :		acq_data->dev_stat='N';break;
			}
		}
		else
		{
			acq_data->dev_stat='N';
		}
	}
#endif 
	//Update this part by Yimning 20210913
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x03,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV1:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		switch(val)
		{
			case 0x00: acq_data->dev_stat='N';break;
			case 0x01: acq_data->dev_stat='N';break;
			case 0x02: acq_data->dev_stat='C';break;
			case 0x03: acq_data->dev_stat='M';break;
			case 0x04: acq_data->dev_stat='M';break;
			case 0x05: acq_data->dev_stat='M';break;
			case 0x06: acq_data->dev_stat='M';break;
			case 0x07: acq_data->dev_stat='M';break;
			case 0x08: acq_data->dev_stat='M';break;
			case 0x0A: acq_data->dev_stat='M';break;
			case 0x0B: acq_data->dev_stat='M';break;
			case 0x0C: acq_data->dev_stat='M';break;
			case 0x0D: acq_data->dev_stat='M';break;
			case 0x0E: acq_data->dev_stat='M';break;
			case 0x0F: acq_data->dev_stat='M';break;
			case 0x10: acq_data->dev_stat='K';break;
			case 0x11: acq_data->dev_stat='M';break;
			case 0x12: acq_data->dev_stat='M';break;
			case 0x13: acq_data->dev_stat='M';break;
			case 0x14: acq_data->dev_stat='M';break;
			case 0x15: acq_data->dev_stat='D';break;
			default: acq_data->dev_stat='N';break;
		}
	}


	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,cod,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_COD_ZeTian_info(struct acquisition_data *acq_data)
{
#define FUNC_MAX_NUM 23

	int status=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	union uf f;
	
	static struct COD_ZeTian_info_data data;
	static unsigned int num=0;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_COD_ZeTian_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	
	sleep(2);
	ret=instrument_protocols_sel(protocol_COD_ZeTian_info_func1,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,0);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func2,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,1);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func3,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,2);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func4,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,3);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func5,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,4);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func6,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,5);
	//ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func7,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,6);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func8,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,7);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func9,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,8);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func10,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,9);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func11,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,10);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func12,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,11);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func13,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,12);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func14,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,13);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func15,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,14);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func16,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,15);
	//ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func17,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,16);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func18,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,17);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func19,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,18);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func20,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,19);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func21,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,20);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func22,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,21);
	ret+=instrument_protocols_sel(protocol_COD_ZeTian_info_func23,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,22);
	
	if(ret==0 && (num%FUNC_MAX_NUM)==22)
	{
		acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,0,INFOR_ARGUMENTS,&arg_n);
	
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,data.i12101,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,data.i12102,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,data.i12103,INFOR_STATUS,&arg_n);

		//acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,data.i13003,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,data.i13004,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,data.i13005,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,data.i13116,INFOR_ARGUMENTS,&arg_n);

		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,data.i13007,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,data.i13008,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,data.i13104,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,data.i13105,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13106",UNIT_PECENT,data.i13106,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,data.i13108,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,data.i13110,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13112",UNIT_PECENT,data.i13112,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,data.i13119,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,data.i13120,INFOR_ARGUMENTS,&arg_n);
		//acqdata_set_value_flag(acq_data,"i13127",UNIT_G_L,data.i13127,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,data.i13129,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,data.i13130,INFOR_ARGUMENTS,&arg_n);

		acqdata_set_value_flag(acq_data,"i13101",data.i13101,0.0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13107",data.i13107,0.0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13128",data.i13128,0.0,INFOR_ARGUMENTS,&arg_n);

		status = 0;
	}
	else
	{
		status = 1;
	}
	
	num++;

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


static int protocol_COD_ZeTian_info_func1(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x03,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV1:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		switch(val)
		{
			case 0x00: data->i12101=0;break;
			case 0x01: data->i12101=1;break;
			case 0x02: data->i12101=5;break;
			case 0x03: data->i12101=5;break;
			case 0x04: data->i12101=5;break;
			case 0x05: data->i12101=2;break;
			case 0x06: data->i12101=5;break;
			case 0x07: data->i12101=99;break;
			case 0x08: data->i12101=2;break;
			case 0x0A: data->i12101=5;break;
			case 0x0B: data->i12101=5;break;
			case 0x0C: data->i12101=99;break;
			case 0x0D: data->i12101=99;break;
			case 0x0E: data->i12101=99;break;
			case 0x0F: data->i12101=99;break;
			case 0x10: data->i12101=6;break;
			case 0x11: data->i12101=1;break;
			case 0x12: data->i12101=99;break;
			case 0x13: data->i12101=99;break;
			case 0x14: data->i12101=4;break;
			case 0x15: data->i12101=3;break;
			default: data->i12101=99;break;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func2(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x04,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV2:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		data->i12102=val;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func3(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x46,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV3:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		switch(val)
		{
			case 0: data->i12103=0; break;
			case 1: data->i12103=1; break;
			case 2: data->i12103=4; break;
			case 3: data->i12103=2; break;
			case 4: data->i12103=3; break;
			case 5: data->i12103=99; break;
			case 6: data->i12103=99; break;
			case 7: data->i12103=7; break;
			case 8: data->i12103=5; break;
			case 9: data->i12103=3; break;
			case 10: data->i12103=7; break;
			case 11: data->i12103=8; break;
			case 12: data->i12103=99; break;
			case 13: data->i12103=99; break;
			case 14: data->i12103=99; break;
			case 15: data->i12103=99; break;
			case 16: data->i12103=6; break;
			default: data->i12103=99; break;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func4(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x26,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV4:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		data->i13116=val;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func5(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x29,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV5:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		data->i13004=val;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func6(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x2B,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND6:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 6: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV6:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		data->i13005=val;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func7(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x45,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND7:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 7: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV7:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		data->i13003=val;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func8(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x58,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND8:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 8: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV8:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13119=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func9(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x5A,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND9:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 9: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV9:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13120=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func10(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x5C,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND10:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 10: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV10:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13104=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func11(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x5E,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND11:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 11: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV11:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13105=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func12(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x60,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND12:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 12: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV12:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13108=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func13(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x62,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND13:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 13: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV13:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13110=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func14(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x79,0x3);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND14:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 14: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV14:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		timer.tm_year=com_rbuf[8]+70;
		timer.tm_mon=com_rbuf[7]-1;
		timer.tm_mday=com_rbuf[6];
		timer.tm_hour=com_rbuf[5];
		timer.tm_min=com_rbuf[4];
		timer.tm_sec=com_rbuf[3];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		data->i13101=t3;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func15(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x7F,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND15:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 15: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV15:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13106=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func16(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x81,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND16:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 16: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV16:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13112=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func17(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x83,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND17:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 17: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV17:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13127=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func18(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x85,0x3);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND18:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 18: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV18:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		timer.tm_year=com_rbuf[8]+70;
		timer.tm_mon=com_rbuf[7]-1;
		timer.tm_mday=com_rbuf[6];
		timer.tm_hour=com_rbuf[5];
		timer.tm_min=com_rbuf[4];
		timer.tm_sec=com_rbuf[3];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		data->i13128=t3;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func19(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x88,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND19:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 19: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV19:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13129=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func20(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x8A,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND20:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 20: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV20:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13130=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func21(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x30,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND21:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 21: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV21:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13008=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func22(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x32,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND22:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 22: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV22:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13007=f.f;
	}
	else
	{
		return -1;
	}
	return 0;
}

static int protocol_COD_ZeTian_info_func23(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;
	struct COD_ZeTian_info_data *data;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data || !arg)
		return -1;

	data=(struct COD_ZeTian_info_data *)arg;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x7C,0x3);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian COD INFO SEND23:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian COD INFO protocol,INFO 23: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian COD INFO RECV23:",com_rbuf,size);
	crc=CRC16_modbus(com_rbuf, size-2);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[size-2]==(crc&0xFF))&&(com_rbuf[size-1]==((crc>>8)&0xFF)))
	{
		timer.tm_year=com_rbuf[8]+70;
		timer.tm_mon=com_rbuf[7]-1;
		timer.tm_mday=com_rbuf[6];
		timer.tm_hour=com_rbuf[5];
		timer.tm_min=com_rbuf[4];
		timer.tm_sec=com_rbuf[3];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		data->i13107=t3;
	}
	else
	{
		return -1;
	}
	return 0;
}

