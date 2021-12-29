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

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.17 Sun.
Description:protocol_CEMS_BJXueDiLong_SCS900PM
TX1:01 03 00 1A 00 02 E5 CC 
RX1:01 03 04 
5D 5D  3C  4B  
93  BA

TX2:01 03 00 00 00 02 C4 0B 
RX2:01 03 04 
5D 5D  3C  4B  
93  BA

DataType and Analysis:
	(FLOAT_CDAB) 5D 5D  3C  4B   = 0.0124
*/

int protocol_CEMS_BJXueDiLong_SCS900PM(struct acquisition_data *acq_data)
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
	int cmd=0;
	int val=0;
	float valf = 0;
	float smoke=0,orig=0;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900PM CEMS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900PM CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900PM CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900PM CEMS RECV1:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
	#if 1 
		f.ch[3] = com_rbuf[5];
		f.ch[2] = com_rbuf[6];	
		f.ch[1] = com_rbuf[3];	
		f.ch[0] = com_rbuf[4];	
	#endif
		smoke=f.f;
		status=0;
	}
	else
	{
		smoke=0;
		status=1;
	}

        sleep(1);
	cmd = 0x03;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900PM CEMS SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900PM: read device2 %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900PM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900PM RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
	#if 1 
		f.ch[3] = com_rbuf[5];
		f.ch[2] = com_rbuf[6];	
		f.ch[1] = com_rbuf[3];	
		f.ch[0] = com_rbuf[4];	
	#endif
		orig=f.f;
	
	}
	else 
	{
		orig=0;	
	}

	acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a34013",UNIT_MG_M3,smoke,orig,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

/*
RX1:01 01 00 00 00 01 FD CA 
TX1:01 01 01 00 00 FF FF



RX2:01 01 00 01 00 01 AC 0A 
TX2:01 01 00 00 00 FF FF

DataType and Analysis:
	(INT_AB) 01 = 1
*/

int protocol_CEMS_BJXueDiLong_SCS900PM_info(struct acquisition_data *acq_data)
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
	int cmd=0;
	int val=0;
	float valf = 0;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_scs900PM_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	cmd = 0x01;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900PM INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900PM protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900PM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900PM INFO RECV1:",com_rbuf,size);
	if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
	        val = com_rbuf[3];
			SYSLOG_DBG("COME1 : %d\n",val);
		switch(val){
        		case 0:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
        		case 1:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				break;
		}
		status=0;
	}

	sleep(1);
	cmd = 0x01;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x01,0x1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900PM INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900PM protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900PM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900PM INFO RECV2:",com_rbuf,size);
	if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
	        val = com_rbuf[3];
		SYSLOG_DBG("COME2 : %d\n",val);
		switch(val){
        		case 0:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
        		case 1:
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				break;
		}
		status=0;
	}else 
	{
		status =1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;

}

/*
RX:01 03 00 1A 00 02 E5 CC 
TX:01 03 20 
47 AE 41 05
00 00 00 00
00 00 00 00
00 00 00 00
5D 5D 3C 4B
5D 5D 3C 4B
5D 5D 3C 4B
5D 5D 3C 4B
FF FF

DataType and Analysis:
	(FLOAT_CDAB) 5D 5D  3C  4B   = 0.0124
*/
#if 1
int protocol_CEMS_BJXueDiLong_SCS900PM_info1(struct acquisition_data *acq_data)
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
	int cmd=0;
	int val=0;
	float valf = 0;
	
	struct tm timer;
	time_t t1,t2,t3;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_scs900PM_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_CDAB;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	sleep(1);
	cmd = 0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0A,0x10);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong SCS_900PM INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong SCS_900PM protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_900PM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong SCS_900PM INFO RECV3:",com_rbuf,size);
	if((size>=37)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		valf = getFloatValue(com_rbuf, 3, dataType);
		//data->i13012 = valf
		//SYSLOG_DBG("valf%f\n",valf);
		acqdata_set_value_flag(acq_data,"i13012",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf= getFloatValue(com_rbuf, 19, dataType);
		//data->i13026 = valf;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(com_rbuf, 23, dataType);
		//data->i13022 = valf;
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(com_rbuf, 27, dataType);
		//data->i13013 = valf;
		//SYSLOG_DBG("valf3013%f\n",valf);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}
	else status = 1;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;

}
#endif
