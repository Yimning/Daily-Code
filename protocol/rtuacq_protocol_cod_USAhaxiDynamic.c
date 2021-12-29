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

int protocol_COD_USAhaxiDynamic(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float total_cod=0;
	int ret=0;
	int arg_n=0;
	unsigned long val=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	
	sleep(1);

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x10);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"haxi dynamic COD SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("haxi dynamic COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("haxi dynamic COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"haxi dynamic COD RECV:",com_rbuf,size);
	if((size>=37)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x20))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		total_cod=f.f;


		val = (com_rbuf[7]<<8)&0xFF00;
		val += com_rbuf[8];
		acq_data->acq_tm[5]=val;
		
		val = (com_rbuf[9]<<8)&0xFF00;
		val += com_rbuf[10];
		acq_data->acq_tm[4]=val/100;
		acq_data->acq_tm[3]=val%100;
		
		val = (com_rbuf[11]<<8)&0xFF00;
		val += com_rbuf[12];
		acq_data->acq_tm[2]=val/100;
		acq_data->acq_tm[1]=val%100;
		
		val = (com_rbuf[13]<<8)&0xFF00;
		val += com_rbuf[14];
		acq_data->acq_tm[0]=val;

		
		val = (com_rbuf[17]<<8)&0xFF00;
		val += com_rbuf[18];
		switch(val)
		{
			case 0: acq_data->instrstat=0;break;
			case 1: acq_data->instrstat=2;break;
			case 2: acq_data->instrstat=2;break;
			case 3: acq_data->instrstat=0;break;
			default: acq_data->instrstat=0;break;
		}

		if(val==1)
		{
			val = (com_rbuf[27]<<24)&0xFF000000;
			val += (com_rbuf[28]<<16)&0xFF0000;
			val += (com_rbuf[25]<<8)&0xFF00;
			val += com_rbuf[26];
			
			if(val==0) acq_data->alarm=0;
			else if((val&0x0001) == 0x0001) acq_data->alarm=8;
			else if((val&0x0008) == 0x0008) acq_data->alarm=1;
			else if((val&0x0020) == 0x0020) acq_data->alarm=7;
			else if((val&0x0400) == 0x0400) acq_data->alarm=8;
			else if((val&0x0800) == 0x0800) acq_data->alarm=2;
		}
		else if(val==2)
		{
			val = (com_rbuf[23]<<24)&0xFF000000;
			val += (com_rbuf[24]<<16)&0xFF0000;
			val += (com_rbuf[21]<<8)&0xFF00;
			val += com_rbuf[22];

			if(val==0) acq_data->alarm=0;
			else if((val&0x00000001) == 0x00000001) acq_data->alarm=8;
			else if((val&0x00000002) == 0x00000002) acq_data->alarm=8;
			else if((val&0x00000004) == 0x00000004) acq_data->alarm=1;
			else if((val&0x00000040) == 0x00000040) acq_data->alarm=5;
			else if((val&0x00000080) == 0x00000080) acq_data->alarm=2;
			else if((val&0x00000100) == 0x00000100) acq_data->alarm=8;
			else if((val&0x00000200) == 0x00000200) acq_data->alarm=3;
			else if((val&0x00000400) == 0x00000400) acq_data->alarm=3;
			else if((val&0x00000800) == 0x00000800) acq_data->alarm=3;
			else if((val&0x00001000) == 0x00001000) acq_data->alarm=3;
			else if((val&0x00002000) == 0x00002000) acq_data->alarm=7;
			else if((val&0x00004000) == 0x00004000) acq_data->alarm=7;
			else if((val&0x00008000) == 0x00008000) acq_data->alarm=7;
			else if((val&0x00010000) == 0x00010000) acq_data->alarm=8;
			else if((val&0x00020000) == 0x00020000) acq_data->alarm=8;
			else if((val&0x00040000) == 0x00040000) acq_data->alarm=8;
			else if((val&0x00080000) == 0x00080000) acq_data->alarm=8;
			else if((val&0x00100000) == 0x00100000) acq_data->alarm=8;
			else if((val&0x00200000) == 0x00200000) acq_data->alarm=8;
			else if((val&0x00800000) == 0x00800000) acq_data->alarm=7;
			else if((val&0x01000000) == 0x01000000) acq_data->alarm=7;
			else if((val&0x02000000) == 0x02000000) acq_data->alarm=6;
			else if((val&0x08000000) == 0x08000000) acq_data->alarm=8;
			else if((val&0x10000000) == 0x10000000) acq_data->alarm=8;
			else if((val&0x20000000) == 0x20000000) acq_data->alarm=1;
			
		}
				
		status=0;
	}
	else
	{
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x10);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"haxi dynamic COD SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(2);

		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("haxi dynamic COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("haxi dynamic COD data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"haxi dynamic COD RECV:",com_rbuf,size);
		if((size>=37)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x20))
		{
			f.ch[0]=com_rbuf[4];
			f.ch[1]=com_rbuf[3];
			f.ch[2]=com_rbuf[6];
			f.ch[3]=com_rbuf[5];
			total_cod=f.f;


			val = (com_rbuf[7]<<8)&0xFF00;
			val += com_rbuf[8];
			acq_data->acq_tm[5]=val;
			
			val = (com_rbuf[9]<<8)&0xFF00;
			val += com_rbuf[10];
			acq_data->acq_tm[4]=val/100;
			acq_data->acq_tm[3]=val%100;
			
			val = (com_rbuf[11]<<8)&0xFF00;
			val += com_rbuf[12];
			acq_data->acq_tm[2]=val/100;
			acq_data->acq_tm[1]=val%100;
			
			val = (com_rbuf[13]<<8)&0xFF00;
			val += com_rbuf[14];
			acq_data->acq_tm[0]=val;

			
			val = (com_rbuf[17]<<8)&0xFF00;
			val += com_rbuf[18];
			switch(val)
			{
				case 0: acq_data->instrstat=0;break;
				case 1: acq_data->instrstat=2;break;
				case 2: acq_data->instrstat=2;break;
				case 3: acq_data->instrstat=0;break;
				default: acq_data->instrstat=0;break;
			}

			if(val==1)
			{
				val = (com_rbuf[27]<<24)&0xFF000000;
				val += (com_rbuf[28]<<16)&0xFF0000;
				val += (com_rbuf[25]<<8)&0xFF00;
				val += com_rbuf[26];
				
				if(val==0) acq_data->alarm=0;
				else if((val&0x0001) == 0x0001) acq_data->alarm=8;
				else if((val&0x0008) == 0x0008) acq_data->alarm=1;
				else if((val&0x0020) == 0x0020) acq_data->alarm=7;
				else if((val&0x0400) == 0x0400) acq_data->alarm=8;
				else if((val&0x0800) == 0x0800) acq_data->alarm=2;
			}
			else if(val==2)
			{
				val = (com_rbuf[23]<<24)&0xFF000000;
				val += (com_rbuf[24]<<16)&0xFF0000;
				val += (com_rbuf[21]<<8)&0xFF00;
				val += com_rbuf[22];

				if(val==0) acq_data->alarm=0;
				else if((val&0x00000001) == 0x00000001) acq_data->alarm=8;
				else if((val&0x00000002) == 0x00000002) acq_data->alarm=8;
				else if((val&0x00000004) == 0x00000004) acq_data->alarm=1;
				else if((val&0x00000040) == 0x00000040) acq_data->alarm=5;
				else if((val&0x00000080) == 0x00000080) acq_data->alarm=2;
				else if((val&0x00000100) == 0x00000100) acq_data->alarm=8;
				else if((val&0x00000200) == 0x00000200) acq_data->alarm=3;
				else if((val&0x00000400) == 0x00000400) acq_data->alarm=3;
				else if((val&0x00000800) == 0x00000800) acq_data->alarm=3;
				else if((val&0x00001000) == 0x00001000) acq_data->alarm=3;
				else if((val&0x00002000) == 0x00002000) acq_data->alarm=7;
				else if((val&0x00004000) == 0x00004000) acq_data->alarm=7;
				else if((val&0x00008000) == 0x00008000) acq_data->alarm=7;
				else if((val&0x00010000) == 0x00010000) acq_data->alarm=8;
				else if((val&0x00020000) == 0x00020000) acq_data->alarm=8;
				else if((val&0x00040000) == 0x00040000) acq_data->alarm=8;
				else if((val&0x00080000) == 0x00080000) acq_data->alarm=8;
				else if((val&0x00100000) == 0x00100000) acq_data->alarm=8;
				else if((val&0x00200000) == 0x00200000) acq_data->alarm=8;
				else if((val&0x00800000) == 0x00800000) acq_data->alarm=7;
				else if((val&0x01000000) == 0x01000000) acq_data->alarm=7;
				else if((val&0x02000000) == 0x02000000) acq_data->alarm=6;
				else if((val&0x08000000) == 0x08000000) acq_data->alarm=8;
				else if((val&0x10000000) == 0x10000000) acq_data->alarm=8;
				else if((val&0x20000000) == 0x20000000) acq_data->alarm=1;
				
			}
					
			status=0;
		}
		else
		{
			total_cod=0;
			status=1;

		}
	}
	sleep(1);

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x47,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"haxi dynamic COD vol SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("haxi dynamic COD vol protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("haxi dynamic COD vol data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"haxi dynamic COD vol RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x04))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		acq_data->voltage=f.f;
	}
	else
	{
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x03,0x47,0x2);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"haxi dynamic COD vol SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(2);

		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("haxi dynamic COD vol protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("haxi dynamic COD vol data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"haxi dynamic COD vol RECV:",com_rbuf,size);
		if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x04))
		{
			f.ch[0]=com_rbuf[4];
			f.ch[1]=com_rbuf[3];
			f.ch[2]=com_rbuf[6];
			f.ch[3]=com_rbuf[5];
			acq_data->voltage=f.f;
		}
		else
		{
			acq_data->voltage=0;
			status=1;

		}
	}

	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

