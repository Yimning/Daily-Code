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

int protocol_ANDAN_NA8000(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float andan=0;
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

	status=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x0F);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NA8000 andan SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NA8000 andan protocol,andan : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NA8000 andan data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NA8000 andan RECV:",com_rbuf,size);
	if((size>=35)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x1E))
	{
		acq_data->acq_tm[5]=((com_rbuf[3]<<8)&0xFF00)+com_rbuf[4];
		acq_data->acq_tm[4]=com_rbuf[5];
		acq_data->acq_tm[3]=com_rbuf[6];
		acq_data->acq_tm[2]=com_rbuf[7];
		acq_data->acq_tm[1]=com_rbuf[8];
		acq_data->acq_tm[0]=com_rbuf[10];
	
		f.ch[0]=com_rbuf[12];
		f.ch[1]=com_rbuf[11];
		f.ch[2]=com_rbuf[14];
		f.ch[3]=com_rbuf[13];
		andan=f.f;


		val = (com_rbuf[15]<<8)&0xFF00;
		val += com_rbuf[16];
		acq_data->voltage=val;
		
	}
	else
	{
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x0F);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NA8000 andan SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(1);

		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("NA8000 andan protocol,andan : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("NA8000 andan data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NA8000 andan RECV:",com_rbuf,size);
		if((size>=35)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x1E))
		{
			acq_data->acq_tm[5]=((com_rbuf[3]<<8)&0xFF00)+com_rbuf[4];
			acq_data->acq_tm[4]=com_rbuf[5];
			acq_data->acq_tm[3]=com_rbuf[6];
			acq_data->acq_tm[2]=com_rbuf[7];
			acq_data->acq_tm[1]=com_rbuf[8];
			acq_data->acq_tm[0]=com_rbuf[10];
		
			f.ch[0]=com_rbuf[12];
			f.ch[1]=com_rbuf[11];
			f.ch[2]=com_rbuf[14];
			f.ch[3]=com_rbuf[13];
			andan=f.f;


			val = (com_rbuf[15]<<8)&0xFF00;
			val += com_rbuf[16];
			acq_data->voltage=val;
			
		}
		else
		{
			andan=0;
			status=1;
		}
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xA4,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NA8000 andan stat SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("NA8000 andan stat protocol,andan : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NA8000 andan stat data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NA8000 andan stat RECV:",com_rbuf,size);
	if((size>=17)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x0C))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		if(val!=0)
			acq_data->acq_status=3;
		else
		{
			val=com_rbuf[5];
			val<<=8;
			val+=com_rbuf[6];

			switch(val)
			{
				case 0: 	acq_data->acq_status=9;break;
				case 1: 	acq_data->acq_status=2;break;
				case 2: 	acq_data->acq_status=4;break;
				case 3: 	acq_data->acq_status=7;break;
				case 4: 	acq_data->acq_status=1;break;
				case 5: 	acq_data->acq_status=2;break;
				case 6: 	acq_data->acq_status=2;break;
				case 7: 	acq_data->acq_status=1;break;
				case 9: 	acq_data->acq_status=1;break;
				case 10: acq_data->acq_status=8;break;
				case 11: acq_data->acq_status=8;break;
				case 12: acq_data->acq_status=1;break;
				case 13: acq_data->acq_status=1;break;
				default: 	acq_data->acq_status=9;break;
			}
		}

		acq_data->alarm=0;

		if((com_rbuf[9]&0x01)==0x01) acq_data->alarm=7;
		if((com_rbuf[9]&0x02)==0x02) acq_data->alarm=1;
		if((com_rbuf[9]&0x04)==0x04) acq_data->alarm=1;
		if((com_rbuf[9]&0x08)==0x08) acq_data->alarm=2;
		if((com_rbuf[9]&0x10)==0x10) acq_data->alarm=5;
		if((com_rbuf[9]&0x20)==0x20) acq_data->alarm=1;
		if((com_rbuf[9]&0x80)==0x80) acq_data->alarm=1;

		if((com_rbuf[10]&0x01)==0x01) acq_data->alarm=1;
		if((com_rbuf[10]&0x08)==0x08) acq_data->alarm=8;
		if((com_rbuf[10]&0x10)==0x10) acq_data->alarm=5;
		if((com_rbuf[10]&0x20)==0x20) acq_data->alarm=8;
		if((com_rbuf[10]&0x40)==0x40) acq_data->alarm=8;
		if((com_rbuf[10]&0x80)==0x80) acq_data->alarm=8;

		if((com_rbuf[11]&0x01)==0x01) acq_data->alarm=8;
		if((com_rbuf[11]&0x08)==0x08) acq_data->alarm=7;
		if((com_rbuf[11]&0x10)==0x10) acq_data->alarm=7;
		if((com_rbuf[11]&0x20)==0x20) acq_data->alarm=3;
		if((com_rbuf[11]&0x40)==0x40) acq_data->alarm=3;
		if((com_rbuf[11]&0x80)==0x80) acq_data->alarm=3;

		if((com_rbuf[12]&0x01)==0x01) acq_data->alarm=8;
		if((com_rbuf[12]&0x02)==0x02) acq_data->alarm=8;
		if((com_rbuf[12]&0x04)==0x04) acq_data->alarm=8;
		if((com_rbuf[12]&0x08)==0x08) acq_data->alarm=8;

		if((com_rbuf[14]&0x04)==0x04) acq_data->alarm=1;
		if((com_rbuf[14]&0x08)==0x08) acq_data->alarm=1;
		if((com_rbuf[14]&0x10)==0x10) acq_data->alarm=1;
		if((com_rbuf[14]&0x20)==0x20) acq_data->alarm=1;
		if((com_rbuf[14]&0x40)==0x40) acq_data->alarm=1;
		if((com_rbuf[14]&0x80)==0x80) acq_data->alarm=7;
		
	}
	else
	{
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x03,0xA4,0x06);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NA8000 andan stat SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("NA8000 andan stat protocol,andan : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("NA8000 andan stat data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NA8000 andan stat RECV:",com_rbuf,size);
		if((size>=17)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03)&&(com_rbuf[2]==0x0C))
		{
			val=com_rbuf[3];
			val<<=8;
			val+=com_rbuf[4];
			if(val!=0)
				acq_data->acq_status=3;
			else
			{
				val=com_rbuf[5];
				val<<=8;
				val+=com_rbuf[6];

				switch(val)
				{
					case 0: 	acq_data->acq_status=9;break;
					case 1: 	acq_data->acq_status=2;break;
					case 2: 	acq_data->acq_status=4;break;
					case 3: 	acq_data->acq_status=7;break;
					case 4: 	acq_data->acq_status=1;break;
					case 5: 	acq_data->acq_status=2;break;
					case 6: 	acq_data->acq_status=2;break;
					case 7: 	acq_data->acq_status=1;break;
					case 9: 	acq_data->acq_status=1;break;
					case 10: acq_data->acq_status=8;break;
					case 11: acq_data->acq_status=8;break;
					case 12: acq_data->acq_status=1;break;
					case 13: acq_data->acq_status=1;break;
					default: 	acq_data->acq_status=9;break;
				}
			}

			acq_data->alarm=0;

			if((com_rbuf[9]&0x01)==0x01) acq_data->alarm=7;
			if((com_rbuf[9]&0x02)==0x02) acq_data->alarm=1;
			if((com_rbuf[9]&0x04)==0x04) acq_data->alarm=1;
			if((com_rbuf[9]&0x08)==0x08) acq_data->alarm=2;
			if((com_rbuf[9]&0x10)==0x10) acq_data->alarm=5;
			if((com_rbuf[9]&0x20)==0x20) acq_data->alarm=1;
			if((com_rbuf[9]&0x80)==0x80) acq_data->alarm=1;

			if((com_rbuf[10]&0x01)==0x01) acq_data->alarm=1;
			if((com_rbuf[10]&0x08)==0x08) acq_data->alarm=8;
			if((com_rbuf[10]&0x10)==0x10) acq_data->alarm=5;
			if((com_rbuf[10]&0x20)==0x20) acq_data->alarm=8;
			if((com_rbuf[10]&0x40)==0x40) acq_data->alarm=8;
			if((com_rbuf[10]&0x80)==0x80) acq_data->alarm=8;

			if((com_rbuf[11]&0x01)==0x01) acq_data->alarm=8;
			if((com_rbuf[11]&0x08)==0x08) acq_data->alarm=7;
			if((com_rbuf[11]&0x10)==0x10) acq_data->alarm=7;
			if((com_rbuf[11]&0x20)==0x20) acq_data->alarm=3;
			if((com_rbuf[11]&0x40)==0x40) acq_data->alarm=3;
			if((com_rbuf[11]&0x80)==0x80) acq_data->alarm=3;

			if((com_rbuf[12]&0x01)==0x01) acq_data->alarm=8;
			if((com_rbuf[12]&0x02)==0x02) acq_data->alarm=8;
			if((com_rbuf[12]&0x04)==0x04) acq_data->alarm=8;
			if((com_rbuf[12]&0x08)==0x08) acq_data->alarm=8;

			if((com_rbuf[14]&0x04)==0x04) acq_data->alarm=1;
			if((com_rbuf[14]&0x08)==0x08) acq_data->alarm=1;
			if((com_rbuf[14]&0x10)==0x10) acq_data->alarm=1;
			if((com_rbuf[14]&0x20)==0x20) acq_data->alarm=1;
			if((com_rbuf[14]&0x40)==0x40) acq_data->alarm=1;
			if((com_rbuf[14]&0x80)==0x80) acq_data->alarm=7;
			
		}
		else
		{
			acq_data->instrstat=0;
			acq_data->alarm=0;
		}
	}

	ret=acqdata_set_value(acq_data,"w21003",UNIT_MG_L,andan,&arg_n);
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

