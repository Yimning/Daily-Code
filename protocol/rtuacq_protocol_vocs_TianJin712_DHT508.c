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
Create Time:2021.12.23 Thur.
Description:protocol_VOCs_TianJin712_DHT508
TX:01 03 00 XX 00 0C FF FF
RX:01 03 18 25 40 17 43 25 40 17 43 25 40 17 43 25 40 17 43 25 40 17 43 25 40 17 43 24 ea


DataType and Analysis:
	(FLOAT_DCBA) 08 00 44 B4    = 1440.25
*/


#define POLCODE_NUM 8
static struct TianJin712_DHT508_info{
	float polcode_valf;
	float i13013;
	int decimal;
	float i13026;
	float i13022;
	int status;
};
static struct TianJin712_DHT508_info TianJin712_DHT508[POLCODE_NUM];
static char *polcode[4]={
		"a24087","a24088","a25003","a05002"
	};
static UNIT_DECLARATION unit[4]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,  UNIT_MG_M3
	};
	
int protocol_VOCs_TianJin712_DHT508(struct acquisition_data *acq_data)
{
   int posnum[POLCODE_NUM]={
	   	0x14,0x28,0x3C,0x50,
		0x64,0x78,0x8C,0xA0
   	};
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int ret=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	float speed = 0,PTC = 0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;

for(i = 0; i < POLCODE_NUM; i++)
{
	sleep(1);
	cmd = 0x03;
	regpos = posnum[i];
	regcnt = 0x0C;
	dataType = FLOAT_DCBA;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"TianJin712 DHT508 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("TianJin712 DHT508 CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("TianJin712 DHT508 CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"TianJin712 DHT508 CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		TianJin712_DHT508[i].polcode_valf = getFloatValue(p, 3, dataType);
		TianJin712_DHT508[i].i13013= getFloatValue(p, 7, dataType);
		TianJin712_DHT508[i].decimal= getInt32Value(p, 11, LONG_CDAB);
		TianJin712_DHT508[i].i13026= getFloatValue(p, 15, dataType);
		TianJin712_DHT508[i].i13022= getFloatValue(p, 19, dataType);
		TianJin712_DHT508[i].status = 0;
		SYSLOG_DBG("TianJin712 -----%f, i=%d\n",TianJin712_DHT508[i].polcode_valf,i);
		status=0;
	}
	else
	{
		status=1;
		TianJin712_DHT508[i].status = 1;
	}

}

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		int pos=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];

		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
				SYSLOG_DBG("TianJin712 -----%f, pos=%d---%s\n",TianJin712_DHT508[pos].polcode_valf,pos,mpolcodearg->polcode);
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,TianJin712_DHT508[pos].polcode_valf,&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}

	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

	return arg_n;
}


int protocol_VOCs_TianJin712_DHT508_STATUS_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int ret=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	float speed = 0,PTC = 0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;
	
	if(isPolcodeEnable(modbusarg_running, "a24087"))
		acqdata_set_value_flag(acq_data,"a24087",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else 	if(isPolcodeEnable(modbusarg_running, "a24088"))
		acqdata_set_value_flag(acq_data,"a24088",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg_running, "a05002"))
		acqdata_set_value_flag(acq_data,"a05002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	sleep(1);
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x03;
	dataType = INT_AB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"TianJin712 DHT508 STATUS INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("TianJin712 DHT508 STATUS protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("TianJin712 DHT508 STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"TianJin712 DHT508 STATUS INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
	        val= getInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		
	        val= getInt16Value(p, 5, INT_AB);
		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		
	        val= getInt16Value(p, 7, INT_AB);
		acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		
		status=0;
	}
	else
	{
		status = 1;
	}

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_VOCs_TianJin712_DHT508_THC_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int ret=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	float speed = 0,PTC = 0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;
	
	if(isPolcodeEnable(modbusarg_running, "a24087"))
		acqdata_set_value_flag(acq_data,"a24087",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		int pos=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];

		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1 &&(!strcmp(mpolcodearg->polcode,"a24087")))
			{
				acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,TianJin712_DHT508[pos].i13013,INFOR_ARGUMENTS,&arg_n);
				acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,TianJin712_DHT508[pos].i13026,INFOR_ARGUMENTS,&arg_n);
				acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,TianJin712_DHT508[pos].i13022,INFOR_ARGUMENTS,&arg_n);
				
				status = 0;
				
				break;
			}
			else
			{
				status = 1;
			}
		}

	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



	
int protocol_VOCs_TianJin712_DHT508_NMHC_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int ret=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	float speed = 0,PTC = 0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;
	
	if(isPolcodeEnable(modbusarg_running, "a24088"))
		acqdata_set_value_flag(acq_data,"a24088",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		int pos=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];

		if(mpolcodearg->enableFlag==1)   
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1 &&(!strcmp(mpolcodearg->polcode,"a24088")))
			{
				acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,TianJin712_DHT508[pos].i13013,INFOR_ARGUMENTS,&arg_n);
				acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,TianJin712_DHT508[pos].i13026,INFOR_ARGUMENTS,&arg_n);
				acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,TianJin712_DHT508[pos].i13022,INFOR_ARGUMENTS,&arg_n);
				
				status = 0;
				
				break;
			}
			else
			{
				status = 1;
			}
		}

	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_VOCs_TianJin712_DHT508_NMHC_CH4_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int ret=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	float speed = 0,PTC = 0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;
	
	if(isPolcodeEnable(modbusarg_running, "a05002"))
		acqdata_set_value_flag(acq_data,"a05002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		int pos=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];

		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1 &&(!strcmp(mpolcodearg->polcode,"a05002")))
			{
				acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,TianJin712_DHT508[pos].i13013,INFOR_ARGUMENTS,&arg_n);
				acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,TianJin712_DHT508[pos].i13026,INFOR_ARGUMENTS,&arg_n);
				acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,TianJin712_DHT508[pos].i13022,INFOR_ARGUMENTS,&arg_n);
				
				status = 0;
				
				break;
			}
			else
			{
				status = 1;
			}
		}

	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.08.16 Mon.
eg.   6537/65535*16+4=5.6mA
*/
int protocol_ai_TianJin712_DHT508(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int val=0; 
	float ai[8]={0};
	float valf=0;
	int ret=0;
	int devaddr;
	float speed = 0,PTC = 0;
	char *p = NULL;

	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x00,0x08);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ai TianJin712 DHT508 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ai TianJin712 DHT508  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ai TianJin712 DHT508 CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ai TianJin712 DHT508 CEMS RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, 0x04, 0x08);
	if(p!=NULL)
	{
		for(i=0;i<8;i++)
		{
			val=p[3+i*2];
			val<<=8;
			val+=p[4+i*2];
			//eg.  6537/65535*16+4=5.6mA
			ai[i]=(val/65535.0)*16+4;
		
		}
		status=0;
	}
	else
	{
		status=1;
	}

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		int pos=0;
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		
		//if((!strcmp(mpolcodearg->polcode,"a01011")) && isPolcodeEnable(modbusarg_running,"a01017"))
			//continue;
		
		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
				valf=(ai[pos]-4)/16;
				valf *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
				valf +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d %f pos=%d, TianJin712 DHT508 %s valf:%f,max %f,min %f\n",i,ai[pos],pos,
					mpolcodearg->polcode,valf,mpolcodearg->alarmMax,mpolcodearg->alarmMin);

				if(!strcmp(mpolcodearg->polcode,"a01017"))
				{
					speed=PTC*sqrt(fabs(valf)*2/SAD);
					acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
				}
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,valf,&arg_n);
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}

	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

	return arg_n;
}



