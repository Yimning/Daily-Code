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

int protocol_COD_Thermo_orion3106(struct acquisition_data *acq_data)
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

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x4,0x20,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"Thermo_orion3106 COD SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("Thermo_orion3106 COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Thermo_orion3106 COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"Thermo_orion3106 COD RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		f.ch[0]=com_rbuf[6];
		f.ch[1]=com_rbuf[5];
		f.ch[2]=com_rbuf[4];
		f.ch[3]=com_rbuf[3];
		total_cod=f.f;
		status=0;
	}
	else
	{
		total_cod=0;
		status=1;
	}

	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_COD_Thermo_orion3106_info(struct acquisition_data *acq_data)
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
	
	float COD_Thermo_orion3106=0;
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
	regpos = 0x40;
	regcnt = 0x06;
	dataType = FLOAT_ABCD;
	SYSLOG_DBG("protocol COD Thermo_orion3106 info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol COD Thermo_orion3106 STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol COD Thermo_orion3106, STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol COD Thermo_orion3106 STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol COD Thermo_orion3106 STATUS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p,3, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 8: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 9: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 11: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 12: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 13: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 14: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 15: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 17: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 18: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 19: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 20: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 21: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 23: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 27: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
				
		val = getInt16Value(p,5, INT_AB);
		if(val == 0){
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}else  acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);

		val = p[8];
		if(val == 0)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else if((val&0x0001)==0x0001)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);
		else if((val&0x0002)==0x0002)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x0004)==0x0004)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		else if((val&0x008)==0x0008)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		else if((val&0x0010)==0x0010)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x0020)==0x0020)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x0040)==0x0040)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x0080)==0x0080)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x0100)==0x0100)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x0200)==0x0200)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x0400)==0x0400)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x0800)==0x0800)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x1000)==0x1000)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if((val&0x2000)==0x2000)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
		else if((val&0x4000)==0x4000)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
		else if((val&0x8000)==0x8000)
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);

	    	status=0;
	}
	else status = 1;

	sleep(2);
	cmd = 0x04;
	regpos = 0x79;
	regcnt = 0x08;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol COD Thermo_orion3106 INFO1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol COD Thermo_orion3106, INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol COD Thermo_orion3106 INFO1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol COD Thermo_orion3106 INFO1 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p,3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,val,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p,11, dataType);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p,15, dataType);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else status = 1;

	sleep(2);
	cmd = 0x04;
	regpos = 0x9F;
	regcnt = 0x12;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol COD Thermo_orion3106 INFO2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol COD Thermo_orion3106, INFO2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol COD Thermo_orion3106 INFO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol COD Thermo_orion3106 INFO2 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p,3, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf= getFloatValue(p,7, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

  		val = getInt16Value(p, 47, INT_AB);
		timer.tm_year=BCD(val)-1900;
		timer.tm_mon=BCD(p[49])-1;
		timer.tm_mday=BCD(p[50]);
		timer.tm_hour=BCD(p[51]);
		timer.tm_min=BCD(p[52]);
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else status = 1;

	sleep(2);
	cmd = 0x04;
	regpos = 0xB3;
	regcnt = 0x05;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol COD Thermo_orion3106 INFO3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol COD Thermo_orion3106, INFO3 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol COD Thermo_orion3106 INFO3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol COD Thermo_orion3106 INFO3 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
  		val = getInt16Value(p, 7, INT_AB);
		timer.tm_year=(val)-1900;
		timer.tm_mon=(p[9])-1;
		timer.tm_mday=(p[10]);
		timer.tm_hour=(p[11]);
		timer.tm_min=(p[12]);
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else status = 1;



	sleep(2);
	cmd = 0x04;
	regpos = 0xD1;
	regcnt = 0x18;
	dataType = FLOAT_ABCD;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol COD Thermo_orion3106 INFO4 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol COD Thermo_orion3106, INFO4 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol COD Thermo_orion3106 INFO4 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol COD Thermo_orion3106 INFO4 RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p,3, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p,11, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p,15, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
		
	    	status=0;
	}
	else status = 1;
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}




