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
Create Time:2022.01.11 Tues.
Description:protocol_ZONGLIN_NJXinRuiPeng_New
TX:01 03 10 05 00 02 D0 CA
RX:01 03 04 42 48 00 00 6E 5D 
DataType and Analysis:
	(FLOAT_ABCD) 42 48 00 00 = 50
*/
int protocol_ZONGLIN_NJXinRuiPeng_New(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float tp=0,orig=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	regpos = 0x1005;
	regcnt = 0x02;
	dataType=FLOAT_ABCD;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng_New ZONGLIN SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng_New ZONGLIN protocol,ZONGLIN 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng_New ZONGLIN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng_New ZONGLIN RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		tp=getFloatValue(p, 3, dataType);
		val=getUInt16Value(p, 9, INT_AB);
		switch(val)
		{
			case 0x004E:	flag='N';break;
			case 0x0054:	flag='T';break;
			case 0x004C:	flag='T';break;
			case 0x0044:	flag='D';break;
			case 0x0042:	flag='E';break;
			case 0x004D:	flag='M';break;
			case 0x637A:	flag='C';break;
			case 0x6373:	flag='C';break;
			default:		flag='N';break;
		}
		
		status=0;
	}
	else
	{
		tp=0;
		status=1;
	}

	sleep(1);
	cmd=0x03;
	regpos = 0x22C2;
	regcnt = 0x04;
	dataType=FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng_New ZONGLIN SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng_New ZONGLIN protocol,ZONGLIN 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng_New HeBei ZONGLIN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng_New HeBei ZONGLIN RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		orig=getFloatValue(p, 3, dataType);
	}
	else
	{
		orig=0;
	}


	acqdata_set_value_orig(acq_data,"w21011",UNIT_MG_L,tp,orig,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_ZONGLIN_NJXinRuiPeng_New_info(struct acquisition_data *acq_data)
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
	MODBUS_DATA_TYPE dataType;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_NJXinRuiPeng_New_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	regpos = 0x2000;
	regcnt = 0x05;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng_New HeBei ZONGLIN STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng_New ZONGLIN STATUS protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng_New ZONGLIN STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng_New ZONGLIN STATUS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		} 

		val=getUInt16Value(com_rbuf, 11, INT_AB);
		if(val==0 || (val&0x0100)==0x0100)
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);

			if((val&0x0001)==0x0001)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			else if((val&0x0002)==0x0002)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);
			else if((val&0x0004)==0x0004)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x0008)==0x0008)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x0020)==0x0020)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else if((val&0x0040)==0x0040)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);
			else if((val&0x0080)==0x0080)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x0200)==0x0200)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);
			else if((val&0x0400)==0x0400)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,8,INFOR_STATUS,&arg_n);
			else if((val&0x0800)==0x0800)
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}

		status=0;

	}

	sleep(1);
	cmd=0x03;
	regpos = 0x2200;
	regcnt = 0x18;
	dataType=FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng_New HeBei ZONGLIN INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng_New HeBei ZONGLIN INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng_New HeBei ZONGLIN INFO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng_New HeBei ZONGLIN INFO RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 5, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
		
		val=getUInt16Value(p, 9, INT_AB);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(BCD(p[27])+2000, BCD(p[28]), BCD(p[29]), BCD(p[30]), BCD(p[31]), BCD(p[32]));
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	cmd=0x03;
	regpos = 0x5211;
	regcnt = 0x09;
	dataType=FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng_New HeBei ZONGLIN INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng_New HeBei ZONGLIN INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng_New HeBei ZONGLIN INFO data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng_New HeBei ZONGLIN INFO RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		t3=getTimeValue(BCD(p[35])+2000, BCD(p[36]), BCD(p[37]), BCD(p[38]), BCD(p[39]), BCD(p[40]));
		acqdata_set_value_flag(acq_data,"i13128",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

