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

int protocol_ANDAN_NJXinRuiPeng(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float nh3n=0,orig=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	dataType=FLOAT_ABCD;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1005,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng NH3N SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng NH3N protocol,NH3N 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng NH3N data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng NH3N RECV1:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		nh3n=getFloatValue(com_rbuf, 3, dataType);
		
		status=0;
	}
	else
	{
		nh3n=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x22C2,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng NH3N SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng NH3N protocol,NH3N 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng HeBei NH3N data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng HeBei NH3N RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		orig=getFloatValue(com_rbuf, 3, dataType);
	}
	else
	{
		orig=0;
	}


	acqdata_set_value_orig(acq_data,"w21003",UNIT_MG_L,nh3n,orig,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}




int protocol_ANDAN_NJXinRuiPeng_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_ANDAN_NJXinRuiPeng_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x2000,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng HeBei NH3N STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng NH3N STATUS protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng NH3N STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng NH3N STATUS RECV:",com_rbuf,size);
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
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng HeBei NH3N INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng HeBei NH3N INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng HeBei NH3N INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng HeBei NH3N INFO RECV1:",com_rbuf,size);
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
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJXinRuiPeng HeBei NH3N INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJXinRuiPeng HeBei NH3N INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJXinRuiPeng HeBei NH3N INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJXinRuiPeng HeBei NH3N INFO RECV2:",com_rbuf,size);
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

