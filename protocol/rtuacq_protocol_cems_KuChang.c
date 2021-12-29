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

static int KuChang_isEnable;
static int KuChang_status;
static int KuChang_alarm_status;
static int KuChang_alarm_info;
static char KuChang_flag;

int protocol_CEMS_KuChang(struct acquisition_data *acq_data)
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
	unsigned int devaddr=0,cmd=0,cnt=0;
	float so2=0,so2_orig=0,nox=0,nox_orig=0,co=0,co_orig=0;
	int unit=0;
	char *p=NULL;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x10;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang CEMS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		so2=getFloatValue(p, 3, dataType);
		so2 = so2 *100.0;
		so2=gas_ppm_to_mg_m3(so2, M_SO2);
		so2_orig=getFloatValue(p, 7, dataType);

		nox=getFloatValue(p, 11, dataType);
		nox = nox *100.0;
		nox=gas_ppm_to_mg_m3(nox, M_NOx);
		nox_orig=getFloatValue(p, 15, dataType);

		co=getFloatValue(p, 27, dataType);
		co = co *100.0;
		co=gas_ppm_to_mg_m3(co, M_CO);
		co_orig=getFloatValue(p, 31, dataType);
		
		status=0;
	}
	else
	{
		so2=0;
		so2_orig=0;
		nox=0;
		nox_orig=0;
		co=0;
		co_orig=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value_orig(acq_data,"a21026",UNIT_MG_M3,so2,so2_orig,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	
	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,nox_orig,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value_orig(acq_data,"a21005",UNIT_MG_M3,co,co_orig,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n);

	if(status == 0)
	{
		if(KuChang_isEnable==1)
			acq_data->dev_stat=KuChang_flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_CEMS_KuChang_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;

	struct tm timer;
	time_t t1;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_KuChang_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x03;

	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;

	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
/*	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x64,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang SO2 STATUS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, dataType);
 		if(val<0 || val>3)	val=99;
		acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		
		val=getUInt16Value(p, 5, dataType);
		val=(val==0)?0:1;
		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(p, 7, dataType);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
		
		status=0;
	}
*/
	if(KuChang_isEnable==1)
	{
		acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,KuChang_status,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,KuChang_alarm_status,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,KuChang_alarm_info,INFOR_STATUS,&arg_n);
	}

	//sleep(1);
	dataType=FLOAT_CDAB;
	cnt=0x1C;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xC8,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang SO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		valf = valf *100.0;
		valf=gas_ppm_to_mg_m3(valf, M_SO2);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 7, INT_AB);
		t1=getTimeValue(BCD(val), BCD(p[9]), BCD(p[10]), BCD(p[11]), BCD(p[12]), BCD(p[13]));
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		valf = valf *100.0;
		valf=gas_ppm_to_mg_m3(valf, M_SO2);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 31, INT_AB);
		t1=getTimeValue(BCD(val), BCD(p[33]), BCD(p[34]), BCD(p[35]), BCD(p[36]), BCD(p[37]));
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(p, 39, dataType);
		valf = valf *100.0;
		valf=gas_ppm_to_mg_m3(valf, M_SO2);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		if(p[55]!=0xFF || p[56]!=0xFF || p[57]!=0xFF || p[58]!=0xFF)
		{
			valf=getFloatValue(p, 55, dataType);
			acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		}
		
		status=0;
	}

	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_KuChang_NOx_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;

	struct tm timer;
	time_t t1;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_KuChang_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x03;

	if(!isPolcodeEnable(modbusarg, "a21002"))
		return 0;

	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
/*	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x67,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang NOx STATUS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, dataType);
 		if(val<0 || val>3)	val=99;
		acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		
		val=getUInt16Value(p, 5, dataType);
		val=(val==0)?0:1;
		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(p, 7, dataType);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
		
		status=0;
	}
*/
	//sleep(1);
	dataType=FLOAT_CDAB;
	cnt=0x1C;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0xE4,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang NOx INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		valf = valf *100.0;
		valf=gas_ppm_to_mg_m3(valf, M_NOx);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 7, INT_AB);
		t1=getTimeValue(BCD(val), BCD(p[9]), BCD(p[10]), BCD(p[11]), BCD(p[12]), BCD(p[13]));
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		valf = valf *100.0;
		valf=gas_ppm_to_mg_m3(valf, M_NOx);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 31, INT_AB);
		t1=getTimeValue(BCD(val), BCD(p[33]), BCD(p[34]), BCD(p[35]), BCD(p[36]), BCD(p[37]));
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(p, 39, dataType);
		valf = valf *100.0;
		valf=gas_ppm_to_mg_m3(valf, M_NOx);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		if(p[55]!=0xFF || p[56]!=0xFF || p[57]!=0xFF || p[58]!=0xFF)
		{
			valf=getFloatValue(p, 55, dataType);
			acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		}
		
		status=0;
	}

	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_KuChang_CO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;

	struct tm timer;
	time_t t1;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_KuChang_CO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x03;

	if(!isPolcodeEnable(modbusarg, "a21005"))
		return 0;

	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
/*	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x6A,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang CO STATUS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, dataType);
 		if(val<0 || val>3)	val=99;
		acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		
		val=getUInt16Value(p, 5, dataType);
		val=(val==0)?0:1;
		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(p, 7, dataType);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
		
		status=0;
	}
*/
	//sleep(1);
	dataType=FLOAT_CDAB;
	cnt=0x2A;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x011C,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang CO INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf=getFloatValue(p, 3, dataType);
		//valf=gas_ppm_to_mg_m3(valf, M_SO2);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 7, INT_AB);
		t1=getTimeValue(BCD(val), BCD(p[9]), BCD(p[10]), BCD(p[11]), BCD(p[12]), BCD(p[13]));
		acqdata_set_value_flag(acq_data,"i13021",t1,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 15, dataType);
		//valf=gas_ppm_to_mg_m3(valf, M_SO2);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 27, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 31, INT_AB);
		t1=getTimeValue(BCD(val), BCD(p[33]), BCD(p[34]), BCD(p[35]), BCD(p[36]), BCD(p[37]));
		acqdata_set_value_flag(acq_data,"i13027",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		
		valf=getFloatValue(p, 39, dataType);
		//valf=gas_ppm_to_mg_m3(valf, M_SO2);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 43, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 47, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		if(p[83]!=0xFF || p[84]!=0xFF || p[85]!=0xFF || p[86]!=0xFF)
		{
			valf=getFloatValue(p, 83, dataType);
			acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		}
		
		status=0;
	}

	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_PLC_KuChang(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	float valf[5]={0};
	int unit=0;
	char *p=NULL;
	float atm_press,PTC;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	
	rdtuinfo=rdtuinfo=get_parent_rdtu_info(acq_data);
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;

	dataType=FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0C;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang PLC", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		valf[0]=getFloatValue(p, 3, dataType);
		valf[1]=getFloatValue(p, 7, dataType);
		valf[2]=getFloatValue(p, 11, dataType);
		valf[3]=getFloatValue(p, 15, dataType);
		valf[3]=valf[3]*1000;

		valf[4]=getFloatValue(p, 19, dataType);
		
		if((p[19]!=0xFF || p[20]!=0xFF || p[21]!=0xFF || p[22]!=0xFF) && valf[4]!=0)
		{	

			if(PTC>0 && (valf[1]+273)>0 && (valf[3]+atm_press)>0)
				valf[4]=PTC*sqrt(((valf[1]+273)/273)*(101325/(valf[3]+atm_press))*(2/SGD)*valf[4]);
				//speed=PTC*sqrt(((valf[1]+273)/273)*(101325/(valf[3]+atm_press))*(2/SAD)*valf[4]);
			else
				valf[4]=0;
		}
		else 
		{
			valf[4]=getFloatValue(p, 23, dataType);
		}
		
		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		valf[2]=0;
		valf[3]=0;
		valf[4]=0;
		status=1;
	}


	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[0],&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);

	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[1],&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[2],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,valf[4],&arg_n);

	acqdata_set_value(acq_data,"a00000",UNIT_M3_H,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_H,0,&arg_n);
	

	if(status == 0)
	{
		if(KuChang_isEnable==1)
			acq_data->dev_stat=KuChang_flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_PLC_KuChang_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	union uf f;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;

	struct tm timer;
	time_t t1;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_PLC_KuChang_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x03;

	
	status=1;
	KuChang_isEnable=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x64,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "KuChang PLC STATUS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		KuChang_isEnable=1;
		
		val=getUInt16Value(p, 3, dataType);
 		switch(val)
		{
			case 0:	KuChang_status=0;KuChang_flag='N';break;
			case 1:	KuChang_status=1;KuChang_flag='M';break;
			case 2:	KuChang_status=2;KuChang_flag='D';break;
			case 3:	KuChang_status=3;KuChang_flag='C';break;
			case 4:	KuChang_status=5;KuChang_flag='z';break;
			default:	KuChang_status=99;KuChang_flag='N';break;
		}
		
		
		val=getUInt16Value(p, 7, dataType);
		KuChang_alarm_status = (val==0) ? 0 : 1;
		

		val=getUInt16Value(p, 5, dataType);
		switch(val)
		{
			case 0:	KuChang_alarm_info=0;break;
			case 2:	KuChang_alarm_info=2;break;
			case 3:	KuChang_alarm_info=3;break;
			case 8:	KuChang_alarm_info=99;break;
			case 9:	KuChang_alarm_info=9;break;
			default:	KuChang_alarm_info=99;break;
		}
		
		
		status=0;
	}

	if(!isPolcodeEnable(modbusarg, "a34013"))
		return 0;

	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	if(KuChang_isEnable==1)
	{
		acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,KuChang_status,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,KuChang_alarm_status,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,KuChang_alarm_info,INFOR_STATUS,&arg_n);
	}
	

	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


