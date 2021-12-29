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

static float ZONGLIN_LiDa_orig=0;

/*
Author:Yimning
Time:2021.06.09 Wed.
Description:protocol_ZONGLIN_LiDa
TX:01 03 06 D7 00 0A 75 7D  
RX:01 03 14 47 AE 41 05 06 25 3D 81 00 17 00 06 00 02 00 08 00 00 00 00 87 87

*/


int protocol_ZONGLIN_LiDa(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_ZONGLIN=0;
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
	size=modbus_pack(com_tbuf,devaddr,0x3,0x06D7,0x0A);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"LiDa ZONGLIN SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("LiDa ZONGLIN protocol,ZONGLIN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("LiDa ZONGLIN data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"LiDa ZONGLIN RECV:",com_rbuf,size);
	if((size>=25)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		total_ZONGLIN=f.f;

		f.ch[0]=com_rbuf[8];
		f.ch[1]=com_rbuf[7];
		f.ch[2]=com_rbuf[10];
		f.ch[3]=com_rbuf[9];
		ZONGLIN_LiDa_orig=f.f;
		
		status=0;
	}
	else
	{
		total_ZONGLIN=0;
		status=1;
	}

	ret=acqdata_set_value_orig(acq_data,"w21011",UNIT_MG_L,total_ZONGLIN,ZONGLIN_LiDa_orig,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

 
/*
Author:Yimning
Time:2021.06.09 Wed.
Description:protocol_ZONGLIN_LiDa_info
TX:01 03 06 E8 00 23 84 AF 
RX:01 03 02 
	00 01 
	00 00 
	00 01 00 00 00 00 00 01 00 17 00 05 00 31 00 10 00 24 00 00 00 01 00 00 41 00 00 01 00 17 00 05 00 31 00 10 00 24 00 00
	00 00 15 84
	00 00 3F 80 00 00 00 00
	00 10 00 00 42 64
	00 00 44 7A CC CD 3D CC
	FF FF

*/	
int protocol_ZONGLIN_LiDa_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_ZONGLIN=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float valf=0;
	int val=0;
	char hexarry[8] = {0};
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	struct tm timer;
	time_t t1,t2,t3;

	t1=0;//1577808000; // 2020-01-01 00:00:00

	float ZONGLIN_zero=0,ZONGLIN_range=0,ZONGLIN_K=0,ZONGLIN_PPB=0;
	float ZONGLIN_correction_factor=0,ZONGLIN_adjust_zero_drift=0,ZONGLIN_range_drift=0,ZONGLIN_intercept=0,ZONGLIN_BDXS=0,cod_original_Val=0;
	float ZONGLIN_digestion_time=0,ZONGLIN_digestion_temp=0;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	SYSLOG_DBG("protocol_ZONGLIN_LiDa_info\n");
	acqdata_set_value_flag(acq_data,"w21011",UNIT_MG_L,valf,INFOR_ARGUMENTS,&arg_n);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x06E8,0x23);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_ZONGLIN_LiDa_info SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_ZONGLIN_LiDa_info,ZONGLIN : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_ZONGLIN_LiDa_info data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_ZONGLIN_LiDa_info RECV:",com_rbuf,size);
	if((size>=75)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val = com_rbuf[3];
		val = val << 8;
		val += com_rbuf[4];
		switch(val){
        		case 1:
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
        		case 2:
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				break;
			case 3:
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				break;
			case 4:
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				break;
			case 5:
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				break;
			case 6:
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
			case 7:
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
			default:
				acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
		}


		val = com_rbuf[5];
		val = val << 8;
		val += com_rbuf[6];
		if(val == 0)
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else 
			acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		
		switch(val){
        		case 0:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
        		case 1:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
        		case 2:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
			case 3:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				break;
			case 4:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				break;
			case 5:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				break;
			case 6:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,9,INFOR_STATUS,&arg_n);
				break;
			case 7:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
			default:
				acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				break;
		}

		f.ch[0]=com_rbuf[30];
		f.ch[1]=com_rbuf[29];
		f.ch[2]=com_rbuf[32];
		f.ch[3]=com_rbuf[31];
		ZONGLIN_zero=f.f;   //ZONGLIN_zero_val
    	//acqdata_set_value_flag(acq_data,"i13104",UNIT_L,ZONGLIN_zero,INFOR_ARGUMENTS,&arg_n);

    	//acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,ZONGLIN_LiDa_orig,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(com_rbuf, 15, INT_AB);
		timer.tm_year=(val)-1900;
		timer.tm_mon=(com_rbuf[18])-1;
		timer.tm_mday=(com_rbuf[20]);
		timer.tm_hour=(com_rbuf[22]);
		timer.tm_min=(com_rbuf[24]);
		timer.tm_sec=(com_rbuf[26]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13101",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val = getInt16Value(com_rbuf, 35, INT_AB);
		timer.tm_year=(val)-1900;
		timer.tm_mon=(com_rbuf[38])-1;
		timer.tm_mday=(com_rbuf[40]);
		timer.tm_hour=(com_rbuf[42]);
		timer.tm_min=(com_rbuf[44]);
		timer.tm_sec=(com_rbuf[46]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13107",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		
	sprintf(hexarry,"%x%x",com_rbuf[49],com_rbuf[50]);
	val = atoi(hexarry);
	//acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,val,INFOR_ARGUMENTS,&arg_n);

		f.ch[0]=com_rbuf[52];
		f.ch[1]=com_rbuf[51];
		f.ch[2]=com_rbuf[54];
		f.ch[3]=com_rbuf[53];
		ZONGLIN_correction_factor=f.f;   //ZONGLIN_correction_factor
    	acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,ZONGLIN_correction_factor,INFOR_ARGUMENTS,&arg_n);


		f.ch[0]=com_rbuf[56];
		f.ch[1]=com_rbuf[55];
		f.ch[2]=com_rbuf[58];
		f.ch[3]=com_rbuf[57];
		ZONGLIN_adjust_zero_drift=f.f;   //ZONGLIN_adjust_zero_drift
    	acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,ZONGLIN_adjust_zero_drift,INFOR_ARGUMENTS,&arg_n);	

	sprintf(hexarry,"%x%x",com_rbuf[59],com_rbuf[60]);
	ZONGLIN_digestion_time = atoi(hexarry);
	acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,ZONGLIN_digestion_time,INFOR_ARGUMENTS,&arg_n);	


		f.ch[0]=com_rbuf[62];
		f.ch[1]=com_rbuf[61];
		f.ch[2]=com_rbuf[64];
		f.ch[3]=com_rbuf[63];
		ZONGLIN_digestion_temp=f.f;
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,ZONGLIN_digestion_temp,INFOR_ARGUMENTS,&arg_n);	

		f.ch[0]=com_rbuf[66];
		f.ch[1]=com_rbuf[65];
		f.ch[2]=com_rbuf[68];
		f.ch[3]=com_rbuf[67];
		ZONGLIN_range=f.f;
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_M3,ZONGLIN_range,INFOR_ARGUMENTS,&arg_n);	
	
		status=0;
	}
	else
	{
		ZONGLIN_zero=0;
		ZONGLIN_correction_factor=0;
		ZONGLIN_digestion_time=0;
		ZONGLIN_digestion_temp=0;
		ZONGLIN_range=0;
		status=1;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
