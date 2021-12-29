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
Email:1148967988@qq.com
Create Time:2021.07.03 Sat.
Description:protocol_sampler_AHWanYi
TX:	:01040BBA000135(ascii)
RX:	:0104020001FF

*/


int protocol_sampler_AHWanYi(struct acquisition_data *acq_data)
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
	int val=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x0BBA,0x01);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"sampler AHWanYi SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("sampler AHWanYi  protocol,sampler AHWanYi : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("sampler AHWanYi  data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"sampler AHWanYi  RECV:",com_rbuf,size);

	if(size>=7 && modbus_ascii_pack_check(com_rbuf, devaddr, cmd))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		val=p[3];
		val<<=8;
		val+=p[4];
		status=0;
		
	}
	else
	{
		status=1;
	}  	
	
	ret=acqdata_set_value(acq_data,"i42001",UNIT_NONE,(float)val,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
	return arg_n;
}



/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.03 Sat.
Description:protocol_sampler_AHWanYi_info
TX1:	:01040BB8000D2B(ascii)
RX1:	:01041A0002000000010001000200030011000147AE4105000147AE4105FF



TX2:	:01030BB8000F2A(ascii)
RX2:	:01031E000000300020001800300009001700000064000000010001000301F40001FF
*/
int protocol_sampler_AHWanYi_info(struct acquisition_data *acq_data)
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
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
        acqdata_set_value_flag(acq_data,"SAMPLER",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x0BB8,0x0D);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"sampler AHWanYi SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("sampler AHWanYi  protocol,sampler AHWanYi : read device1 %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("sampler AHWanYi  data1 : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"sampler AHWanYi  RECV1:",com_rbuf,size);

	if(size>=31 && modbus_ascii_pack_check(com_rbuf, devaddr, cmd))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		val=p[3];
		val<<=8;
		val+=p[4];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
		}

		val=p[17];
		val<<=8;
		val+=p[18];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			//case 4:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			//case 5:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,12,INFOR_STATUS,&arg_n);break;
			//case 7:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			//case 8:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			//case 9:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			//case 10:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,10,INFOR_STATUS,&arg_n);break;
			//case 11:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,11,INFOR_STATUS,&arg_n);break;
			//case 12:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,12,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
		}

		val=p[23];
		val<<=8;
		val+=p[24];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			//case 4:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			//case 5:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,12,INFOR_STATUS,&arg_n);break;
			//case 7:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			//case 8:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
			//case 9:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			//case 10:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,10,INFOR_STATUS,&arg_n);break;
			//case 11:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,11,INFOR_STATUS,&arg_n);break;
			//case 12:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,12,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
		}
		status=0;
	}

	cmd=0x03;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,cmd,0x0BB8,0x0F);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"sampler AHWanYi SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("sampler AHWanYi  protocol,sampler AHWanYi : read device2 %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("sampler AHWanYi  data2 : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"sampler AHWanYi  RECV2:",com_rbuf,size);

	if(size>=35 && modbus_ascii_pack_check(com_rbuf, devaddr, cmd))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		timer.tm_year=hex2dec(p[16])+100;
		timer.tm_mon=hex2dec(p[14])-1;
		timer.tm_mday=hex2dec(p[12]);
		timer.tm_hour=hex2dec(p[10]);
		timer.tm_min=hex2dec(p[8]);
		timer.tm_sec=hex2dec(p[6]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i42002",t3,0.0,INFOR_STATUS,&arg_n);

		
		val=p[17];
		val<<=8;
		val+=p[18];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,1,INFOR_ARGUMENTS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,2,INFOR_ARGUMENTS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,4,INFOR_ARGUMENTS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,3,INFOR_ARGUMENTS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,5,INFOR_ARGUMENTS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,6,INFOR_ARGUMENTS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
		}

		val=p[19];
		val<<=8;
		val+=p[20];
		acqdata_set_value_flag(acq_data,"i43003",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);


		val=p[23];
		val<<=8;
		val+=p[24];
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,1,INFOR_ARGUMENTS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,2,INFOR_ARGUMENTS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,3,INFOR_ARGUMENTS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,5,INFOR_ARGUMENTS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,4,INFOR_ARGUMENTS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,6,INFOR_ARGUMENTS,&arg_n);break;
		}
		
		val=p[29];
		val<<=8;
		val+=p[30];
		acqdata_set_value_flag(acq_data,"i43005",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=p[31];
		val<<=8;
		val+=p[32];
		acqdata_set_value_flag(acq_data,"i43006",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		status=0;
		
	}

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
	return arg_n;
}

