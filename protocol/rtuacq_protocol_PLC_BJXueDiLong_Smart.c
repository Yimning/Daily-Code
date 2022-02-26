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
#include <time.h>s
#include <sys/time.h>
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.17 Suns.
Description:protocol_PLC_BJXueDiLong_Smart
TX:01 03 00 09 00 05 55 CB
RX:01 03 10
27 FF
25 FF 
24 49 
34 30
01 10 
9C BA
DataType and Analysis:
	(int) 00 01 = 1
*/
int protocol_PLC_BJXueDiLong_Smart(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf[6];
	int ret=0;
	int arg_n=0;    
	int devaddr=0;
	int cmd = 0;
	float speed,atm_press,PTC;
	char flag=0;
	float O2_val = 0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	devaddr=modbusarg->devaddr&0xffff;
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;
	cmd = 0x03;

	status=0;
	if(isPolcodeEnable(modbusarg, "a19001"))
	{
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,cmd,0x08,0x01);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_PLC_BJXueDiLong_Smart PLC O2 SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("protocol_PLC_BJXueDiLong_Smart PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("protocol_PLC_BJXueDiLong_Smart PLC  data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_PLC_BJXueDiLong_Smart PLC  O2 RECV:",com_rbuf,size);
		if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
		{
			//O2
			val=com_rbuf[3];
			val<<=8;
			val+=com_rbuf[4];
			O2_val=PLCtoValue(modbusarg, 5530, 27648, val,"a19001");
			
			//status=0;
		}
		else
		{
			O2_val = 0;
			status=1;
		}
	
		acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,O2_val,&arg_n);
		acqdata_set_value(acq_data,"a19001",UNIT_PECENT,O2_val,&arg_n);

		sleep(1);
	}


	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x09,0x0C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_PLC_BJXueDiLong_Smart PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_PLC_BJXueDiLong_Smart PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_PLC_BJXueDiLong_Smart PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_PLC_BJXueDiLong_Smart PLC RECV:",com_rbuf,size);
	if((size>=29)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		//flue gas humidity
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf[0]=PLCtoValue(modbusarg, 5530, 27648, val,"a01014");

		
		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		//smoke dust value  //flue gas original value
		valf[1]=PLCtoValue(modbusarg, 5530, 27648, val,"a34013");
		//acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,val,&arg_n);


		//flue gas velocity    or    differential pressure
		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		valf[2]=PLCtoValue(modbusarg,5530, 27648, val,"a01011");

		val=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		//flue gas temperature
		valf[3]=PLCtoValue(modbusarg,5530, 27648, val,"a01012");

		
		//flue gas static pressure
		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		valf[4]=PLCtoValue(modbusarg, 5530, 27648, val,"a01013");

		val=com_rbuf[25];
		val<<=8;
		val+=com_rbuf[26];
		valf[5]=PLCtoValue(modbusarg, 5530, 27648, val,"a21001");


		if(PTC>0 && (valf[0]+273)>0 && (valf[1]+atm_press)>0)
			speed=PTC*valf[2]*sqrt(((valf[3]+273)/273)*(101325/(valf[4]+atm_press))*(2/1.2928));
		else
			speed=0;
		
		//status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		speed=0;
		valf[3]=0;
		valf[4]=0;
		valf[5]=0;
		status=1;
	}
	
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a21001a",UNIT_MG_M3,valf[5],&arg_n);
	acqdata_set_value(acq_data,"a21001",UNIT_MG_M3,valf[5],&arg_n);
	acqdata_set_value(acq_data,"a21001z",UNIT_MG_M3,0,&arg_n);

	if(isPolcodeEnable(modbusarg, "a34013"))
	{
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[1],&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[1],&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_PLC_BJXueDiLong_Smart PLC SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_PLC_BJXueDiLong_Smart PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_PLC_BJXueDiLong_Smart PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_PLC_BJXueDiLong_Smart PLC RECV1:",com_rbuf,size);	
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		//val=com_rbuf[4];
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		if(val==0)
			flag='N';
		else if((val&0x0100)==0x0100)
			flag='N';
		else if((val&0x0200)==0x0200)
			flag='D';
		else if((val&0x0400)==0x0400)
			flag='C';
		else if((val&0x0800)==0x0800)
			flag='M';
		else if((val&0x1000)==0x1000)
			flag='C';
		else if((val&0x2000)==0x2000)
			flag='C';
		else if((val&0x4000)==0x4000)
			flag='z';
		else if((val&0x8000)==0x8000)
			flag='z';
		else if((val&0x0002)==0x0002)
			flag='F';
		else if((val&0x0040)==0x0040)
			flag='v';
		else
			flag='D';
		//status = 0;
	}
	else
	{
		status = 1;
		flag='V';
	}

	if(status == 0)
	{
		acq_data->dev_stat=flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}




