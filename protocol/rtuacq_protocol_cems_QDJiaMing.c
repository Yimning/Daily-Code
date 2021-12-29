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

int protocol_CEMS_QDJiaMing(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float so2,nox,o2;
	int val=0;
	char check[7];
	char *p;
	char flag;
	unsigned int crc=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x50;
	com_tbuf[1]=0x50;
	com_tbuf[2]=0x50;
	com_tbuf[3]=0x50;
	com_tbuf[4]=0x50;
	size=5;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDJiaMing CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDJiaMing CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDJiaMing CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDJiaMing CEMS RECV:",com_rbuf,size);
	check[0]=0x78;
	check[1]=0x78;
	check[2]=0x78;
	check[3]=0x78;
	check[4]=0x78;
	check[5]=0x50;
	check[6]=0x00;
	p=memstr(com_rbuf, size, check);
	crc=CRC16_modbus(p, 86);
	if((size>=88)&&(p!=NULL)&&(p[86]==(crc&0xFF))&&(p[87]==((crc>>8)&0xFF)))
	{
		switch(p[6])
		{
			case 0:	flag='M';break;
			case 1:	flag='z';break;
			case 2:	flag='N';break;
			case 3:	flag='C';break;
			default:	flag='N';break;
		}
	
		val=p[8];
		val<<=8;
		val+=p[9];
		if((val&0x8000)==0x8000)
			so2=(val-0x8000)*(-1)/2.0;
		else
			so2=val/2.0;

		val=p[10];
		val<<=8;
		val+=p[11];
		if((val&0x8000)==0x8000)
			nox=(val-0x8000)*(-1)/2.0;
		else
			nox=val/2.0;

		val=p[12];
		val<<=8;
		val+=p[13];
		o2=val/10.0;
		
		status=0;
	}
	else
	{
		so2=0;
		nox=0;
		o2=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acq_data->dev_stat = flag;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_CEMS_QDJiaMing_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	short int val=0;
	int devaddr=0;
	union uf f;
	char check[7];
	char *p;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_QDJiaMing_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x50;
	com_tbuf[1]=0x50;
	com_tbuf[2]=0x50;
	com_tbuf[3]=0x50;
	com_tbuf[4]=0x50;
	size=5;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDJiaMing SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDJiaMing SO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDJiaMing SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDJiaMing SO2 INFO RECV:",com_rbuf,size);
	check[0]=0x78;
	check[1]=0x78;
	check[2]=0x78;
	check[3]=0x78;
	check[4]=0x78;
	check[5]=0x50;
	check[6]=0x00;
	p=memstr(com_rbuf, size, check);
	crc=CRC16_modbus(p, 86);
	if((size>=88)&&(p!=NULL)&&(p[86]==(crc&0xFF))&&(p[87]==((crc>>8)&0xFF)))
	{
		switch(p[6])
		{
			case 0: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 1: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 2: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 3: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			default: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}

		if(p[7]==0)
		{
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);

			switch(p[7])
			{
				case 1: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 2: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 3: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 4: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				case 5: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 6: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				default: acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
		}
	
		val=p[18];
		val<<=8;
		val+=p[19];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
	
		val=p[20];
		val<<=8;
		val+=p[21];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		val=p[22];
		val<<=8;
		val+=p[23];
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[24])-1;
		timer.tm_mday=BCD(p[25]);
		timer.tm_hour=BCD(p[26]);
		timer.tm_min=BCD(p[27]);
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):0;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=p[28];
		val<<=8;
		val+=p[29];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_MG_M3,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[30];
		val<<=8;
		val+=p[31];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_MG_M3,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[32];
		val<<=8;
		val+=p[33];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[34];
		val<<=8;
		val+=p[35];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,val/10.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_QDJiaMing_NOx_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	short int val=0;
	int devaddr=0;
	union uf f;
	char check[7];
	char *p;
	unsigned int crc;


	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_QDJiaMing_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x50;
	com_tbuf[1]=0x50;
	com_tbuf[2]=0x50;
	com_tbuf[3]=0x50;
	com_tbuf[4]=0x50;
	size=5;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDJiaMing NOx INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDJiaMing NOx protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDJiaMing NOx data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDJiaMing NOx INFO RECV:",com_rbuf,size);
	check[0]=0x78;
	check[1]=0x78;
	check[2]=0x78;
	check[3]=0x78;
	check[4]=0x78;
	check[5]=0x50;
	check[6]=0x00;
	p=memstr(com_rbuf, size, check);
	crc=CRC16_modbus(p, 86);
	if((size>=88)&&(p!=NULL)&&(p[86]==(crc&0xFF))&&(p[87]==((crc>>8)&0xFF)))
	{
		val=p[40];
		val<<=8;
		val+=p[41];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
	
		val=p[42];
		val<<=8;
		val+=p[43];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		val=p[44];
		val<<=8;
		val+=p[45];
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[46])-1;
		timer.tm_mday=BCD(p[47]);
		timer.tm_hour=BCD(p[48]);
		timer.tm_min=BCD(p[49]);
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):0;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=p[50];
		val<<=8;
		val+=p[51];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_MG_M3,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[52];
		val<<=8;
		val+=p[53];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_MG_M3,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[54];
		val<<=8;
		val+=p[55];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[56];
		val<<=8;
		val+=p[57];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,val/10.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_QDJiaMing_O2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	short int val=0;
	int devaddr=0;
	union uf f;
	char check[7];
	char *p;
	unsigned int crc;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_QDJiaMing_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x50;
	com_tbuf[1]=0x50;
	com_tbuf[2]=0x50;
	com_tbuf[3]=0x50;
	com_tbuf[4]=0x50;
	size=5;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"QDJiaMing O2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("QDJiaMing O2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("QDJiaMing O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"QDJiaMing O2 INFO RECV:",com_rbuf,size);
	check[0]=0x78;
	check[1]=0x78;
	check[2]=0x78;
	check[3]=0x78;
	check[4]=0x78;
	check[5]=0x50;
	check[6]=0x00;
	p=memstr(com_rbuf, size, check);
	crc=CRC16_modbus(p, 86);
	if((size>=88)&&(p!=NULL)&&(p[86]==(crc&0xFF))&&(p[87]==((crc>>8)&0xFF)))
	{
		val=p[62];
		val<<=8;
		val+=p[63];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val/100.0,INFOR_ARGUMENTS,&arg_n);
	
		val=p[64];
		val<<=8;
		val+=p[65];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[66];
		val<<=8;
		val+=p[67];
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[68])-1;
		timer.tm_mday=BCD(p[69]);
		timer.tm_hour=BCD(p[70]);
		timer.tm_min=BCD(p[71]);
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):0;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=p[72];
		val<<=8;
		val+=p[73];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_MG_M3,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[74];
		val<<=8;
		val+=p[75];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_MG_M3,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[76];
		val<<=8;
		val+=p[77];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,val/100.0,INFOR_ARGUMENTS,&arg_n);

		val=p[78];
		val<<=8;
		val+=p[79];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,val/100.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
