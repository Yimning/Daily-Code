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

static int  modbus_tcp_pack(char modpack[12],char addr, char cmd, int reg, int cnt)
{
        modpack[0] = 0x00;
        modpack[1] = 0x01;
        modpack[2] = 0x00;
        modpack[3] = 0x00;
        modpack[4] = 0x00;
        modpack[5] = 0x06;
        modpack[6] = addr;
        modpack[7] = cmd;
        modpack[8] = (char)(reg >> 8)&0xff;
        modpack[9] = (char)(reg & 0xff);
        modpack[10] = (char)(cnt >> 8)&0xff;
        modpack[11] = (char)(cnt & 0xff);
        return 12;
}


/*
Author:Yimning
Create Time:2021.09.02 	Thur.
Description:protocol_MARK_SIS_ModbusTCP
TX:00 01 00 00 00 06 01 03 00 00 00 0A
RX:00 01 00 00 00 17 01 03 14 
01 31 41 03
02 32 41 03
03 33 41 03
04 34 41 03
05 35 41 03


TX1:00 01 00 00 00 06 01 01 00 00 00 06
RX1:00 01 00 00 00 03 01 01 01 08

DataType and Analysis:
	(FLOAT_ABCD)  41 03 32 33 = 8.200
*/

int protocol_modtcp_SIS_ModbusTCP(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 5
	char *polcodestrflag[MODBUS_POLCODE_NUM]={"p10101","p10102","p10103","p10104","p10105"};
	char *polcodestr[MODBUS_POLCODE_NUM]={"p10201","p10202","p10203","p10204","p10205"};
	int *adrrstr[MODBUS_POLCODE_NUM]={0x00,0x06,0x0C,0x12,0x18};
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0}; 
	char check[3] = {0};
	int size=0;
	int val;int  i = 0 , j = 0;
	union uf f;
	float pt=0,evaporation=0,evaporation1=0,evaporation2=0;
	CONDITION_MARK mark;
	int ret=0,length = 0;
	unsigned int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	float valfloat[MODBUS_POLCODE_NUM];
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
	regcnt = 0x0A;
	dataType = FLOAT_CDAB ;

        check[0] = 0x01;
        check[1] = 0x03;
        check[2] = 0x14;


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	 for(i=0;i<MODBUS_POLCODE_NUM;i++)
	 {
		acqdata_set_value_flag(acq_data,polcodestr[i],UNIT_T_H,0,'D',&arg_n);
		acqdata_set_value_flag(acq_data,polcodestrflag[i],UNIT_NONE,0,'D',&arg_n);
	 }
	 
	acq_data->acq_status = ACQ_ERR;
	return arg_n;
     }
ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_tcp_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SIS_ModbusTCP MARK SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SIS_ModbusTCP MARK protocol,MARK: read device %s , size=%d\n",  (acq_data),size);
	SYSLOG_DBG_HEX("SIS_ModbusTCP MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SIS_ModbusTCP MARK RECV:",com_rbuf,size);
	p=memstr(com_rbuf, size, check);
	length = getInt16Value(com_rbuf, 4, INT_AB);
	length = length + 6;
	SYSLOG_DBG("p = %s , size=%d\n",p);
	if((size >= length)&&(com_rbuf[0]==0x00)&&(com_rbuf[1]==0x01)&&(com_rbuf[2]==0x00)&&(com_rbuf[3]==0x00)&&(com_rbuf[6]==devaddr)&&(com_rbuf[7]==cmd))
	{
	    for(i=0;i<MODBUS_POLCODE_NUM;i++)
	    {
		valfloat[i] = getFloatValue(com_rbuf, 9+i*4, dataType);
		acqdata_set_value_flag(acq_data,polcodestr[i],UNIT_T_H,valfloat[i],'N',&arg_n);
	    }
		
	    status=0;
	}else
	{
		 for(i=0;i<MODBUS_POLCODE_NUM;i++)
		    {
			acqdata_set_value_flag(acq_data,polcodestr[i],UNIT_T_H,0,'D',&arg_n);
		    }
		//acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	    
	}


for(i=0;i<MODBUS_POLCODE_NUM;i++)
{
	cmd = 0x01;
	regpos = adrrstr[i];
	regcnt = 0x06;
	dataType = FLOAT_CDAB ;

        check[0] = 0x01;
        check[1] = 0x01;
        check[2] = 0x01;

ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_tcp_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SIS_ModbusTCP MARK SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SIS_ModbusTCP MARK protocol,MARK: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SIS_ModbusTCP MARK data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SIS_ModbusTCP MARK RECV:",com_rbuf,size);
	p=memstr(com_rbuf, size, check);
	length = getInt16Value(com_rbuf, 4, INT_AB);
	length = length + 6;
	SYSLOG_DBG("p1 = %s , size=%d\n",p);
	if((size >= length)&&(com_rbuf[0]==0x00)&&(com_rbuf[1]==0x01)&&(com_rbuf[2]==0x00)&&(com_rbuf[3]==0x00)&&(com_rbuf[6]==devaddr)&&(com_rbuf[7]==cmd))
	{
	    	val= com_rbuf[9];
		SYSLOG_DBG("SIS_ModbusTCP MARK%d = %d\n",i,val);
		if(val == 0)
			mark=MARK_N;
		
		else if((val & 0x01) == 0x01)
		{
			mark=MARK_DC_Fb;
		}
		else if((val & 0x02) == 0x02)
		{
			mark=MARK_DC_Sta;
		}
		 else if((val & 0x04) == 0x04)
		{
			mark=MARK_DC_Stb;
		}
		 else if((val & 0x08) == 0x08)
		{
			mark=MARK_DC_Sd;
		}
		 else if((val & 0x010) == 0x010)
		{
			mark=MARK_DC_Fa;
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
	   	 //acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	    	
		mark=MARK_DC_Sr;
		acqdata_set_value_flag(acq_data,polcodestrflag[i],UNIT_NONE,(float)mark,'D',&arg_n);
	}
}

/*
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
*/

	if(status == 1)
	{
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	}
	
	acq_data->dev_stat = 0xAA;
	acq_data->acq_status = ACQ_OK;

	NEED_ERROR_CACHE_ONE_POLCODE_VALUE(acq_data, arg_n, 10);

	return arg_n;
}

