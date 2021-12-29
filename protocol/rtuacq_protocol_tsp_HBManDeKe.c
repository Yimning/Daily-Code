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
static float HBManDeKe_original_value;
#if 0
int protocol_TSP_HBManDeKe(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf=0.0;
	int val=0,i=0,j=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

#define POLCODE_NUM 5
	char *polcode[POLCODE_NUM]={
		"a01012","a01013","a01011","a01014","a34013a"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_0C,		UNIT_KPA,	UNIT_M_S,	UNIT_PECENT,	UNIT_MG_M3
	};

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg->polcode_arg;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe TSP SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe TSP protocol,TSP : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe TSP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe TSP RECV:",com_rbuf,size);
	if((size>=15)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			val=com_rbuf[3+i*2];
			val<<=8;
			val+=com_rbuf[4+i*2];

			for(j=0;j<modbusarg->array_count;j++)
			{
				mpolcodearg=&modbusarg->polcode_arg[j];
				if(strcmp(polcode[i],mpolcodearg->polcode))
					continue;	//polcode[i]  !=  mpolcodearg->polcode

				valf=(val-6400)*(mpolcodearg->alarmMax-mpolcodearg->alarmMin)/(32000-6400)+mpolcodearg->alarmMin;
				break;
			}
			
	        mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,polcode[i]);
	        if(mpolcodearg_temp!=NULL &&
				mpolcodearg_temp->enableFlag==1)
	        {
	        	acqdata_set_value(acq_data,polcode[i],unit[i],valf,&arg_n);
				if(!strcmp(polcode[i],"a34013a"))
				{
				    acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
					acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);				
				}
				if(!strcmp(polcode[i],"a01011"))
				{
					acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
					acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);			
				}				
	        }
		
		}
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
	        mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,polcode[i]);
	        if(mpolcodearg_temp!=NULL &&
				mpolcodearg_temp->enableFlag==1)
	        {
	        	acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
				if(!strcmp(polcode[i],"a34013a"))
				{
				    acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
					acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);				
				}
				if(!strcmp(polcode[i],"a01011"))
				{
					acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
					acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);			
				}				
	        }
		}
		status=1;
	}

    //acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
	//acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	//acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	//acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	
	return arg_n;
}
#endif

int protocol_TSP_HBManDeKe(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf=0.0;
	int val=0,i=0,j=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
#if 0
#define POLCODE_NUM 5
	char *polcode[POLCODE_NUM]={
		"a01012","a01013","a01011","a01014","a34013a"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_0C,		UNIT_KPA,	UNIT_M_S,	UNIT_PECENT,	UNIT_MG_M3
	};
#endif
	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg->polcode_arg;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe TSP SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe TSP protocol,TSP : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe TSP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe TSP RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
#if 0	
		for(i=0;i<POLCODE_NUM;i++)
		{
			val=com_rbuf[3+i*2];
			val<<=8;
			val+=com_rbuf[4+i*2];

			for(j=0;j<modbusarg->array_count;j++)
			{
				mpolcodearg=&modbusarg->polcode_arg[j];
				if(strcmp(polcode[i],mpolcodearg->polcode))
					continue;	//polcode[i]  !=  mpolcodearg->polcode

				valf=(val-6400)*(mpolcodearg->alarmMax-mpolcodearg->alarmMin)/(32000-6400)+mpolcodearg->alarmMin;
				break;
			}
			
	        mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,polcode[i]);
	        if(mpolcodearg_temp!=NULL &&
				mpolcodearg_temp->enableFlag==1)
	        {
	        	acqdata_set_value(acq_data,polcode[i],unit[i],valf,&arg_n);
				if(!strcmp(polcode[i],"a34013a"))
				{
				    acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
					acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);				
				}
				if(!strcmp(polcode[i],"a01011"))
				{
					acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
					acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);			
				}				
	        }
		
		}
#endif		

		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		valf=f.f;
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf,&arg_n);
	    acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);			
		status=0;
	}
	else
	{
/*	
		for(i=0;i<POLCODE_NUM;i++)
		{
	        mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,polcode[i]);
	        if(mpolcodearg_temp!=NULL &&
				mpolcodearg_temp->enableFlag==1)
	        {
	        	acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
				if(!strcmp(polcode[i],"a34013a"))
				{
				    acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
					acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);				
				}
				if(!strcmp(polcode[i],"a01011"))
				{
					acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
					acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);			
				}				
	        }
		}
*/
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
		status=1;
	}

    //acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
	//acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	//acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	//acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	
	return arg_n;
}



/*
Author:Yimning
Time:2021.06.08 Tues.
Description:protocol_TSP_HBManDeKe_info
SEND1: size=8,01 03 00 0c 00 14 85 c6
RECV1: size=45,01 03 28 00 01 00 00 3e f0 55 10 3f 82 ab 76 41 f0 00 00 3e f0 55 10 be 12 fd 00 3f 54 64 1c 00 00 00 00 3f 80 00 00 ec 47 00 40 d4 08
SEND2: size=8,01 03 00 21 00 12 95 cd
RECV2: size=41,01 03 24 07 e5 00 06 00 08 00 0e 00 03 00 30 07 e5 00 06 00 07 00 0f 00 2f 00 35 07 e5 00 06 00 08 00 0e 00 03 00 30 b6 6f 

*/


#if 1
int protocol_TSP_HBManDeKe_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_TSP_HBManDeKe_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xC,0x14);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe TSP INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe TSP protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe TSP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe TSP INFO RECV1:",com_rbuf,size);
	if((size>=45)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];

		switch(val)
		{
			case 1: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];

		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);

			if((val&0x01)==0x01)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x02)==0x02)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x04)==0x04)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x08)==0x08)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x10)==0x10)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x20)==0x20)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}

		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[23];
		f.ch[2]=com_rbuf[24];
		f.ch[1]=com_rbuf[25];
		f.ch[0]=com_rbuf[26];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[27];
		f.ch[2]=com_rbuf[28];
		f.ch[1]=com_rbuf[29];
		f.ch[0]=com_rbuf[30];
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		HBManDeKe_original_value=f.f;

		f.ch[3]=com_rbuf[31];
		f.ch[2]=com_rbuf[32];
		f.ch[1]=com_rbuf[33];
		f.ch[0]=com_rbuf[34];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[35];
		f.ch[2]=com_rbuf[36];
		f.ch[1]=com_rbuf[37];
		f.ch[0]=com_rbuf[38];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

	}


	#if 0
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x70,0x12);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZCuiYunGu TSP INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe TSP INFO protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe TSP INFO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe TSP INFO RECV1:",com_rbuf,size);
	if((size>=41)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];

		switch(val)
		{
			case 1: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 2: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 3: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 4: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 5: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];

		if(val==0)
		{
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);

			if((val&0x01)==0x01)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x02)==0x02)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x04)==0x04)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			else if((val&0x08)==0x08)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			else if((val&0x10)==0x10)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else if((val&0x20)==0x20)
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,5,INFOR_STATUS,&arg_n);
			else
				acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}

		f.ch[3]=com_rbuf[19];
		f.ch[2]=com_rbuf[20];
		f.ch[1]=com_rbuf[21];
		f.ch[0]=com_rbuf[22];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[23];
		f.ch[2]=com_rbuf[24];
		f.ch[1]=com_rbuf[25];
		f.ch[0]=com_rbuf[26];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[27];
		f.ch[2]=com_rbuf[28];
		f.ch[1]=com_rbuf[29];
		f.ch[0]=com_rbuf[30];
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		HBManDeKe_original_value=f.f;

		f.ch[3]=com_rbuf[31];
		f.ch[2]=com_rbuf[32];
		f.ch[1]=com_rbuf[33];
		f.ch[0]=com_rbuf[34];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[35];
		f.ch[2]=com_rbuf[36];
		f.ch[1]=com_rbuf[37];
		f.ch[0]=com_rbuf[38];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);
	}
	#endif

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x21,0x12);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe TSP INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe TSP protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe TSP data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe TSP INFO RECV2:",com_rbuf,size);
	if((size>=41)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		timer.tm_year=val-1900;
		timer.tm_mon=com_rbuf[6]-1;
		timer.tm_mday=com_rbuf[8];
		timer.tm_hour=com_rbuf[10];
		timer.tm_min=com_rbuf[12];
		timer.tm_sec=com_rbuf[14];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);


		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		timer.tm_year=val-1900;
		timer.tm_mon=com_rbuf[18]-1;
		timer.tm_mday=com_rbuf[20];
		timer.tm_hour=com_rbuf[22];
		timer.tm_min=com_rbuf[24];
		timer.tm_sec=com_rbuf[26];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
/*
		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		timer.tm_year=val-1900;
		timer.tm_mon=com_rbuf[18]-1;
		timer.tm_mday=com_rbuf[20];
		timer.tm_hour=com_rbuf[22];
		timer.tm_min=com_rbuf[24];
		timer.tm_sec=com_rbuf[26];
		t2=mktime(&timer);
		t3=(t2>t1)? (t2-t1):t1;
		//acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13021",0,0.0,INFOR_ARGUMENTS,&arg_n);
*/	

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
#endif
