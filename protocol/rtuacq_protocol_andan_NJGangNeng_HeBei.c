/******************************************************************
*
*author:Ben
*Date:2018.7.12
*debugdate:2018.7.13
*********************************************************************/
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


int protocol_ANDAN_NJGangNeng_HeBei(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float nh3n=0;
	union uf f;

	if(!acq_data) return -1;


	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x0C, 0x03, 0x06D7, 0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJGangNeng NH3N SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJGangNeng HeBei NH3N protocol,NH3N : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJGangNeng HeBei NH3N data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJGangNeng NH3N RECV:",com_rbuf,size);	
	if(size>=9 && com_rbuf[0]==0x0C && com_rbuf[1]==0x03)
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		nh3n=f.f;

		status=0;
	}
	else
	{
		nh3n=0;
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w21003",UNIT_MG_L,nh3n,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_ANDAN_NJGangNeng_HeBei_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ANDAN_NJGangNeng_HeBei_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=0x0C;
	cmd=0x03;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x06E8,0x30);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJGangNeng NH3N INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJGangNeng NH3N INFO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJGangNeng NH3N INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJGangNeng NH3N INFO RECV:",com_rbuf,size);
	if((size>=101)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		switch(val)
		{
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
	
		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		if(val==0)
		{
				acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			switch(val)
			{
				case 1:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 2:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n);break;
				case 3:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 4:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 5:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 6:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
				case 7:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
				case 8:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				default:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
		}

		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=com_rbuf[16]+100;
		timer.tm_mon=com_rbuf[18]-1;
		timer.tm_mday=com_rbuf[20];
		timer.tm_hour=com_rbuf[22];
		timer.tm_min=com_rbuf[24];
		timer.tm_sec=com_rbuf[26];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[31];
		f.ch[2]=com_rbuf[32];
		f.ch[1]=com_rbuf[29];
		f.ch[0]=com_rbuf[30];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=com_rbuf[36]+100;
		timer.tm_mon=com_rbuf[38]-1;
		timer.tm_mday=com_rbuf[40];
		timer.tm_hour=com_rbuf[42];
		timer.tm_min=com_rbuf[44];
		timer.tm_sec=com_rbuf[46];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[53];
		f.ch[2]=com_rbuf[54];
		f.ch[1]=com_rbuf[51];
		f.ch[0]=com_rbuf[52];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[57];
		f.ch[2]=com_rbuf[58];
		f.ch[1]=com_rbuf[55];
		f.ch[0]=com_rbuf[56];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[59];
		val<<=8;
		val+=com_rbuf[60];
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[63];
		f.ch[2]=com_rbuf[64];
		f.ch[1]=com_rbuf[61];
		f.ch[0]=com_rbuf[62];
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[67];
		f.ch[2]=com_rbuf[68];
		f.ch[1]=com_rbuf[65];
		f.ch[0]=com_rbuf[66];
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[93];
		f.ch[2]=com_rbuf[94];
		f.ch[1]=com_rbuf[91];
		f.ch[0]=com_rbuf[92];
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,f.f/100,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[97];
		f.ch[2]=com_rbuf[98];
		f.ch[1]=com_rbuf[95];
		f.ch[0]=com_rbuf[96];
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,f.f/100,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}