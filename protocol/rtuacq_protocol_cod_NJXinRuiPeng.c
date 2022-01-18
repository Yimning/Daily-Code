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

int protocol_COD_NJXinRuiPeng(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_cod=0,orig=0;
	int ret=0;
	int arg_n=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char *p=NULL;
	char flag=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x04;
	dataType=FLOAT_ABCD;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1005,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "NJXinRuiPeng COD", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		total_cod=getFloatValue(p, 3, dataType);

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
		total_cod=0;
		status=1;
	}

	sleep(1);
	cmd=0x03;
	cnt=0x02;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x22C2,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "NJXinRuiPeng COD orig", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		orig=getFloatValue(com_rbuf, 3, dataType);
	}
	else
	{
		orig=0;
	}


	acqdata_set_value_orig(acq_data,"w01018",UNIT_MG_L,total_cod,orig,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_COD_NJXinRuiPeng_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_COD_NJXinRuiPeng_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x2000,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng HeBei COD STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng COD STATUS protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng COD STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng COD STATUS RECV:",com_rbuf,size);
	if((size>=15)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=getInt32Value(com_rbuf, 3, LONG_ABCD);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
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
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x2200,0x18);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng HeBei COD INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng HeBei COD INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng HeBei COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng HeBei COD INFO RECV1:",com_rbuf,size);
	if((size>=53)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 5, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
		
		val=getUInt16Value(com_rbuf, 9, INT_AB);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(BCD(com_rbuf[27])+2000, BCD(com_rbuf[28]), BCD(com_rbuf[29]), BCD(com_rbuf[30]), BCD(com_rbuf[31]), BCD(com_rbuf[32]));
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13101",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x22C2,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng HeBei COD INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng HeBei COD INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng HeBei COD INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng HeBei COD INFO RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13115",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_water_NJXinRuiPeng(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char str[50]={0};
	char *polcode;
	int size=0;
	float total_cod=0,orig=0;
	int ret=0;
	int arg_n=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char *p=NULL;
	char flag=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x04;
	dataType=FLOAT_ABCD;

	polcode=&(modbusarg->polcode_arg[0].polcode);
	if((!polcode) || (!strcmp(polcode,"")))
		return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1005,cnt);
	memset(str,0,sizeof(str));
	strcpy(str,"NJXinRuiPeng ");
	strcat(str,polcode2name2017(polcode));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		total_cod=getFloatValue(p, 3, dataType);

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
		total_cod=0;
		status=1;
	}

	sleep(1);
	cmd=0x03;
	cnt=0x02;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x22C2,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"NJXinRuiPeng %s orig",polcode2name2017(polcode));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		orig=getFloatValue(p, 3, dataType);
	}
	else
	{
		orig=0;
	}

	acqdata_set_value_orig(acq_data,polcode,UNIT_MG_L,total_cod,orig,&arg_n);
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

int protocol_water_NJXinRuiPeng_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	char *polcode;
	char str[50]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	polcode=&(modbusarg->polcode_arg[0].polcode);
	if((!polcode))
		return -1;
	
	SYSLOG_DBG("protocol_water_NJXinRuiPeng_info,polcode:%s\n",polcode);

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,polcode,UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	
	cnt=0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x2000,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"NJXinRuiPeng %s STATUS",polcode2name2017(polcode));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
	
		val=getUInt16Value(p, 11, INT_AB);
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
	memset(com_tbuf,0,sizeof(com_tbuf));
	cnt=0x18;
	size=modbus_pack(com_tbuf,devaddr,cmd,0x2200,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"NJXinRuiPeng %s INFO 1",polcode2name2017(polcode));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
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

		valf=getFloatValue(p, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13101",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	cnt=0x02;
	size=modbus_pack(com_tbuf,devaddr,cmd,0x22C2,cnt);
	memset(str,0,sizeof(str));
	sprintf(str,"NJXinRuiPeng %s INFO 2",polcode2name2017(polcode));
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), str, 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13115",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

