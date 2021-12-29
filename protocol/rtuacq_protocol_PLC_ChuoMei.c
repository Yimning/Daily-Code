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

int protocol_PLC_ChuoMei(struct acquisition_data *acq_data)
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

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;
	struct modbus_polcode_arg * modbus_polcode_arg_tmp;

	//struct rdtu_info * rdtuinfo;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	devaddr=modbusarg->devaddr&0xffff;
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;

	devaddr=modbusarg->devaddr&0xffff;

	minVal=1;
	maxVal=5;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x00,0x08);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ChuoMei PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ChuoMei PLC RECV:",com_rbuf,size);
	if((size>=21)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=getUInt16Value(com_rbuf, 9, INT_AB);
		valf[0]=5/32768.0*val;
		valf[0]=PLCtoValue(modbusarg, minVal, maxVal, valf[0], "a34013");

		val=getUInt16Value(com_rbuf, 11, INT_AB);
		valf[1]=5/32768.0*val;
		valf[1]=PLCtoValue(modbusarg, minVal, maxVal, valf[1], "a01011");

		val=getUInt16Value(com_rbuf, 13, INT_AB);
		valf[2]=5/32768.0*val;
		valf[2]=PLCtoValue(modbusarg, minVal, maxVal, valf[2], "a01012");

		val=getUInt16Value(com_rbuf, 15, INT_AB);
		valf[3]=5/32768.0*val;
		valf[3]=PLCtoValue(modbusarg, minVal, maxVal, valf[3], "a01013");

		val=getUInt16Value(com_rbuf, 17, INT_AB);
		valf[4]=5/32768.0*val;
		valf[4]=PLCtoValue(modbusarg, minVal, maxVal, valf[4], "a01014");
		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		valf[2]=0;
		valf[3]=0;
		valf[4]=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a01011",UNIT_M_S,valf[1],&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[2],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);

	modbus_polcode_arg_tmp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,"a34013a");
	if(modbus_polcode_arg_tmp!=NULL && modbus_polcode_arg_tmp->enableFlag==1)
	{
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[0],&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[0],&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,valf[0],&arg_n);	
	}
	else
	{
		modbus_polcode_arg_tmp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,"a34013");
		if(modbus_polcode_arg_tmp!=NULL && modbus_polcode_arg_tmp->enableFlag==1)
		{
			acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[0],&arg_n);
			acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[0],&arg_n);
			acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,valf[0],&arg_n);	
		}
	}

	//rdtuinfo=get_parent_rdtu_info(acq_data);
	acq_data->dev_stat=rdtuinfo->dev_stat;

	SYSLOG_DBG("%s,%d,%d\n",__FUNCTION__,acq_data->dev_stat,rdtuinfo->dev_stat);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


