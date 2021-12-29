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

int protocol_ZONGLIN_HZLuHeng_HeBei(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float tp=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x3A98,0x52);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HZLuHeng_HeBei TP SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HZLuHeng_HeBei TP protocol,TP : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HZLuHeng_HeBei TP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HZLuHeng_HeBei TP RECV:",com_rbuf,size);
	if((size>=169)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		tp=f.f;
		
		status=0;
	}
	else
	{
		tp=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w21011",UNIT_MG_L,tp,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



int protocol_ZONGLIN_HZLuHeng_HeBei_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_ZONGLIN_HZLuHeng_HeBei_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x3A98,0x52);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HZLuHeng TP INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HZLuHeng TP INFO protocol,INFO: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HZLuHeng TP INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HZLuHeng TP INFO RECV:",com_rbuf,size);
	if((size>=169)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[39];
		val<<=8;
		val+=com_rbuf[40];

		switch(val)
		{
			case 1: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 7: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val=com_rbuf[41];
		val<<=8;
		val+=com_rbuf[42];

		if(val==1)
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		}
		
		switch(val)
		{
			case 1: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 6: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n); break;
			case 7: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		f.ch[3]=com_rbuf[47];
		f.ch[2]=com_rbuf[48];
		f.ch[1]=com_rbuf[45];
		f.ch[0]=com_rbuf[46];
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=hex2dec(com_rbuf[52])+100;
		timer.tm_mon=hex2dec(com_rbuf[54])-1;
		timer.tm_mday=hex2dec(com_rbuf[56]);
		timer.tm_hour=hex2dec(com_rbuf[58]);
		timer.tm_min=hex2dec(com_rbuf[60]);
		timer.tm_sec=hex2dec(com_rbuf[62]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		timer.tm_year=hex2dec(com_rbuf[72])+100;
		timer.tm_mon=hex2dec(com_rbuf[74])-1;
		timer.tm_mday=hex2dec(com_rbuf[76]);
		timer.tm_hour=hex2dec(com_rbuf[78]);
		timer.tm_min=hex2dec(com_rbuf[80]);
		timer.tm_sec=hex2dec(com_rbuf[82]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[89];
		f.ch[2]=com_rbuf[90];
		f.ch[1]=com_rbuf[87];
		f.ch[0]=com_rbuf[88];
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[93];
		f.ch[2]=com_rbuf[94];
		f.ch[1]=com_rbuf[91];
		f.ch[0]=com_rbuf[92];
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		val=com_rbuf[95];
		val<<=8;
		val+=com_rbuf[96];
		val=(val/0x1000*1000)+(val/0x100%0x10*100)+(val/0x10%0x10*10)+(val%0x10);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[99];
		f.ch[2]=com_rbuf[100];
		f.ch[1]=com_rbuf[97];
		f.ch[0]=com_rbuf[98];
		acqdata_set_value_flag(acq_data,"i13104",UNIT_0C,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[103];
		f.ch[2]=com_rbuf[104];
		f.ch[1]=com_rbuf[101];
		f.ch[0]=com_rbuf[102];
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[111];
		f.ch[2]=com_rbuf[112];
		f.ch[1]=com_rbuf[109];
		f.ch[0]=com_rbuf[110];
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[115];
		f.ch[2]=com_rbuf[116];
		f.ch[1]=com_rbuf[113];
		f.ch[0]=com_rbuf[114];
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[119];
		f.ch[2]=com_rbuf[120];
		f.ch[1]=com_rbuf[117];
		f.ch[0]=com_rbuf[118];
		acqdata_set_value_flag(acq_data,"i13109",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[123];
		f.ch[2]=com_rbuf[124];
		f.ch[1]=com_rbuf[121];
		f.ch[0]=com_rbuf[122];
		acqdata_set_value_flag(acq_data,"i13111",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[134];
		f.ch[2]=com_rbuf[135];
		f.ch[1]=com_rbuf[132];
		f.ch[0]=com_rbuf[133];
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

