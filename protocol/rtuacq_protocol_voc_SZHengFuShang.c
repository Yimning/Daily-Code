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

int protocol_VOCs_SZHengFuSheng(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char polcode[7]={0};
	int size=0;
	float valf;
	int val=0;
	int i;
	float ec,power,electric;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	
	devaddr=modbusarg->devaddr&0xffff;
	status=0;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZHengFuSheng voc SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZHengFuSheng voc protocol,voc 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZHengFuSheng voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZHengFuSheng voc RECV1:",com_rbuf,size);
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];

		acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,val/10.0,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,0,&arg_n);
		status=1;
	}



	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x04,0x09);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZHengFuSheng voc SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZHengFuSheng voc protocol,voc 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZHengFuSheng voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZHengFuSheng voc RECV2:",com_rbuf,size);
	if((size>=23)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		acqdata_set_value(acq_data,"g28001",UNIT_0C,val/10.0,&arg_n);

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		acqdata_set_value(acq_data,"g28201",UNIT_PA,(float)val,&arg_n);

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		acqdata_set_value(acq_data,"g20201",UNIT_M_S,val/100.0,&arg_n);

		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		val<<=8;
		val+=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		acqdata_set_value(acq_data,"g20101",UNIT_M3_H,(float)val,&arg_n);

		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		val<<=8;
		val+=com_rbuf[13];
		val<<=8;
		val+=com_rbuf[14];
		acqdata_set_value(acq_data,"g18401",UNIT_PA,val/100.0,&arg_n);

		val=com_rbuf[17];
		val<<=8;
		val+=com_rbuf[18];
		acqdata_set_value(acq_data,"g18001",UNIT_0C,val/10.0,&arg_n);

		val=com_rbuf[19];
		val<<=8;
		val+=com_rbuf[20];
		acqdata_set_value(acq_data,"g18101",UNIT_PECENT,val/10.0,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"g28001",UNIT_0C,0,&arg_n);
		acqdata_set_value(acq_data,"g28201",UNIT_PA,0,&arg_n);
		acqdata_set_value(acq_data,"g20201",UNIT_M_S,0,&arg_n);
		acqdata_set_value(acq_data,"g20101",UNIT_M3_H,0,&arg_n);
		acqdata_set_value(acq_data,"g18401",UNIT_PA,0,&arg_n);
		acqdata_set_value(acq_data,"g18001",UNIT_0C,0,&arg_n);
		acqdata_set_value(acq_data,"g18101",UNIT_PECENT,0,&arg_n);
		status=1;
	}



	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x1D,0x08);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZHengFuSheng voc SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZHengFuSheng voc protocol,voc 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZHengFuSheng voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZHengFuSheng voc RECV3:",com_rbuf,size);
	if((size>=21)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		for(i=0;i<2;i++)
		{
			sprintf(polcode,"e3020%d",i+1);
			val=com_rbuf[3+i*8];
			val<<=8;
			val+=com_rbuf[4+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_A,val/100.0,&arg_n);

			sprintf(polcode,"e3010%d",i+1);
			val=com_rbuf[5+i*8];
			val<<=8;
			val+=com_rbuf[6+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_KW,val/100.0,&arg_n);

			sprintf(polcode,"e3000%d",i+1);
			val=com_rbuf[9+i*8];
			val<<=8;
			val+=com_rbuf[10+i*8];
			val<<=8;
			val+=com_rbuf[7+i*8];
			val<<=8;
			val+=com_rbuf[8+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_KWH,val/100.0,&arg_n);
		}
	}
	else
	{
		for(i=0;i<2;i++)
		{
			sprintf(polcode,"e3020%d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_A,0,&arg_n);
			sprintf(polcode,"e3010%d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_KW,0,&arg_n);
			sprintf(polcode,"e3000%d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_KWH,0,&arg_n);
		}
		
		status=1;
	}

	
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x29,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZHengFuSheng voc SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZHengFuSheng voc protocol,voc 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZHengFuSheng voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZHengFuSheng voc RECV4:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		acqdata_set_value(acq_data,"e30203",UNIT_A,val/100.0,&arg_n);

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		acqdata_set_value(acq_data,"e30103",UNIT_KW,val/100.0,&arg_n);

		val=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		val<<=8;
		val+=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		acqdata_set_value(acq_data,"e30003",UNIT_KWH,val/100.0,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"e30203",UNIT_A,0,&arg_n);
		acqdata_set_value(acq_data,"e30103",UNIT_KW,0,&arg_n);
		acqdata_set_value(acq_data,"e30003",UNIT_KWH,0,&arg_n);
		
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x31,0x10);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZHengFuSheng voc SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZHengFuSheng voc protocol,voc 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZHengFuSheng voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZHengFuSheng voc RECV5:",com_rbuf,size);
	if((size>=37)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		for(i=0;i<3;i++)
		{
			sprintf(polcode,"e602%02d",i+1);
			val=com_rbuf[3+i*8];
			val<<=8;
			val+=com_rbuf[4+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_A,val/100.0,&arg_n);

			sprintf(polcode,"e601%02d",i+1);
			val=com_rbuf[5+i*8];
			val<<=8;
			val+=com_rbuf[6+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_KW,val/100.0,&arg_n);

			sprintf(polcode,"e600%02d",i+1);
			val=com_rbuf[9+i*8];
			val<<=8;
			val+=com_rbuf[10+i*8];
			val<<=8;
			val+=com_rbuf[7+i*8];
			val<<=8;
			val+=com_rbuf[8+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_KWH,val/100.0,&arg_n);
		}
		val=com_rbuf[27];
		val<<=8;
		val+=com_rbuf[28];
		acqdata_set_value(acq_data,"e70201",UNIT_A,val/100.0,&arg_n);

		val=com_rbuf[29];
		val<<=8;
		val+=com_rbuf[30];
		acqdata_set_value(acq_data,"e70101",UNIT_KW,val/100.0,&arg_n);

		val=com_rbuf[33];
		val<<=8;
		val+=com_rbuf[34];
		val<<=8;
		val+=com_rbuf[31];
		val<<=8;
		val+=com_rbuf[32];
		acqdata_set_value(acq_data,"e70001",UNIT_KWH,val/100.0,&arg_n);
	}
	else
	{
		for(i=0;i<3;i++)
		{
			sprintf(polcode,"e6020%d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_A,0,&arg_n);
			sprintf(polcode,"e6010%d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_KW,0,&arg_n);
			sprintf(polcode,"e6000%d",i+1);
			acqdata_set_value(acq_data,polcode,UNIT_KWH,0,&arg_n);
		}
		acqdata_set_value(acq_data,"e70201",UNIT_A,0,&arg_n);
		acqdata_set_value(acq_data,"e70101",UNIT_KW,0,&arg_n);
		acqdata_set_value(acq_data,"e70001",UNIT_KWH,0,&arg_n);
		
		status=1;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0xC9,0x14);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SZHengFuSheng voc SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SZHengFuSheng voc protocol,voc 6: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SZHengFuSheng voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SZHengFuSheng voc RECV6:",com_rbuf,size);
	if((size>=45)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==03))
	{
		for(i=0;i<4;i++)
		{
			sprintf(polcode,"e302%02d",i+4);
			val=com_rbuf[3+i*8];
			val<<=8;
			val+=com_rbuf[4+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_A,val/100.0,&arg_n);

			sprintf(polcode,"e301%02d",i+4);
			val=com_rbuf[5+i*8];
			val<<=8;
			val+=com_rbuf[6+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_KW,val/100.0,&arg_n);

			sprintf(polcode,"e300%02d",i+4);
			val=com_rbuf[9+i*8];
			val<<=8;
			val+=com_rbuf[10+i*8];
			val<<=8;
			val+=com_rbuf[7+i*8];
			val<<=8;
			val+=com_rbuf[8+i*8];
			acqdata_set_value(acq_data,polcode,UNIT_KWH,val/100.0,&arg_n);
		}

		val=com_rbuf[35];
		val<<=8;
		val+=com_rbuf[36];
		acqdata_set_value(acq_data,"e70202",UNIT_A,val/100.0,&arg_n);

		val=com_rbuf[37];
		val<<=8;
		val+=com_rbuf[38];
		acqdata_set_value(acq_data,"e70102",UNIT_KW,val/100.0,&arg_n);

		val=com_rbuf[41];
		val<<=8;
		val+=com_rbuf[42];
		val<<=8;
		val+=com_rbuf[39];
		val<<=8;
		val+=com_rbuf[40];
		acqdata_set_value(acq_data,"e70002",UNIT_KWH,val/100.0,&arg_n);
	}
	else
	{
		for(i=0;i<4;i++)
		{
			sprintf(polcode,"e302%02d",i+4);
			acqdata_set_value(acq_data,polcode,UNIT_A,0,&arg_n);
			sprintf(polcode,"e301%02d",i+4);
			acqdata_set_value(acq_data,polcode,UNIT_KW,0,&arg_n);
			sprintf(polcode,"e300%02d",i+4);
			acqdata_set_value(acq_data,polcode,UNIT_KWH,0,&arg_n);
		}
		acqdata_set_value(acq_data,"e70202",UNIT_A,0,&arg_n);
		acqdata_set_value(acq_data,"e70102",UNIT_KW,0,&arg_n);
		acqdata_set_value(acq_data,"e70002",UNIT_KWH,0,&arg_n);
		
		status=1;
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


