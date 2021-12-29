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

int protocol_ZONGDAN_GLYunJing_HeBei(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float tn=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0,cnt=0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x02;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1010,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "GLYunJing HeBei TN", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		tn=getFloatValue(p, 3, dataType);
		status=0;
	}
	else
	{
		sleep(1);
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,cmd,0x1000,cnt);
		size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "GLYunJing HeBei TN again", 1);
		p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
		if(p!=NULL)
		{
			tn=getFloatValue(p, 3, dataType);
			status=0;
		}
		else
		{
			tn=0;
			status=1;
		}
	}

	ret=acqdata_set_value(acq_data,"w21001",UNIT_MG_L,tn,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_ZONGDAN_GLYunJing_HeBei_Stay(struct acquisition_data *acq_data)
{
static char water_GLYunJing_HeBei_Mark = 'N';
static float  ZONGDAN_GLYunJing_HeBei_Stay = 0;
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char str[50];
	int size=0;
	float valf=0;
	int val=0;
	int pos=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0,cnt=0;
	char *p,*polcode;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if(polcode==NULL) return -2;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x06;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1000,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"GLYunJing HeBei %s",polcode);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		pos=get_cmd_03_04_pos(0x1000, 0x1000);
		valf=getFloatValue(p, pos, FLOAT_CDAB);

		pos=get_cmd_03_04_pos(0x1000, 0x1005);
		val=getUInt16Value(p, pos, INT_AB);
		switch(val)
		{
			case 0:	flag='N';ZONGDAN_GLYunJing_HeBei_Stay = valf;break;
			//case 1:	flag='T';break;
			//case 2:	flag='T';break;
			case 3:	flag='D';break;
			case 4:	flag='M';break;
			case 5:	flag='C';break;
			case 6:	flag='C';break;
			case 7:	flag='M';break;
			case 8:	flag='M';break;
			default:	flag='N';ZONGDAN_GLYunJing_HeBei_Stay = valf;break;
		}
		if(flag == 'C')
			valf = ZONGDAN_GLYunJing_HeBei_Stay;
		
		status=0;
	}
	else
	{
		valf=0;
		status=1;
	}
	

	ret=acqdata_set_value(acq_data,polcode,UNIT_MG_L,valf,&arg_n);
	if(status == 0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



int protocol_ZONGDAN_GLYunJing_HeBei_info(struct acquisition_data *acq_data)
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
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGDAN_GLYunJing_HeBei_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21001",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x10A0,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"GLYunJing HeBei TN STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("GLYunJing HeBei TN STATUS protocol,STATUS: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("GLYunJing HeBei TN STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"GLYunJing HeBei TN STATUS RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		

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

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x1100,0x2B);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"GLYunJing HeBei TN INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("GLYunJing HeBei TN INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("GLYunJing HeBei TN INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"GLYunJing HeBei TN INFO RECV1:",com_rbuf,size);
	if((size>=91)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		timer.tm_year=com_rbuf[3]+100;
		timer.tm_mon=com_rbuf[4]-1;
		timer.tm_mday=com_rbuf[5];
		timer.tm_hour=com_rbuf[6];
		timer.tm_min=com_rbuf[7];
		timer.tm_sec=com_rbuf[8];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[15];
		f.ch[2]=com_rbuf[16];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		acqdata_set_value_flag(acq_data,"i13103",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[17];
		f.ch[0]=com_rbuf[18];
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[23];
		f.ch[2]=com_rbuf[24];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[27];
		f.ch[2]=com_rbuf[28];
		f.ch[1]=com_rbuf[25];
		f.ch[0]=com_rbuf[26];
		acqdata_set_value_flag(acq_data,"i13106",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=com_rbuf[29]+100;
		timer.tm_mon=com_rbuf[30]-1;
		timer.tm_mday=com_rbuf[31];
		timer.tm_hour=com_rbuf[32];
		timer.tm_min=com_rbuf[33];
		timer.tm_sec=com_rbuf[34];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[37];
		f.ch[2]=com_rbuf[38];
		f.ch[1]=com_rbuf[35];
		f.ch[0]=com_rbuf[36];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[41];
		f.ch[2]=com_rbuf[42];
		f.ch[1]=com_rbuf[39];
		f.ch[0]=com_rbuf[40];
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[45];
		f.ch[2]=com_rbuf[46];
		f.ch[1]=com_rbuf[43];
		f.ch[0]=com_rbuf[44];
		acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[49];
		f.ch[2]=com_rbuf[50];
		f.ch[1]=com_rbuf[47];
		f.ch[0]=com_rbuf[48];
		acqdata_set_value_flag(acq_data,"i13112",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[53];
		f.ch[2]=com_rbuf[54];
		f.ch[1]=com_rbuf[51];
		f.ch[0]=com_rbuf[52];
		acqdata_set_value_flag(acq_data,"i13113",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[55];
		val<<=8;
		val+=com_rbuf[56];
		acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		f.ch[3]=com_rbuf[63];
		f.ch[2]=com_rbuf[64];
		f.ch[1]=com_rbuf[61];
		f.ch[0]=com_rbuf[62];
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[67];
		f.ch[2]=com_rbuf[68];
		f.ch[1]=com_rbuf[65];
		f.ch[0]=com_rbuf[66];
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[71];
		f.ch[2]=com_rbuf[72];
		f.ch[1]=com_rbuf[60];
		f.ch[0]=com_rbuf[70];
		acqdata_set_value_flag(acq_data,"i13118",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[75];
		f.ch[2]=com_rbuf[76];
		f.ch[1]=com_rbuf[73];
		f.ch[0]=com_rbuf[74];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[79];
		f.ch[2]=com_rbuf[80];
		f.ch[1]=com_rbuf[77];
		f.ch[0]=com_rbuf[78];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[83];
		f.ch[2]=com_rbuf[84];
		f.ch[1]=com_rbuf[81];
		f.ch[0]=com_rbuf[82];
		acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[87];
		f.ch[2]=com_rbuf[88];
		f.ch[1]=com_rbuf[85];
		f.ch[0]=com_rbuf[86];
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x1140,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"GLYunJing HeBei TN INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("GLYunJing HeBei TN INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("GLYunJing HeBei TN INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"GLYunJing HeBei TN INFO RECV2:",com_rbuf,size);
	if((size>=19)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		timer.tm_year=com_rbuf[3]+100;
		timer.tm_mon=com_rbuf[4]-1;
		timer.tm_mday=com_rbuf[5];
		timer.tm_hour=com_rbuf[6];
		timer.tm_min=com_rbuf[7];
		timer.tm_sec=com_rbuf[8];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13128",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[15];
		f.ch[2]=com_rbuf[16];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

