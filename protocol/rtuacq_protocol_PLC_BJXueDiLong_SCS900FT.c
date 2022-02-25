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
/*
RX:01 03 00 00 00 0F 05 CE
TX:01 03 1E 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 CD 00 00 00 00 00 00 00 00 00 00 1C 8A
*/
int protocol_PLC_BJXueDiLong_SCS900FT(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf[5];
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	float speed,atm_press,PTC;
	int minVal,maxVal;
	char flag='N';
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;
	struct modbus_polcode_arg * modbus_polcode_arg_tmp;
	MODBUS_DATA_TYPE dataType;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	devaddr=modbusarg->devaddr&0xffff;
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;
	minVal=5530;
	maxVal=27648;
	
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x0F;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS-900FT PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS-900FT PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS-900FT PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS-900FT PLC RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val=p[3];
		if(val==0)
			flag='N';
		else if((val&0x01)==0x01)
			flag='N';
		else if((val&0x02)==0x02)
			flag='D';
		else if((val&0x04)==0x04)
			flag='C';
		else if((val&0x08)==0x08)
			flag='M';
		else if((val&0x40)==0x40)
			flag='z';
		else
			flag='N';

		
		val=getUInt16Value(p, 21, INT_AB);
		valf[0]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01014");

		val=getUInt16Value(p, 23, INT_AB);
		valf[1]=PLCtoValue(modbusarg, minVal, maxVal, val, "a34013");

		val=getUInt16Value(p, 25, INT_AB);
		valf[2]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01017");

		val=getUInt16Value(p, 27, INT_AB);
		valf[3]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01012");

		val=getUInt16Value(p, 29, INT_AB);
		valf[4]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01013");


		if(PTC>0 && (valf[3]+273)>0 && (valf[4]+atm_press)>0)
			speed=PTC*valf[2]*sqrt(((valf[3]+273)/273)*(101325/(valf[4]+atm_press))*(2/SAD));
		else
			speed=0;
		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		speed=0;
		valf[3]=0;
		valf[4]=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[1],&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[1],&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	

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
