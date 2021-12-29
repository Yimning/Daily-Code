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



struct ZONGDAN_JuGuang_HeBei_info_data{
	int i12101;
	int i12102;
	int i12103;

	int i13004;
	int i13005;
	int i13003;
	float i13116;
	float i13008;
	float i13007;
	float i13119;
	float i13120;

	float i13105;
	time_t i13101;
	float i13110;
	time_t i13107;
	float i13104;
	float i13108;

	float i13130;
	float i13129;
	time_t i13128;
};

/*
Description:protocol_ZONGDAN_JuGuang_HeBei
Update by Yimning.
Update Time:2021.07.12 Mon.
*/
int protocol_ZONGDAN_JuGuang_HeBei(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float zongdan=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	char *p;
	struct tm timer;
	time_t t1,t2,t3;


	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1205,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN protocol,ZONGDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x02);
	if(p!=NULL)
	{
		f.ch[3]=p[3];
		f.ch[2]=p[4];
		f.ch[1]=p[5];
		f.ch[0]=p[6];
		zongdan=f.f;
		
		status=0;
	}
	else
	{
		zongdan=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w21001",UNIT_MG_L,zongdan,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


//   1---3
int protocol_ZONGDAN_JuGuang_HeBei_info_func1(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x2000,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN STATUS1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN STATUS protocol,STATUS1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN STATUS1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN STATUS1 RECV:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];

		switch(val)
		{
			case 1: data->i12101=0; break;
			case 2: data->i12101=1; break;
			case 3: data->i12101=3; break;
			case 4: data->i12101=4; break;
			case 5: data->i12101=5; break;
			case 6: data->i12101=6; break;
			case 7: data->i12101=99; break;
			case 8: data->i12101=2; break;
			default: data->i12101=99; break;
		}

		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];

		if(val==0)
			data->i12103=0;
		else if((val&0x0001)==0x0001)
			data->i12103=99;
		else if((val&0x0002)==0x0002)
			data->i12103=99;
		else if((val&0x0004)==0x0004)
			data->i12103=4;
		else if((val&0x0008)==0x0008)
			data->i12103=1;
		else if((val&0x0010)==0x0010)
			data->i12103=5;
		else if((val&0x0080)==0x0080)
			data->i12103=99;
		else if((val&0x0100)==0x0100)
			data->i12103=7;
		else if((val&0x0200)==0x0200)
			data->i12103=8;
		else 
			data->i12103=99;
			
		
		val=com_rbuf[36];
		val<<=8;
		val+=com_rbuf[37];
		data->i12102=val;
		
		return 0;
	}
	return -1;
}

int protocol_ZONGDAN_JuGuang_HeBei_info_func2(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x2004,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN STATUS2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN STATUS protocol,STATUS2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN STATUS2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN STATUS2 RECV:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];

		if(val==0)
			data->i12103=0;
		else if((val&0x0001)==0x0001)
			data->i12103=99;
		else if((val&0x0002)==0x0002)
			data->i12103=99;
		else if((val&0x0004)==0x0004)
			data->i12103=4;
		else if((val&0x0008)==0x0008)
			data->i12103=1;
		else if((val&0x0010)==0x0010)
			data->i12103=5;
		else if((val&0x0080)==0x0080)
			data->i12103=99;
		else if((val&0x0100)==0x0100)
			data->i12103=7;
		else if((val&0x0200)==0x0200)
			data->i12103=8;
		else 
			data->i12103=99;
		return 0;
	}
	return -1;
}

int protocol_ZONGDAN_JuGuang_HeBei_info_func3(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x2011,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN STATUS3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN STATUS protocol,STATUS3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN STATUS3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN STATUS3 RECV:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[3];
		data->i12102=val;
		
		return 0;
	}
	return -1;
}


// 4---11
int protocol_ZONGDAN_JuGuang_HeBei_info_func4(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1000,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO4 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV4:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		data->i13004=val;
		
		return 0;
	}
	return -1;
}

int protocol_ZONGDAN_JuGuang_HeBei_info_func5(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1001,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO5 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV5:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		data->i13005=val;
		
		return 0;
	}
	return -1;
}



int protocol_ZONGDAN_JuGuang_HeBei_info_func6(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1002,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND6:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO6: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO6 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV6:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		data->i13003=val;
		
		return 0;
	}
	return -1;
}




int protocol_ZONGDAN_JuGuang_HeBei_info_func7(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1007,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND7:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO7: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO7 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV7:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13116=f.f;
		
		return 0;
	}
	return -1;
}



int protocol_ZONGDAN_JuGuang_HeBei_info_func8(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1009,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND8:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO8: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO8 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV8:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13008=f.f;
		
		return 0;
	}
	return -1;
}

int protocol_ZONGDAN_JuGuang_HeBei_info_func9(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x100B,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND9:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO9: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO9 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV9:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13007=f.f;
		
		return 0;
	}
	return -1;
}

int protocol_ZONGDAN_JuGuang_HeBei_info_func10(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1015,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND10:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO10: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO10 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV10:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13119=f.f;
		
		return 0;
	}
	return -1;
}



int protocol_ZONGDAN_JuGuang_HeBei_info_func11(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1017,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND11:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO11: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO11 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV11:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13120=f.f;
		return 0;
	}
	return -1;
}


//12-----17
int protocol_ZONGDAN_JuGuang_HeBei_info_func12(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1104,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND12:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO12: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO12 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV12:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13105=f.f;
		
		return 0;
	}
	return -1;
}


int protocol_ZONGDAN_JuGuang_HeBei_info_func13(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1106,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND13:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO13: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO13 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV13:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		timer.tm_year=BCD(com_rbuf[3])+100;
		timer.tm_mon=BCD(com_rbuf[4])-1;
		timer.tm_mday=BCD(com_rbuf[5]);
		timer.tm_hour=BCD(com_rbuf[6]);
		timer.tm_min=BCD(com_rbuf[7]);
		timer.tm_sec=BCD(com_rbuf[8]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):0;
		data->i13101=t3;
	
		return 0;
	}
	return -1;
}


int protocol_ZONGDAN_JuGuang_HeBei_info_func14(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x110C,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND14:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO14: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO14 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV14:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13110=f.f;
		
		return 0;
	}
	return -1;
}

int protocol_ZONGDAN_JuGuang_HeBei_info_func15(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x110E,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND15:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO15: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO15 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV15:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		timer.tm_year=BCD(com_rbuf[3])+100;
		timer.tm_mon=BCD(com_rbuf[4])-1;
		timer.tm_mday=BCD(com_rbuf[5]);
		timer.tm_hour=BCD(com_rbuf[6]);
		timer.tm_min=BCD(com_rbuf[7]);
		timer.tm_sec=BCD(com_rbuf[8]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):0;
		data->i13107=t3;

		return 0;
	}
	return -1;
}


int protocol_ZONGDAN_JuGuang_HeBei_info_func16(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1116,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND16:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO16: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO16 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV16:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13104=f.f;
		
		return 0;
	}
	return -1;
}

int protocol_ZONGDAN_JuGuang_HeBei_info_func17(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1118,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND17:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO17: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO17 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV17:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13108=f.f;
		
		return 0;
	}
	return -1;
}



// 18---20
int protocol_ZONGDAN_JuGuang_HeBei_info_func18(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1305,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND18:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO18: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO18 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV18:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13130=f.f;
	
		return 0;
	}
	return -1;
}


int protocol_ZONGDAN_JuGuang_HeBei_info_func19(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1309,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND19:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO19: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO19 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV19:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		data->i13129=f.f;
		
		return 0;
	}
	return -1;
}

int protocol_ZONGDAN_JuGuang_HeBei_info_func20(struct acquisition_data *acq_data,unsigned int devaddr,void *arg)
{
	struct ZONGDAN_JuGuang_HeBei_info_data *data;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data || !arg)
		return -1;
	
	data=(struct ZONGDAN_JuGuang_HeBei_info_data *)arg;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x130C,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang_HeBei ZONGDAN INFO SEND20:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang_HeBei ZONGDAN INFO protocol,INFO20: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang_HeBei ZONGDAN INFO20 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang_HeBei ZONGDAN INFO RECV20:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		timer.tm_year=BCD(com_rbuf[3])+100;
		timer.tm_mon=BCD(com_rbuf[4])-1;
		timer.tm_mday=BCD(com_rbuf[5]);
		timer.tm_hour=BCD(com_rbuf[6]);
		timer.tm_min=BCD(com_rbuf[7]);
		timer.tm_sec=BCD(com_rbuf[8]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):0;
		data->i13128=t3;
		
		return 0;
	}
	return -1;
}




int protocol_ZONGDAN_JuGuang_HeBei_info(struct acquisition_data *acq_data)
{
#define FUNC_MAX_NUM 21
	int status=0;
	static int num=0;
	static struct ZONGDAN_JuGuang_HeBei_info_data data;
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
	SYSLOG_DBG("protocol_ZONGDAN_JuGuang_HeBei_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21001",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	//ret=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func1,acq_data,devaddr,(void *)(&data),0,0);
	ret=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func1,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,0);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func2,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,1);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func3,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,2);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func4,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,3);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func5,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,4);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func6,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,5);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func7,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,6);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func8,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,7);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func9,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,8);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func10,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,9);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func11,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,10);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func12,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,11);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func13,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,12);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func14,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,13);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func15,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,14);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func16,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,15);	
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func17,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,16);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func18,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,17);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func19,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,18);
	ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func20,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,19);
	//ret+=instrument_protocols_sel(protocol_ZONGDAN_JuGuang_HeBei_info_func21,acq_data,devaddr,(void *)(&data),num%FUNC_MAX_NUM,20);

	if(ret==0 && (num%FUNC_MAX_NUM)==19)
	{
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,data.i12101,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,data.i12102,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,data.i12103,INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,data.i13004,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,data.i13005,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,data.i13003,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,data.i13116,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,data.i13008,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,data.i13007,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,data.i13119,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,data.i13120,INFOR_ARGUMENTS,&arg_n);

		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,data.i13105,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13101",data.i13101,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,data.i13110,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13107",data.i13107,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,data.i13104,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,data.i13108,INFOR_ARGUMENTS,&arg_n);

		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,data.i13130,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,data.i13129,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13128",data.i13128,0,INFOR_ARGUMENTS,&arg_n);

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



