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
Create Time:2021.09.03 Fri.
Description:protocol_CEMS_HBKaiErUniMAT120
TX:30 03 10 00 00 10 44 E7 
RX:30 03 20 42 B1 00 00 42 B7 00 00 41 48 00 00 42 96 66 66 43 8E 99 9A 41 CC CC CD 3F C0 00 00 43 26 B3 33 57 9E

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/

int protocol_CEMS_HBKaiErUniMAT120(struct acquisition_data *acq_data)
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
	int cmd = 0,regpos = 0,regcnt = 0;
	int val=0;
	float valf = 0;
	float SO2=0,NO=0,O2=0,orig=0,nox=0;
	float SO2_orig=0,NO_orig=0,O2_orig=0,nox_orig=0;
	char *p;
	char flag =0;
		
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	
	cmd = 0x03;
	regpos = 0x1000;
	regcnt = 0x06;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBKaiErUniMAT120 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBKaiErUniMAT120 CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBKaiErUniMAT120 CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBKaiErUniMAT120 CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		SO2 = getFloatValue(p, 3, dataType);
		SO2_orig= SO2;
		
		NO= getFloatValue(p, 7, dataType);
		NO_orig= NO;
		
		O2 = getFloatValue(p, 11, dataType);
		O2_orig= O2;

		//nox = getFloatValue(p, 15, dataType);
		//nox_orig= nox;

		nox=NO_to_NOx(NO);
		
		status=0;
	}
	else
	{
		SO2 = 0;
		NO = 0;
		O2 = 0;
		nox =0;
		status=1;
	}
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,SO2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,SO2,SO2_orig,&arg_n); 
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,NO,&arg_n);
	acqdata_set_value_orig(acq_data,"a21003",UNIT_MG_M3,NO,NO_orig,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a19001a",UNIT_MG_M3,O2,&arg_n);
	acqdata_set_value_orig(acq_data,"a19001",UNIT_MG_M3,O2,O2_orig,&arg_n);
	

	if(status == 0)
	{
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.09.03 Fri.
Description:protocol_PLC_HBKaiErUniMAT120
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
int protocol_PLC_HBKaiErUniMAT120(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 5
	char *polcode[MODBUS_POLCODE_NUM]={"a34013a","a01012","a01013","a01017","a01014"};
	char *polcodeUnit[MODBUS_POLCODE_NUM]={"UNIT_MG_M3","UNIT_0C","UNIT_PA","UNIT_PA","UNIT_PECENT"};
	int *adrrstr[MODBUS_POLCODE_NUM]={0x00,0x06,0x0C,0x12,0x18};

	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int i=0;
	int val=0; 
	float valfArray[8]={0};
	float valf=0;
	int ret=0;
	int devaddr;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	float speed = 0,PTC = 0;
	float humi_per = 0;
	float temp = 0;
	
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
	regpos = 0x03;
	regcnt = 0x05;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"PLC HBKaiErUniMAT120 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("PLC HBKaiErUniMAT120  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("PLC HBKaiErUniMAT120 CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"PLC HBKaiErUniMAT120 CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			val = getInt16Value(p, 3+i*2, INT_AB);
			valfArray[i]=PLCtoValue(modbusarg_running, 6400, 32000, val, polcode[i]);
		}

		acqdata_set_value(acq_data,"a01017",UNIT_PA,valfArray[3],&arg_n);
		acqdata_set_value(acq_data,"a01012",UNIT_0C,valfArray[1],&arg_n);
		acqdata_set_value(acq_data,"a01013",UNIT_PA,valfArray[2],&arg_n);
		//acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valfArray[4],&arg_n);
		
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valfArray[0],&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valfArray[0],&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);	
		
		acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
		acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);


#if 1
		mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(modbusarg_running->polcode_arg,modbusarg_running->array_count,"a01017");
		if(mpolcodearg_temp!=NULL && mpolcodearg_temp->enableFlag==1)
		{
			speed=1.0*0.076*PTC*sqrt(fabs(valfArray[3]))*sqrt(fabs(273.0+valfArray[1]));
			acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
		}
		
		temp =fabs((17.27*valfArray[1])/(valfArray[1]+273.0));

		humi_per = 100.0*1000.0*0.6108*exp(temp)*valfArray[4]/100.0/(101325+valfArray[2]);
		
		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,humi_per,&arg_n);
		
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
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
		
		status = 1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data, 10);

#undef 	MODBUS_POLCODE_NUM
	return arg_n;
}

