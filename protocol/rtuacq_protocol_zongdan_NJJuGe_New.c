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

/*
Author:Yimning
Time:2021.11.11 Thur.
Description:protocol_ZONGDAN_NJJuGe_New
TX:01 03 06 40 00 05 84 95 
RX:01 03 0A 00 54 00 00 3F C8 00 00 3F C8 1E CC 
DataType and Analysis:
	(FLOAT_CDAB) 00 00 3F C8  = 1.5625
*/

static float zongdan_orig = 0;
static int TN_NJJuGe_MARK = 0;
int protocol_ZONGDAN_NJJuGe_New(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float zongdan = 0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;

	MODBUS_DATA_TYPE dataType;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x0640;
	regcnt = 0x05;
	dataType = FLOAT_CDAB ;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJJuGe TN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJJuGe TN protocol,TN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJJuGe TN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJJuGe TN RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		zongdan = getFloatValue(p, 5, dataType);
		
		zongdan_orig= getFloatValue(p, 9, dataType);

		val = getInt16Value(p, 3, INT_AB);
		
		switch(TN_NJJuGe_MARK)
		{
			case 0x004E:acq_data->dev_stat = 'N';break;
			case 0x0054:acq_data->dev_stat = 'T';break;
			case 0x004D:acq_data->dev_stat = 'M';break;
			case 0x5343:acq_data->dev_stat = 'K';break;
			case 0x0044:acq_data->dev_stat = 'D';break;
			case 0x004C:acq_data->dev_stat = 'N';break;
			case 0x0050:acq_data->dev_stat = 'P';break;
			case 0x0046:acq_data->dev_stat = 'V';break;
			default:acq_data->dev_stat = 'N';break;
		}
		
		status=0;
	}
	else
	{
		zongdan=0;
		status=1;
	}

	ret=acqdata_set_value_orig(acq_data,"w21003",UNIT_MG_L,zongdan,zongdan_orig,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}




int protocol_ZONGDAN_NJJuGe_New_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p = NULL;
	int val=0;
	float valf = 0;
	
	union uf f;
	MODBUS_DATA_TYPE dataType;
	struct tm timer;
	time_t t1,t2,t3;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_ZONGDAN_NJJuGe_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x03;
	regpos = 0x0618;
	regcnt = 0x05;
	dataType = FLOAT_CDAB ;

	
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"w21003",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJJuGe TN INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJJuGe TN INFO protocol,INFO: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJJuGe TN INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJJuGe TN INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
 		val = getInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 7:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 8:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
			case 9:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 10:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 11:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 12:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 13:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 14:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}

		val= getInt16Value(p, 7, INT_AB);
		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			switch(val)
			{
				case 1:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 2:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
				case 3:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
				case 4:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
				case 5:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
				case 6:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
				case 7:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
				case 8:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,10,INFOR_STATUS,&arg_n);break;
				case 9:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,8,INFOR_STATUS,&arg_n);break;
				case 10:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				default:	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
		}
		TN_NJJuGe_MARK = getInt16Value(p, 11, INT_AB);
		status = 0;
	}

	cmd = 0x03;
	regpos = 0x0629;
	regcnt = 0x67;
	dataType = FLOAT_CDAB ;
	status=1;
	t1 = 0;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJJuGe TN INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJJuGe TN INFO protocol,INFO: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJJuGe TN INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJJuGe TN INFO RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		valf = getFloatValue(p, 3,  dataType);	
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		 year = getInt16Value(p, 15,  INT_AB);
	 	 mon = getInt16Value(p, 17,  INT_AB);
	  	 day = getInt16Value(p, 19,  INT_AB);
	 	 hour = getInt16Value(p, 21,  INT_AB);
	 	 min = getInt16Value(p, 23,  INT_AB);
	 	 sec = getInt16Value(p, 25,  INT_AB);
		 t1 = getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13128",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 31,  dataType);	
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 89,  dataType);	
		acqdata_set_value_flag(acq_data,"i13104",UNIT_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 93,  dataType);	
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		/*
		year = getInt16Value(p, 97,  INT_AB);
	 	 mon = getInt16Value(p, 99,  INT_AB);
	  	 day = getInt16Value(p, 101,  INT_AB);
	 	 hour = getInt16Value(p, 103,  INT_AB);
	 	 min = getInt16Value(p, 105,  INT_AB);
	 	 sec = getInt16Value(p, 107,  INT_AB);
		 t1 = getTimeValue(year, mon, day, hour, min, sec);
		 acqdata_set_value_flag(acq_data,"i13101",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		 */

		timer.tm_year=BCD(p[98])+100;
		timer.tm_mon=BCD(p[100])-1;
		timer.tm_mday=BCD(p[102]);
		timer.tm_hour=BCD(p[104]);
		timer.tm_min=BCD(p[106]);
		timer.tm_sec=BCD(p[108]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 109,  dataType);	
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 113,  dataType);	
		acqdata_set_value_flag(acq_data,"i13109",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		/*
		 year = getInt16Value(p, 117,  INT_AB);
	 	 mon = getInt16Value(p, 119,  INT_AB);
	  	 day = getInt16Value(p, 121,  INT_AB);
	 	 hour = getInt16Value(p, 123,  INT_AB);
	 	 min = getInt16Value(p, 125,  INT_AB);
	 	 sec = getInt16Value(p, 127,  INT_AB);
		 t1 = getTimeValue(year, mon, day, hour, min, sec);
		 acqdata_set_value_flag(acq_data,"i13107",t1,0.0,INFOR_ARGUMENTS,&arg_n);
		 */
		 
		timer.tm_year=BCD(p[118])+100;
		timer.tm_mon=BCD(p[120])-1;
		timer.tm_mday=BCD(p[122]);
		timer.tm_hour=BCD(p[124]);
		timer.tm_min=BCD(p[126]);
		timer.tm_sec=BCD(p[128]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		

		valf = getFloatValue(p, 133,  dataType);	
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 137,  dataType);	
		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 141,  dataType);	
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 145,  dataType);	
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(p, 153, INT_AB);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,val/60,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 155,  dataType);	
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 159,  dataType);	
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);

		val=getInt16Value(p, 163, INT_AB);
		acqdata_set_value_flag(acq_data,"i13123",UNIT_MINUTE,val/60,INFOR_ARGUMENTS,&arg_n);
		
		val = getInt16Value(p, 177, INT_AB);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val/60,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

