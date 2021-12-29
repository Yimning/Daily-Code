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

char CEMS_JuGuang_HeBei_2000D_STATUS;


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.08.25 Wed.
Description:protocol_CEMS_JuGuang_HeBei_2000D_SO2
TX:01 03 00 00 00 02 C4 0B 
RX:01 03 04 1C D5 3F 8E 7D CF

DataType and Analysis:
	(FLOAT_CDAB) 08 00 44 B4  = 1440.25
*/
int protocol_CEMS_JuGuang_HeBei_2000D_SO2(struct acquisition_data *acq_data)
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
	float valf = 0;
	int val = 0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x02;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang 2000D SO2 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang 2000D SO2  CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang 2000D SO2  CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang 2000D SO2  CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		status=0;
	}
	else
	{
		status=1;
	}
	
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,valf,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,valf,valf,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
/*
TX:01 03 00 02 00 23 A5 D3
RX:01 03 46
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
20 20 12 09 09 09
00 00
1C D5 3F 8E
20 21 01 09 09 09
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
20 21 01 09 00 00 00 00
D5 8C

*/
int protocol_CEMS_JuGuang_HeBei_2000D_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	union uf f;
	
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x02;
	regcnt = 0x23;
	dataType = FLOAT_CDAB;
	t1 = 0;
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang 2000D SO2  INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang 2000D SO2  INFO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang 2000D SO2  INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang 2000D SO2  INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf =  getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf =  getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf =  getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 19, INT_AB);
		t3=getTimeValue(val, BCD(p[21]), BCD(p[22]), BCD(p[23]), BCD(p[24]), 0);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		valf =  getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(p, 31, INT_AB);
		t3=getTimeValue(val, BCD(p[33]), BCD(p[34]), BCD(p[35]), BCD(p[36]), 0);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.08.25 Wed.
Description:protocol_CEMS_JuGuang_HeBei_2000D_NOx
TX:01 03 00 00 00 06 C5 C8 
RX:01 03 0C 
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
89 81

DataType and Analysis:
	(FLOAT_CDAB) 08 00 44 B4   = 1440.25
*/
int protocol_CEMS_JuGuang_HeBei_2000D_NOx(struct acquisition_data *acq_data)
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
	float valf = 0;
	int val = 0;
	char *p=NULL;
	char flag=0;
	float nox= 0,no = 0,no2 = 0;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x00;
	regcnt = 0x06;
	dataType = FLOAT_CDAB;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang 2000D NOx CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang  2000D NOx CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang  2000D NOx CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang  2000D NOx CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		nox =  getFloatValue(p, 3, dataType);

		no =  getFloatValue(p, 7, dataType);

		no2 =  getFloatValue(p, 11, dataType);
		
		status=0;
	}
	else
	{
		status=1;
	}

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value_orig(acq_data,"a21003",UNIT_MG_M3,no,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21004",UNIT_MG_M3,no2,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


/*
RX:01 03 00 06 00 23 E4 12 
TX:01 03 46
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
1C D5 3F 8E
20 20 12 09 09 09
00 00
1C D5 3F 8E
20 21 01 09 09 09
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
20 21 01 09 00 00 00 00
D5 8C
*/

int protocol_CEMS_JuGuang_HeBei_2000D_NOx_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	union uf f;
	
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x06;
	regcnt = 0x23;
	dataType = FLOAT_CDAB;
	t1 = 0;
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang 2000D NOx  INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang 2000D NOx  INFO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang 2000D NOx  INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang 2000D NOx  INFO RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf =  getFloatValue(p, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf =  getFloatValue(p, 7, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf =  getFloatValue(p, 11, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf =  getFloatValue(p, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 19, INT_AB);
		t3=getTimeValue(val, BCD(p[21]), BCD(p[22]), BCD(p[23]), BCD(p[24]), 0);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		valf =  getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	        val = getInt16Value(p, 31, INT_AB);
		t3=getTimeValue(val, BCD(p[33]), BCD(p[34]), BCD(p[35]), BCD(p[36]), 0);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
	else
	{
		status=1;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}




