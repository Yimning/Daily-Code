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
Time:2021.08.27 Fri.
Description:protocol_ANDAN_ZhongJieNengTianRong
TX:01 03 00 21 00 02 D5 C0 
RX:01 03 04 
41 03 32 33
74 D5

TX1:01 03 00 26 00 01 D5 C0 
RX1:01 03 02 
00 01
79 84

DataType and Analysis:
	(FLOAT_ABCD)  41 03 32 33 = 8.200
*/
int protocol_ANDAN_ZhongJieNengTianRong(struct acquisition_data *acq_data)
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

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x21;
	regcnt = 0x02;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongJieNengTianRong ANDAN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongJieNengTianRong ANDAN protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongJieNengTianRong ANDAN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongJieNengTianRong ANDAN RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3,dataType);
		
		status=0;
	}
	else
	{
		status=1;
	}
	ret=acqdata_set_value(acq_data,"w21003",UNIT_MG_L,valf,&arg_n);
	
	sleep(1);
	cmd = 0x03;
	regpos = 0x26;
	regcnt = 0x01;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongJieNengTianRong ANDAN Flag SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZhongJieNengTianRong ANDAN Flag protocol,ANDAN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZhongJieNengTianRong ANDAN Flag data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongJieNengTianRong ANDAN Flag RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		val = getInt16Value(p, 3,INT_AB);
		switch(val){
			case 0: flag='N'; break;
			case 1: flag='K'; break;
			case 2: flag='K'; break;
			case 3: flag='N'; break;
			case 4: flag='N'; break;
			case 5: flag='K'; break;
			case 6: flag='M'; break;
			default: flag='N'; break;
		}
		status=0;
	}
	
	if(status == 0){
		acq_data->dev_stat=flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


 
/*
Author:Yimning
Time:2021.08.27 Fri.
Description:protocol_ANDAN_ZhongJieNengTianRong_info
TX:01 03 00 00 00 27 05 D0S 
RX:01 03 4E
00 30
00 30
00 30
00 30
41 03 32 33
41 03 32 33
41 03 32 33
41 03 32 33
41 03 32 33
41 03 32 33
41 03 32 33
00 00
00 00
00 01
00 00
00 00
00 00
20 20 00 08 00 10 00 10 00 30 00 00
00 00 00 00
00 00
32 33 41 03
00 00
00 00
00 00
00 00
D8 F4

DataType and Analysis:
	(INT_AB) 00 01    = 1
	(FLOAT_ABCD) 41 03 32 33 = 8.200

*/
int protocol_ANDAN_ZhongJieNengTianRong_info(struct acquisition_data *acq_data)
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
	
	float ANDAN_ZhongJieNengTianRong=0;

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
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x27;
	dataType = FLOAT_ABCD ;
	SYSLOG_DBG("protocol ANDAN ZhongJieNengTianRong info\n");
	memset(com_tbuf,0,sizeof(com_tbuf));
	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol ANDAN ZhongJieNengTianRong INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol ANDAN ZhongJieNengTianRong, INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol ANDAN ZhongJieNengTianRong INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol ANDAN ZhongJieNengTianRong INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		#if 0
		val = getInt16Value(p,39, INT_AB);
		if(val == 0) {
			acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		#endif
		
		val = getInt16Value(p,41, INT_AB);
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
				
		val = getInt16Value(p,45, INT_AB);
		if(val == 0){
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}else  acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		
		switch(val){
			case 0: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}
		
		val= getInt16Value(p,3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_L,val,INFOR_ARGUMENTS,&arg_n);

		val= getInt16Value(p,5, INT_AB);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);
		
		val= getInt16Value(p,7, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);
		
		val = getInt16Value(p, 9, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p,11, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf= getFloatValue(p,15, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p,19, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p,23, dataType);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf= getFloatValue(p,27, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf= getFloatValue(p,31, dataType);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);			

	    	status=0;
	}
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


