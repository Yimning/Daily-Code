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
//#include "devices_mng_api.h"

/*
return : com_f arguments amount;
*/
int protocol_sampler_HBShiJiaZhuang(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int val=0;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	com_tbuf[1]=0x23;
	com_tbuf[2]=0x03;
	com_tbuf[3]=0xFF;
	com_tbuf[4]=0xFF;
	com_tbuf[5]=0xFF;
	com_tbuf[6]=0xFF;
	com_tbuf[7]=0xFF;
	com_tbuf[8]=0xFF;
	com_tbuf[9]=0xFF;
	com_tbuf[10]=0xFF;
	com_tbuf[11]=0xFF;
	com_tbuf[12]=0xFF;
	com_tbuf[13]=0x38;
	com_tbuf[14]=0x23;
	com_tbuf[15]=0x26;
	com_tbuf[16]=0x26;
	size=17;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBShiJiaZhuang SAMPLER STATUS  SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBShiJiaZhuang SAMPLER STATUS protocol, STATUS: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBShiJiaZhuang SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBShiJiaZhuang SAMPLER STATUS RECV:",com_rbuf,size);
	if((size>=31)&&(com_rbuf[0]==0x23)&&(com_rbuf[1]==0x23)&&(com_rbuf[29]==0x26)&&(com_rbuf[30]==0x26))
	{
		val=com_rbuf[7];
		status=0;
	}
	else
	{
		val=0;
		status=1;
	}
	
	ret=acqdata_set_value(acq_data,"i42001",UNIT_NONE,val,&arg_n);

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_sampler_HBShiJiaZhuang_info(struct acquisition_data *acq_data)
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

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_sampler_HBShiJiaZhuang_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"SAMPLER",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	com_tbuf[1]=0x23;
	com_tbuf[2]=0x03;
	com_tbuf[3]=0xFF;
	com_tbuf[4]=0xFF;
	com_tbuf[5]=0xFF;
	com_tbuf[6]=0xFF;
	com_tbuf[7]=0xFF;
	com_tbuf[8]=0xFF;
	com_tbuf[9]=0xFF;
	com_tbuf[10]=0xFF;
	com_tbuf[11]=0xFF;
	com_tbuf[12]=0xFF;
	com_tbuf[13]=0x38;
	com_tbuf[14]=0x23;
	com_tbuf[15]=0x26;
	com_tbuf[16]=0x26;
	size=17;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBShiJiaZhuang SAMPLER INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBShiJiaZhuang SAMPLER INFO protocol,INFO: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBShiJiaZhuang SAMPLER INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBShiJiaZhuang SAMPLER INFO RECV:",com_rbuf,size);
	if((size>=31)&&(com_rbuf[0]==0x23)&&(com_rbuf[1]==0x23)&&(com_rbuf[29]==0x26)&&(com_rbuf[30]==0x26))
	{
		acqdata_set_value_flag(acq_data,"i42001",UNIT_NONE,com_rbuf[7],INFOR_STATUS,&arg_n);

		timer.tm_year=BCD(com_rbuf[8])+100;
		timer.tm_mon=BCD(com_rbuf[9])-1;
		timer.tm_mday=BCD(com_rbuf[10]);
		timer.tm_hour=BCD(com_rbuf[11]);
		timer.tm_min=BCD(com_rbuf[12]);
		timer.tm_sec=BCD(com_rbuf[13]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):0;
		acqdata_set_value_flag(acq_data,"i42002",t3,0.0,INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,com_rbuf[14],INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		acqdata_set_value_flag(acq_data,"i43003",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[17];
		val<<=8;
		val+=com_rbuf[18];
		acqdata_set_value_flag(acq_data,"i43004",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,com_rbuf[19],INFOR_ARGUMENTS,&arg_n);

		acqdata_set_value_flag(acq_data,"i43006",UNIT_NONE,com_rbuf[20],INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[21];
		val<<=8;
		val+=com_rbuf[22];
		acqdata_set_value_flag(acq_data,"i43005",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,com_rbuf[23],INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,com_rbuf[24],INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i42003",UNIT_NONE,com_rbuf[25],INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i42006",UNIT_NONE,com_rbuf[26],INFOR_STATUS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

