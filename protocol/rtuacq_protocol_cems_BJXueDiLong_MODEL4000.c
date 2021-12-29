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


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.12 Mon.
Description:protocol_CEMS_BJXueDiLong_MODEL4000
TX:01 03 00 00 00 24 45 D1
RX:01 03 48 
41 20 00 00 
42 48 00 00
41 A0 00 00
42 48 00 00 
41 80 00 00 
00 00 00 00 
00 00 00 00 
42 48 00 00 
41 80 00 00 
42 48 00 00 
41 80 00 00 
42 48 00 00 
41 80 00 00 
42 48 00 00 
41 80 00 00 
42 48 00 00 
41 80 00 00 
42 48 00 00 
1E 37
DataType and Analysis:
	(Float) 41 20 00 00  = 10.0
*/
//rtuacq_protocol_cems_BJXueDiLong_scs900uv.c
int protocol_CEMS_BJXueDiLong_MODEL4000(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,no2=0,co=0,co2=0;
	float nox_ori=0,so2_ori=0,o2_ori=0,no_ori=0,no2_ori=0,co_ori=0,co2_ori=0 ,hcl=0,hcl_ori=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd=0;
	char *p,*q;
	char hex_pack[1024];

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x24);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong MODEL4000 HeBei CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong MODEL4000 HeBei CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong MODEL4000 HeBei CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong MODEL4000 HeBei CEMS RECV:",com_rbuf,size);
	if((size>=77)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		so2=f.f;


		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[8];
		f.ch[3]=com_rbuf[7];
		so2_ori=f.f;
		
		f.ch[0]=com_rbuf[14];
		f.ch[1]=com_rbuf[13];
		f.ch[2]=com_rbuf[12];
		f.ch[3]=com_rbuf[11];
		no=f.f;
		
		f.ch[0]=com_rbuf[18];
		f.ch[1]=com_rbuf[17];
		f.ch[2]=com_rbuf[16];
		f.ch[3]=com_rbuf[15];
		no_ori=f.f;

		f.ch[0]=com_rbuf[22];
		f.ch[1]=com_rbuf[21];
		f.ch[2]=com_rbuf[20];
		f.ch[3]=com_rbuf[19];
		no2=f.f;
		
		f.ch[0]=com_rbuf[26];
		f.ch[1]=com_rbuf[25];
		f.ch[2]=com_rbuf[24];
		f.ch[3]=com_rbuf[23];
		no2_ori=f.f;

		f.ch[0]=com_rbuf[38];
		f.ch[1]=com_rbuf[37];
		f.ch[2]=com_rbuf[36];
		f.ch[3]=com_rbuf[35];
		hcl=f.f;
		
		f.ch[0]=com_rbuf[42];
		f.ch[1]=com_rbuf[41];
		f.ch[2]=com_rbuf[40];
		f.ch[3]=com_rbuf[39];
		hcl_ori=f.f;


		f.ch[0]=com_rbuf[62];
		f.ch[1]=com_rbuf[61];
		f.ch[2]=com_rbuf[60];
		f.ch[3]=com_rbuf[59];
		co=f.f;
		
		f.ch[0]=com_rbuf[66];
		f.ch[1]=com_rbuf[65];
		f.ch[2]=com_rbuf[64];
		f.ch[3]=com_rbuf[63];
		co_ori=f.f;

		f.ch[0]=com_rbuf[70];
		f.ch[1]=com_rbuf[69];
		f.ch[2]=com_rbuf[68];
		f.ch[3]=com_rbuf[67];
		co2=f.f;
		
		f.ch[0]=com_rbuf[74];
		f.ch[1]=com_rbuf[73];
		f.ch[2]=com_rbuf[72];
		f.ch[3]=com_rbuf[71];
		co2_ori=f.f;

		nox=NO_and_NO2_to_NOx(no, no2);
		
		status=0;
	}
	else
	{
		so2=0;
		so2_ori=0;
		no=0;
		no_ori=0;
		no2=0;
		no2_ori=0;
		hcl=0;
		hcl_ori=0;
		co=0;
		co_ori=0;
		co2=0;
		co2_ori=0;
		nox=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,so2,so2_ori,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value_orig(acq_data,"a21003",UNIT_MG_M3,no,no_ori,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21004",UNIT_MG_M3,no2,no2_ori,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21024a",UNIT_MG_M3,hcl,&arg_n);
	acqdata_set_value_orig(acq_data,"a21024",UNIT_MG_M3,hcl,hcl_ori,&arg_n);
	acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value_orig(acq_data,"a21005",UNIT_MG_M3,co,co_ori,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value_orig(acq_data,"a05001",UNIT_MG_M3,co2,co2_ori,&arg_n);

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;

}

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.12 Mon.
Description:protocol_CEMS_BJXueDiLong_MODEL4000_STATUS_info
TX1:01 03 00 64 00 01 C5 D5
RX1:01 03 02 
00 00 
1E 37
DataType and Analysis:
	(int) 00 00  = 0
*/
//rtuacq_protocol_cems_BJXueDiLong_scs900uv.c

int protocol_CEMS_BJXueDiLong_MODEL4000_STATUS_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,no2=0,co=0,co2=0;
	float nox_ori=0,so2_ori=0,o2_ori=0,no_ori=0,no2_ori=0,co_ori=0,co2_ori=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd=0;
	char *p,*q;
	char hex_pack[1024];
	int val = 0;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	float valf=0;
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

        memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x64,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_CEMS_BJXueDiLong_MODEL4000_INFOR_STATUS HeBei CEMS INFOR_STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);

	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_INFOR_STATUS HeBei CEMS protocol,CEMS INFOR_STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_CEMS_BJXueDiLong_MODEL4000_INFOR_STATUS HeBei CEMS INFOR_STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_CEMS_BJXueDiLong_MODEL4000_INFOR_STATUS HeBei CEMS INFOR_STATUS RECV:",com_rbuf,size);

	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val =  com_rbuf[3];
		val <<= 8;
		val +=com_rbuf[4];
		switch(val){
			case 0:acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1:acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2:acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 3:acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 4:acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			default:acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		//acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		
	} 

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.12 Mon.
Description:protocol_CEMS_BJXueDiLong_MODEL4000_SO2_info
TX:01 03 00 C8 00 1C C5 FD
RX:01 03 28 
42 C8 00 00 
20 20 10 16 08 00 00 00 
00 00 00 00 
00 00 00 00 
3F 80 00 00 
00 00 00 00 
20 20 10 16 09 00 00 00 
42 C8 00 00 
43 48 00 00 
3F 80 00 00 
00 00 00 00 
FF FF FF FF
1E 37
DataType and Analysis:
	(Float) 42 C8 00 00  = 100.0
*/
int protocol_CEMS_BJXueDiLong_MODEL4000_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,no2=0,co=0,co2=0;
	float nox_ori=0,so2_ori=0,o2_ori=0,no_ori=0,no2_ori=0,co_ori=0,co2_ori=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd=0;
	char *p,*q;
	char hex_pack[1024];
	int val = 0;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	float valf=0;
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xC8,0x1C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong_MODEL4000 SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong_MODEL4000 SO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong_MODEL4000 SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong_MODEL4000 SO2 INFO RECV:",com_rbuf,size);
	if((size>=61)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{      
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

	        val = getUInt16Value(com_rbuf, 7, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[9]);
		day=BCD(com_rbuf[10]);
		hour=BCD(com_rbuf[11]);
		min=BCD(com_rbuf[12]);
		sec=BCD(com_rbuf[14]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[18];
		f.ch[1]=com_rbuf[17];
		f.ch[2]=com_rbuf[16];
		f.ch[3]=com_rbuf[15];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[22];
		f.ch[1]=com_rbuf[21];
		f.ch[2]=com_rbuf[20];
		f.ch[3]=com_rbuf[19];
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	

		f.ch[0]=com_rbuf[26];
		f.ch[1]=com_rbuf[25];
		f.ch[2]=com_rbuf[24];
		f.ch[3]=com_rbuf[23];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	
		
		f.ch[0]=com_rbuf[30];
		f.ch[1]=com_rbuf[29];
		f.ch[2]=com_rbuf[28];
		f.ch[3]=com_rbuf[27];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);	
		
	        val = getUInt16Value(com_rbuf, 31, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[33]);
		day=BCD(com_rbuf[34]);
		hour=BCD(com_rbuf[35]);
		min=BCD(com_rbuf[36]);
		sec=BCD(com_rbuf[38]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		f.ch[0]=com_rbuf[42];
		f.ch[1]=com_rbuf[41];
		f.ch[2]=com_rbuf[40];
		f.ch[3]=com_rbuf[39];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[46];
		f.ch[1]=com_rbuf[45];
		f.ch[2]=com_rbuf[44];
		f.ch[3]=com_rbuf[43];
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		f.ch[0]=com_rbuf[50];
		f.ch[1]=com_rbuf[49];
		f.ch[2]=com_rbuf[48];
		f.ch[3]=com_rbuf[47];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[54];
		f.ch[1]=com_rbuf[53];
		f.ch[2]=com_rbuf[52];
		f.ch[3]=com_rbuf[51];
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

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.12 Mon.
Description:protocol_CEMS_BJXueDiLong_MODEL4000_NO_info
TX:01 03 00 E4 00 1C 40 34
RX:01 03 28 
42 C8 00 00 
20 20 10 16 08 00 00 00 
00 00 00 00 
00 00 00 00 
3F 80 00 00 
00 00 00 00 
20 20 10 16 09 00 00 00 
42 C8 00 00 
43 48 00 00 
3F 80 00 00 
00 00 00 00 
FF FF FF FF
1E 37
DataType and Analysis:
	(Float) 42 C8 00 00  = 100.0
*/

int protocol_CEMS_BJXueDiLong_MODEL4000_NO_info(struct acquisition_data *acq_data)
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
	int cmd=0;
	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_NO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21003"))
		return 0;
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xE4,0x1C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong_MODEL4000 NO INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong_MODEL4000 NO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong_MODEL4000 NO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong_MODEL4000 NO INFO RECV:",com_rbuf,size);
	if((size>=61)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{      
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);
	
	        val = getUInt16Value(com_rbuf, 7, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[9]);
		day=BCD(com_rbuf[10]);
		hour=BCD(com_rbuf[11]);
		min=BCD(com_rbuf[12]);
		sec=BCD(com_rbuf[14]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[18];
		f.ch[1]=com_rbuf[17];
		f.ch[2]=com_rbuf[16];
		f.ch[3]=com_rbuf[15];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[22];
		f.ch[1]=com_rbuf[21];
		f.ch[2]=com_rbuf[20];
		f.ch[3]=com_rbuf[19];
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	

		f.ch[0]=com_rbuf[26];
		f.ch[1]=com_rbuf[25];
		f.ch[2]=com_rbuf[24];
		f.ch[3]=com_rbuf[23];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	
		
		f.ch[0]=com_rbuf[30];
		f.ch[1]=com_rbuf[29];
		f.ch[2]=com_rbuf[28];
		f.ch[3]=com_rbuf[27];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);	
		
	        val = getUInt16Value(com_rbuf, 31, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[33]);
		day=BCD(com_rbuf[34]);
		hour=BCD(com_rbuf[35]);
		min=BCD(com_rbuf[36]);
		sec=BCD(com_rbuf[38]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		f.ch[0]=com_rbuf[42];
		f.ch[1]=com_rbuf[41];
		f.ch[2]=com_rbuf[40];
		f.ch[3]=com_rbuf[39];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[46];
		f.ch[1]=com_rbuf[45];
		f.ch[2]=com_rbuf[44];
		f.ch[3]=com_rbuf[43];
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		f.ch[0]=com_rbuf[50];
		f.ch[1]=com_rbuf[49];
		f.ch[2]=com_rbuf[48];
		f.ch[3]=com_rbuf[47];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[54];
		f.ch[1]=com_rbuf[53];
		f.ch[2]=com_rbuf[52];
		f.ch[3]=com_rbuf[51];
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


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.12 Mon.
Description:protocol_CEMS_BJXueDiLong_MODEL4000_NO2_info
TX:01 03 01 9E 00 1A E0 13
RX:01 03 1A 
42 C8 00 00 
20 20 10 16 08 00 00 00 
00 00 00 00 
00 00 00 00 
3F 80 00 00 
00 00 00 00 
20 20 10 16 09 00 00 00 
42 C8 00 00 
43 48 00 00 
3F 80 00 00 
00 00 00 00 
1E 37
DataType and Analysis:
	(Float) 41 20 00 00  = 10.0
*/
int protocol_CEMS_BJXueDiLong_MODEL4000_NO2_info(struct acquisition_data *acq_data)
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
	int cmd=0;
	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_NO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21004"))
		return 0;
	acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x19E,0x1A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong_MODEL4000 NO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong_MODEL4000 NO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong_MODEL4000 NO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong_MODEL4000 NO2 INFO RECV:",com_rbuf,size);
	if((size>=57)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{      
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);
	
	        val = getUInt16Value(com_rbuf, 7, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[9]);
		day=BCD(com_rbuf[10]);
		hour=BCD(com_rbuf[11]);
		min=BCD(com_rbuf[12]);
		sec=BCD(com_rbuf[14]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[18];
		f.ch[1]=com_rbuf[17];
		f.ch[2]=com_rbuf[16];
		f.ch[3]=com_rbuf[15];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[22];
		f.ch[1]=com_rbuf[21];
		f.ch[2]=com_rbuf[20];
		f.ch[3]=com_rbuf[19];
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	

		f.ch[0]=com_rbuf[26];
		f.ch[1]=com_rbuf[25];
		f.ch[2]=com_rbuf[24];
		f.ch[3]=com_rbuf[23];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	
		
		f.ch[0]=com_rbuf[30];
		f.ch[1]=com_rbuf[29];
		f.ch[2]=com_rbuf[28];
		f.ch[3]=com_rbuf[27];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);	
		
	        val = getUInt16Value(com_rbuf, 31, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[33]);
		day=BCD(com_rbuf[34]);
		hour=BCD(com_rbuf[35]);
		min=BCD(com_rbuf[36]);
		sec=BCD(com_rbuf[38]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		f.ch[0]=com_rbuf[42];
		f.ch[1]=com_rbuf[41];
		f.ch[2]=com_rbuf[40];
		f.ch[3]=com_rbuf[39];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[46];
		f.ch[1]=com_rbuf[45];
		f.ch[2]=com_rbuf[44];
		f.ch[3]=com_rbuf[43];
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		f.ch[0]=com_rbuf[50];
		f.ch[1]=com_rbuf[49];
		f.ch[2]=com_rbuf[48];
		f.ch[3]=com_rbuf[47];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[54];
		f.ch[1]=com_rbuf[53];
		f.ch[2]=com_rbuf[52];
		f.ch[3]=com_rbuf[51];
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


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.12 Mon.
Description:protocol_CEMS_BJXueDiLong_MODEL4000_HCL_info
TX:01 03 01 36 00 1A 25 F3
RX:01 03 34 43 48 00 00 20 21 12 29 07 05 05 00 00 00 00 00 3d dd 63 88 bd e1 47 ae 3d 5d 63 88 
20 21 12 24 13 54 31 00 43 35 00 00 43 1a f8 52 3f 95 7f fa 00 00 00 00 b1 db
DataType and Analysis:
	(Float) 41 20 00 00  = 10.0
*/
int protocol_CEMS_BJXueDiLong_MODEL4000_HCL_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;

	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_HCL_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21024"))
		return 0;
	acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	cmd = 0x03;
	regpos = 0x136;
	regcnt = 0x01A;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong_MODEL4000 HCL INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong_MODEL4000 HCL protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong_MODEL4000 HCL data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong_MODEL4000 HCL INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{    
		valf = getFloatValue(p, 3 , dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	
	        val = getUInt16Value(p, 7, INT_AB);
 		year=BCD(val);
		mon=BCD(p[9]);
		day=BCD(p[10]);
		hour=BCD(p[11]);
		min=BCD(p[12]);
		sec=BCD(p[14]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 15 , dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 19 , dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);	

		valf = getFloatValue(p, 23 , dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);	
		
		valf = getFloatValue(p, 27 , dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);	
		
	        val = getUInt16Value(p, 31, INT_AB);
 		year=BCD(val);
		mon=BCD(p[33]);
		day=BCD(p[34]);  
		hour=BCD(p[35]); 
		min=BCD(p[36]);
		sec=BCD(p[38]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		valf = getFloatValue(p, 39 , dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 43 , dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 47 , dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 51 , dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;  
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.12 Mon.
Description:protocol_CEMS_BJXueDiLong_MODEL4000_CO_info
TX:01 03 01 84 00 1A 85 D4
RX:01 03 1A 
42 C8 00 00 
20 20 10 16 08 00 00 00 
00 00 00 00 
00 00 00 00 
3F 80 00 00 
00 00 00 00 
20 20 10 16 09 00 00 00 
42 C8 00 00 
43 48 00 00 
3F 80 00 00 
00 00 00 00 
1E 37
DataType and Analysis:
	(Float) 41 20 00 00  = 10.0
*/
int protocol_CEMS_BJXueDiLong_MODEL4000_CO_info(struct acquisition_data *acq_data)
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
	int cmd=0;
	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_CO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a21005"))
		return 0;
	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x184,0x1A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong_MODEL4000 CO INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong_MODEL4000 CO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong_MODEL4000 CO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong_MODEL4000 CO INFO RECV:",com_rbuf,size);
	if((size>=57)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{      
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);
	
	        val = getUInt16Value(com_rbuf, 7, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[9]);
		day=BCD(com_rbuf[10]);
		hour=BCD(com_rbuf[11]);
		min=BCD(com_rbuf[12]);
		sec=BCD(com_rbuf[14]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[18];
		f.ch[1]=com_rbuf[17];
		f.ch[2]=com_rbuf[16];
		f.ch[3]=com_rbuf[15];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[22];
		f.ch[1]=com_rbuf[21];
		f.ch[2]=com_rbuf[20];
		f.ch[3]=com_rbuf[19];
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	

		f.ch[0]=com_rbuf[26];
		f.ch[1]=com_rbuf[25];
		f.ch[2]=com_rbuf[24];
		f.ch[3]=com_rbuf[23];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	
		
		f.ch[0]=com_rbuf[30];
		f.ch[1]=com_rbuf[29];
		f.ch[2]=com_rbuf[28];
		f.ch[3]=com_rbuf[27];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);	
		
	        val = getUInt16Value(com_rbuf, 31, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[33]);
		day=BCD(com_rbuf[34]);
		hour=BCD(com_rbuf[35]);
		min=BCD(com_rbuf[36]);
		sec=BCD(com_rbuf[38]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		f.ch[0]=com_rbuf[42];
		f.ch[1]=com_rbuf[41];
		f.ch[2]=com_rbuf[40];
		f.ch[3]=com_rbuf[39];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[46];
		f.ch[1]=com_rbuf[45];
		f.ch[2]=com_rbuf[44];
		f.ch[3]=com_rbuf[43];
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		f.ch[0]=com_rbuf[50];
		f.ch[1]=com_rbuf[49];
		f.ch[2]=com_rbuf[48];
		f.ch[3]=com_rbuf[47];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[54];
		f.ch[1]=com_rbuf[53];
		f.ch[2]=com_rbuf[52];
		f.ch[3]=com_rbuf[51];
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


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.12 Mon.
Description:protocol_CEMS_BJXueDiLong_MODEL4000_CO2_info
TX:01 03 01 1C 00 1A 04 3B 
RX:01 03 1A 
42 C8 00 00 
20 20 10 16 08 00 00 00 
00 00 00 00 
00 00 00 00 
3F 80 00 00 
00 00 00 00 
20 20 10 16 09 00 00 00 
42 C8 00 00 
43 48 00 00 
3F 80 00 00 
00 00 00 00 
1E 37
DataType and Analysis:
	(Float) 41 20 00 00  = 10.0
*/
int protocol_CEMS_BJXueDiLong_MODEL4000_CO2_info(struct acquisition_data *acq_data)
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
	int cmd=0;
	union uf f;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL4000_CO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	if(!isPolcodeEnable(modbusarg, "a05001"))
		return 0;
	acqdata_set_value_flag(acq_data,"a05001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x11C,0x1A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong_MODEL4000 CO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong_MODEL4000 CO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong_MODEL4000 CO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong_MODEL4000 CO2 INFO RECV:",com_rbuf,size);
	if((size>=57)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{      
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);
	
	        val = getUInt16Value(com_rbuf, 7, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[9]);
		day=BCD(com_rbuf[10]);
		hour=BCD(com_rbuf[11]);
		min=BCD(com_rbuf[12]);
		sec=BCD(com_rbuf[14]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[18];
		f.ch[1]=com_rbuf[17];
		f.ch[2]=com_rbuf[16];
		f.ch[3]=com_rbuf[15];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[22];
		f.ch[1]=com_rbuf[21];
		f.ch[2]=com_rbuf[20];
		f.ch[3]=com_rbuf[19];
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	

		f.ch[0]=com_rbuf[26];
		f.ch[1]=com_rbuf[25];
		f.ch[2]=com_rbuf[24];
		f.ch[3]=com_rbuf[23];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);	
		
		f.ch[0]=com_rbuf[30];
		f.ch[1]=com_rbuf[29];
		f.ch[2]=com_rbuf[28];
		f.ch[3]=com_rbuf[27];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);	
		
	        val = getUInt16Value(com_rbuf, 31, INT_AB);
 		year=BCD(val);
		mon=BCD(com_rbuf[33]);
		day=BCD(com_rbuf[34]);
		hour=BCD(com_rbuf[35]);
		min=BCD(com_rbuf[36]);
		sec=BCD(com_rbuf[38]);
		t3=getTimeValue(year,mon, day,hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		f.ch[0]=com_rbuf[42];
		f.ch[1]=com_rbuf[41];
		f.ch[2]=com_rbuf[40];
		f.ch[3]=com_rbuf[39];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[46];
		f.ch[1]=com_rbuf[45];
		f.ch[2]=com_rbuf[44];
		f.ch[3]=com_rbuf[43];
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		f.ch[0]=com_rbuf[50];
		f.ch[1]=com_rbuf[49];
		f.ch[2]=com_rbuf[48];
		f.ch[3]=com_rbuf[47];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[54];
		f.ch[1]=com_rbuf[53];
		f.ch[2]=com_rbuf[52];
		f.ch[3]=com_rbuf[51];
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

