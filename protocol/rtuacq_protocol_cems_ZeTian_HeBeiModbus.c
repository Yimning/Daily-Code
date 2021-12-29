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

static float ZeTian_SO2_original_value;
static float ZeTian_NOx_original_value;

#if 0
int protocol_CEMS_ZeTian_HeBeiModbus(struct acquisition_data *acq_data)
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
	float nox;
	char flag=0;
	int val=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

#define CEMS_POLCODE_NUM 6
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026a","a21003","a19001a","a21004","a21005","a05001"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT, UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT
	};
	float valf[CEMS_POLCODE_NUM];

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x01,0x0E);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian HeBeiModbus CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian HeBeiModbus CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian HeBeiModbus CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian HeBeiModbus CEMS RECV:",com_rbuf,size);
	if((size>=33)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			f.ch[0]=com_rbuf[6+i*4];
			f.ch[1]=com_rbuf[5+i*4];
			f.ch[2]=com_rbuf[4+i*4];
			f.ch[3]=com_rbuf[3+i*4];
			valf[i]=f.f;
			acqdata_set_value(acq_data,polcode[i],unit[i],f.f,&arg_n);

			if(!strcmp(polcode[i],"a21026a"))
			{
				//acqdata_set_value(acq_data,"a21026",unit[i],f.f,&arg_n);
				acqdata_set_value_orig(acq_data,"a21026",unit[i],f.f,ZeTian_SO2_original_value,&arg_n);
			}
			if(!strcmp(polcode[i],"a19001a"))
				acqdata_set_value(acq_data,"a19001",unit[i],f.f,&arg_n);			
		}

		val=getUInt16Value(com_rbuf, 29, INT_AB);
		switch(val)
		{
			case 0x01:	flag='N';break;
			case 0x10:	flag='z';break;
			case 0x20:	flag='C';break;
			case 0x40:	flag='M';break;
			case 0x80:	flag='D';break;
			default:		flag='N';break;
		}
		status=0;
	}
	else
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
			if(!strcmp(polcode[i],"a21026a"))
			{
				//acqdata_set_value(acq_data,"a21026",unit[i],0,&arg_n);
				acqdata_set_value_orig(acq_data,"a21026",unit[i],0,0,&arg_n);
			}
			if(!strcmp(polcode[i],"a19001a"))
				acqdata_set_value(acq_data,"a19001",unit[i],0,&arg_n);				
		}
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x4,0x015F,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian HeBeiModbus NOx SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian HeBeiModbus NOx protocol,NOx : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian HeBeiModbus NOx data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian HeBeiModbus NOx RECV:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		if(com_rbuf[4]==0x00)
		{
			nox=valf[1]*46/30+valf[3];
		}
		else
		{
			nox=valf[1];
		}
	}
	else
	{
		nox=valf[1];
	}

    acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	//acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,ZeTian_NOx_original_value,&arg_n);

	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	//acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	//acqdata_set_value(acq_data,"a21004z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);

	acq_data->dev_stat=flag;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
#else
int protocol_CEMS_ZeTian_HeBeiModbus(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0,j=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float no=0,no2=0,nox=0;
	char flag=0;
	int val=0;
	int nox_flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

#define CEMS_POLCODE_NUM 6
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026","a21003","a19001","a21004","a21005","a05001"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT, UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT
	};
	float valf[CEMS_POLCODE_NUM];

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	nox_flag=0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x01,0x14);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian HeBeiModbus CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian HeBeiModbus CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian HeBeiModbus CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian HeBeiModbus CEMS RECV:",com_rbuf,size);
	if((size>=45)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			f.ch[0]=com_rbuf[6+i*4];
			f.ch[1]=com_rbuf[5+i*4];
			f.ch[2]=com_rbuf[4+i*4];
			f.ch[3]=com_rbuf[3+i*4];
			valf[i]=f.f;
		}

		val=getUInt16Value(com_rbuf, 41, INT_AB);
		switch(val)
		{
			case 0x01:	flag='N';break;
			case 0x10:	flag='z';break;
			case 0x20:	flag='C';break;
			case 0x40:	flag='M';break;
			case 0x80:	flag='D';break;
			default:		flag='N';break;
		}
		status=0;
	}
	else
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			valf[i]=0;
						
		}
		status=1;
	}

	j=0;
	for(i=0;i<CEMS_POLCODE_NUM;i++)
	{
		if((i==1) && (isPolcodeEnable(modbusarg, "a21002")) && (!isPolcodeEnable(modbusarg, "a21003")))
		{
			nox=valf[j];
		
			nox_flag=1;
			j++;
			continue;
		}
	
		if(isPolcodeEnable(modbusarg, polcode[i]))
		{
			switch(i)
			{
				case 0:
					acqdata_set_value(acq_data,"a21026a",unit[i],valf[j],&arg_n);
					acqdata_set_value_orig(acq_data,polcode[i],unit[i],valf[j],ZeTian_SO2_original_value,&arg_n);
					acqdata_set_value(acq_data,"a21026z",unit[i],0,&arg_n);
					break;
					
				case 1:
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					no=valf[j];
					break;

				case 2:
					acqdata_set_value(acq_data,"a19001a",unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					break;

				case 3:
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					no2=valf[j];
					break;

				case 4:
					acqdata_set_value(acq_data,"a21005a",unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,"a21005z",unit[i],0,&arg_n);
					break;

				case 5:
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					break;
			}
			j++;
		}
	}

	if(nox_flag==0)
	{
		sleep(1);
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x4,0x015F,0x01);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian HeBeiModbus NOx SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("ZeTian HeBeiModbus NOx protocol,NOx : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("ZeTian HeBeiModbus NOx data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian HeBeiModbus NOx RECV:",com_rbuf,size);
		if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
		{
			if(com_rbuf[4]==0x00)
			{
				nox=NO_and_NO2_to_NOx(no, no2);
			}
			else
			{
				nox=no;
			}
		}
		else
		{
			nox=no;
		}
	}

    	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,ZeTian_NOx_original_value,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);


	acq_data->dev_stat=flag;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
#endif

int protocol_CEMS_ZeTian_HeBeiModbus_old(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0,j=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float no=0,no2=0,nox=0;
	char flag=0;
	int val=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

#define CEMS_POLCODE_NUM 6
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026","a21003","a19001","a21004","a21005","a05001"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT, UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT
	};
	float valf[CEMS_POLCODE_NUM];

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x01,0x0C);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian HeBeiModbus CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian HeBeiModbus CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian HeBeiModbus CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian HeBeiModbus CEMS RECV:",com_rbuf,size);
	if((size>=29)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			f.ch[0]=com_rbuf[6+i*4];
			f.ch[1]=com_rbuf[5+i*4];
			f.ch[2]=com_rbuf[4+i*4];
			f.ch[3]=com_rbuf[3+i*4];
			valf[i]=f.f;
		}

		
		status=0;
	}
	else
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			valf[i]=0;
						
		}
		status=1;
	}

	j=0;
	for(i=0;i<CEMS_POLCODE_NUM;i++)
	{
		if(isPolcodeEnable(modbusarg, polcode[i]))
		{
			switch(i)
			{
				case 0:
					acqdata_set_value(acq_data,"a21026a",unit[i],valf[j],&arg_n);
					acqdata_set_value_orig(acq_data,polcode[i],unit[i],valf[j],ZeTian_SO2_original_value,&arg_n);
					acqdata_set_value(acq_data,"a21026z",unit[i],0,&arg_n);
					break;
					
				case 1:
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					no=valf[j];
					break;

				case 2:
					acqdata_set_value(acq_data,"a19001a",unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					break;

				case 3:
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					no2=valf[j];
					break;

				case 4:
					acqdata_set_value(acq_data,"a21005a",unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,"a21005z",unit[i],0,&arg_n);
					break;

				case 5:
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					break;
			}
			j++;
		}
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x4,0x015F,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian HeBeiModbus NOx SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian HeBeiModbus NOx protocol,NOx : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian HeBeiModbus NOx data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian HeBeiModbus NOx RECV:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		if(com_rbuf[4]==0x00)
		{
			nox=NO_and_NO2_to_NOx(no, no2);
		}
		else
		{
			nox=no;
		}
	}
	else
	{
		nox=no;
	}

    	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,ZeTian_NOx_original_value,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);


	//acq_data->dev_stat=flag;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


int protocol_CEMS_ZeTian_HeBeiModbus2(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0,j=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float no=0,no2=0,nox=0;
	char flag=0;
	int val=0;
	int nox_flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct modbus_polcode_arg *mpolcodearg;

#define CEMS_POLCODE_NUM 6
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026","a21003","a19001","a21004","a21005","a05001"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT, UNIT_MG_M3, UNIT_MG_M3, UNIT_PECENT
	};
	float valf[CEMS_POLCODE_NUM];

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	nox_flag=0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x01,0x14);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian HeBeiModbus CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZeTian HeBeiModbus CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZeTian HeBeiModbus CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian HeBeiModbus CEMS RECV:",com_rbuf,size);
	if((size>=45)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			f.ch[0]=com_rbuf[6+i*4];
			f.ch[1]=com_rbuf[5+i*4];
			f.ch[2]=com_rbuf[4+i*4];
			f.ch[3]=com_rbuf[3+i*4];
			valf[i]=f.f;
		}

		val=getUInt16Value(com_rbuf, 41, INT_AB);
		switch(val)
		{
			case 0x01:	flag='N';break;
			case 0x10:	flag='z';break;
			case 0x20:	flag='C';break;
			case 0x40:	flag='M';break;
			case 0x80:	flag='D';break;
			default:		flag='N';break;
		}
		status=0;
	}
	else
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			valf[i]=0;
						
		}
		status=1;
	}

	j=0;
	for(i=0;i<CEMS_POLCODE_NUM;i++)
	{
		mpolcodearg=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg, modbusarg->array_count, polcode[i]);
		if(mpolcodearg==NULL)
			continue;
	
		if(mpolcodearg->enableFlag==1)
		{
			j=mpolcodearg->regaddr-1;
			if((j<0) || (j>5))
				j=0;

			if((i==1) && (isPolcodeEnable(modbusarg, "a21002")) && (!isPolcodeEnable(modbusarg, "a21003")))
			{
				nox=valf[j];
			
				nox_flag=1;
				j++;
				continue;
			}
					
			switch(i)
			{
				case 0:
					acqdata_set_value(acq_data,"a21026a",unit[i],valf[j],&arg_n);
					acqdata_set_value_orig(acq_data,polcode[i],unit[i],valf[j],ZeTian_SO2_original_value,&arg_n);
					acqdata_set_value(acq_data,"a21026z",unit[i],valf[j],&arg_n);
					break;
					
				case 1:
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					no=valf[j];
					break;

				case 2:
					acqdata_set_value(acq_data,"a19001a",unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					break;

				case 3:
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					no2=valf[j];
					break;

				case 4:
					acqdata_set_value(acq_data,"a21005a",unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,"a21005z",unit[i],valf[j],&arg_n);
					break;

				case 5:
					acqdata_set_value(acq_data,"a05001a",unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,polcode[i],unit[i],valf[j],&arg_n);
					acqdata_set_value(acq_data,"a05001z",unit[i],valf[j],&arg_n);
					break;
			}
		}
	}

	if(nox_flag==0)
	{
		sleep(1);
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x4,0x015F,0x01);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZeTian HeBeiModbus NOx SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("ZeTian HeBeiModbus NOx protocol,NOx : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("ZeTian HeBeiModbus NOx data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZeTian HeBeiModbus NOx RECV:",com_rbuf,size);
		if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
		{
			if(com_rbuf[4]==0x00)
			{
				nox=NO_and_NO2_to_NOx(no, no2);
			}
			else
			{
				nox=no;
			}
		}
		else
		{
			nox=no;
		}
	}

    	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value_orig(acq_data,"a21002",UNIT_MG_M3,nox,ZeTian_NOx_original_value,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);


	acq_data->dev_stat=flag;

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
	
#undef CEMS_POLCODE_NUM	
}

int protocol_CEMS_ZeTian_HeBeiModbus_STATUS(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0,cmd=0,cnt=0;
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ZeTian_HeBeiModbus_STATUS\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(isPolcodeEnable(modbusarg, "a21026"))
		acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21003"))
		acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21002"))
		acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a19001"))
		acqdata_set_value_flag(acq_data,"a19001",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21005"))
		acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a05001"))
		acqdata_set_value_flag(acq_data,"a05001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	cnt=0x01;

	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x14,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian STATUS", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getUInt16Value(p, 3, INT_AB);

		switch(val)
		{
			case 0: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 1: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
			case 16: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n); break;
			case 32: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
			case 64: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n); break;
			case 128: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n); break;
			default: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
		}

		status = 0;
	}

	
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_ZeTian_HeBeiModbus_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	MODBUS_DATA_TYPE dataType;
	int devaddr=0,cmd=0,cnt=0;
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ZeTian_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	cnt=0x1F;
	dataType=FLOAT_ABCD;

	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x012C,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian SO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(p, 5, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 9, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 25, INT_AB);
		t3=getTimeValue(val, p[28], p[30], p[32], p[34], p[36]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 37, INT_AB);
		t3=getTimeValue(val, p[40], p[42], p[44], p[46], p[48]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 49, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 53, dataType);
		ZeTian_SO2_original_value=valf;
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 57, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 61, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_ZeTian_HeBeiModbus_NOx_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	MODBUS_DATA_TYPE dataType;
	int devaddr=0,cmd=0,cnt=0;
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ZeTian_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	cnt=0x20;
	dataType=FLOAT_ABCD;

	if(isPolcodeEnable(modbusarg, "a21003"))
		acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else if(isPolcodeEnable(modbusarg, "a21002"))
		acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	else
		return 0;

	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x0154,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian NOx INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(p, 5, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 9, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 27, INT_AB);
		t3=getTimeValue(val, p[30], p[32], p[34], p[36], p[38]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 39, INT_AB);
		t3=getTimeValue(val, p[42], p[44], p[46], p[48], p[50]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 51, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 55, dataType);
		ZeTian_NOx_original_value=valf;
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 59, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 63, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_ZeTian_HeBeiModbus_NO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	MODBUS_DATA_TYPE dataType;
	int devaddr=0,cmd=0,cnt=0;
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ZeTian_NO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	cnt=0x1F;
	dataType=FLOAT_ABCD;

	if(!isPolcodeEnable(modbusarg, "a21004"))
		return 0;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21004",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x01CC,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian NO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(p, 5, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 9, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 25, INT_AB);
		t3=getTimeValue(val, p[28], p[30], p[32], p[34], p[36]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 37, INT_AB);
		t3=getTimeValue(val, p[40], p[42], p[44], p[46], p[48]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 49, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 53, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 57, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 61, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_ZeTian_HeBeiModbus_O2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	MODBUS_DATA_TYPE dataType;
	int devaddr=0,cmd=0,cnt=0;
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ZeTian_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	cnt=0x1F;
	dataType=FLOAT_ABCD;

	if(!isPolcodeEnable(modbusarg, "a19001"))
		return 0;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x01A4,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian O2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,val,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(p, 5, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 9, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 25, INT_AB);
		t3=getTimeValue(val, p[28], p[30], p[32], p[34], p[36]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 37, INT_AB);
		t3=getTimeValue(val, p[40], p[42], p[44], p[46], p[48]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 49, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 53, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 57, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 61, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_ZeTian_HeBeiModbus_CO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	MODBUS_DATA_TYPE dataType;
	int devaddr=0,cmd=0,cnt=0;
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ZeTian_CO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	cnt=0x1F;
	dataType=FLOAT_ABCD;

	if(!isPolcodeEnable(modbusarg, "a21005"))
		return 0;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x01F4,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian CO INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(p, 5, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 9, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 25, INT_AB);
		t3=getTimeValue(val, p[28], p[30], p[32], p[34], p[36]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 37, INT_AB);
		t3=getTimeValue(val, p[40], p[42], p[44], p[46], p[48]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 49, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 53, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 57, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 61, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_ZeTian_HeBeiModbus_CO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	MODBUS_DATA_TYPE dataType;
	int devaddr=0,cmd=0,cnt=0;
	char *p;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_ZeTian_CO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x04;
	cnt=0x1F;
	dataType=FLOAT_ABCD;

	if(!isPolcodeEnable(modbusarg, "a05001"))
		return 0;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a05001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x01F4,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "ZeTian CO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		val=getInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);
	
		valf=getFloatValue(p, 5, dataType);
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 9, dataType);
		acqdata_set_value_flag(acq_data,"i13022",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 13, dataType);
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 17, dataType);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 21, dataType);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 25, INT_AB);
		t3=getTimeValue(val, p[28], p[30], p[32], p[34], p[36]);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, 37, INT_AB);
		t3=getTimeValue(val, p[40], p[42], p[44], p[46], p[48]);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 49, dataType);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 53, dataType);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 57, dataType);
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getFloatValue(p, 61, dataType);
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


