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
Create Time:2021.09.03 Fri.
Description:protocol_CEMS_HBManDeKe1_DCEM3000
TX:01 04 00 0C 00 11 F0 05
RX:01 04 22
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
00 00
5F 65

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/

int protocol_CEMS_HBManDeKe1_DCEM3000(struct acquisition_data *acq_data)
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

	cmd = 0x04;
	regpos = 0x0C;
	regcnt = 0x11;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);

		//val = getInt16Value(p, 35, INT_AB);
		
		
		status=0;
	}
	else
	{
		status=1;
	}
	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,valf,valf,&arg_n); 
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	
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
Description:protocol_CEMS_HBManDeKe1_DCEM3000_info
TX:01 03 00 30 00 2D 85 D8
RX:01 03 5A
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
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
07 E5 00 08 00 09 00 12 00 00 00 00
07 E5 00 08 00 09 00 12 00 00
E2 72

DataType and Analysis:
	(INT_AB) 19 64  = 6500
*/
int protocol_CEMS_HBManDeKe1_DCEM3000_info(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 5
	char *polcode[MODBUS_POLCODE_NUM]={"a34013a","a01012","a01013","a01011","a01014"};
	char *polcodeUnit[MODBUS_POLCODE_NUM]={"UNIT_MG_M3","UNIT_0C","UNIT_PA","UNIT_M_S","UNIT_PECENT"};
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
	int range = 0;

	
	struct tm timer;
	time_t t1,t2,t3;	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;

	cmd = 0x02;
	regpos = 0x04;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 Range SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 Range protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 Range data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 Range RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	range = 0;
		else range = 1;
		
		status = 0;
	}
	else
	{
		status = 1;
	}


	sleep(1);
	cmd = 0x01;
	regpos = 0x05;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS1 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS1 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if(p[3] == 1)
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		status = 0;
	}else status = 1;

	
	sleep(1);
	cmd = 0x01;
	regpos = 0x10;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS2 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);
		else if(p[3] == 1){
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		}
		else 
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		status = 0;
	}
	else
	{
		status = 1;
	}

	sleep(1);
	cmd = 0x02;
	regpos = 0x03;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS3 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS3 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		
		status = 0;
	}
	else
	{
		status = 1;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x30;
	regcnt = 0x2D;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 INFO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 INFO RECV:",com_rbuf,size);
 
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		
		valf= getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf= getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 63, dataType);   //register number 78 
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		if(range == 0)
			valf= getFloatValue(p, 63, dataType);        //register number 78 
		else 
			valf= getFloatValue(p, 67, dataType);       //register number 80
		
		//acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		
		#if 0
		timer.tm_year=BCD(p[71])+100;
		timer.tm_mon=BCD(p[72])-1;
		timer.tm_mday=BCD(p[73]);
		timer.tm_hour=BCD(p[74]);
		timer.tm_min=BCD(p[75]);
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		#endif
		val = getInt16Value(p, 71, INT_AB);
		t3 = getTimeValue(BCD(val),BCD(p[74]), BCD(p[76]), BCD(p[78]), BCD(p[80]), 0);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		
		
		status = 0;
	}
	else
	{
		
		status = 1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}



/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.09.03 Fri.
Description:protocol_CEMS_HBManDeKe2_DCEM3000
TX:01 04 00 0C 00 11 F0 05
RX:01 04 22
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
00 00
5F 65

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/
int protocol_CEMS_HBManDeKe2_DCEM3000(struct acquisition_data *acq_data)
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
	
	cmd = 0x04;
	regpos = 0x0C;
	regcnt = 0x11;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{ 
		valf= getFloatValue(p, 3, dataType);

		//val = getInt16Value(p, 35, INT_AB);
		
		status=0;
	}
	else
	{
		status=1;
	}
	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,valf,valf,&arg_n); 
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	
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
Description:protocol_CEMS_HBManDeKe2_DCEM3000_info
TX:01 03 00 30 00 2D 85 D8
RX:01 03 5A
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
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
07 E5 00 08 00 09 00 12 00 00 00 00
07 E5 00 08 00 09 00 12 00 00
E2 72

DataType and Analysis:
	(INT_AB) 19 64  = 6500
*/
int protocol_CEMS_HBManDeKe2_DCEM3000_info(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 5
	char *polcode[MODBUS_POLCODE_NUM]={"a34013a","a01012","a01013","a01011","a01014"};
	char *polcodeUnit[MODBUS_POLCODE_NUM]={"UNIT_MG_M3","UNIT_0C","UNIT_PA","UNIT_M_S","UNIT_PECENT"};
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
	int range = 0;
	
	struct tm timer;
	time_t t1,t2,t3;	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;

	cmd = 0x02;
	regpos = 0x04;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 Range SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 Range protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 Range data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 Range RECV:",com_rbuf,size);
 
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	range = 0;
		else range = 1;
		
		status = 0;
	}
	else
	{
		status = 1;
	}

	sleep(1);
	cmd = 0x01;
	regpos = 0x05;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS1 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS1 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if(p[3] == 1)
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		status = 0;
	}else status = 1;

	
	sleep(1);
	cmd = 0x01;
	regpos = 0x10;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS2 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);
		else if(p[3] == 1){
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		}
		else 
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		status = 0;
	}
	else
	{
		status = 1;
	}

	sleep(1);
	cmd = 0x02;
	regpos = 0x03;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS3 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS3 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		
		status = 0;
	}
	else
	{
		status = 1;
	}

	sleep(1);
	cmd = 0x03;
	regpos = 0x30;
	regcnt = 0x27;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 INFO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 INFO RECV:",com_rbuf,size);
 
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		
		valf= getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf= getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 63, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		if(range == 0)
			valf= getFloatValue(p, 39, dataType);
		else 
			valf= getFloatValue(p, 67, dataType);
		
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		
		#if 0
		timer.tm_year=BCD(p[71])+100;
		timer.tm_mon=BCD(p[72])-1;
		timer.tm_mday=BCD(p[73]);
		timer.tm_hour=BCD(p[74]);
		timer.tm_min=BCD(p[75]);
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		#endif
		val = getInt16Value(p, 71, INT_AB);
		t3 = getTimeValue(BCD(val),BCD(p[74]), BCD(p[76]), BCD(p[78]), BCD(p[80]), 0);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		status = 0;
	}
	else
	{
		
		status = 1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.09.03 Fri.
Description:protocol_CEMS_HBManDeKe1_DCEM3000
TX:01 04 00 0C 00 11 F0 05
RX:01 04 22
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
00 00
5F 65

DataType and Analysis:
	(FLOAT_ABCD) 44 B4 08 00   = 1440.25
*/

int protocol_CEMS_HBManDeKe3_DCEM3000(struct acquisition_data *acq_data)
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

	cmd = 0x04;
	regpos = 0x0C;
	regcnt = 0x11;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf= getFloatValue(p, 3, dataType);

		//val = getInt16Value(p, 35, INT_AB);
		
		
		status=0;
	}
	else
	{
		status=1;
	}
	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,valf,valf,&arg_n); 
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	
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
Description:protocol_CEMS_HBManDeKe1_DCEM3000_info
TX:01 03 00 30 00 2D 85 D8
RX:01 03 5A
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
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
32 33 41 03
07 E5 00 08 00 09 00 12 00 00 00 00
07 E5 00 08 00 09 00 12 00 00
E2 72

DataType and Analysis:
	(INT_AB) 19 64  = 6500
*/
int protocol_CEMS_HBManDeKe3_DCEM3000_info(struct acquisition_data *acq_data)
{
#define MODBUS_POLCODE_NUM 5
	char *polcode[MODBUS_POLCODE_NUM]={"a34013a","a01012","a01013","a01011","a01014"};
	char *polcodeUnit[MODBUS_POLCODE_NUM]={"UNIT_MG_M3","UNIT_0C","UNIT_PA","UNIT_M_S","UNIT_PECENT"};
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
	int range = 0;

	
	struct tm timer;
	time_t t1,t2,t3;	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	struct rdtu_info *rdtuinfo;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;
	devaddr=modbusarg_running->devaddr;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;

	cmd = 0x02;
	regpos = 0x04;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 Range SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 Range protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 Range data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 Range RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	range = 0;
		else range = 1;
		
		status = 0;
	}
	else
	{
		status = 1;
	}


	sleep(1);
	cmd = 0x01;
	regpos = 0x05;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS1 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS1 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS1 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS1 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if(p[3] == 1)
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		status = 0;
	}else status = 1;

	
	sleep(1);
	cmd = 0x01;
	regpos = 0x10;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS2 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);
		else if(p[3] == 1){
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		}
		else 
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		status = 0;
	}
	else
	{
		status = 1;
	}

	sleep(1);
	cmd = 0x02;
	regpos = 0x03;
	regcnt = 0x01;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 STATUS3 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 STATUS3 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 STATUS3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 STATUS3 RECV:",com_rbuf,size);
	p = modbus_crc_check_coil(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		if(p[3] == 0 )
		 	acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		
		status = 0;
	}
	else
	{
		status = 1;
	}


	sleep(1);
	cmd = 0x03;
	regpos = 0x30;
	regcnt = 0x2D;
	dataType = FLOAT_CDAB ;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe DCEM3000 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe DCEM3000 INFO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe DCEM3000 INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe DCEM3000 INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		
		valf= getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf= getFloatValue(p, 55, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(p, 67, dataType);//register number 80
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		if(range == 0)
			valf= getFloatValue(p, 63, dataType);        //register number 78 
		else 
			valf= getFloatValue(p, 67, dataType);       //register number 80
		
		//acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		
		#if 0
		timer.tm_year=BCD(p[71])+100;
		timer.tm_mon=BCD(p[72])-1;
		timer.tm_mday=BCD(p[73]);
		timer.tm_hour=BCD(p[74]);
		timer.tm_min=BCD(p[75]);
		timer.tm_sec=0;
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		#endif
		val = getInt16Value(p, 71, INT_AB);
		t3 = getTimeValue(BCD(val),BCD(p[74]), BCD(p[76]), BCD(p[78]), BCD(p[80]), 0);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		status = 0;
	}
	else
	{
		
		status = 1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

