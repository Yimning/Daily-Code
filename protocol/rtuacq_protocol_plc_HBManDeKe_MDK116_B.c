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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"


static float adVal2IntValue(struct modbus_arg *modbusarg, int minVal, int maxVal, float ADval)
{
	struct modbus_polcode_arg *mpolcodearg;
	int i=0;
	float valf=0;

	if(!modbusarg ) return -1;

	if(ADval<minVal) return mpolcodearg->alarmMin;
	
	valf=ADval-minVal;
	valf/=(float)(maxVal-minVal);
	valf*=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
	valf+=mpolcodearg->alarmMin;
	return valf;

}

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.09.06 Mon.
Description:protocol_PLC_HBManDeKe_MDK116_B
TX:01 03 00 03 00 05 75 C9 
RX:01 03 0A
19 64
19 C8
1A 2C
1A 90
1A F4
79 4A

DataType and Analysis:
	(INT_AB) 19 64  = 6500
*/
int protocol_PLC_HBManDeKe_MDK116_B(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 5
	char *polcode[MODBUS_POLCODE_NUM]={"a01012","a01013","a01011","a01014","a19001"};
	char *polcodeUnit[MODBUS_POLCODE_NUM]={"UNIT_0C","UNIT_KPA","UNIT_M_S","UNIT_PECENT","UNIT_PECENT"};
	int *adrrstr[MODBUS_POLCODE_NUM]={0x00,0x06,0x0C,0x12,0x18};

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int val=0; 
	float valfArray[8]={0};
	int valArray[8]={0};
	float valf=0;
	int ret=0;
	int devaddr;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
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

	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x05;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"PLC HBManDeKe MDK116-B CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("PLC HBManDeKe MDK116-B  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("PLC HBManDeKe MDK116-B CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"PLC HBManDeKe MDK116-B CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			val= getInt16Value(p, 3+i*2, INT_AB);
			valfArray[i]=PLCtoValue(modbusarg_running, 6400, 32000, val, polcode[i]);
			//SYSLOG_DBG("valf%d = %f\n",i,valfArray[i]);
		}
		
		acqdata_set_value(acq_data,"a01011",UNIT_M_S,valfArray[2],&arg_n);
		acqdata_set_value(acq_data,"a01012",UNIT_0C,valfArray[0],&arg_n);
		acqdata_set_value(acq_data,"a01013",UNIT_KPA,valfArray[1],&arg_n);
		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valfArray[3],&arg_n);
		acqdata_set_value(acq_data,"a19001a",UNIT_MG_M3,valfArray[4],&arg_n);
		acqdata_set_value(acq_data,"a19001",UNIT_MG_M3,valfArray[4],&arg_n);
		
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);	
		acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
		acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);

		#if 0
		mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(modbusarg_running->polcode_arg,modbusarg_running->array_count,"a01017");
		if(mpolcodearg_temp!=NULL && mpolcodearg_temp->enableFlag==1)
		{
					speed=PTC*sqrt(fabs(valf)*2/SAD);
					acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
		}
		#endif
		
		status = 0;
	}
	else
	{
		acqdata_set_value(acq_data,"a01011",UNIT_M_S,0,&arg_n);
		acqdata_set_value(acq_data,"a01012",UNIT_0C,0,&arg_n);
		acqdata_set_value(acq_data,"a01013",UNIT_PA,0,&arg_n);
		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,0,&arg_n);
		acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
		acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
		acqdata_set_value(acq_data,"a19001a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a19001",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);	
		acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
		acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
		
		status = 1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

	return arg_n;
}



