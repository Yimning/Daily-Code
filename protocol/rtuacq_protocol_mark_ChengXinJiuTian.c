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
Create Time:2021.09.26 	Sun.
Description:protocol_MARK_ChengXinJiuTian
TX:01 03 00 00 00 03 F8 08
RX:01 03 06 01 31 41 03 02 32 39 E9


TX1:01 02 00 00 00 06
RX1:01 02 01 01 

DataType and Analysis:
	(INT_AB)  01 31 = 305

*/

int protocol_MARK_ChengXinJiuTian(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 3
	char *polcodestrflag[MODBUS_POLCODE_NUM]={"p10101","p10102","p10103"};
	char *polcodestr[MODBUS_POLCODE_NUM]={"p10201","p10202","p10203"};
	int *adrrstr[MODBUS_POLCODE_NUM]={0x00,0x06,0x0C};
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0}; 
	char check[4] = {0};
	int size=0;
	int val;int  i = 0 , j = 0;
	union uf f;
	float pt=0,evaporation=0,evaporation1=0,evaporation2=0;
	CONDITION_MARK mark;
	int ret=0,length = 0;
	unsigned int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	float valFloat[MODBUS_POLCODE_NUM];
	int valIntLen = 30;
	int valInt[30] = {0};

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x03;
	dataType = FLOAT_CDAB ;

        check[0] = 0x00;
        check[1] = 0x01;
        check[2] = 0x00;
        check[3] = 0x00;
        //check[4] = 0x00;


	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_wpack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ChengXinJiuTian MARK SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ChengXinJiuTian MARK protocol,MARK: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ChengXinJiuTian MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ChengXinJiuTian MARK RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
	    for(i=0;i<MODBUS_POLCODE_NUM;i++)
	    {
		val = getInt16Value(p, 3+i*2, INT_AB);
		valFloat[i] = val / 10.0;
		acqdata_set_value_flag(acq_data,polcodestr[i],UNIT_T_H,valFloat[i],'N',&arg_n);
	    }
		
	    status=0;
	}else
	{
		 for(i=0;i<MODBUS_POLCODE_NUM;i++)
		    {
			acqdata_set_value_flag(acq_data,polcodestr[i],UNIT_T_H,0,'D',&arg_n);
		    }
		 status=1;
	}


for(i=0;i<MODBUS_POLCODE_NUM;i++)
{
	cmd = 0x02;
	regpos = adrrstr[i];
	regcnt = 0x06;
	dataType = FLOAT_CDAB ;

        check[0] = devaddr;
        check[1] = cmd;
        check[2] = 0x01;


	sleep(2);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_wpack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ChengXinJiuTian MARK SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ChengXinJiuTian MARK protocol,MARK: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ChengXinJiuTian MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ChengXinJiuTian MARK RECV:",com_rbuf,size);
	p=memstr(com_rbuf, size, check);
	
	if((size >= 4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(com_rbuf[2]==0x01))
	{
	    	val= com_rbuf[3];
		SYSLOG_DBG("ChengXinJiuTian MARK%d = %d\n",i,val);
		if(val == 0)
			mark=MARK_N;
		
		else if((val & 0x01) == 0x01)
		{
			mark=MARK_DC_Sd;
		}
		else if((val & 0x02) == 0x02)
		{
			mark=MARK_DC_Fa;
		}
		 else if((val & 0x04) == 0x04)
		{
			mark=MARK_DC_Fb;
		}
		 else if((val & 0x08) == 0x08)
		{
			mark=MARK_DC_Sta;
		}
		 else if((val & 0x010) == 0x010)
		{
			mark=MARK_DC_Stb;
		}
		 else if((val & 0x20) == 0x20)
		{
			mark=MARK_DC_Sr;
		}
		 else mark=MARK_N;
		
		acqdata_set_value_flag(acq_data,polcodestrflag[i],UNIT_NONE,(float)mark,'N',&arg_n);

	    status=0;
	}
	else
	{
		mark=MARK_DC_Sr;
		acqdata_set_value_flag(acq_data,polcodestrflag[i],UNIT_NONE,(float)mark,'D',&arg_n);
	}
}

	acq_data->dev_stat = 0xAA;
	acq_data->acq_status = ACQ_OK;

	NEED_ERROR_CACHE_ONE_POLCODE_VALUE(acq_data, arg_n, 10);
#undef MODBUS_POLCODE_NUM
	return arg_n;
}


