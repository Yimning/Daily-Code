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

static char SHBeFen_SBF1500_STATUS;
static int SHBeFen_SBF1500_flag;

int protocol_PLC_SHBeiFen_SBF1500(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf[13];
	float nox=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int maxVal=0,minVal=0;
	char *p=NULL;
	char flag=0;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	//cnt=0x19;
	cnt=modbusarg->regcnt&0xFFFF;

	maxVal=32000;
	minVal=0;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHBeiFen SBF1500 PLC", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 11, INT_AB);
		valf[0]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01014");
		
		val=getInt16Value(p, 13, INT_AB);
		valf[1]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01012");

		val=getInt16Value(p, 15, INT_AB);
		valf[2]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01013");

		val=getInt16Value(p, 17, INT_AB);
		valf[3]=PLCtoValue(modbusarg, minVal, maxVal, val, "a01011");

		val=getInt16Value(p, 19, INT_AB);
		if(isPolcodeEnable(modbusarg, "a21026a"))
			valf[4]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21026a");
		else
			valf[4]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21026");

		val=getInt16Value(p, 21, INT_AB);
		valf[5]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21003");

		val=getInt16Value(p, 23, INT_AB);
		valf[6]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21004");

		val=getInt16Value(p, 25, INT_AB);
		if(isPolcodeEnable(modbusarg, "a19001a"))
			valf[7]=PLCtoValue(modbusarg, minVal, maxVal, val, "a19001a");
		else
			valf[7]=PLCtoValue(modbusarg, minVal, maxVal, val, "a19001");

		val=getInt16Value(p, 25, INT_AB);
		if(isPolcodeEnable(modbusarg, "a21005a"))
			valf[8]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21005a");
		else
			valf[8]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21005");

		val=getInt16Value(p, 29, INT_AB);
		if(isPolcodeEnable(modbusarg, "a21024a"))
			valf[9]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21024a");
		else
			valf[9]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21024");

		val=getInt16Value(p, 31, INT_AB);
		if(isPolcodeEnable(modbusarg, "a05001a"))
			valf[10]=PLCtoValue(modbusarg, minVal, maxVal, val, "a05001a");
		else
			valf[10]=PLCtoValue(modbusarg, minVal, maxVal, val, "a05001");

		val=getInt16Value(p, 33, INT_AB);
		if(isPolcodeEnable(modbusarg, "a21001a"))
			valf[11]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21001a");
		else
			valf[11]=PLCtoValue(modbusarg, minVal, maxVal, val, "a21001");

		valf[12]=getInt16Value(p, 51, INT_AB);

		nox=NO_and_NO2_to_NOx(valf[5], valf[6]);
		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		valf[2]=0;
		valf[3]=0;
		valf[4]=0;
		valf[5]=0;
		valf[6]=0;
		valf[7]=0;
		valf[8]=0;
		valf[9]=0;
		valf[10]=0;
		valf[11]=0;
		valf[12]=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[1],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[2],&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,valf[3],&arg_n);

	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,valf[4],&arg_n);

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);

	//acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,valf[5],&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,valf[5],&arg_n);
	//acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,valf[5],&arg_n);

	//acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,valf[6],&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,valf[6],&arg_n);
	//acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,valf[6],&arg_n);

	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,valf[7],&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,valf[7],&arg_n);

	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,valf[8],&arg_n);
	acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,valf[8],&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,valf[8],&arg_n);

	acqdata_set_value(acq_data,"a21024a",UNIT_MG_M3,valf[9],&arg_n);
	acqdata_set_value(acq_data,"a21024",UNIT_MG_M3,valf[9],&arg_n);
	acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,valf[9],&arg_n);

	acqdata_set_value(acq_data,"a05001a",UNIT_PECENT,valf[10],&arg_n);
	acqdata_set_value(acq_data,"a05001",UNIT_PECENT,valf[10],&arg_n);
	acqdata_set_value(acq_data,"a05001z",UNIT_PECENT,valf[10],&arg_n);

	acqdata_set_value(acq_data,"a21001a",UNIT_MG_M3,valf[11],&arg_n);
	acqdata_set_value(acq_data,"a21001",UNIT_MG_M3,valf[11],&arg_n);
	acqdata_set_value(acq_data,"a21001z",UNIT_MG_M3,valf[11],&arg_n);

	acqdata_set_value(acq_data,"a01016",UNIT_M2,valf[12],&arg_n);
	
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);

	if(SHBeFen_SBF1500_flag==1)
		flag=SHBeFen_SBF1500_STATUS;
	else
		flag=0;

	
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



int protocol_PLC_SHBeiFen_SBF1500_STATUS(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char flag=0;
	char *p=NULL;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x02;
	cnt=0x0B;
#if 0	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHBeiFen SBF1500 PLC STATUS", 1);
	p=modbus_crc_check_coil(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 3, INT_BA);
		if(val==0)
			flag='N';
		else if((val&0x0001)==0x0001)
			flag='F';
		else if((val&0x0002)==0x0002)
			flag='D';
		else if((val&0x0004)==0x0004)
			flag='M';
		else if((val&0x0008)==0x0008)
			flag='C';
		else if((val&0x0010)==0x0010)
			flag='T';
		else if((val&0x0020)==0x0020)
			flag='t';
		else if((val&0x0040)==0x0040)
			flag='d';
		else if((val&0x0080)==0x0080)
			flag='B';
		else if((val&0x0100)==0x0100)
			flag='N';
		else if((val&0x0200)==0x0200)
			flag='D';
		else if((val&0x0400)==0x0400)
			flag='T';
		else
			flag='N';
			
		
		SHBeFen_SBF1500_flag=1;
	}
	else
	{
		flag=0;
		SHBeFen_SBF1500_flag=0;
	}

	SHBeFen_SBF1500_STATUS=flag;
#endif
	acq_data->acq_status = ACQ_OK;
	
	return 0;
}


int protocol_PLC_SHBeiFen_SBF1500_SO2_INFO(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int val=0;
	float valf = 0;
	char *p ;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_PLC_SHBeiFen_SBF1500_SO2_INFO\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;

	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;

	cmd=0x01;
	cnt=0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHBeiFen SBF1500 STATUS", 1);
	p=modbus_crc_check_coil(com_rbuf, size, devaddr, cmd, cnt);
#if 1 	
	if(p!=NULL)
	{
		val=p[3];
		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			SHBeFen_SBF1500_STATUS='N';
		}
		else if((val&0x10)==0x10)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			SHBeFen_SBF1500_STATUS='D';
		}
		else if((val&0x02)==0x02)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			SHBeFen_SBF1500_STATUS='z';
		}
		else if((val&0x04)==0x04)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			SHBeFen_SBF1500_STATUS='M';
		}
		else if((val&0x08)==0x08)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			SHBeFen_SBF1500_STATUS='C';
		}
		else if((val&0x01)==0x01)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			SHBeFen_SBF1500_STATUS='N';
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			SHBeFen_SBF1500_STATUS='N';
		}
		
		SHBeFen_SBF1500_flag=1;
	}
	else
	{
		SHBeFen_SBF1500_flag=0;
	}
#else
	if(p!=NULL)
	{
		val=p[3];
		if(val==0)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else if((val&0x10)==0x10)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if((val&0x02)==0x02)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);
		else if((val&0x04)==0x04)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		else if((val&0x08)==0x08)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if((val&0x01)==0x01)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
	}
#endif

	cmd = 0x03;
	cnt=0x16;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x48,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHBeiFen SBF1500 SO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_PLC_SHBeiFen_SBF1500_NO_INFO(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int val=0;
	float valf = 0;
	char *p ;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_PLC_SHBeiFen_SBF1500_NO_INFO\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;


	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;

	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	cnt=0x14;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x5E,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHBeiFen SBF1500 NO INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 31, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_PLC_SHBeiFen_SBF1500_NO2_INFO(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int val=0;
	float valf = 0;
	char *p ;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_PLC_SHBeiFen_SBF1500_NO2_INFO\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;

	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	cnt=0x14;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x72,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHBeiFen SBF1500 NO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 31, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_PLC_SHBeiFen_SBF1500_HCL_INFO(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int val=0;
	float valf = 0;
	char *p ;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_PLC_SHBeiFen_SBF1500_HCL_INFO\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21024",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;

	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	cnt=0x14;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x9A,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHBeiFen SBF1500 HCL INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 31, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_PLC_SHBeiFen_SBF1500_O2_INFO(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	int val=0;
	float valf = 0;
	char *p ;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_PLC_SHBeiFen_SBF1500_O2_INFO\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	status=1;

	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	cnt=0x14;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xC2,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHBeiFen SBF1500 O2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);



		valf=getFloatValue(p, 31, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

