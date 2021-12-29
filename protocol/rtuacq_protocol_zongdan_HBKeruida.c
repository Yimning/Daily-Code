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
static float ZONGDAN_Keruida_orig=0;
/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.06.21 Fri.
Description:protocol_ZONGDAN_HBKeruida
TX:01 03 00 0A 00 03 25 C9 
RX:01 03 06 00 01 47 AE 41 05 99 B3

DataType and Analysis:
	(Float) 47 AE 41 05 = 8.33
*/
int protocol_ZONGDAN_HBKeruida(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int ret=0;
	float total_andan=0;
	union uf f;
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
	cmd = 0x03;
	regpos = 0x0A;
	regcnt = 0x03;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZONGDAN_HBKeruida protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZONGDAN_HBKeruida protocol",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		val = getInt16Value(p, 3, INT_AB);
		if((val == 3))
			flag = 'N';
		else if (val == 1)
			flag = 'M';
		else if (val == 2)
			flag = 'D';
		else flag = 'N';
		
		total_andan=getFloatValue(p, 5, dataType);

		status=0;
	}
	else
	{
		status=1;
		total_andan=0;
		flag = 'D';
	}

	acqdata_set_value(acq_data,"w21001",UNIT_MG_L,total_andan,&arg_n);	
	acq_data->dev_stat = flag;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.06.21 Fri.
Description:protocol_ZONGDAN_HBKeruida_info
TX1:01 03 00 4D 00 01 14 1D 
RX1:01 03 02 00 00 B8 44

TX2:01 03 00 0D 00 17 94 07 
RX2:01 03 2E
00 00
00 01
00 15
00 30
00 00
00 00
47 AE 41 00
47 AE 41 01
00 00 00 00
47 AE 41 05
00 00 00 00
00 00 00 00
00 33
00 00
00 00
00 00 00 00
BF B8

*/	
int protocol_ZONGDAN_HBKeruida_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_andan=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	float valf=0;
	int val=0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	SYSLOG_DBG("protocol_ZONGDAN_HBKeruida_info\n");
	acqdata_set_value(acq_data,"w21001",UNIT_MG_L,total_andan,&arg_n);	

	cmd = 0x03;
	regpos = 0x4D;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_ZONGDAN_HBKeruida_info SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_ZONGDAN_HBKeruida_info1,ZONGDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_ZONGDAN_HBKeruida_info1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_ZONGDAN_HBKeruida_info RECV1:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		val = getInt16Value(p,3,INT_AB);
		if(val == 0)
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);

		switch(val)
		{
			case 0:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
			case 1:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n);
				break;
			case 2:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);
				break;
			case 3:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				break;
			case 4:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				break;
			case 5:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				break;
			case 6:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
			case 7:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				break;
			default:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
		}
		status=0;
	}
	else
	{
		status = 1;
	}
	

	sleep(2);
	cmd = 0x03;
	regpos = 0x0D;
	regcnt = 0x17;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_ZONGDAN_HBKeruida_info SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_ZONGDAN_HBKeruida_info2,ZONGDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_ZONGDAN_HBKeruida_info2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_ZONGDAN_HBKeruida_info RECV2:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!= NULL)
	{
		val = getInt16Value(p,3,INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 12:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 13:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				
		}
	
		val = getInt16Value(p,5,INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);	
		
		val = getInt16Value(p,7,INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);	

		valf = getFloatValue(p, 15, dataType);
    	    	acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 19, dataType);
    	    	acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 27, dataType);//ZONGDAN_Keruida_orig
		ZONGDAN_Keruida_orig=valf;   
    		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,ZONGDAN_Keruida_orig,INFOR_ARGUMENTS,&arg_n);


		val = getInt16Value(p,39,INT_AB); //andan_zero_val
    		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,val,INFOR_ARGUMENTS,&arg_n);
	
		status=0;
	}
	else
	{
		ZONGDAN_Keruida_orig=0;
		status = 1;
	}


	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}




