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

static char TSP_AnRomhXin_Mark = 'N';
int protocol_TSP_AnRomhXin(struct acquisition_data *acq_data)
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
	float tsp;
	unsigned int crc;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=devaddr;
	com_tbuf[1]=0x66;
	com_tbuf[2]=0x04;
	com_tbuf[3]=0x9B;
	com_tbuf[4]=0x00;
	com_tbuf[5]=0x00;
	com_tbuf[6]=0x00;
	com_tbuf[7]=0x00;
	crc = CRC16_modbus(com_tbuf, 8);
	com_tbuf[8]=crc&0xFF;
	com_tbuf[9]=(crc>>8)&0xFF;
	
	size=10;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"AnRomhXin TSP SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnRomhXin TSP protocol,TSP : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnRomhXin TSP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"AnRomhXin TSP RECV:",com_rbuf,size);
	if((size>=36)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x66)&&(com_rbuf[2]==0x1E))
	{
		f.ch[0]=com_rbuf[12];
		f.ch[1]=com_rbuf[11];
		f.ch[2]=com_rbuf[10];
		f.ch[3]=com_rbuf[9];
		tsp=f.f;
		
		status=0;
	}
	else
	{
		tsp=0;
		status=1;
	}

    acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,tsp,&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,tsp,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);

	if(status == 0){
		acq_data->acq_status = ACQ_OK;
		acq_data->dev_stat = TSP_AnRomhXin_Mark;
	}else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_TSP_AnRomhXin_info(struct acquisition_data *acq_data)
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
	unsigned int crc;
	MODBUS_DATA_TYPE dataType;
	float i13013;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_TSP_AnRomhXin_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	dataType=FLOAT_ABCD;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=devaddr;
	com_tbuf[1]=0x66;
	com_tbuf[2]=0x04;
	com_tbuf[3]=0x9B;
	com_tbuf[4]=0x00;
	com_tbuf[5]=0x00;
	com_tbuf[6]=0x00;
	com_tbuf[7]=0x00;
	crc = CRC16_modbus(com_tbuf, 8);
	com_tbuf[8]=crc&0xFF;
	com_tbuf[9]=(crc>>8)&0xFF;
	
	size=10;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"AnRomhXin TSP STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnRomhXin TSP STATUS protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnRomhXin TSP STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"AnRomhXin TSP STATUS RECV:",com_rbuf,size);
	if((size>=36)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x66)&&(com_rbuf[2]==0x1E))
	{
		val=com_rbuf[30]&0x03;
		if(val==0x00)
		{
			val=(com_rbuf[30]>>3)&0x03;
			if(val==0x00){
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				TSP_AnRomhXin_Mark = 'N';
			}
			else{
				acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				TSP_AnRomhXin_Mark = 'C';
			}
		}
		else if(val==0x01){
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			TSP_AnRomhXin_Mark = 'D';
		}
		else if(val==0x02){
			acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			TSP_AnRomhXin_Mark = 'M';
		}
		
		val=com_rbuf[31];
		if(val==0x00)
		{
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=devaddr;
	com_tbuf[1]=0x66;
	com_tbuf[2]=0x00;
	com_tbuf[3]=0x9C;
	crc = CRC16_modbus(com_tbuf, 4);
	com_tbuf[4]=crc&0xFF;
	com_tbuf[5]=(crc>>8)&0xFF;

	size=6;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"AnRomhXin TSP INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AnRomhXin TSP protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("AnRomhXin TSP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"AnRomhXin TSP INFO RECV:",com_rbuf,size);
	if((size>=63)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x66)&&(com_rbuf[2]==0x39))
	{
		valf=getFloatValue(com_rbuf, 8, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 12, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(com_rbuf, 16, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		i13013=valf;

		valf=getFloatValue(com_rbuf, 18, dataType);
		acqdata_set_value_flag(acq_data,"i13015",UNIT_SECOND,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(com_rbuf, 29, INT_AB);
		t3=getTimeValue(val, com_rbuf[31], com_rbuf[32], com_rbuf[33], com_rbuf[34], com_rbuf[35]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13027",t3,0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 36, dataType);
		valf=valf/i13013*100;
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 40, dataType);
		valf=valf/i13013*100;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

