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

/*
Author:Yimning
Create Time:2021.08.30 Mon.
Description:protocol_ANDAN_NJHongGuang2

*/
int protocol_ANDAN_NJHongGuang2(struct acquisition_data *acq_data)
{
#define CNT_MAX 10

	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float andan=0;
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
	cmd = 0x03;
	regpos = 0x1008;
	regcnt = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	
	ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJHongGuang NH3N SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJHongGuang NH3N protocol,NH3N : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJHongGuang NH3N data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang NH3N RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		f.ch[0]=p[4];
		f.ch[1]=p[3];
		f.ch[2]=p[6];
		f.ch[3]=p[5];
		andan=f.f;

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
			case 0x4600:	dev_stat='D';break;
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
		
		if(cnt>CNT_MAX)
		{
			dev_stat='D';
		}
		else
		{
			andan=bak_val;
			dev_stat=bak_flag;
			cnt++;
		}

		status=1;
	}

    sleep(1);

	if(status==0 && (dev_stat == 'N' || dev_stat == 'T'))
	{
	    usleep(300000);
		cmd = 0x03;
		regpos = 0x109A;
		regcnt = 0x01;
		memset(com_tbuf,0,sizeof(com_tbuf));

	ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	
		size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJHongGuang NH3N SEND2:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(2);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("NJHongGuang NH3N protocol,COD 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("NJHongGuang NH3N data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang NH3N RECV2:",com_rbuf,size);
		p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
		if((p!= NULL))
		{
			val=p[3];
			val<<=8;
			val+=p[4];
			switch(val)
			{
				case 1:	dev_stat='C';break;
				case 2:	dev_stat='K';break;
				case 3:	dev_stat='D';break;
				default :	break;
			}
		}
		else
		{
			if(cnt>CNT_MAX)
			{
				dev_stat='D';
			}
			else
			{
				andan=bak_val;
				dev_stat=bak_flag;
				cnt++;
			}
				
			status=1;
		}
	}
	
	if(status==0)
	{
		bak_val=andan;
		bak_flag=dev_stat;
		cnt=0;
	}

	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	acq_data->dev_stat = dev_stat;

	acq_data->acq_status = ACQ_OK;

	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_ANDAN_NJHongGuang2_info(struct acquisition_data *acq_data)
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
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;

	return 0;//no parameter

	
	SYSLOG_DBG("protocol_ANDAN_NJHongGuang_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00

	ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer

	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	cmd = 0x03;
	regpos = 0x1010;
	regcnt = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x1010,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJHongGuang ANDAN INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJHongGuang ANDAN protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJHongGuang ANDAN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang ANDAN INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		timer.tm_year=BCD(p[3])+100;//update BCD code of the time by Yimning 
		timer.tm_mon=BCD(p[4])-1;
		timer.tm_mday=BCD(p[5]);
		timer.tm_hour=BCD(p[6]);
		timer.tm_min=BCD(p[7]);
		timer.tm_sec=BCD(p[8]);

		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13128",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	sleep(1);

ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x1083,0x17);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJHongGuang ANDAN INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJHongGuang ANDAN protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJHongGuang ANDAN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang ANDAN INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, 0x3, 0x17);
	if(p!= NULL)
	{
		val=p[3];
		val<<=8;
		val+=p[4];
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

		val=p[7];
		val<<=8;
		val+=p[8];
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

		val=p[15];
		val<<=8;
		val+=p[16];
		valf=val;
		acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		val=p[47];
		val<<=8;
		val+=p[48];
		valf=val;
		acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,valf,INFOR_STATUS,&arg_n);
		
		status=0;
	}
	
	sleep(1);

ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x10A1,0xA4);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJHongGuang ANDAN INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJHongGuang ANDAN protocol,INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJHongGuang ANDAN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJHongGuang ANDAN INFO RECV3:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, 0x3, 0xA4);
	//if(p!= NULL)
	//{
	if((size>=333)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x48))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val;
		acqdata_set_value_flag(acq_data,"i13004",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		valf=val;
		acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[13];
		f.ch[2]=com_rbuf[14];
		f.ch[1]=com_rbuf[11];
		f.ch[0]=com_rbuf[12];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13116",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[17];
		f.ch[2]=com_rbuf[18];
		f.ch[1]=com_rbuf[15];
		f.ch[0]=com_rbuf[16];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[21];
		f.ch[2]=com_rbuf[22];
		f.ch[1]=com_rbuf[19];
		f.ch[0]=com_rbuf[20];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[31];
		f.ch[2]=com_rbuf[32];
		f.ch[1]=com_rbuf[29];
		f.ch[0]=com_rbuf[30];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13104",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[35];
		f.ch[2]=com_rbuf[36];
		f.ch[1]=com_rbuf[33];
		f.ch[0]=com_rbuf[34];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[39];
		f.ch[2]=com_rbuf[40];
		f.ch[1]=com_rbuf[37];
		f.ch[0]=com_rbuf[38];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13108",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[43];
		f.ch[2]=com_rbuf[44];
		f.ch[1]=com_rbuf[41];
		f.ch[0]=com_rbuf[42];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[79];
		f.ch[2]=com_rbuf[80];
		f.ch[1]=com_rbuf[77];
		f.ch[0]=com_rbuf[78];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13115",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[81])+100;
		timer.tm_mon=BCD(com_rbuf[82])-1;
		timer.tm_mday=BCD(com_rbuf[83]);
		timer.tm_hour=BCD(com_rbuf[84]);
		timer.tm_min=BCD(com_rbuf[85]);
		timer.tm_sec=BCD(com_rbuf[86]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[87])+100;
		timer.tm_mon=BCD(com_rbuf[88])-1;
		timer.tm_mday=BCD(com_rbuf[89]);
		timer.tm_hour=BCD(com_rbuf[90]);
		timer.tm_min=BCD(com_rbuf[91]);
		timer.tm_sec=BCD(com_rbuf[92]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[289];
		f.ch[2]=com_rbuf[290];
		f.ch[1]=com_rbuf[287];
		f.ch[0]=com_rbuf[288];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13103",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[293];
		f.ch[2]=com_rbuf[294];
		f.ch[1]=com_rbuf[291];
		f.ch[0]=com_rbuf[292];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13106",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[297];
		f.ch[2]=com_rbuf[298];
		f.ch[1]=com_rbuf[295];
		f.ch[0]=com_rbuf[296];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13109",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[301];
		f.ch[2]=com_rbuf[302];
		f.ch[1]=com_rbuf[299];
		f.ch[0]=com_rbuf[300];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[305];
		f.ch[2]=com_rbuf[306];
		f.ch[1]=com_rbuf[303];
		f.ch[0]=com_rbuf[304];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13112",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[309];
		f.ch[2]=com_rbuf[310];
		f.ch[1]=com_rbuf[307];
		f.ch[0]=com_rbuf[308];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13113",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[313];
		f.ch[2]=com_rbuf[314];
		f.ch[1]=com_rbuf[311];
		f.ch[0]=com_rbuf[312];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13118",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[317];
		f.ch[2]=com_rbuf[318];
		f.ch[1]=com_rbuf[315];
		f.ch[0]=com_rbuf[316];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[321];
		f.ch[2]=com_rbuf[322];
		f.ch[1]=com_rbuf[319];
		f.ch[0]=com_rbuf[320];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[325];
		f.ch[2]=com_rbuf[326];
		f.ch[1]=com_rbuf[323];
		f.ch[0]=com_rbuf[324];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13129",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[329];
		f.ch[2]=com_rbuf[330];
		f.ch[1]=com_rbuf[327];
		f.ch[0]=com_rbuf[328];
		valf=f.f;
		acqdata_set_value_flag(acq_data,"i13130",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}



	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}