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

int protocol_TSP_ZhongLv_SGEP300PM(struct acquisition_data *acq_data)
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
	float smoke;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xC8,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongLv SGEP-300PM CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongLv SGEP-300PM CEMS RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		smoke=getFloatValue(com_rbuf, 3, FLOAT_ABCD);
		
		status=0;
	}
	else
	{
		smoke=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,smoke,&arg_n);
	

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_TSP_ZhongLv_SGEP300PM_info(struct acquisition_data *acq_data)
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
	MODBUS_DATA_TYPE dataType;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_TSP_ZhongLv_SGEP300PM_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x64,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongLv SGEP-300PM TSP STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongLv SGEP-300PM TSP STATUS RECV:",com_rbuf,size);
	if((size>=15)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[12];
		if(val!=0)
		{
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else
		{
			val=com_rbuf[11];
			if(val==0)
			{
				val=com_rbuf[3];
				if(val==0 || ((val&0x01)==0x01))
					acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				else if((val&0x02)==0x02)
					acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				else
					acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			}
			else if((val&0x01)==0x01)
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			

			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xCE,0x0C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZhongLv SGEP-300PM TSP INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZhongLv SGEP-300PM TSP INFO RECV:",com_rbuf,size);
	if((size>=29)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		valf=getFloatValue(com_rbuf, 3, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 23, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


