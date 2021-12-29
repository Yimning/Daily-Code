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

int protocol_ZONGLIN_NJHongGuang(struct acquisition_data *acq_data)
{
#define CNT_MAX 10

	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float tp=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int di=0;
	char dev_stat='\0';
	unsigned int val=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	
	static float bak_val;
	static char bak_flag;
	static int cnt=10;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	status=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	size=modbus_pack(com_tbuf,devaddr,0x3,0x1008,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJHongGuang TP SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJHongGuang TP protocol,TP : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJHongGuang TP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang TP RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, 0x3, 0x03);
	if(p!= NULL)
	{
		f.ch[0]=p[4];
		f.ch[1]=p[3];
		f.ch[2]=p[6];
		f.ch[3]=p[5];
		tp=f.f;

		val=p[7];
		val<<=8;
		val+=p[8];
		switch(val)
		{
			case 0x0000:	dev_stat='N';break;
			case 0x4E00:	dev_stat='N';break;
			case 0x5400:	dev_stat='T';break;
			case 0x4C00:	dev_stat='T';break;
			case 0x4400:	dev_stat='D';break;
			case 0x4600:	dev_stat='V';break;
			case 0x4D00:	dev_stat='M';break;
			case 0x6C72:	dev_stat='D';break;
			case 0x6C70:	dev_stat='D';break;
			case 0x6C77:	dev_stat='D';break;
			case 0x6C73:	dev_stat='D';break;
			case 0x6274:	dev_stat='N';break;
			case 0x7363:	dev_stat='N';break;
			case 0x7261:	dev_stat='N';break;
			case 0x7073:	dev_stat='N';break;
			case 0x647A:	dev_stat='N';break;
			case 0x6473:	dev_stat='N';break;
			default :		dev_stat='D';break;
		}
	}
	else
	{
		size=read_device(DEV_DI2,(char *)(&di),4);
		SYSLOG_DBG("NJHongGuang TP DI protocol,TP DI: read device %s ,size=%d, di=%d\n",DEV_DI2,size,di);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang TP DI RECV:",&di,size);
		if(size>=4 && di==1)
		{
			dev_stat='P';
		}
		else
		{
			if(cnt>CNT_MAX)
			{
				dev_stat='V';
			}
			else
			{
				tp=bak_val;
				dev_stat=bak_flag;
				cnt++;
			}
			
		}
		
		status=1;
	}

    
	sleep(1);
	
	if(status==0 && (dev_stat == 'N' || dev_stat == 'T'))
	{
	    usleep(300000);
		memset(com_tbuf,0,sizeof(com_tbuf));
//ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
		size=modbus_pack(com_tbuf,devaddr,0x3,0x109A,0x01);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJHongGuang TP SEND2:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(2);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("NJHongGuang TP protocol,TP 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("NJHongGuang TP data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang TP RECV2:",com_rbuf,size);
		p = modbus_crc_check(com_rbuf,size, devaddr, 0x3, 0x01);
		if(p!= NULL)
		{
			val=p[3];
			val<<=8;
			val+=p[4];
			switch(val)
			{
				case 1:	dev_stat='C';break;
				case 2:	dev_stat='K';break;
				case 3:	dev_stat='v';break;
				default :	break;
			}
			
		}
		else
		{
			size=read_device(DEV_DI2,(char *)(&di),4);
			SYSLOG_DBG("NJHongGuang TP DI protocol,TP DI: read device %s ,size=%d, di=%d\n",DEV_DI2,size,di);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang TP DI RECV:",&di,size);
			if(size>=4 && di==1)
			{
				dev_stat='P';
			}
			else
			{
				if(cnt>CNT_MAX)
				{
					dev_stat='V';
				}
				else
				{
					tp=bak_val;
					dev_stat=bak_flag;
					cnt++;
				}
				
			}
		
			status=1;
		}
	}
	
	if(status==0)
	{
		bak_val=tp;
		bak_flag=dev_stat;
		cnt=0;
	}

	acqdata_set_value(acq_data,"w21011",UNIT_MG_L,tp,&arg_n);
	acq_data->dev_stat = dev_stat;

	acq_data->acq_status = ACQ_OK;

	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_water_NJHongGuang_info1(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char str[50];
	int size=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0,startaddr=0;
	char *p=NULL;
	char *polcode=NULL;
	char flag=0;
	float valf=0;
	int val=0;
	int pos=0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;

	SYSLOG_DBG("protocol_water_NJHongGuang_info1\n");
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL)
		return -2;

	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	startaddr=4228;
	cnt = 0x16;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr-1,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"NJHongGuang %s info1",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		pos=get_cmd_03_04_pos(startaddr, 4228);
		val=getUInt16Value(p, pos, INT_AB);
		switch(val)
		{
			case 0:	valf=0;break;
			case 1:	valf=1;break;
			case 2:	valf=6;break;
			case 3:	valf=99;break;
			case 4:	valf=99;break;
			case 5:	valf=3;break;
			case 6:	valf=3;break;
			case 7:	valf=99;break;
			case 8:	valf=5;break;
			case 9:	valf=5;break;
			case 10:	valf=2;break;
			case 11:	valf=99;break;
			case 12:	valf=99;break;
			case 19:	valf=5;break;
			case 20:	valf=3;break;
			default:	valf=99;break;
		}
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4250);
		val=getUInt16Value(p, pos, INT_AB);
		valf=val;
		acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4230);
		val=getUInt16Value(p, pos, INT_AB);
		switch(val)
		{
			case 0:	valf=0;break;
			case 1:	valf=1;break;
			case 2:	valf=4;break;
			case 3:	valf=2;break;
			case 4:	valf=3;break;
			case 5:	valf=99;break;
			case 8:	valf=5;break;
			case 10:	valf=7;break;
			case 12:	valf=99;break;
			case 13:	valf=99;break;
			default:	valf=99;break;
		}
		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4234);
		val=getUInt16Value(p, pos, INT_AB);
		valf=val;
		acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_water_NJHongGuang_info2(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char str[50];
	int size=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0,startaddr=0;
	char *p=NULL;
	char *polcode=NULL;
	char flag=0;
	float valf=0;
	int val=0;
	int pos=0;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;

	SYSLOG_DBG("protocol_water_NJHongGuang_info2\n");
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL)
		return -2;

	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	startaddr=4258;
	cnt = 0x2D;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr-1,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"NJHongGuang %s info2",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		pos=get_cmd_03_04_pos(startaddr, 4258);
		valf=getUInt16Value(p, pos, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4259);
		valf=getUInt16Value(p, pos, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4262);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4264);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4266);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4271);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4273);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4275);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4277);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4295);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13115",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4297);
		year=BCD(p[pos])+2000;
		mon=BCD(p[pos+1]);
		day=BCD(p[pos+2]);
		hour=BCD(p[pos+3]);
		min=BCD(p[pos+4]);
		sec=BCD(p[pos+5]);
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13101",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4300);
		year=BCD(p[pos])+2000;
		mon=BCD(p[pos+1]);
		day=BCD(p[pos+2]);
		hour=BCD(p[pos+3]);
		min=BCD(p[pos+4]);
		sec=BCD(p[pos+5]);
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13107",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_water_NJHongGuang_info3(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char str[50];
	int size=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0,startaddr=0;
	char *p=NULL;
	char *polcode=NULL;
	char flag=0;
	float valf=0;
	int val=0;
	int pos=0;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;

	SYSLOG_DBG("protocol_water_NJHongGuang_info3\n");
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL)
		return -2;

	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	startaddr=4400;
	cnt = 0x16;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr-1,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"NJHongGuang %s info3",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		pos=get_cmd_03_04_pos(startaddr, 4400);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13103",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4402);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13106",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4404);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13109",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4406);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4408);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13112",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4410);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13113",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4412);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13118",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4414);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4416);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4418);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		pos=get_cmd_03_04_pos(startaddr, 4420);
		valf=getFloatValue(p, pos, dataType);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_water_NJHongGuang_info4(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char str[50];
	int size=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0,startaddr=0;
	char *p=NULL;
	char *polcode=NULL;
	char flag=0;
	float valf=0;
	int val=0;
	int pos=0;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;

	SYSLOG_DBG("protocol_water_NJHongGuang_info4\n");
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL)
		return -2;

	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	startaddr=4113;
	cnt = 0x03;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr-1,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"NJHongGuang %s info4",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		pos=get_cmd_03_04_pos(startaddr, 4113);
		year=BCD(p[pos])+2000;
		mon=BCD(p[pos+1]);
		day=BCD(p[pos+2]);
		hour=BCD(p[pos+3]);
		min=BCD(p[pos+4]);
		sec=BCD(p[pos+5]);
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13128",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}