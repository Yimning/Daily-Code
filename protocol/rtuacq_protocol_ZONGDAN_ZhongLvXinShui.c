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

static time_t get_ZhongLv_time_val(float valf1, float valf2)
{
	int val;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;

	val=(int)valf1;
	year=(val/10000)+2000;
	mon=(val/100)%100;
	day=val%100;
	val=(int)valf2;
	hour=(val/10000)+2000;
	min=(val/100)%100;
	sec=val%100;

	return getTimeValue(year, mon, day,hour, min, sec);
	
}

int protocol_ZONGDAN_ZhongLv(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float tn=0,orig=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x02A4,0x6E);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"zhonglv TN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zhonglv TN RECV1:",com_rbuf,200);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zhonglv TN RECV2:",&com_rbuf[200],size-200);
	if((size>=225)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		tn=getFloatValue(com_rbuf, 3, dataType);
		
		orig=getFloatValue(com_rbuf, 219, dataType);
		
		status=0;
	}
	else
	{
		tn=0;
		orig=0;
		status=1;
	}

	ret=acqdata_set_value_orig(acq_data,"w21001",UNIT_MG_L,tn,orig,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_ZONGDAN_ZhongLv_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val=0;
	float valf=0;
	struct tm timer;
	time_t t1,t2,t3;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	MODBUS_DATA_TYPE dataType;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGDAN_ZhongLv_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	acqdata_set_value_flag(acq_data,"w21001",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x031A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"zhonglv TN STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zhonglv TN STATUS RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=getUInt16Value(com_rbuf, 5, INT_AB);
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(com_rbuf, 3, INT_AB);
		if(val==0)
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,val,INFOR_STATUS,&arg_n);
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x02A4,0x6E);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"zhonglv TN INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	if(size>200)
	{
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zhonglv TN INFO RECV1:",com_rbuf,200);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zhonglv TN INFO RECV2:",&com_rbuf[200],size-200);
	}
	else
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"zhonglv TN INFO RECV:",com_rbuf,size);
	if((size>=225)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		t3=get_ZhongLv_time_val(getFloatValue(com_rbuf, 7, dataType),getFloatValue(com_rbuf, 11, dataType));
		acqdata_set_value_flag(acq_data,"i13101",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 31, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13106",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		t3=get_ZhongLv_time_val(getFloatValue(com_rbuf, 43, dataType),getFloatValue(com_rbuf, 47, dataType));
		acqdata_set_value_flag(acq_data,"i13107",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 71, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 91, dataType);
		acqdata_set_value_flag(acq_data,"i13112",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 127, dataType);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 131, dataType);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		t3=get_ZhongLv_time_val(getFloatValue(com_rbuf, 135, dataType),getFloatValue(com_rbuf, 139, dataType));
		acqdata_set_value_flag(acq_data,"i13128",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 143, dataType);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 147, dataType);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 167, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 171, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 175, dataType);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 179, dataType);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 187, dataType);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 191, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
	
}


