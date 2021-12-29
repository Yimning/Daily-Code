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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

int protocol_O2_BJXueDiLong_MODEL1080(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float o2=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0,cmd=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x33);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong MODEL1080 O2 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong MODEL1080 O2 protocol,O2 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong MODEL1080 O2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong MODEL1080 O2 RECV:",com_rbuf,size);
	if((size>=107)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		o2=getInt16Value(com_rbuf, 5, INT_AB);
		o2*=0.01;
		
		status=0;
	}
	else
	{
		o2=0;
		status=1;
	}
	
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



int protocol_O2_BJXueDiLong_MODEL1080_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int y,m,d,h,min;
	int val=0;
	int devaddr=0,cmd=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_O2_BJXueDiLong_MODEL1080_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x33);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SO2 INFO RECV:",com_rbuf,size);
	if((size>=107)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		if(com_rbuf[3]!=0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else
		{
			val=com_rbuf[4]&0x0F;
			switch(val)
			{
				case 1:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 3:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 4:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 5:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 6:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				default:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
			
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
	

		val=getUInt16Value(com_rbuf, 19, INT_AB);
		y=(val&0xFE00)>>9;
		m=(val&0x01E0)>>5;
		d=(val&0x001F)>>0;
		h=com_rbuf[21];
		min=com_rbuf[22];
		t3=getTimeValue(y, m, d, h, min, 0);
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(com_rbuf, 23, INT_AB);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 27, INT_AB);
		y=(val&0xFE00)>>9;
		m=(val&0x01E0)>>5;
		d=(val&0x001F)>>0;
		h=com_rbuf[29];
		min=com_rbuf[30];
		t3=getTimeValue(y, m, d, h, min, 0);
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(com_rbuf, 31, INT_AB);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(com_rbuf, 35, INT_AB);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(com_rbuf, 39, INT_AB);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(com_rbuf, 43, INT_AB);
		valf*=0.01;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


