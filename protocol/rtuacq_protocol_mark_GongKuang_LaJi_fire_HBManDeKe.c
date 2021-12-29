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
Email:1148967988@qq.com
Create Time:2021.10.21 Thur.
Description:protocol_mark_GongKuang_LaJi_fire_HBManDeKe
TX:01 04 00 00 00 1A 71 C1 
RX: 

TX1:01 04 03 E8 00 1E F0 72 
RX1: 

DataType and Analysis:
	(FLOAT_CDAB) 00 00 42 C8  = 100
*/

int protocol_mark_GongKuang_LaJi_fire_HBManDeKe(struct acquisition_data *acq_data)
{
#define Fire_POLCODE_NUM   11
   char polcodeStr[][20]={"SCXBH","CSXTLJCLL","TXSSTXHYJYL","TLSSBGFSHJYL","TLSSGFSHSYL","TLSSXSTNJYPHZ",
	"TLSSSFYHFJDL","TLSSJYXHBYXZT","HXTTJXTHXTYL","CCSSDSCCQJCKYC","GXSJ"
	};

   UNIT_DECLARATION unitStr[Fire_POLCODE_NUM]={UNIT_NONE,
   	UNIT_NONE,UNIT_NONE,UNIT_KG,UNIT_KG,UNIT_NONE,
	UNIT_A,UNIT_NONE,UNIT_KG,UNIT_V,UNIT_NONE
	};

#define MODBUS_POLCODE_NUM 8
	char *polcode[MODBUS_POLCODE_NUM]={"a01901","i33311","i33310","i33312","i33321"
	,"i33320","i33322","a01902"};

   	UNIT_DECLARATION polcodeUnit[1]={UNIT_0C};
	int *adrrstr[MODBUS_POLCODE_NUM]={0x00,0x06,0x0C,0x12,0x18};


   int posnum[Fire_POLCODE_NUM]={
   	0,
   	2,4,6,8,10,
	12,14,16,18,20
   	};

	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0}; 
	int size=0;
	union uf f;
	int ret=0;
	int i=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	float valfArray[Fire_POLCODE_NUM] = {0};
	
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
	int log_len=0;
	int tm[TIME_ARRAY]={0};
	struct tm timer;
	time_t t1,t2,t3;
	int year=0;
	int mon=0;
	int day=0;
	int hour=0;
	int min=0;
	int sec=0;
	
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	
	cmd = 0x04;
	regpos = 0x00;
	regcnt = 0x1A;
	dataType = FLOAT_CDAB ;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,regpos,regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_mark_GongKuang_LaJi_fire_HBManDeKe CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_mark_GongKuang_LaJi_fire_HBManDeKe CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_mark_GongKuang_LaJi_fire_HBManDeKe CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_mark_GongKuang_LaJi_fire_HBManDeKe CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p!=NULL)
	{
		for(i=0;i<Fire_POLCODE_NUM-1;i++)
		{
			valfArray[i]= getFloatValue(p, 3+i*4, dataType);
			acqdata_set_value(acq_data,polcodeStr[i],unitStr[i],valfArray[i],&arg_n);
		}
		
		 year = getInt16Value(p,43 , INT_AB);
		 mon = getInt16Value(p,45 , INT_AB);
		 day = getInt16Value(p,47 , INT_AB);
		 hour = getInt16Value(p,49 , INT_AB);
		 min = getInt16Value(p,51 , INT_AB);
		 sec = getInt16Value(p,53 , INT_AB);

	        t1 = getTimeValue( year,  mon,  day,  hour,  min,  sec);
		acqdata_set_value(acq_data,polcodeStr[10],t1,0,&arg_n);
			
		status = 0;
	}
	else
	{
		for(i=0;i<Fire_POLCODE_NUM;i++)
		{
			valfArray[i]= 0;
			acqdata_set_value(acq_data,polcodeStr[i],unitStr[i],valfArray[i],&arg_n);
		}
		status=1;
	}


	sleep(2);
	cmd = 0x04;
	regpos = 0x3E8;
	regcnt=0x1E;
	dataType = FLOAT_CDAB ;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LaJi_pack_HBManDeKeDCS CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKeDCS CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBZhongJieNeng DCS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LaJi_pack_HBManDeKeDCS CEMS RECV:",com_rbuf,size);
	p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
	if(p !=NULL)
	{
		for(i=0;i<MODBUS_POLCODE_NUM-1;i++)
		{
			valfArray[i] = getFloatValue(p, 7+i*4, dataType);
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,valfArray[i],&arg_n);
		}
		
		valf= getFloatValue(p, 55, dataType);
		acqdata_set_value(acq_data,polcode[7],UNIT_0C,valf,&arg_n);
		
		status = 0; 
	}
	else
	{
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],UNIT_0C,0,&arg_n);
		}

		status = 1;
	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}