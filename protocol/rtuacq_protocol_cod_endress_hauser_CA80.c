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


int protocol_COD_EndressHauser_CA80(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[50]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x04;
	regpos = 0x242F;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "Endress+Hauser CA80x COD", 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		//valf = PLCtoValue(modbusarg, 4, 20, valf, "w01018");
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x24AF;
	regcnt = 0x01;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "Endress+Hauser CA80x NH3N MARK", 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0x0000:	acq_data->dev_stat='N';break;
			case 0x004E:	acq_data->dev_stat='N';break;
			case 0x004D:	acq_data->dev_stat='M';break;
			case 0x0053:	acq_data->dev_stat='C';break;
			case 0x004B:	acq_data->dev_stat='K';break;
			case 0x0054:	acq_data->dev_stat='T';break;
			case 0x0045:	acq_data->dev_stat='D';break;
			default :		acq_data->dev_stat='N';break;
		}
		status=0;
	}
	
	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,valf,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_COD_EndressHauser_CA80_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[50]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;char *pt = NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	char buffer[2048]={0}; 	

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data) return -1;
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

	cmd = 0x04;
	regpos = 0x23FD;
	regcnt = 0x01;
	dataType = FLOAT_CDAB;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "Endress+Hauser CA80x COD STATUS", 1);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			//case 43: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 44: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		status=0;
	}


	sleep(1);
	cmd = 0x04;
	regpos = 0x01FA;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"Endress+Hauser CA80x COD STATUS1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("Endress+Hauser CA80x protocol,COD: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Endress+Hauser CA80x COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"Endress+Hauser CA80x COD STATUS1 RECV",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		if((val&0x0001) == 0){
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}else if((val&0x0001) == 1)
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			val = getInt16Value(p, 5, INT_AB);
			switch(val)
			{
				case 241: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
				case 930: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
				case 372: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
				case 369: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
				case 731: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n); break;
				default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			}
		
		}else{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x2460;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"Endress+Hauser CA80x COD INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("Endress+Hauser CA80x protocol,COD: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Endress+Hauser CA80x COD data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"Endress+Hauser CA80x COD INFO RECV1",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}	

	sleep(1);
	cmd = 0x04;
	regpos = 0x242D;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"Endress+Hauser CA80x COD INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("Endress+Hauser CA80x protocol,COD: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Endress+Hauser CA80x COD data2",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"Endress+Hauser CA80x COD INFO RECV2",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	
	sleep(1);
	cmd = 0x04;
	regpos = 0x2421;
	regcnt = 0x08;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"Endress+Hauser CA80x COD INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("Endress+Hauser CA80x protocol,COD: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Endress+Hauser CA80x COD data3",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"Endress+Hauser CA80x COD INFO RECV3",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		valf = getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);	

		t3= getInt32Value(p, 7, LONG_ABCD);
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		t3= getInt32Value(p, 15, LONG_ABCD);
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x243D;
	regcnt = 0x10;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"Endress+Hauser CA80x COD INFO SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("Endress+Hauser CA80x protocol,COD: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Endress+Hauser CA80x COD data4",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"Endress+Hauser CA80x COD INFO RECV4",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		memset(buffer,0,sizeof(buffer));
		buffer[0] = 0x33;
		buffer[1] = 0x30;
		buffer[2] = 0x30;
		buffer[3] = 0x30;
		buffer[4] = 0x6D;
		buffer[5] = 0x67;
		buffer[6] = 0x2F;
		buffer[7] = 0x4C;
		pt = memstr(p, size, buffer);
		if(pt != NULL)
			val = 3000;
		
		memset(buffer,0,sizeof(buffer));
		buffer[0] = 0x35;
		buffer[1] = 0x30;
		buffer[2] = 0x30;
		buffer[3] = 0x30;
		buffer[4] = 0x6D;
		buffer[5] = 0x67;
		buffer[6] = 0x2F;
		buffer[7] = 0x4C;
		pt = memstr(p, size, buffer);
		if(pt != NULL)
			val = 5000;
		
		memset(buffer,0,sizeof(buffer));
		buffer[0] = 0x32;
		buffer[1] = 0x30;
		buffer[2] = 0x30;
		buffer[3] = 0x30;
		buffer[4] = 0x30;
		buffer[5] = 0x6D;
		buffer[6] = 0x67;
		buffer[7] = 0x2F;
		buffer[8] = 0x4C;
		pt = memstr(p, size, buffer);
		if(pt != NULL)
			val = 20000;		

		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	sleep(1);
	cmd = 0x04;
	regpos = 0x244D;
	regcnt = 0x07;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"Endress+Hauser CA80x COD INFO SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("Endress+Hauser CA80x protocol,COD: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("Endress+Hauser CA80x COD data5",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"Endress+Hauser CA80x COD INFO RECV5",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		val = getInt16Value(p,3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);	

		val = getInt16Value(p,7, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);
		
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


