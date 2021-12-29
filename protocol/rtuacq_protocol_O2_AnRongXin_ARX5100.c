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
Description:protocol_O2_AnRongXin_ARX5100
DataType and Analysis:
	(FLOAT_ABCD) 
*/

int protocol_O2_AnRongXin_ARX5100(struct acquisition_data *acq_data)
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
	int val = 0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	float o2=0,orig=0;
	int unit=0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	cmd = 0x04;
	regpos = 0x02;
	regcnt = 0x02;
	dataType = FLOAT_ABCD ;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "AnRongXin_ARX5100 O2", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		o2=getFloatValue(p, 3, dataType);
		
		status=0;
	}
	else
	{
		o2=0;
		status=1;
	}
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_O2_AnRongXin_ARX5100_info(struct acquisition_data *acq_data)
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
	int val = 0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	float o2=0,orig=0;
	float valf=0;
	int unit=0;
	char *p=NULL;
	
	struct tm timer;
	time_t t1,t2,t3;
	
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;

	SYSLOG_DBG("protocol_O2_AnRongXin_ARX5100_info\n");

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	
	if(isPolcodeEnable(modbusarg, "a19001"))
		acqdata_set_value_flag(acq_data,"a19001",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;
	
	cmd = 0x04;
	regpos = 0x04;
	regcnt = 0x1C;
	dataType = FLOAT_ABCD ;

	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"AnRongXin_ARX5100 O2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnRongXin_ARX5100 O2 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnRongXin_ARX5100 O2 data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"AnRongXin_ARX5100 O2 INFO RECV1:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 35, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 39, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}





	sleep(1);
	cmd = 0x04;
	regpos = 0x0103;
	regcnt = 0x0D;
	dataType = FLOAT_ABCD ;

	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"AnRongXin_ARX5100 O2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnRongXin_ARX5100 O2 protocol,INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnRongXin_ARX5100 O2 data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"AnRongXin_ARX5100 O2 INFO RECV2:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, INT_AB);
		if(val == 1)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);

		year=getUInt16Value(p, 5, INT_AB);
		mon=getUInt16Value(p, 7, INT_AB);
		day=getUInt16Value(p, 9, INT_AB);
		hour=getUInt16Value(p, 11, INT_AB);
		min=getUInt16Value(p, 13, INT_AB);
		sec=getUInt16Value(p, 15, INT_AB);
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);


		year=getUInt16Value(p, 19, INT_AB);
		mon=getUInt16Value(p, 21, INT_AB);
		day=getUInt16Value(p, 23, INT_AB);
		hour=getUInt16Value(p, 25, INT_AB);
		min=getUInt16Value(p, 27, INT_AB);
		sec=getUInt16Value(p, 29, INT_AB);
		t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


