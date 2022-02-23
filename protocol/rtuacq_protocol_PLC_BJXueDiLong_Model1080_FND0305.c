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
Author:Yimning
Email:1148967988@qq.com
Create Time:2022.01.27 Thur.
Description:protocol_CEMS_BJXueDiLong_Model1080_FND0305
DataType and Analysis:
	(FLOAT_ABCD) 
*/

int protocol_PLC_BJXueDiLong_Model1080_FND0305(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf[5];
	float nh3=0;
	int ret=0;
	int arg_n=0;
	int devaddr =0,cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	int smoke_flag;
	float speed,atm_press,PTC,cal_press_Pa=0;
	char flag ='N';
	float nox=0,so2=0,o2=0,no=0,no2=0,co=0,co2=0,hcl=0;
	float nox_ori=0,so2_ori=0,o2_ori=0,no_ori=0,no2_ori=0,co_ori=0,co2_ori=0;
	
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
	cmd=0x03;
	regpos = 0x09;
	regcnt = 0x1A;
	dataType = FLOAT_ABCD;
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong Model1080_FND0305 PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong Model1080_FND0305 PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong Model1080_FND0305 PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong Model1080_FND0305 PLC RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		valf[0]=PLCtoValue(modbusarg,  5530, 27648, val, "a01014");
		
		val = getInt16Value(p, 5, INT_AB);
		valf[1]=PLCtoValue(modbusarg, 5530, 27648, val, "a34013");
		
		val = getInt16Value(p, 7, INT_AB);
		valf[2]=PLCtoValue(modbusarg, 5530, 27648, val, "a01017");

		val = getInt16Value(p, 9, INT_AB);
		valf[3]=PLCtoValue(modbusarg, 5530, 27648, val, "a01012");

		val = getInt16Value(p, 11, INT_AB);
		valf[4]=PLCtoValue(modbusarg, 5530, 27648, val, "a01013");

		if(PTC>0 && (valf[3]+273)>0 && (valf[4]+atm_press)>0)
			speed=PTC*valf[2]*sqrt(((valf[3]+273)/273)*(101325/(valf[4]+atm_press))*(2/1.2928));
		else
			speed=0;
		
#if 0
		speed=PTC*sqrt(fabs(valf[2])*2/SAD);
#endif		

		so2 = getFloatValue(p, 29, dataType);

		no = getFloatValue(p, 33, dataType);
		
		o2 = getFloatValue(p, 37, dataType);
		
		co = getFloatValue(p, 41, dataType);
		
		no2 = getFloatValue(p, 47, dataType);
		
		co2 = getFloatValue(p, 51, dataType);
		
		nox=NO_and_NO2_to_NOx(no, no2);
		
		status=0;
		
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		speed=0;
		valf[3]=0;
		valf[4]=0;
		so2=0;
		no=0;
		no2=0;
		co=0;
		co2=0;
		nox=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a01017",UNIT_PA,valf[2],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);


	if(isPolcodeEnable(modbusarg, "a34013"))
	{
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[1],&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[1],&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	}

	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);

	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a05001",UNIT_MG_M3,co2,&arg_n);


	sleep(1);
	cmd=0x03;
	regpos = 0x00;
	regcnt = 0x01;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong Model1080_FND0305 Mark SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong Model1080_FND0305 Mark protocol,Mark : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong Model1080_FND0305 Mark data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong Model1080_FND0305 Mark RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, INT_AB);
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
		status = 0;
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
