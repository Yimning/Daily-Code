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

extern char SaiMoFei_PLC_Flag1;

int protocol_CEMS_SaiMoFei1_42i_HI(struct acquisition_data *acq_data)
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
	float no,no2,nox;
	float valf;
	int index=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei 42i HI CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SaiMoFei 42i HI CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SaiMoFei 42i HI CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei 42i HI CEMS RECV1:",com_rbuf,size);
	if((size>=17)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//no=gas_ppm_to_mg_m3(valf, 30);
		no=valf;


		f.ch[3]=com_rbuf[9];
		f.ch[2]=com_rbuf[10];
		f.ch[1]=com_rbuf[7];
		f.ch[0]=com_rbuf[8];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//no2=gas_ppm_to_mg_m3(valf, 46);
		no2=valf;

#if 0
		f.ch[3]=com_rbuf[13];
		f.ch[2]=com_rbuf[14];
		f.ch[1]=com_rbuf[11];
		f.ch[0]=com_rbuf[12];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//nox=gas_ppm_to_mg_m3(valf, 46);
#endif 
		
		nox= no*1.53 + no2;
		
		status=0;
	}
	else
	{
		no=0;
		no2=0;
		nox=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_CEMS_SaiMoFei1_42i_HI_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_CEMS_SaiMoFei_42i_HI_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x02,0x04,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei 42i HI CEMS STATUS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SaiMoFei 42i HI CEMS STATUS protocol,STATUS 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SaiMoFei 42i HI CEMS STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei 42i HI CEMS STATUS RECV1:",com_rbuf,size);
	if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x02))
	{
		if(com_rbuf[3]==0)
		{
			sleep(1);
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,0x02,0x3F,0x04);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei 42i HI CEMS STATUS SEND2:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("SaiMoFei 42i HI CEMS STATUS protocol,STATUS 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("SaiMoFei 42i HI CEMS STATUS data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei 42i HI CEMS STATUS RECV2:",com_rbuf,size);
			if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x02))
			{
				if(com_rbuf[3]==0x00)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if((com_rbuf[3]&0x01)==0x01)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if((com_rbuf[3]&0x02)==0x02)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if((com_rbuf[3]&0x04)==0x04)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if((com_rbuf[3]&0x08)==0x08)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				}
				else
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
			}
		}
		else if((com_rbuf[3]&0x40)==0x40)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else if((com_rbuf[3]&0x02)==0x02)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}

		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x80,0x44);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei 42i HI INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SaiMoFei 42i HI protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SaiMoFei 42i HI data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei 42i HI INFO RECV:",com_rbuf,size);
	if((size>=141)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[25];
		f.ch[2]=com_rbuf[26];
		f.ch[1]=com_rbuf[23];
		f.ch[0]=com_rbuf[24];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
	
		f.ch[3]=com_rbuf[33];
		f.ch[2]=com_rbuf[34];
		f.ch[1]=com_rbuf[31];
		f.ch[0]=com_rbuf[32];
		acqdata_set_value_flag(acq_data,"i13021",f.l,0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[57];
		f.ch[2]=com_rbuf[58];
		f.ch[1]=com_rbuf[55];
		f.ch[0]=com_rbuf[56];
		acqdata_set_value_flag(acq_data,"i13027",f.l,0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[65];
		f.ch[2]=com_rbuf[66];
		f.ch[1]=com_rbuf[63];
		f.ch[0]=com_rbuf[64];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[89];
		f.ch[2]=com_rbuf[90];
		f.ch[1]=com_rbuf[87];
		f.ch[0]=com_rbuf[88];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[113];
		f.ch[2]=com_rbuf[114];
		f.ch[1]=com_rbuf[111];
		f.ch[0]=com_rbuf[112];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//valf=gas_ppm_to_mg_m3(valf, 46);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[137];
		f.ch[2]=com_rbuf[138];
		f.ch[1]=com_rbuf[135];
		f.ch[0]=com_rbuf[136];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//valf=gas_ppm_to_mg_m3(valf, 46);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_SaiMoFei1_42i_LO(struct acquisition_data *acq_data)
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
	float no,no2,nox;
	float valf;
	int index=0;
	char flag;
	int pos;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct modbus_polcode_arg *mpolcodearg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei 42i LO CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SaiMoFei 42i LO CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SaiMoFei 42i LO CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei 42i LO CEMS RECV1:",com_rbuf,size);
	if((size>=17)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//no=gas_ppm_to_mg_m3(valf, 30);
		no=valf;

		f.ch[3]=com_rbuf[9];
		f.ch[2]=com_rbuf[10];
		f.ch[1]=com_rbuf[7];
		f.ch[0]=com_rbuf[8];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//no2=gas_ppm_to_mg_m3(valf, 46);
		no2=valf;

#if 0 
		f.ch[3]=com_rbuf[13];
		f.ch[2]=com_rbuf[14];
		f.ch[1]=com_rbuf[11];
		f.ch[0]=com_rbuf[12];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//nox=gas_ppm_to_mg_m3(valf, 46);
#endif 

		nox= no*1.53 + no2;
		
		status=0;
	}
	else
	{
		no=0;
		no2=0;
		nox=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21004a",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004",UNIT_MG_M3,no2,&arg_n);
	acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);


	if(status == 0)
	{
		flag=SaiMoFei_PLC_Flag1;
		if(SaiMoFei_PLC_Flag1=='N' || SaiMoFei_PLC_Flag1=='K')
		{
			pos=-1;
			for(i=0;i<modbusarg->array_count;i++)
			{
				mpolcodearg=&(modbusarg->polcode_arg[i]);
				if((mpolcodearg->enableFlag == 1))
				{
					if(!strcmp(mpolcodearg->polcode,"a21002a"))
					{
						pos=i;
						break;
					}
					else if(!strcmp(mpolcodearg->polcode,"a21002"))
					{
						pos=i;
					}
				}
				else
				{
					continue;
				}
			}

			if(pos!=-1)
			{
				mpolcodearg=&(modbusarg->polcode_arg[pos]);

				if((nox < mpolcodearg->alarmMin) || (nox > mpolcodearg->alarmMax))
				{
					flag='T';
				}
			}
			
		}

		acq_data->dev_stat=flag;
		acq_data->acq_status = ACQ_OK;
	}
	else
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_CEMS_SaiMoFei1_42i_LO_info(struct acquisition_data *acq_data)
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
	SYSLOG_DBG("protocol_CEMS_SaiMoFei_42i_LO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x02,0x04,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei 42i LO CEMS STATUS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SaiMoFei 42i LO CEMS STATUS protocol,STATUS 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SaiMoFei 42i LO CEMS STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei 42i LO CEMS STATUS RECV1:",com_rbuf,size);
	if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x02))
	{
		if((com_rbuf[3]&0x43)==0)
		{
			sleep(1);
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf,devaddr,0x02,0x3F,0x04);
			LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei 42i LO CEMS STATUS SEND2:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("SaiMoFei 42i LO CEMS STATUS protocol,STATUS 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG_HEX("SaiMoFei 42i LO CEMS STATUS data",com_rbuf,size);
			LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei 42i LO CEMS STATUS RECV2:",com_rbuf,size);
			if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x02))
			{
				if(com_rbuf[3]==0x00)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if((com_rbuf[3]&0x01)==0x01)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if((com_rbuf[3]&0x02)==0x02)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if((com_rbuf[3]&0x04)==0x04)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
				else if((com_rbuf[3]&0x08)==0x08)
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				}
				else
				{
					acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
					acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				}
			}
		}
		else if((com_rbuf[3]&0x40)==0x40)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else if((com_rbuf[3]&0x02)==0x02)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		else
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}

		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x80,0x44);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SaiMoFei 42i LO INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SaiMoFei 42i LO protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SaiMoFei 42i LO data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SaiMoFei 42i LO INFO RECV:",com_rbuf,size);
	if((size>=141)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[13];
		f.ch[2]=com_rbuf[14];
		f.ch[1]=com_rbuf[11];
		f.ch[0]=com_rbuf[12];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
	
		f.ch[3]=com_rbuf[33];
		f.ch[2]=com_rbuf[34];
		f.ch[1]=com_rbuf[31];
		f.ch[0]=com_rbuf[32];
		acqdata_set_value_flag(acq_data,"i13021",f.l,0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[45];
		f.ch[2]=com_rbuf[46];
		f.ch[1]=com_rbuf[43];
		f.ch[0]=com_rbuf[44];
		acqdata_set_value_flag(acq_data,"i13027",f.l,0,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[65];
		f.ch[2]=com_rbuf[66];
		f.ch[1]=com_rbuf[63];
		f.ch[0]=com_rbuf[64];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[77];
		f.ch[2]=com_rbuf[78];
		f.ch[1]=com_rbuf[75];
		f.ch[0]=com_rbuf[76];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[101];
		f.ch[2]=com_rbuf[102];
		f.ch[1]=com_rbuf[99];
		f.ch[0]=com_rbuf[100];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//valf=gas_ppm_to_mg_m3(valf, 46);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[133];
		f.ch[2]=com_rbuf[134];
		f.ch[1]=com_rbuf[131];
		f.ch[0]=com_rbuf[132];
		valf=f.f;
		valf*=0.001;	//ppb --->ppm
		//valf=gas_ppm_to_mg_m3(valf, 46);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


