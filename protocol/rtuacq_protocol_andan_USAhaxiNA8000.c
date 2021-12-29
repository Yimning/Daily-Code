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
#include "rtuacq_protocol_error_cache.h"



/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.06 Tues.
Description:protocol_CEMS_QiLanHuanBao_YCCEMS50
TX:01 03 00 04 00 02 85 CA 
RX:01 03 04
47 AE 41 05
FF FF
DataType and Analysis:
	(Float) 47 AE 41 05 = 8.33
*/
int protocol_ANDAN_USAhaxiNA8000(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float andan=0;
    	int devaddr=0;
	int cmd=0;
	int val=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, 0x04, 0x02);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "USAhaxi NA8000 COD", 1);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];		
		andan=f.f;
		status=0;
	}
	else
	{
		andan=0;
		status=1;
		
	}
	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

    	return arg_n;
}


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.06 Tues.
Description:protocol_CEMS_QiLanHuanBao_YCCEMS50
TX1:01 03 00 07 00 1A 75 C0 
RX1:01 03 34
47 AE 41 05
47 AE 29 05
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
7A E1 3F 54
70 00 3F 54
7A E1 3F 54
70 00 3F 54
00 01
00 00
FF FF

DataType and Analysis:
	(Float) 47 AE 41 05(CDAB) = 8.33 

TX2:01 03 00 74 00 07 44 12
RX2:01 03 0E
00 00 40 A0
00 00 42 48
00 00 00 00 00 00
FF FF


TX3:01 03 00 A4 00 01 C5 E9
RX3:01 03 02 00 02 FF FF


*/
int protocol_ANDAN_USAhaxiNA8000_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float andan=0;
    	int devaddr=0;
	int cmd=0;
	int val=0;
	struct tm timer;
	time_t t1,t2,t3;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
        acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_L,0,INFOR_ARGUMENTS,&arg_n);
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, 0x07, 0x1A);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "USAhaxi NA8000 COD INFO1", 1);
	if((size>=57)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{

		f.ch[3] = com_rbuf[5];
                f.ch[2] = com_rbuf[6];
		f.ch[1] = com_rbuf[3];
		f.ch[0] = com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3] = com_rbuf[9];
                f.ch[2] = com_rbuf[10];
		f.ch[1] = com_rbuf[7];
		f.ch[0] = com_rbuf[8];
		acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3] = com_rbuf[37];
                f.ch[2] = com_rbuf[38];
		f.ch[1] = com_rbuf[35];
		f.ch[0] = com_rbuf[36];
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3] = com_rbuf[41];
                f.ch[2] = com_rbuf[42];
		f.ch[1] = com_rbuf[39];
		f.ch[0] = com_rbuf[40];
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3] = com_rbuf[45];
                f.ch[2] = com_rbuf[46];
		f.ch[1] = com_rbuf[43];
		f.ch[0] = com_rbuf[44];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3] = com_rbuf[49];
                f.ch[2] = com_rbuf[50];
		f.ch[1] = com_rbuf[47];
		f.ch[0] = com_rbuf[48];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		val = com_rbuf[51];
                val <<= 8;
		val= com_rbuf[52];
		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,15,INFOR_ARGUMENTS,&arg_n);break;
			case 1: acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,30,INFOR_ARGUMENTS,&arg_n);break;
			case 2: acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,100,INFOR_ARGUMENTS,&arg_n);break;
			case 3: acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,1000,INFOR_ARGUMENTS,&arg_n);break;
		}
		status=0;
	}
	else
	{
		status=1;
	}

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, 0x74, 0x07);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "USAhaxi NA8000 COD INFO2", 1);
	if((size>=19)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{

		f.ch[3] = com_rbuf[5];
                f.ch[2] = com_rbuf[6];
		f.ch[1] = com_rbuf[3];
		f.ch[0] = com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3] = com_rbuf[9];
                f.ch[2] = com_rbuf[10];
		f.ch[1] = com_rbuf[7];
		f.ch[0] = com_rbuf[8];
		acqdata_set_value_flag(acq_data,"i13109",UNIT_L,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=BCD(com_rbuf[11])+100;
		timer.tm_mon=BCD(com_rbuf[12])-1;
		timer.tm_mday=BCD(com_rbuf[13]);
		timer.tm_hour=BCD(com_rbuf[14]);
		timer.tm_min=BCD(com_rbuf[15]);
		timer.tm_sec=BCD(com_rbuf[16]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}
	else
	{
		status=1;
	}

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, 0xA4, 0x01);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "USAhaxi NA8000 COD INFO3", 1);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val = com_rbuf[3];
		val <<= 8;
		val = com_rbuf[4];
		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
		//acqdata_set_value_flag(acq_data,"i121002",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		//acqdata_set_value_flag(acq_data,"i121003",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		status=0;
	}
	else
	{
		status=1;
	}

	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);//added by miles zhang

    	return arg_n;
}



