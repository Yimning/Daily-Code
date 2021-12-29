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

int protocol_COD_HBHuaHouTianCheng(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float cod=0,orig=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x7532,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBHuaHouTianCheng COD SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBHuaHouTianCheng COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBHuaHouTianCheng COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBHuaHouTianCheng COD RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x04);
	if(p!=NULL)
	{
		f.ch[3]=p[3];
		f.ch[2]=p[4];
		f.ch[1]=p[5];
		f.ch[0]=p[6];
		cod=f.f;

		orig=getFloatValue(p, 7,  FLOAT_ABCD);
		
		status=0;
	}
	else
	{
		cod=0;
		orig=0;
		status=1;
	}

	SYSLOG_DBG("YUIKI: cod=%.2f,orig=%.2f,status=%d\n",cod,orig,status);

	acqdata_set_value_orig(acq_data,"w01018",UNIT_MG_L,cod,orig,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);

      SYSLOG_DBG("YUIKI: cod=%.2f,orig=%.2f,status=%d,com_f[0]=%.2f\n",cod,orig,status,acq_data->com_f[0]);
	
	return arg_n;
}



int protocol_COD_HBHuaHouTianCheng_info(struct acquisition_data *acq_data)
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
	char *p=NULL;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_COD_HBHuaHouTianCheng_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x7534,0x38);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBHuaHouTianCheng COD INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBHuaHouTianCheng COD INFO protocol,INFO1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBHuaHouTianCheng COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBHuaHouTianCheng COD INFO RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x38);
	if(p!=NULL)
	{
		val=p[35];
		val<<=8;
		val+=p[36];

		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 7: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 9: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val=p[37];
		val<<=8;
		val+=p[38];
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
			case 0: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,11,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 7: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 9: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 10: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 11: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n); break;
			case 12: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,8,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		valf=getFloatValue(p, 3,  FLOAT_ABCD);
		acqdata_set_value_flag(acq_data,"i13115",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		f.ch[3]=p[47];
		f.ch[2]=p[48];
		f.ch[1]=p[49];
		f.ch[0]=p[50];
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[51];
		f.ch[2]=p[52];
		f.ch[1]=p[53];
		f.ch[0]=p[54];
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=p[58]+100;
		timer.tm_mon=p[60]-1;
		timer.tm_mday=p[62];
		timer.tm_hour=p[64];
		timer.tm_min=p[66];
		timer.tm_sec=p[68];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[73];
		f.ch[2]=p[74];
		f.ch[1]=p[75];
		f.ch[0]=p[76];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[77];
		f.ch[2]=p[78];
		f.ch[1]=p[79];
		f.ch[0]=p[80];
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=p[84]+100;
		timer.tm_mon=p[86]-1;
		timer.tm_mday=p[88];
		timer.tm_hour=p[90];
		timer.tm_min=p[92];
		timer.tm_sec=p[94];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[95];
		f.ch[2]=p[96];
		f.ch[1]=p[97];
		f.ch[0]=p[98];
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[99];
		f.ch[2]=p[100];
		f.ch[1]=p[101];
		f.ch[0]=p[102];
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		val=p[109];
		val<<=8;
		val+=p[110];
		acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[111];
		f.ch[2]=p[112];
		f.ch[1]=p[113];
		f.ch[0]=p[114];
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x75F8,0x13);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBHuaHouTianCheng COD INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBHuaHouTianCheng COD INFO protocol,INFO2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBHuaHouTianCheng COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBHuaHouTianCheng COD INFO RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x13);
	if(p!=NULL)
	{
		f.ch[3]=p[3];
		f.ch[2]=p[4];
		f.ch[1]=p[5];
		f.ch[0]=p[6];
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[11];
		f.ch[2]=p[12];
		f.ch[1]=p[13];
		f.ch[0]=p[14];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[15];
		f.ch[2]=p[16];
		f.ch[1]=p[17];
		f.ch[0]=p[18];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 19, INT_AB);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(p[22]+2000, p[24], p[26], p[28], p[30], p[32]);
		acqdata_set_value_flag(acq_data,"i13128",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 33, FLOAT_ABCD);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 37, FLOAT_ABCD);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		sleep(1);
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x03,0x75F8,0x0A);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBHuaHouTianCheng COD INFO SEND2 again:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("HBHuaHouTianCheng COD INFO protocol,INFO2: read again device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("HBHuaHouTianCheng COD INFO data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBHuaHouTianCheng COD INFO RECV2 again:",com_rbuf,size);
		p=modbus_crc_check(com_rbuf, size, devaddr, 0x03, 0x0A);
		if(p!=NULL)
		{
			f.ch[3]=p[3];
			f.ch[2]=p[4];
			f.ch[1]=p[5];
			f.ch[0]=p[6];
			acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

			f.ch[3]=p[11];
			f.ch[2]=p[12];
			f.ch[1]=p[13];
			f.ch[0]=p[14];
			acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

			f.ch[3]=p[15];
			f.ch[2]=p[16];
			f.ch[1]=p[17];
			f.ch[0]=p[18];
			acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

			f.ch[3]=p[19];
			f.ch[2]=p[20];
			f.ch[1]=p[21];
			f.ch[0]=p[22];
			acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
			
			status=0;
		}
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

