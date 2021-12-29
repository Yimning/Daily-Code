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
int protocol_sampler_BJGeRuiSiPu_FC9624YLAB(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int val=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	char *p;
	union uf f;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x01;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1045,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJGeRuiSiPu FC-9624YLAB sampler", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, INT_AB);
		val = (val==1)?1:0;
		status=0;
	}
	else
	{
		val=0;
		status=1;
	}

	acqdata_set_value(acq_data,"i42001",UNIT_NONE,val,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_sampler_BJGeRuiSiPu_FC9624YLAB_info(struct acquisition_data *acq_data)
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
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_sampler_BJGeRuiSiPu_FC9624YLAB_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0E;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"SAMPLER",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1042,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJGeRuiSiPu FC-9624YLAB sampler INFO1", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=(p[4]==0)?1:0;
		acqdata_set_value_flag(acq_data,"i42003",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(p, 7, INT_AB);
		acqdata_set_value_flag(acq_data,"i43006",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 9, INT_AB);
		val = (val==1)?1:0;
		acqdata_set_value_flag(acq_data,"i42001",UNIT_NONE,val,INFOR_STATUS,&arg_n);

		val=getUInt16Value(p, 23, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i42004",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
		}

		val=getUInt16Value(p, 27, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,13,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i42005",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
		}
		
		status=0;
	}

	sleep(1);
	cnt=0x0B;
	size=modbus_pack(com_tbuf,devaddr,cmd,0x1000,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJGeRuiSiPu FC-9624YLAB sampler INFO2", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		t3=getTimeValue(BCD(p[3])+2000, BCD(p[4]), BCD(p[5]), BCD(p[6]), BCD(p[7]),BCD(p[8]));
		acqdata_set_value_flag(acq_data,"i42002",t3,0,INFOR_STATUS,&arg_n);

		val=(p[9]==1)?5:6;
		acqdata_set_value_flag(acq_data,"i43002",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 11, INT_AB);
		switch(val)
		{
			case 2:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,2,INFOR_ARGUMENTS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,3,INFOR_ARGUMENTS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,4,INFOR_ARGUMENTS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,6,INFOR_ARGUMENTS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i43001",UNIT_NONE,7,INFOR_ARGUMENTS,&arg_n);break;
		}

		val=getUInt16Value(p, 17, INT_AB);
		acqdata_set_value_flag(acq_data,"i43005",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 21, INT_AB);
		acqdata_set_value_flag(acq_data,"i43004",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 23, INT_AB);
		acqdata_set_value_flag(acq_data,"i43003",UNIT_NONE,val,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

