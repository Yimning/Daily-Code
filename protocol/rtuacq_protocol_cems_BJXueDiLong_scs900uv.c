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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

int protocol_CEMS_BJXueDiLong_scs900uv(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,no2=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val=0;
	char *p,*q;
	char hex_pack[1024];
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x0A,0x0C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS-900UV CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS-900UV CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS-900UV CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS-900UV CEMS RECV:",com_rbuf,size);
	if((size>=29)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		so2=f.f;

		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		no=f.f;

		f.ch[3]=com_rbuf[15];
		f.ch[2]=com_rbuf[16];
		f.ch[1]=com_rbuf[17];
		f.ch[0]=com_rbuf[18];
		o2=f.f;

		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		no2=f.f;

		f.ch[3]=com_rbuf[23];
		f.ch[2]=com_rbuf[24];
		f.ch[1]=com_rbuf[25];
		f.ch[0]=com_rbuf[26];
		nox=f.f;

		if(nox==0) 
			nox=no*1.53+no2;

		val=com_rbuf[3];
		if(val!=0)
		{
			flag='D';
		}
		else
		{
			val=com_rbuf[6];
			if(val!=0)
			{
				flag='M';
			}
			else
			{
				val=com_rbuf[4];
				if(val>=3 && val<=7)
				{
					flag='C';
				}
				else
				{
					flag='N';
				}
			}
		}
		
		status=0;
	}
	else
	{
		nox=0;
		so2=0;
		o2=0;
		no2=0;
		no=0;
		flag='D';
		status=1;
	}

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
	
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,no,&arg_n);
	
	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,no2,&arg_n);
	
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,so2,&arg_n);
	
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);

	acq_data->dev_stat=flag;
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_CEMS_BJXueDiLong_scs900uv_SO2_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_scs900uv_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x0A,0x21);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS-900UV SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS-900UV SO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS-900UV SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS-900UV SO2 INFO RECV:",com_rbuf,size);
	if((size>=71)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		if(val!=0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else
		{
			val=com_rbuf[6];
			if(val!=0)
			{
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			}
			else
			{
				val=com_rbuf[4];
				if(val==0)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if(val>=3 && val<=7)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
			}
		}



		f.ch[3]=com_rbuf[27];
		f.ch[2]=com_rbuf[28];
		f.ch[1]=com_rbuf[29];
		f.ch[0]=com_rbuf[30];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[35];
		val<<=8;
		val+=com_rbuf[36];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[37];
		f.ch[2]=com_rbuf[38];
		f.ch[1]=com_rbuf[39];
		f.ch[0]=com_rbuf[40];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[41])+100;
		timer.tm_mon=BCD(com_rbuf[42])-1;
		timer.tm_mday=BCD(com_rbuf[43]);
		timer.tm_hour=BCD(com_rbuf[44]);
		timer.tm_min=BCD(com_rbuf[45]);
		timer.tm_sec=BCD(com_rbuf[46]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[47])+100;
		timer.tm_mon=BCD(com_rbuf[48])-1;
		timer.tm_mday=BCD(com_rbuf[49]);
		timer.tm_hour=BCD(com_rbuf[50]);
		timer.tm_min=BCD(com_rbuf[51]);
		timer.tm_sec=BCD(com_rbuf[52]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[61];
		f.ch[2]=com_rbuf[62];
		f.ch[1]=com_rbuf[63];
		f.ch[0]=com_rbuf[64];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[65];
		f.ch[2]=com_rbuf[66];
		f.ch[1]=com_rbuf[67];
		f.ch[0]=com_rbuf[68];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_scs900uv_NOx_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_scs900uv_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x2B,0x15);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS-900UV NOx INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS-900UV NOx protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS-900UV NOx data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS-900UV NOx INFO RECV:",com_rbuf,size);
	if((size>=47)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[13];
		f.ch[2]=com_rbuf[14];
		f.ch[1]=com_rbuf[15];
		f.ch[0]=com_rbuf[16];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[17])+100;
		timer.tm_mon=BCD(com_rbuf[18])-1;
		timer.tm_mday=BCD(com_rbuf[19]);
		timer.tm_hour=BCD(com_rbuf[20]);
		timer.tm_min=BCD(com_rbuf[21]);
		timer.tm_sec=BCD(com_rbuf[22]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[23])+100;
		timer.tm_mon=BCD(com_rbuf[24])-1;
		timer.tm_mday=BCD(com_rbuf[25]);
		timer.tm_hour=BCD(com_rbuf[26]);
		timer.tm_min=BCD(com_rbuf[27]);
		timer.tm_sec=BCD(com_rbuf[28]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[37];
		f.ch[2]=com_rbuf[38];
		f.ch[1]=com_rbuf[39];
		f.ch[0]=com_rbuf[40];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[41];
		f.ch[2]=com_rbuf[42];
		f.ch[1]=com_rbuf[43];
		f.ch[0]=com_rbuf[44];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_scs900uv_O2_info(struct acquisition_data *acq_data)
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
	char *p;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_scs900uv_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x00,0x54);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS-900UV O2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS-900UV O2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS-900UV O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS-900UV O2 INFO RECV:",com_rbuf,size);
	if((size>=173)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		p=&(com_rbuf[128]);
		
		val=p[3];
		val<<=8;
		val+=p[4];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[7];
		f.ch[0]=p[8];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(p[9])+100;
		timer.tm_mon=BCD(p[10])-1;
		timer.tm_mday=BCD(p[11]);
		timer.tm_hour=BCD(p[12]);
		timer.tm_min=BCD(p[13]);
		timer.tm_sec=BCD(p[14]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(p[15])+100;
		timer.tm_mon=BCD(p[16])-1;
		timer.tm_mday=BCD(p[17]);
		timer.tm_hour=BCD(p[18]);
		timer.tm_min=BCD(p[19]);
		timer.tm_sec=BCD(p[20]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[29];
		f.ch[2]=p[30];
		f.ch[1]=p[31];
		f.ch[0]=p[32];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[33];
		f.ch[2]=p[34];
		f.ch[1]=p[35];
		f.ch[0]=p[36];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		val=p[37];
		val<<=8;
		val+=p[38];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[39];
		f.ch[2]=p[40];
		f.ch[1]=p[41];
		f.ch[0]=p[42];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


static float setValue_TSP_BJXueDiLong_scs900uv(struct modbus_arg *modbusarg, float val, char *polcode)
{
	struct modbus_polcode_arg *mpolcodearg;
	int i=0;
	float valf=0;

	if(!modbusarg || !polcode) return -1;

	for(i=0;i<modbusarg->array_count ;i++)
	{
		mpolcodearg=&modbusarg->polcode_arg[i];
		if(!strcmp(mpolcodearg->polcode,polcode))
		{

			val=(val<5530) ? 5530 : val;
			
			valf=(val-5530)/(27648-5530);
			valf*=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			valf+=mpolcodearg->alarmMin;
			return valf;
		}

	}

	return 0;
}

int protocol_PLC_BJXueDiLong_scs900uv(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf[5];
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int smoke_flag;
	float speed,atm_press,PTC;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;
	struct modbus_polcode_arg * modbus_polcode_arg_tmp;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	devaddr=modbusarg->devaddr&0xffff;
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;

	smoke_flag=0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0FA4,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS-900UV PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS-900UV PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS-900UV PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS-900UV PLC RECV:",com_rbuf,size);
	if((size>=15)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf[0]=setValue_TSP_BJXueDiLong_scs900uv(modbusarg,val,"a01014");

		modbus_polcode_arg_tmp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,"a34013a");
		if(modbus_polcode_arg_tmp!=NULL && modbus_polcode_arg_tmp->enableFlag==1)
		{
			val=com_rbuf[5];
			val<<=8;
			val+=com_rbuf[6];
			valf[1]=setValue_TSP_BJXueDiLong_scs900uv(modbusarg,val,"a34013a");
			smoke_flag=1;				
		}
		else
		{
			modbus_polcode_arg_tmp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,"a34013");
			if(modbus_polcode_arg_tmp!=NULL && modbus_polcode_arg_tmp->enableFlag==1)
			{
				val=com_rbuf[5];
				val<<=8;
				val+=com_rbuf[6];
				valf[1]=setValue_TSP_BJXueDiLong_scs900uv(modbusarg,val,"a34013");
				smoke_flag=1;
			}
		}

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		valf[2]=setValue_TSP_BJXueDiLong_scs900uv(modbusarg,val,"a01011");

		val=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		valf[3]=setValue_TSP_BJXueDiLong_scs900uv(modbusarg,val,"a01012");

		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		valf[4]=setValue_TSP_BJXueDiLong_scs900uv(modbusarg,val,"a01013");

		modbus_polcode_arg_tmp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,"a01013");

		if(modbus_polcode_arg_tmp!=NULL)
		{
		SYSLOG_DBG("SCS-900UV %f,%f,%f,%f,%f,%f,%f,%d\n",valf[0],valf[1],valf[2],valf[3],valf[4],
		modbus_polcode_arg_tmp->alarmMax,modbus_polcode_arg_tmp->alarmMin,val);
		}

		if(PTC>0 && (valf[3]+273)>0 && (valf[4]+atm_press)>0)
		speed=PTC*valf[2]*sqrt(((valf[3]+273)/273)*(101325/(valf[4]+atm_press))*(2/1.2928));
		else
		speed=0;

		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		speed=0;
		valf[3]=0;
		valf[4]=0;
		status=1;
	}
	
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);

	if(smoke_flag!=0)
	{
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[1],&arg_n); //del by miles zhang conflict with 8009 TL_PMM180
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[1],&arg_n); 
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);//del by miles zhang conflict with 8009 TL_PMM180
	}

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


#if 0
int protocol_TSP_BJXueDiLong_HeBei_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_TSP_BJXueDiLong_HeBei_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x0F9B,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong TSP INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong TSP protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong TSP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong TSP INFO RECV:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		if((com_rbuf[3]&0x02)==0x02)
		{
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else if((com_rbuf[3]&0x08)==0x08)
		{
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else if((com_rbuf[3]&0x40)==0x40)
		{
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else if((com_rbuf[3]&0x01)==0x01)
		{
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
#endif
