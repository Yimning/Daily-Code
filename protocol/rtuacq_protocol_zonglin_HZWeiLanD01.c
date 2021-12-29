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


int protocol_ZONGLIN_ZJHZWeiLanD01(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float TP=0;
	int devaddr=0;
	union uf f;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 0x02, 0x02);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJHZWeiLanD01 TP SEND:",com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HZWeiLanD01 TP protocol,TP : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HZWeiLanD01 TP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJHZWeiLanD01 TP RECV:",com_tbuf,size);

	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
#if 1		
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
#else
		f.ch[0]=com_rbuf[4]; //66
		f.ch[1]=com_rbuf[3]; //c6
		f.ch[2]=com_rbuf[6]; //45  791.1=66 c6 45 44
		f.ch[3]=com_rbuf[5]; //44		
#endif
		TP=f.f;
		status=0;
	}
	else
	{
		TP=0;
		status=1;
		//printf("ppack:%s\n",com_rbuf);
	}
	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,TP,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_ZONGLIN_ZJHZWeiLanD01_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	int log_len=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_ZJHZWeiLanD01_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x14,0x52);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJHZWeiLanD01 TP INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZJHZWeiLanD01 TP protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZJHZWeiLanD01 TP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJHZWeiLanD01 TP INFO RECV1:",com_rbuf,size);
	if((size>=169)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
		}

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];

		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		}

		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 12:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 13:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 14:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 15:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			case 16:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
		}
		

		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[7];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[14];
		f.ch[1]=com_rbuf[13];
		f.ch[2]=com_rbuf[12];
		f.ch[3]=com_rbuf[11];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
	
		f.ch[0]=com_rbuf[26];
		f.ch[1]=com_rbuf[25];
		f.ch[2]=com_rbuf[24];
		f.ch[3]=com_rbuf[23];
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[30];
		f.ch[1]=com_rbuf[29];
		f.ch[2]=com_rbuf[28];
		f.ch[3]=com_rbuf[27];
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[44];
		f.ch[1]=com_rbuf[43];
		f.ch[2]=com_rbuf[42];
		f.ch[3]=com_rbuf[41];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[48];
		f.ch[1]=com_rbuf[47];
		f.ch[2]=com_rbuf[46];
		f.ch[3]=com_rbuf[45];
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[166];
		f.ch[1]=com_rbuf[165];
		f.ch[2]=com_rbuf[164];
		f.ch[3]=com_rbuf[163];
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);
	}
		
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x86,0x20);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJHZWeiLanD01 TP INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZJHZWeiLanD01 TP protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZJHZWeiLanD01 TP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJHZWeiLanD01 TP INFO RECV2:",com_rbuf,size);
	if((size>=69)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val/60.0,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[65];
		val<<=8;
		val+=com_rbuf[66];
		acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0xFA,0x16);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJHZWeiLanD01 TP INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZJHZWeiLanD01 TP protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZJHZWeiLanD01 TP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJHZWeiLanD01 TP INFO RECV3:",com_rbuf,size);
	if((size>=49)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[7];
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);
		
		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		timer.tm_year=val-1900;
		timer.tm_mon=com_rbuf[14]-1;
		timer.tm_mday=com_rbuf[16];
		timer.tm_hour=com_rbuf[18];
		timer.tm_min=com_rbuf[20];
		timer.tm_sec=com_rbuf[22];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13128",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[23];
		val<<=8;
		val+=com_rbuf[24];
		timer.tm_year=val-1900;
		timer.tm_mon=com_rbuf[26]-1;
		timer.tm_mday=com_rbuf[28];
		timer.tm_hour=com_rbuf[30];
		timer.tm_min=com_rbuf[32];
		timer.tm_sec=com_rbuf[34];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[35];
		val<<=8;
		val+=com_rbuf[36];
		timer.tm_year=val-1900;
		timer.tm_mon=com_rbuf[38]-1;
		timer.tm_mday=com_rbuf[40];
		timer.tm_hour=com_rbuf[42];
		timer.tm_min=com_rbuf[44];
		timer.tm_sec=com_rbuf[46];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


