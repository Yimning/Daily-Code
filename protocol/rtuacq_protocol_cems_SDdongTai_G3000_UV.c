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

#include "common.h"
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

/*
Author:Yimning
Time:2021.06.03
Description:measurement
TX:01 03 00 00 00 03 05 CB 
RX:01 03 06 03 83 12 7F 08 20 FF FF

*/

int protocol_CEMS_SDdongTai_G3000_UV(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float so2=0,o2=0,no=0,nox=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	char *p,*q;
	char hex_pack[1024];
	int val=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_CEMS_SDdongTai_G3000_UV CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_CEMS_SDdongTai_G3000_UV protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_CEMS_SDdongTai_G3000_UV CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_CEMS_SDdongTai_G3000_UV CEMS RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val = com_rbuf[3];
		val <<= 8;
		val += com_rbuf[4];
		so2 = val/1.0;

		val = com_rbuf[5];
		val <<= 8;
		val += com_rbuf[6];
		no=val/10.0;

		val = com_rbuf[7];
		val <<= 8;
		val += com_rbuf[8];
		o2=val/100.0;
		
		//if(nox==0) 
		//	nox=no*1.53+no2;
		
		status=0;
	}
	else
	{
		so2=0,
		o2=0,
		no=0,
		status=1;
	}

	nox=NO_to_NOx(no);

	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);

	
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

/*
Author:Yimning
Description:INFOR_ARGUMENTS
Time:2021.06.03

TX:01 03 00 0A 00 55 A5 F7  
RX:01 03 AA
   00 00 00 00 78 01 1C 0C 1E 00 03 83 00 50 78 01 1C 0C 1E 00
   00 00 00 01 15 9A 03 E8 03 E9 78 01 1C 0C 1E 00 00 00 03 E7 03 E8 00 10 00 11 00 12 00 13
   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
   01 00 00 01 15 9A 03 E8 03 E9 78 01 1C 0C 1E 00 00 00 03 E7 03 E8 00 10 00 11 00 12 00 13
   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
   02 00 00 01 15 9A 03 E8 03 E9 78 01 1C 0C 1E 00 00 00 03 E7 03 E8 00 10 00 11 00 12 00 13 
   FF FF
*/


int protocol_CEMS_SDdongTai_G3000_UV_SO2_info(struct acquisition_data *acq_data)
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
	int decimal_num = 0;
	
	union uf f;
	float SO2_zero=0,SO2_range=0,SO2_K=0,SO2_PPB=0,SO2_zero_drift=0,SO2_adjust_zero_drift=0,SO2_range_drift=0,SO2_intercept=0,SO2_BDXS=0,SO2_original_Val=0;
	float static_pressure=0,temperature=0;
	struct tm timer;
	time_t t1,t2,t3;

	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SDdongTai_G3000_UV_SO2_info1\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x55);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_CEMS_SDdongTai_G3000_UV SO2 INFO1 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_CEMS_SDdongTai_G3000_UV SO2 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_CEMS_SDdongTai_G3000_UV SO2 data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_CEMS_SDdongTai_G3000_UV SO2 INFO1 RECV1:",com_rbuf,size);
	if((size>=175)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		//val=com_rbuf[3];
		//val<<=8;
		val = com_rbuf[4];

		if(val&0x00 == 0x00){
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}else if(val&0x01 == 0x01)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x02 == 0x02)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
		else if(val&0x03 == 0x03)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if(val&0x04 == 0x04)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x05 == 0x05)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		//acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);

		val = com_rbuf[6];
		if(val&0x00 == 0){
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		}else if(val&0x01 == 0x01)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x02 == 0x02)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x03 == 0x03)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x04 == 0x04)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x05 == 0x05)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		


		timer.tm_year=BCD(com_rbuf[7])+100;
		timer.tm_mon=BCD(com_rbuf[8])-1;
		timer.tm_mday=BCD(com_rbuf[9]);
		timer.tm_hour=BCD(com_rbuf[10]);
		timer.tm_min=BCD(com_rbuf[11]);
		timer.tm_sec=BCD(com_rbuf[12]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		//instrument_time
		//acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);

		
		val = com_rbuf[13];
		val <<= 8;
		val += com_rbuf[14];
		static_pressure = val/10.0;
		
		val = com_rbuf[15];
		val <<= 8;
		val += com_rbuf[16];
		temperature = val/10.0;

		timer.tm_year=BCD(com_rbuf[17])+100;
		timer.tm_mon=BCD(com_rbuf[18])-1;
		timer.tm_mday=BCD(com_rbuf[19]);
		timer.tm_hour=BCD(com_rbuf[20]);
		timer.tm_min=BCD(com_rbuf[21]);
		timer.tm_sec=BCD(com_rbuf[22]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		//adjust_zero_time
		//acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);


		
		val = com_rbuf[23]; //High Byte
		decimal_num = com_rbuf[26];
		//if it is divide by the decimal_num,uncomment the line as like "decimal_num = 1.0;"
		decimal_num =decimal_num*10.0;

		//if do not divide decimal_num 
		decimal_num = 1.0;

		//Judge whether it is So2
		//if(val == 0){
			val = com_tbuf[27];
			val <<= 8;
			val += com_tbuf[28];
			SYSLOG_DBG("com_tbuf[29][30] %d\n,val");
			SO2_range = val / decimal_num;

			val = com_tbuf[29];
			val <<= 8;
			val += com_tbuf[30];
			SO2_PPB = val / decimal_num;


			val = com_tbuf[31];
			val <<= 8;
			val += com_tbuf[32];
			SO2_zero= val / decimal_num;


			timer.tm_year=BCD(com_rbuf[33])+100;
			timer.tm_mon=BCD(com_rbuf[34])-1;
			timer.tm_mday=BCD(com_rbuf[35]);
			timer.tm_hour=BCD(com_rbuf[36]);
			timer.tm_min=BCD(com_rbuf[37]);
			timer.tm_sec=BCD(com_rbuf[38]);
			t2=mktime(&timer);
			t3=(t2>t1)? (t2-t1):t1;
		
			//This part has writen the SO2_zero_drift_time 
			acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);
			

			val = com_tbuf[39];
			val <<= 8;
			val += com_tbuf[40];
			SO2_adjust_zero_drift= val / decimal_num;

			
			val = com_tbuf[41];
			val <<= 8;
			val += com_tbuf[42];
			SO2_zero_drift= val / decimal_num;

			val = com_tbuf[43];
			val <<= 8;
			val += com_tbuf[44];
			SO2_range_drift= val / decimal_num;


			val = com_tbuf[45];
			val <<= 8;
			val += com_tbuf[46];
			SO2_K= val / decimal_num;

			val = com_tbuf[47];
			val <<= 8;
			val += com_tbuf[48];
			SO2_intercept= val / decimal_num;



			val = com_tbuf[49];
			val <<= 8;
			val += com_tbuf[50];
			SO2_BDXS= val / decimal_num;
			

			val = com_tbuf[51];
			val <<= 8;
			val += com_tbuf[52];
			SO2_original_Val= val / decimal_num;
		//}

		status=0;
		
	}else{
		static_pressure=0;
		temperature=0;
		status=1;
		SO2_zero=0;
		SO2_range=0;
		SO2_K=0;
		SO2_PPB=0;
		SO2_zero_drift=0;
		SO2_range_drift=0;
		SO2_intercept=0;
		SO2_BDXS=0;
		SO2_adjust_zero_drift=0;
		SO2_original_Val=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,SO2_zero,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,SO2_range,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,SO2_K,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,SO2_PPB,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,SO2_zero_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,SO2_range_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,SO2_intercept,INFOR_ARGUMENTS,&arg_n);
	
	//acqdata_set_value_flag(acq_data,"i13029",0,SO2_original_Val,INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	//NEED_ERROR_CACHE(acq_data, 10); //del by Yimning 20210824
	return arg_n;
	
}


int protocol_CEMS_SDdongTai_G3000_UV_NO_info(struct acquisition_data *acq_data)
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
	int decimal_num = 0;
	
	union uf f;
	float NO_zero=0,NO_range=0,NO_K=0,NO_PPB=0,NO_zero_drift=0,NO_adjust_zero_drift=0,NO_range_drift=0,NO_intercept=0,NO_BDXS=0,NO_original_Val=0;
	float static_pressure=0,temperature=0;
	struct tm timer;
	time_t t1,t2,t3;

	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SDdongTai_G3000_UV_NO_info1\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00s
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x55);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_CEMS_SDdongTai_G3000_UV NO INFO1 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_CEMS_SDdongTai_G3000_UV NO protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_CEMS_SDdongTai_G3000_UV NO data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_CEMS_SDdongTai_G3000_UV NO INFO1 RECV1:",com_rbuf,size);
	if((size>=175)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		//val=com_rbuf[3];
		//val<<=8;
		val = com_rbuf[4];

		if(val&0x00 == 0){
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}else if(val&0x01 == 0x01)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x02 == 0x02)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
		else if(val&0x03 == 0x03)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if(val&0x04 == 0x04)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x05 == 0x05)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		//acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);

		val = com_rbuf[6];
		if(val&0x00 == 0){
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		}else if(val&0x01 == 0x01)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x02 == 0x02)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x03 == 0x03)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x04 == 0x04)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x05 == 0x05)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		


		timer.tm_year=BCD(com_rbuf[7])+100;
		timer.tm_mon=BCD(com_rbuf[8])-1;
		timer.tm_mday=BCD(com_rbuf[9]);
		timer.tm_hour=BCD(com_rbuf[10]);
		timer.tm_min=BCD(com_rbuf[11]);
		timer.tm_sec=BCD(com_rbuf[12]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		//instrument_time
		//acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);

		
		val = com_rbuf[13];
		val <<= 8;
		val += com_rbuf[14];
		static_pressure = val/10.0;
		
		val = com_rbuf[15];
		val <<= 8;
		val += com_rbuf[16];
		temperature = val/10.0;

		timer.tm_year=BCD(com_rbuf[17])+100;
		timer.tm_mon=BCD(com_rbuf[18])-1;
		timer.tm_mday=BCD(com_rbuf[19]);
		timer.tm_hour=BCD(com_rbuf[20]);
		timer.tm_min=BCD(com_rbuf[21]);
		timer.tm_sec=BCD(com_rbuf[22]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		//adjust_zero_time
		//acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);


		
		val = com_rbuf[83]; //High Byte
		decimal_num = com_rbuf[86];
		//if it is divide by the decimal_num,uncomment the line as like "decimal_num = 1.0;"
		decimal_num =decimal_num*10.0;

		//if do not divide decimal_num 
		decimal_num = 1.0;

		
		//Judge whether it is No
		//if(val == 1){
			val = com_tbuf[87];
			val <<= 8;
			val += com_tbuf[88];
			NO_range = val / decimal_num;

			val = com_tbuf[89];
			val <<= 8;
			val += com_tbuf[90];
			NO_PPB = val / decimal_num;


			val = com_tbuf[91];
			val <<= 8;
			val += com_tbuf[92];
			NO_zero= val / decimal_num;


			timer.tm_year=BCD(com_rbuf[93])+100;
			timer.tm_mon=BCD(com_rbuf[94])-1;
			timer.tm_mday=BCD(com_rbuf[95]);
			timer.tm_hour=BCD(com_rbuf[96]);
			timer.tm_min=BCD(com_rbuf[97]);
			timer.tm_sec=BCD(com_rbuf[98]);
			t2=mktime(&timer);
			t3=(t2>t1)? (t2-t1):t1;
		
			//This part has writen the NO_zero_drift_time 
			acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);



			val = com_tbuf[99];
			val <<= 8;
			val += com_tbuf[100];
			NO_adjust_zero_drift= val / decimal_num;
			

			val = com_tbuf[101];
			val <<= 8;
			val += com_tbuf[102];
			NO_zero_drift= val / decimal_num;

			val = com_tbuf[103];
			val <<= 8;
			val += com_tbuf[104];
			NO_range_drift= val / decimal_num;


			val = com_tbuf[105];
			val <<= 8;
			val += com_tbuf[106];
			NO_K= val / decimal_num;

			val = com_tbuf[107];
			val <<= 8;
			val += com_tbuf[108];
			NO_intercept= val / decimal_num;



			val = com_tbuf[109];
			val <<= 8;
			val += com_tbuf[110];
			NO_BDXS= val / decimal_num;
			

			val = com_tbuf[111];
			val <<= 8;
			val += com_tbuf[112];
			NO_original_Val= val / decimal_num;
		//}
		status=0;
		
	}else{
		static_pressure=0;
		temperature=0;
		status=1;
		NO_zero=0;
		NO_range=0;
		NO_K=0;
		NO_PPB=0;
		NO_zero_drift=0;
		NO_range_drift=0;
		NO_intercept=0;
		NO_BDXS=0;
		NO_adjust_zero_drift=0;
		NO_original_Val=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,NO_zero,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,NO_range,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,NO_K,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,NO_PPB,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,NO_zero_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,NO_range_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,NO_intercept,INFOR_ARGUMENTS,&arg_n);
	
	//acqdata_set_value_flag(acq_data,"i13029",0,NO_original_Val,INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	//NEED_ERROR_CACHE(acq_data, 10);  //del by Yimning 20210824
	return arg_n;
}



int protocol_CEMS_SDdongTai_G3000_UV_O2_info(struct acquisition_data *acq_data)
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
	int decimal_num = 0;
	
	union uf f;
	float O2_zero=0,O2_range=0,O2_K=0,O2_PPB=0,O2_zero_drift=0,O2_adjust_zero_drift=0,O2_range_drift=0,O2_intercept=0,O2_BDXS=0,O2_original_Val=0;
	float static_pressure=0,temperature=0;
	struct tm timer;
	time_t t1,t2,t3;

	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_SDdongTai_G3000_UV_O2_info1\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x55);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_CEMS_SDdongTai_G3000_UV O2 INFO1 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_CEMS_SDdongTai_G3000_UV O2 protocol,INFO1 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_CEMS_SDdongTai_G3000_UV O2 data1",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_CEMS_SDdongTai_G3000_UV O2 INFO1 RECV1:",com_rbuf,size);
	if((size>=175)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		//val=com_rbuf[3];
		//val<<=8;
		val = com_rbuf[4];

		if(val&0x00 == 0){
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}else if(val&0x01 == 0x01)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x02 == 0x02)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
		else if(val&0x03 == 0x03)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else if(val&0x04 == 0x04)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x05 == 0x05)
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		//acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);

		val = com_rbuf[6];
		if(val&0x00 == 0){
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		}else if(val&0x01 == 0x01)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x02 == 0x02)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x03 == 0x03)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else if(val&0x04 == 0x04)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,2,INFOR_STATUS,&arg_n);
		else if(val&0x05 == 0x05)
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,3,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		


		timer.tm_year=BCD(com_rbuf[7])+100;
		timer.tm_mon=BCD(com_rbuf[8])-1;
		timer.tm_mday=BCD(com_rbuf[9]);
		timer.tm_hour=BCD(com_rbuf[10]);
		timer.tm_min=BCD(com_rbuf[11]);
		timer.tm_sec=BCD(com_rbuf[12]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		//instrument_time
		//acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);

		
		val = com_rbuf[13];
		val <<= 8;
		val += com_rbuf[14];
		static_pressure = val/10.0;
		
		val = com_rbuf[15];
		val <<= 8;
		val += com_rbuf[16];
		temperature = val/10.0;

		timer.tm_year=BCD(com_rbuf[17])+100;
		timer.tm_mon=BCD(com_rbuf[18])-1;
		timer.tm_mday=BCD(com_rbuf[19]);
		timer.tm_hour=BCD(com_rbuf[20]);
		timer.tm_min=BCD(com_rbuf[21]);
		timer.tm_sec=BCD(com_rbuf[22]);
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		//adjust_zero_time
		//acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);


		
		val = com_rbuf[143]; //High Byte
		decimal_num = com_rbuf[146];
		//if it is divide by the decimal_num,uncomment the line as like "decimal_num = 1.0;"
		decimal_num =decimal_num*10.0;

		//if do not divide decimal_num 
		decimal_num = 1.0;
		
		
		//Judge whether it is O2
		//if(val == 2){
			val = com_tbuf[147];
			val <<= 8;
			val += com_tbuf[148];
			O2_range = val / decimal_num;

			val = com_tbuf[149];
			val <<= 8;
			val += com_tbuf[150];
			O2_PPB = val / decimal_num;


			val = com_tbuf[151];
			val <<= 8;
			val += com_tbuf[152];
			O2_zero= val / decimal_num;


			timer.tm_year=BCD(com_rbuf[153])+100;
			timer.tm_mon=BCD(com_rbuf[154])-1;
			timer.tm_mday=BCD(com_rbuf[155]);
			timer.tm_hour=BCD(com_rbuf[156]);
			timer.tm_min=BCD(com_rbuf[157]);
			timer.tm_sec=BCD(com_rbuf[158]);
			t2=mktime(&timer);
			t3=(t2>t1)? (t2-t1):t1;
		
			//This part has writen the O2_zero_drift_time 
			acqdata_set_value_flag(acq_data,"i13021",t3,UNIT_NONE,INFOR_ARGUMENTS,&arg_n);



			val = com_tbuf[159];
			val <<= 8;
			val += com_tbuf[160];
			O2_adjust_zero_drift= val / decimal_num;
			

			val = com_tbuf[161];
			val <<= 8;
			val += com_tbuf[162];
			O2_zero_drift= val / decimal_num;

			val = com_tbuf[163];
			val <<= 8;
			val += com_tbuf[164];
			O2_range_drift= val / decimal_num;


			val = com_tbuf[165];
			val <<= 8;
			val += com_tbuf[166];
			O2_K= val / decimal_num;

			val = com_tbuf[167];
			val <<= 8;
			val += com_tbuf[168];
			O2_intercept= val / decimal_num;



			val = com_tbuf[169];
			val <<= 8;
			val += com_tbuf[170];
			O2_BDXS= val / decimal_num;
			

			val = com_tbuf[171];
			val <<= 8;
			val += com_tbuf[172];
			O2_original_Val= val / decimal_num;
		//}

		status=0;
		
	}else{
		static_pressure=0;
		temperature=0;
		status=1;
		O2_zero=0;
		O2_range=0;
		O2_K=0;
		O2_PPB=0;
		O2_zero_drift=0;
		O2_range_drift=0;
		O2_intercept=0;
		O2_BDXS=0;
		O2_adjust_zero_drift=0;
		O2_original_Val=0;
		status=1;
	}

	acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,O2_zero,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,O2_range,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,O2_K,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,O2_PPB,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,O2_zero_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,O2_range_drift,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,O2_intercept,INFOR_ARGUMENTS,&arg_n);
	
	//acqdata_set_value_flag(acq_data,"i13029",0,O2_original_Val,INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	//NEED_ERROR_CACHE(acq_data, 10); //del by Yimning 20210824
	return arg_n;
}

