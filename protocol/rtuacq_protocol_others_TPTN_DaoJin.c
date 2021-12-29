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

int protocol_others_TPTN_DaoJin_HeBei(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float tp=0,tn=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	unsigned int cmd=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x08,0x16);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DaoJin HeBei TPTN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DaoJin HeBei TPTN protocol,TPTN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DaoJin HeBei TPTN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DaoJin HeBei TPTN RECV:",com_rbuf,size);
	if((size>=49)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		tn=f.f;

		f.ch[3]=com_rbuf[45];
		f.ch[2]=com_rbuf[46];
		f.ch[1]=com_rbuf[43];
		f.ch[0]=com_rbuf[44];
		tp=f.f;
		
		status=0;
	}
	else
	{
		tp=0;
		tn=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w21001",UNIT_MG_L,tn,&arg_n);
	acqdata_set_value(acq_data,"w21011",UNIT_MG_L,tp,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}




int protocol_others_TPTN_DaoJin_HeBei_info_TN(struct acquisition_data *acq_data)
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
	unsigned int cmd=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_others_TPTN_DaoJin_HeBei_info_TN\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21001",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0262,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DaoJin HeBei TN STATUS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DaoJin HeBei TN STATUS protocol,STATUS 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DaoJin HeBei TN STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DaoJin HeBei TN STATUS RECV1:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		if(val==0)
		{
			val=com_rbuf[5];
			val<<=8;
			val+=com_rbuf[6];
			if(val==0)
			{
				memset(com_tbuf,0,sizeof(com_tbuf));
				size=modbus_pack(com_tbuf,devaddr,cmd,0x013B,0x01);
				LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DaoJin HeBei TN STATUS SEND2:",com_tbuf,size);
				size=write_device(DEV_NAME(acq_data),com_tbuf,size);
				sleep(1);
				memset(com_rbuf,0,sizeof(com_rbuf));
				size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
				SYSLOG_DBG("DaoJin HeBei TN STATUS protocol,STATUS 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
				SYSLOG_DBG_HEX("DaoJin HeBei TN STATUS data",com_rbuf,size);
				LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DaoJin HeBei TN STATUS RECV2:",com_rbuf,size);
				if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
				{
					val=com_rbuf[3];
					val<<=8;
					val+=com_rbuf[4];
					if(val==0x2000)
					{
						acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
						acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
						acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
					}
					else
					{
						switch(val)
						{
							case 0x0000:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
							case 0x1000:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
							case 0x1001:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
							case 0x1002:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
							case 0x1005:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
							case 0x1007:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
							default:		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
						}
						acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
						acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					}
				}
			}
			else
			{
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				switch(val)
				{
					case 0x0001:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0002:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0004:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0008:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0020:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0040:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0100:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
					case 0x1000:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x2000:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					default:		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				}
			}
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			switch(val)
			{
				case 0x0001:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0002:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0004:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0008:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0010:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0020:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0040:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 0x0080:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
				case 0x0100:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				default:		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
			
		}
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xA2,0x5B);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DaoJin HeBei TN INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DaoJin HeBei TN INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DaoJin HeBei TN INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DaoJin HeBei TN INFO RECV1:",com_rbuf,size);
	if((size>=187)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		timer.tm_year=com_rbuf[4]+100;
		timer.tm_mon=com_rbuf[6]-1;
		timer.tm_mday=com_rbuf[8];
		timer.tm_hour=com_rbuf[10];
		timer.tm_min=com_rbuf[12];
		timer.tm_sec=com_rbuf[14];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[31];
		f.ch[2]=com_rbuf[32];
		f.ch[1]=com_rbuf[33];
		f.ch[0]=com_rbuf[34];
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=com_rbuf[44]+100;
		timer.tm_mon=com_rbuf[46]-1;
		timer.tm_mday=com_rbuf[48];
		timer.tm_hour=com_rbuf[50];
		timer.tm_min=com_rbuf[52];
		timer.tm_sec=com_rbuf[54];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[71];
		f.ch[2]=com_rbuf[72];
		f.ch[1]=com_rbuf[73];
		f.ch[0]=com_rbuf[74];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[177];
		f.ch[2]=com_rbuf[178];
		f.ch[1]=com_rbuf[179];
		f.ch[0]=com_rbuf[180];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[181];
		f.ch[2]=com_rbuf[182];
		f.ch[1]=com_rbuf[183];
		f.ch[0]=com_rbuf[184];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_others_TPTN_DaoJin_HeBei_info_TP(struct acquisition_data *acq_data)
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
	unsigned int cmd=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_others_TPTN_DaoJin_HeBei_info_TP\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0262,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DaoJin HeBei TP STATUS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DaoJin HeBei TP STATUS protocol,STATUS 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DaoJin HeBei TP STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DaoJin HeBei TP STATUS RECV1:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		if(val==0)
		{
			val=com_rbuf[5];
			val<<=8;
			val+=com_rbuf[6];
			if(val==0)
			{
				memset(com_tbuf,0,sizeof(com_tbuf));
				size=modbus_pack(com_tbuf,devaddr,cmd,0x013B,0x01);
				LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DaoJin HeBei TP STATUS SEND2:",com_tbuf,size);
				size=write_device(DEV_NAME(acq_data),com_tbuf,size);
				sleep(1);
				memset(com_rbuf,0,sizeof(com_rbuf));
				size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
				SYSLOG_DBG("DaoJin HeBei TP STATUS protocol,STATUS 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
				SYSLOG_DBG_HEX("DaoJin HeBei TP STATUS data",com_rbuf,size);
				LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DaoJin HeBei TP STATUS RECV2:",com_rbuf,size);
				if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
				{
					val=com_rbuf[3];
					val<<=8;
					val+=com_rbuf[4];
					if(val==0x2000)
					{
						acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
						acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
						acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
					}
					else
					{
						switch(val)
						{
							case 0x0000:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
							case 0x1000:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
							case 0x1001:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
							case 0x1002:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
							case 0x1005:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
							case 0x1007:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
							default:		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
						}
						acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
						acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					}
				}
			}
			else
			{
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				switch(val)
				{
					case 0x0001:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0002:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0004:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0008:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0020:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0040:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x0100:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
					case 0x1000:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					case 0x2000:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
					default:		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				}
			}
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			switch(val)
			{
				case 0x0001:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0002:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0004:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0008:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0010:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0020:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 0x0040:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 0x0080:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
				case 0x0100:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				default:		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
			
		}
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xCA,0x39);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DaoJin HeBei TP INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DaoJin HeBei TP INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DaoJin HeBei TP INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DaoJin HeBei TP INFO RECV1:",com_rbuf,size);
	if((size>=119)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		timer.tm_year=com_rbuf[4]+100;
		timer.tm_mon=com_rbuf[6]-1;
		timer.tm_mday=com_rbuf[8];
		timer.tm_hour=com_rbuf[10];
		timer.tm_min=com_rbuf[12];
		timer.tm_sec=com_rbuf[14];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[31];
		f.ch[2]=com_rbuf[32];
		f.ch[1]=com_rbuf[33];
		f.ch[0]=com_rbuf[34];
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=com_rbuf[44]+100;
		timer.tm_mon=com_rbuf[46]-1;
		timer.tm_mday=com_rbuf[48];
		timer.tm_hour=com_rbuf[50];
		timer.tm_min=com_rbuf[52];
		timer.tm_sec=com_rbuf[54];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[71];
		f.ch[2]=com_rbuf[72];
		f.ch[1]=com_rbuf[73];
		f.ch[0]=com_rbuf[74];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[109];
		f.ch[2]=com_rbuf[110];
		f.ch[1]=com_rbuf[111];
		f.ch[0]=com_rbuf[112];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[113];
		f.ch[2]=com_rbuf[114];
		f.ch[1]=com_rbuf[115];
		f.ch[0]=com_rbuf[116];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

