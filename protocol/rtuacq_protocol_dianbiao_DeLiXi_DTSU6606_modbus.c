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

int protocol_dianbiao_DeLiXi_DTSU6606_modbus(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int val=0;
	float eq=0,pt=0,ia=0,ib=0,ic=0,ec=0;
	int ret=0;
	int arg_n=0;
	
	int devaddr=0;
	float IrAt=0,UrAt=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	status=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 EQ SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 EQ protocol,EQ :read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 EQ data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 EQ RECV:",com_rbuf,size);
	if((size==9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val<<=8;
		val+=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		eq=val*0.01;
	}
	else
	{
		eq=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0303,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 PT SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 PT protocol,PT :read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 PT data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 PT RECV:",com_rbuf,size);
	if((size==19)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		ia=val*0.01;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		ib=val*0.01;

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		ic=val*0.01;

		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		pt=val*0.01;

		ec=ia+ib+ic;
	}
	else
	{
		pt=0;
		ia=0;
		ib=0;
		ic=0;
		ec=0;
		status=1;
	}

	acqdata_set_value(acq_data,"e30001",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30101",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30002",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30102",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30003",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30103",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30004",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30104",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30201",UNIT_A,ia,&arg_n);
	acqdata_set_value(acq_data,"e30202",UNIT_A,ib,&arg_n);
	acqdata_set_value(acq_data,"e30203",UNIT_A,ic,&arg_n);
	acqdata_set_value(acq_data,"e80201",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e80202",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e80203",UNIT_A,ec,&arg_n);

	acqdata_set_value(acq_data,"eg0001",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"eg0101",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"eg0002",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"eg0102",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"eg0003",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"eg0103",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e40004",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e40104",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"ed0201",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"ec0201",UNIT_A,ec,&arg_n);

	acqdata_set_value(acq_data,"ef0001",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"ef0101",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"ef0002",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"ef0102",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e70001",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e70101",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e70201",UNIT_A,ec,&arg_n);
	acqdata_set_value(acq_data,"e70002",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e70102",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e70202",UNIT_A,ec,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,30);
	return arg_n;
}



