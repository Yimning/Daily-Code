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

int protocol_COD_USAHaXi_maxII(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"USAHaXi maxII COD SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("USAHaXi maxII COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("USAHaXi maxII COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"USAHaXi maxII COD RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		total_cod=f.f;
		status=0;
	}
	else
	{
		total_cod=0;
		status=1;
	}
	

	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}




int protocol_COD_USAHaXi_maxII_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	unsigned long int val=0;
	int devaddr=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_COD_USAHaXi_maxII_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x06,0x3D);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"USAHaXi maxII COD INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("USAHaXi maxII COD INFO protocol,INFO1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("USAHaXi maxII COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"USAHaXi maxII COD INFO RECV1:",com_rbuf,size);
	if((size>=127)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		if(val==1)
		{
			acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			val=com_rbuf[13];
			val<<=8;
			val+=com_rbuf[14];
			if((val&0x0001)==0x0001)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n);
			else if((val&0x0020)==0x0020)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);
			else if((val&0x0800)==0x0800)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			else if((val&0x0008)==0x0008)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x0400)==0x0400)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else if(val==2)
		{
			acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			f.ch[3]=com_rbuf[9];
			f.ch[2]=com_rbuf[10];
			f.ch[1]=com_rbuf[11];
			f.ch[0]=com_rbuf[12];
			val=f.l;
			if((val&0x00000001)==0x00000001)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x00000002)==0x00000002)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x00000004)==0x00000004)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x00000040)==0x00000040)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x00000080)==0x00000080)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			else if((val&0x00000100)==0x00000100)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((val&0x00000200)==0x00000200)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((val&0x00000400)==0x00000400)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((val&0x00000800)==0x00000800)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x00001000)==0x00001000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x00002000)==0x00002000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x00004000)==0x00004000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x00008000)==0x00008000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x00010000)==0x00010000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x00020000)==0x00020000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x00040000)==0x00040000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x00080000)==0x00080000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x00100000)==0x00100000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x00200000)==0x00200000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x00800000)==0x00800000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x01000000)==0x01000000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x02000000)==0x02000000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n);
			else if((val&0x08000000)==0x08000000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n);
			else if((val&0x10000000)==0x10000000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n);
			else if((val&0x20000000)==0x20000000)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else
		{
			val=com_rbuf[93];
			val<<=8;
			val+=com_rbuf[94];
			switch(val)
			{
				case 0x0000:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 0x0101:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 0x0002:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 0x4004:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 0x8004:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 0x0008:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 0x0110:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 0x0080:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 0x0120:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0220:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0420:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0820:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x1020:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x2020:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0102:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				default :		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
			
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[33];
		f.ch[2]=com_rbuf[34];
		f.ch[1]=com_rbuf[31];
		f.ch[0]=com_rbuf[32];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[35];
		val<<=8;
		val+=com_rbuf[36];
		if(val>24)
			val=0;
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val*60,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[85];
		val<<=8;
		val+=com_rbuf[86];
		timer.tm_year=val-1900;
		
		val=com_rbuf[87];
		val<<=8;
		val+=com_rbuf[88];
		timer.tm_mon=val/100-1;
		timer.tm_mday=val%100;
		
		val=com_rbuf[89];
		val<<=8;
		val+=com_rbuf[90];
		timer.tm_hour=val/100;
		timer.tm_min=val%100;
		
		timer.tm_sec=com_rbuf[92];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
/*
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x8E,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"USAHaXi maxII COD INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("USAHaXi maxII COD INFO protocol,INFO2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("USAHaXi maxII COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"USAHaXi maxII COD INFO RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
*/
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xDD,0x19);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"USAHaXi maxII COD INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("USAHaXi maxII COD INFO protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("USAHaXi maxII COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"USAHaXi maxII COD INFO RECV3:",com_rbuf,size);
	if((size>=53)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,val,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[25];
		val<<=8;
		val+=com_rbuf[26];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[47];
		f.ch[2]=com_rbuf[48];
		f.ch[1]=com_rbuf[45];
		f.ch[0]=com_rbuf[46];
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[51];
		f.ch[2]=com_rbuf[52];
		f.ch[1]=com_rbuf[49];
		f.ch[0]=com_rbuf[50];
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x010F,0x0C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"USAHaXi maxII COD INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("USAHaXi maxII COD INFO protocol,INFO 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("USAHaXi maxII COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"USAHaXi maxII COD INFO RECV4:",com_rbuf,size);
	if((size>=29)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[9];
		f.ch[2]=com_rbuf[10];
		f.ch[1]=com_rbuf[7];
		f.ch[0]=com_rbuf[8];
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[21];
		val<<=8;
		val+=com_rbuf[22];
		timer.tm_year=val-1900;
		timer.tm_mon=com_rbuf[23]-1;
		timer.tm_mday=com_rbuf[24];
		timer.tm_hour=com_rbuf[25];
		timer.tm_min=com_rbuf[26];
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13128",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

