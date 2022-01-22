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
Time:2021.09.24 Fri.
Description:protocol_CEMS_AnRongXin_AGA1000
TX:01 04 00 00 00 06 70 08
RX:01 04 0C 41 9F 70 A4 41 43 00 00 41 43  00 00 D8 EA

DataType and Analysis:
	(FLOAT_ABCD) 41 9F 70 A4  = 12.1875
*/

int protocol_CEMS_AnRongXin_AGA1000(struct acquisition_data *acq_data)
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
	float SO2=0,NO=0,O2=0,NOx=0;
	float SO2_orig=0,NO_orig=0,O2_orig=0,NOX_orig=0;
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x04;
	regpos = 0x00;
	regcnt = 0x06;
	dataType = FLOAT_ABCD ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"AnRongXin_AGA1000 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnRongXin_AGA1000 CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnRongXin_AGA1000 CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"AnRongXin_AGA1000 CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		NO = getFloatValue(p, 3,dataType);
		NO_orig = NO;
		
		NOx=NO_to_NOx(NO);

		SO2 = getFloatValue(p, 7,dataType);
		SO2_orig = SO2;
		
		O2 = getFloatValue(p, 11,dataType);
		O2_orig = O2;
		
		status=0;
	}
	else
	{
		SO2=0,NO=0,O2=0,NOx=0;
	 	SO2_orig=0,NO_orig=0,O2_orig=0,NOX_orig=0;
		status=1;
	}
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,SO2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,SO2,SO2_orig,&arg_n); 
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,NO,&arg_n);
	acqdata_set_value_orig(acq_data,"a21003",UNIT_MG_M3,NO,NO_orig,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,NOx,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,NOx,NOx,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	if(isPolcodeEnable(modbusarg, "a19001"))
	{
		acqdata_set_value(acq_data,"a19001a",UNIT_MG_M3,O2,&arg_n);
		acqdata_set_value_orig(acq_data,"a19001",UNIT_MG_M3,O2,O2_orig,&arg_n);
	}
	
	if(status == 0){
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


static int SO2_code=0,NOx_code=0,O2_code = 0;

int protocol_CEMS_AnRongXin_AGA1000_NOx_info(struct acquisition_data *acq_data)
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
	float k=0,b=0;
	
	float CEMS_AnRongXin_AGA1000=0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x04;
	regpos = 0x031F;
	regcnt = 0x0E;
	dataType = FLOAT_ABCD;
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_NOx info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_NOx INFO1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_NOx, INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_NOx INFO1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_NOx INFO1 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 5,INT_AB);
		t3 = getTimeValue(BCD(val), BCD(p[4]), BCD(p[3]), BCD(p[7]), BCD(p[8]), BCD(p[9]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 11,dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(p, 21,INT_AB);
		t3 = getTimeValue(BCD(val), BCD(p[20]), BCD(p[19]), BCD(p[23]), BCD(p[24]), BCD(p[25]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 27,dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	    	status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x07CF;
	regcnt = 0x03;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_NOx INFO2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_NOx, INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_NOx INFO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_NOx INFO2 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		
		NOx_code=getInt16Value(p, 3,INT_AB);

	    	status=0;
	}
	else
	{
		status=1;
	}


	sleep(1);
	cmd = 0x04;
	regpos = 0x0833;
	regcnt = 0x08;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_NOx INFO3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_NOx, INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_NOx INFO3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_NOx INFO3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		
		if(NOx_code==1)
			valf = getFloatValue(p, 7, dataType);
		else 
			valf = getFloatValue(p, 15, dataType);

		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else
	{
		status=1;
	}

	
	sleep(1);
	cmd = 0x04;
	regpos = 0x0BB7;
	regcnt = 0x10;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_NOx INFO4 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_NOx, INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_NOx INFO4 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_NOx INFO4 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(NOx_code==1)
		{
			b = getFloatValue(p, 11, dataType);
			k = getFloatValue(p, 15, dataType);
		}
		else
		{
			b = getFloatValue(p, 27, dataType);
			k = getFloatValue(p, 31, dataType);
		}

		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,b,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,k,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else
	{
		status=1;
	}

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_AnRongXin_AGA1000_SO2_info(struct acquisition_data *acq_data)
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
	float k=0,b=0;
	
	float CEMS_AnRongXin_AGA1000=0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	cmd = 0x02;
	regpos = 0x00;
	regcnt = 0x03;
	dataType = FLOAT_ABCD;
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_SO2 info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_SO2 STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_SO2, STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_SO2 STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_SO2 STATUS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if((size >= 4)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd)&&(com_rbuf[2]==0x01))
	{
		val = com_rbuf[3];
		if(val == 0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else if((val & 0x01) == 0x01)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		}
		else if((val & 0x02) == 0x02)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
		}
		 else if((val & 0x04) == 0x04)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		}
		 else
		 {
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		 }
		
	    	status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x032F;
	regcnt = 0x0E;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_SO2 INFO1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_SO2, INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_SO2 INFO1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_SO2 INFO1 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 5,INT_AB);
		t3 = getTimeValue(BCD(val), BCD(p[4]), BCD(p[3]), BCD(p[7]), BCD(p[8]), BCD(p[9]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 11,dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(p, 21,INT_AB);
		t3 = getTimeValue(BCD(val), BCD(p[20]), BCD(p[19]), BCD(p[23]), BCD(p[24]), BCD(p[25]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 27,dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	    	status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x07CF;
	regcnt = 0x03;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_SO2 INFO2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_SO2, INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_SO2 INFO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_SO2 INFO2 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		SO2_code=getInt16Value(p, 5,INT_AB);
		
	    	status=0;
	}
	else
	{
		status=1;
	}


	
	sleep(1);
	cmd = 0x04;
	regpos = 0x0843;
	regcnt = 0x08;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_SO2 INFO3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_SO2, INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_SO2 INFO3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_SO2 INFO3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		
		if(SO2_code==1)
			valf = getFloatValue(p, 7, dataType);
		else 
			valf = getFloatValue(p, 15, dataType);

		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else
	{
		status=1;
	}

	
	sleep(1);
	cmd = 0x04;
	regpos = 0x0BD7;
	regcnt = 0x10;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_SO2 INFO4 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_SO2, INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_SO2 INFO4 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_SO2 INFO4 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(SO2_code==1)
		{
			b = getFloatValue(p, 11, dataType);
			k = getFloatValue(p, 15, dataType);
		}
		else
		{
			b = getFloatValue(p, 27, dataType);
			k = getFloatValue(p, 31, dataType);
		}

		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,b,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,k,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else
	{
		status=1;
	}

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_AnRongXin_AGA1000_O2_info(struct acquisition_data *acq_data)
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
	float k=0,b=0;
	
	float CEMS_AnRongXin_AGA1000=0;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;
	t1=0;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x04;
	regpos = 0x033F;
	regcnt = 0x0E;
	dataType = FLOAT_ABCD;
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_O2 info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_O2 INFO1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_O2, INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_O2 INFO1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_O2 INFO1 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 5,INT_AB);
		t3 = getTimeValue(BCD(val), BCD(p[4]), BCD(p[3]), BCD(p[7]), BCD(p[8]), BCD(p[9]));
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 11,dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(p, 21,INT_AB);
		t3 = getTimeValue(BCD(val), BCD(p[20]), BCD(p[19]), BCD(p[23]), BCD(p[24]), BCD(p[25]));
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 27,dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	    	status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x07CF;
	regcnt = 0x03;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_O2 INFO2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_O2, INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_O2 INFO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_O2 INFO2 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		O2_code=getInt16Value(p, 7,INT_AB);
		
	    	status=0;
	}
	else
	{
		status=1;
	}


	
	sleep(1);
	cmd = 0x04;
	regpos = 0x0853;
	regcnt = 0x04;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_O2 INFO3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_O2, INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_O2 INFO3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_O2 INFO3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 7, dataType);

		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else
	{
		status=1;
	}

	
	sleep(1);
	cmd = 0x04;
	regpos = 0x0BF7;
	regcnt = 0x08;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol CEMS AnRongXin_AGA1000_O2 INFO4 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol CEMS AnRongXin_AGA1000_O2, INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol CEMS AnRongXin_AGA1000_O2 INFO4 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol CEMS AnRongXin_AGA1000_O2 INFO4 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{

		b = getFloatValue(p, 11, dataType);
		k = getFloatValue(p, 15, dataType);

		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,b,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,k,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else
	{
		status=1;
	}

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

