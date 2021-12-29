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

int protocol_ANDAN_AHWanYi(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char p[1024];
	int size=0;
	float nh3n=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x07FF,0x02);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi HeBei NH3N SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi HeBei NH3N protocol,NH3N : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi HeBei NH3N data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi HeBei NH3N RECV:",com_rbuf,size);
	if(size>=19 && modbus_ascii_pack_check(com_rbuf, devaddr, cmd))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
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
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}




int protocol_ANDAN_AHWanYi_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char p[1024];
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
	SYSLOG_DBG("protocol_ANDAN_AHWanYi_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x07D0,0x04);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi NH3N STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi NH3N STATUS protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi NH3N STATUS data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi NH3N STATUS RECV:",com_rbuf,size);
	if(size>=27 && modbus_ascii_pack_check(com_rbuf, devaddr, cmd))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		
		val=p[9];
		val<<=8;
		val+=p[10];
		if(val==0)
		{
			val=p[3];
			val<<=8;
			val+=p[4];
			switch(val)
			{
				case 0x00:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 0x02:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x04:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 0x08:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 0x20:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 0x21:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 0x22:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 0x23:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 0x24:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 0x25:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				default:		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			switch(val)
			{
				case 0x0101:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x0102:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x0109:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x010A:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x010B:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x010C:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x010D:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x010E:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x010F:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 0x0110:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x0111:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x0112:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x0113:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x0114:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				case 0x0115:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				case 0x0116:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				case 0x0117:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				case 0x0118:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x011A:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x011B:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 0x011D:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
				case 0x0120:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 0x0121:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
				case 0x0122:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 0x0126:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0127:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0128:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0129:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x012B:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x012C:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x012D:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				default:		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
		}
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x07F3,0x18);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi NH3N INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi NH3N INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi NH3N INFO data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi NH3N INFO RECV1:",com_rbuf,size);
	if(size>=107 && modbus_ascii_pack_check(com_rbuf, devaddr, cmd))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);


		val=p[3];
		val<<=8;
		val+=p[4];
		timer.tm_sec=val;
		val=p[5];
		val<<=8;
		val+=p[6];
		timer.tm_min=val;
		val=p[7];
		val<<=8;
		val+=p[8];
		timer.tm_hour=val;
		val=p[9];
		val<<=8;
		val+=p[10];
		timer.tm_mday=val;
		val=p[11];
		val<<=8;
		val+=p[12];
		timer.tm_mon=val-1;
		val=p[13];
		val<<=8;
		val+=p[14];
		timer.tm_year=val-1900;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=p[15];
		val<<=8;
		val+=p[16];
		timer.tm_sec=val;
		val=p[17];
		val<<=8;
		val+=p[18];
		timer.tm_min=val;
		val=p[19];
		val<<=8;
		val+=p[20];
		timer.tm_hour=val;
		val=p[21];
		val<<=8;
		val+=p[22];
		timer.tm_mday=val;
		val=p[23];
		val<<=8;
		val+=p[24];
		timer.tm_mon=val-1;
		val=p[25];
		val<<=8;
		val+=p[26];
		timer.tm_year=val-1900;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[33];
		f.ch[2]=p[34];
		f.ch[1]=p[31];
		f.ch[0]=p[32];
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[37];
		f.ch[2]=p[38];
		f.ch[1]=p[35];
		f.ch[0]=p[36];
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[41];
		f.ch[2]=p[42];
		f.ch[1]=p[39];
		f.ch[0]=p[40];
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[45];
		f.ch[2]=p[46];
		f.ch[1]=p[43];
		f.ch[0]=p[44];
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x086D,0x02);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi NH3N INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi NH3N INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi NH3N INFO data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi NH3N INFO RECV2:",com_rbuf,size);
	if(size>=19 && modbus_ascii_pack_check(com_rbuf, devaddr, cmd))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	cmd=0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x07D1,0x11);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi NH3N INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi NH3N INFO protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi NH3N INFO data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi NH3N INFO RECV3:",com_rbuf,size);
	if(size>=99 && modbus_ascii_pack_check(com_rbuf, devaddr, cmd))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		val=p[9];
		val<<=8;
		val+=p[10];
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);

		val=p[11];
		val<<=8;
		val+=p[12];
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[31];
		f.ch[2]=p[32];
		f.ch[1]=p[29];
		f.ch[0]=p[30];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[35];
		f.ch[2]=p[36];
		f.ch[1]=p[33];
		f.ch[0]=p[34];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

