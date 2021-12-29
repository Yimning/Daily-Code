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

extern char SaiMoFei_PLC_Flag;

int protocol_PLC_SaiMoFei_HBDongTai(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float valf[5];
	int val=0;
	int minVal,maxVal;
	float speed,atm_press,PTC;
	CONDITION_MARK mark;
	char flag;

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

	devaddr=modbusarg->devaddr&0xffff;

	minVal=5530;
	maxVal=27648;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x2A,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei PLC RECV:",com_rbuf,size);
	if((size>=15)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		valf[0]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01014");

		//val=getUInt16Value(com_rbuf, 5, INT_AB);
		//valf[1]=PLCtoValue(modbusarg, minVal, maxVal, val, "a34013");

		val=getUInt16Value(com_rbuf, 7, INT_AB);
		valf[2]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01011");

		val=getUInt16Value(com_rbuf, 9, INT_AB);
		valf[3]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01012");
		//SaiMoFei_PLC_T=valf[3];

		val=getUInt16Value(com_rbuf, 11, INT_AB);
		valf[4]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01013");
		//SaiMoFei_PLC_P=valf[4];

		if(PTC>0 && (valf[3]+273)>0 && (valf[4]+atm_press)>0)
			speed=PTC*sqrt(valf[2]*((valf[3]+273)/273)*(101325/(valf[4]+atm_press))*(2/1.2928));
		else
			speed=0;
		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		valf[2]=0;
		valf[3]=0;
		valf[4]=0;
		speed=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei PLC mark SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei PLC mark RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		if(val==0)
			SaiMoFei_PLC_Flag='N';
		else if((val&0x08)==0x08)
			SaiMoFei_PLC_Flag='M';
		else if((val&0x01)==0x01)
			SaiMoFei_PLC_Flag='C';
		else
			SaiMoFei_PLC_Flag='N';


		val=com_rbuf[4];
		if(val==0)
			mark=MARK_N;
		else if((val&0x01)==0x01)
			mark=MARK_DC_Sd;
		else if((val&0x02)==0x02)
			mark=MARK_DC_Fa;
		else if((val&0x04)==0x04)
			mark=MARK_DC_Fb;
		else if((val&0x08)==0x08)
			mark=MARK_DC_Sta;
		else if((val&0x10)==0x10)
			mark=MARK_DC_Stb;
		else
			mark=MARK_DC_Sr;
		
		flag='N';
	}
	else
	{
		mark=MARK_DC_Sr;
		flag='D';
	}


	acqdata_set_value_flag(acq_data,"a01011",UNIT_M_S,speed,SaiMoFei_PLC_Flag,&arg_n);
	acqdata_set_value_flag(acq_data,"a01012",UNIT_0C,valf[3],SaiMoFei_PLC_Flag,&arg_n);
	acqdata_set_value_flag(acq_data,"a01013",UNIT_PA,valf[4],SaiMoFei_PLC_Flag,&arg_n);
	acqdata_set_value_flag(acq_data,"a01014",UNIT_PECENT,valf[0],SaiMoFei_PLC_Flag,&arg_n);
	//acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[1],&arg_n);
	//acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[1],&arg_n);
	//acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,valf[1],&arg_n);
	acqdata_set_value_flag(acq_data,"a00000",UNIT_M3_S,0,SaiMoFei_PLC_Flag,&arg_n);
	acqdata_set_value_flag(acq_data,"a00000z",UNIT_M3_S,0,SaiMoFei_PLC_Flag,&arg_n);

	acqdata_set_value_flag(acq_data,"p10101",UNIT_NONE,mark,flag,&arg_n);
	
	acq_data->dev_stat=0xAA;
	acq_data->acq_status = ACQ_OK;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
