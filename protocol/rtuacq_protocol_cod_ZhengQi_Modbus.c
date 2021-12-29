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
Create Time:2021.08.31 Tues.
Description:protocol_COD_ZhengQi_Modbus
TX:01 03 00 00 00 08 44 0C 
RX:01 03 10 32 33 41 03 32 33 41 03 00 00 00 00 00 00 00 01 89 59

DataType and Analysis:
	(FLOAT_ABCD) 32 33 41 03   = 8.200
*/


int protocol_COD_ZhengQi_Modbus(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	float org = 0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x08;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhengQi_Modbus COD SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhengQi_Modbus COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhengQi_Modbus COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhengQi_Modbus COD RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		valf = getFloatValue(p, 3,dataType);

		org = getFloatValue(p, 7,dataType);

		val = getInt16Value(p, 17, INT_AB);
		switch(val)
		{
			case 0:flag = 'N';break;
			case 1:flag = 'F';break;
			case 2:flag = 'M';break;
			case 4:flag = 'D';break;
			case 5:flag = 'C';break;
			case 8:flag = 'T';break;
			case 9:flag = 'N';break;
			default:flag = 'N';break;
		}
		status=0;
	}else status=1;

	ret=acqdata_set_value_orig(acq_data,"w01018",UNIT_MG_L,valf,org,&arg_n);
	
	if(status == 0)
	{
		acq_data->acq_status = ACQ_OK;
		acq_data->dev_stat=flag;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


 
/*
Author:Yimning
Create Time:2021.08.31 Tues.
Description:protocol_COD_ZhengQi_Modbus_info
TX:01 03 00 10 00 44 44 3C 
RX:01 03 88
00 10
00 10
00 10
00 10
00 00 00 00 00 00
00 00 00 00 00 00
00 30
00 20
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
20 08 15 12 30 00
20 08 15 12 30 00
00 00
00 00
32 33 41 03
32 33 41 03
00 00
00 00
20 08 15 12 30 00
00 30
00 00
00 00 00 00 00 00 00 00 00 00
00 00
00 00
32 33 41 03
32 33 41 03
32 33 41 03
C5 B1

DataType and Analysis:
	(INT_CDAB) 00 01    = 1
	(FLOAT_ABCD) 32 33 41 03   = 8.200

*/
int protocol_COD_ZhengQi_Modbus_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	float valf=0;
	int val=0;

	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	
	float COD_ZhengQi_Modbus=0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	t1 = 0;
	cmd = 0x03;
	regpos = 0x0A;
	regcnt = 0x43;
	dataType = FLOAT_ABCD ;
	SYSLOG_DBG("protocol COD ZhengQi_Modbus info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol COD ZhengQi_Modbus info SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol COD ZhengQi_Modbus, Info : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol COD ZhengQi_Modbus Info data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol COD ZhengQi_Modbus info RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		val = getInt16Value(p,3, INT_AB);
		switch(val){
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 16: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 32: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 64: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 128: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 256: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 1024: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 2048: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		
		#if 0
		val = getInt16Value(p,5, INT_AB);
		
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
		}
		#endif
		
		val = getInt16Value(p,7, INT_AB);
		switch(val){
			case 1: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 16: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 32: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 64: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 128: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 1024: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 2048: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 32768: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val = getInt16Value(p,9, INT_AB);
		switch(val){
			case 2: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val = getInt16Value(p,23, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);	

		val = getInt16Value(p,25, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 31, dataType);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 31, dataType);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);	

		valf = getFloatValue(p, 59, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);	

		valf = getFloatValue(p, 67, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 71, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 75, INT_AB);
		timer.tm_year=(val/100)+100;
		timer.tm_mon=(val%100)-1;
		val=getUInt16Value(p, 77, INT_AB);
		timer.tm_mday=val/100;
		timer.tm_hour=val%100;
		val=getUInt16Value(p, 79, INT_AB);
		timer.tm_min=val/100;
		timer.tm_sec=val%100;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 81, INT_AB);
		timer.tm_year=(val/100)+100;
		timer.tm_mon=(val%100)-1;
		val=getUInt16Value(p, 83, INT_AB);
		timer.tm_mday=val/100;
		timer.tm_hour=val%100;
		val=getUInt16Value(p, 85, INT_AB);
		timer.tm_min=val/100;
		timer.tm_sec=val%100;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 95, dataType);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(p,109, INT_AB);
        	acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
			
		status=0;
	}
	else {
		status = 1;
	}
	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
