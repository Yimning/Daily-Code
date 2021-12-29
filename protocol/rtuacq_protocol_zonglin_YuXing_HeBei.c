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

int protocol_ZONGLIN_YuXing_HeBei(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float tp=0,orig=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0;
	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	dataType=FLOAT_CDAB;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1008,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"YuXing HeBei TP SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("YuXing HeBei TP protocol,TP 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("YuXing HeBei TP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"YuXing HeBei TP RECV1:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		tp=getFloatValue(com_rbuf, 3, dataType);
		
		status=0;
	}
	else
	{
		tp=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x10C6,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"YuXing HeBei TP SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("YuXing HeBei TP protocol,TP 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("YuXing HeBei TP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"YuXing HeBei TP RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		orig=getFloatValue(com_rbuf, 3, dataType);
	}
	else
	{
		orig=0;
	}


	acqdata_set_value_orig(acq_data,"w21011",UNIT_MG_L,tp,orig,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}




int protocol_ZONGLIN_YuXing_HeBei_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	unsigned int devaddr=0,cmd=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGLIN_YuXing_HeBei_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	dataType=FLOAT_CDAB;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1083,0x08);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"YuXing HeBei TP INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("YuXing HeBei TP INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("YuXing HeBei TP INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"YuXing HeBei TP INFO RECV1:",com_rbuf,size);
	if((size>=21)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 12:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 13:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 14:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 15:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 16:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 17:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 18:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 19:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 20:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 21:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 22:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 23:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}
	
		val=getUInt16Value(com_rbuf, 7, INT_AB);
		if(val==0 || val==9)
		{
				acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			switch(val)
			{
				case 1:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 2:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
				case 3:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 4:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 5:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 6:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 7:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
				case 8:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 10:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
				case 11:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
				case 12:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 13:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 14:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 15:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 16:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 17:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 18:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 19:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 20:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 21:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 22:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
				case 31:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 32:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 33:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 34:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				default:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
		}

		valf=getUInt16Value(com_rbuf, 15, INT_AB);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,valf,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x10A1,0x2D);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"YuXing HeBei TP INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("YuXing HeBei TP INFO protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("YuXing HeBei TP INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"YuXing HeBei TP INFO RECV2:",com_rbuf,size);
	if((size>=95)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,val,INFOR_ARGUMENTS,&arg_n);


		val=getUInt16Value(com_rbuf, 5, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);


		valf=getFloatValue(com_rbuf, 11, dataType);
		//acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);//update by Yimning 20210915

		valf=getFloatValue(com_rbuf, 15, dataType);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 19, dataType);
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 25, dataType);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 29, dataType);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(com_rbuf, 77, dataType);
		acqdata_set_value_flag(acq_data,"i13115",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(BCD(com_rbuf[81])+2000, BCD(com_rbuf[82]), BCD(com_rbuf[83]), BCD(com_rbuf[84]), BCD(com_rbuf[85]), BCD(com_rbuf[86]));
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		t3=getTimeValue(BCD(com_rbuf[87])+2000, BCD(com_rbuf[88]), BCD(com_rbuf[89]), BCD(com_rbuf[90]), BCD(com_rbuf[91]), BCD(com_rbuf[92]));
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	sleep(1);//update by Yimning 20210915
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1156,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"YuXing HeBei COD INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("YuXing HeBei COD3 INFO protocol,INFO3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("YuXing HeBei COD INFO3 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"YuXing HeBei COD INFO RECV3:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==cmd))
	{
		val=getUInt16Value(com_rbuf, 3, INT_AB);
		switch(val)
		{
			case 1:	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,1,INFOR_ARGUMENTS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,5,INFOR_ARGUMENTS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,100,INFOR_ARGUMENTS,&arg_n);break;
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

