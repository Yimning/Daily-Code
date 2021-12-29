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

int protocol_dianbiao_ZJZhengTai_DTSU666_modbus(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf=0,eq=0,pt=0;
	int ret=0;
	int arg_n=0;
	char *p;
	char head[4];
	
	int devaddr=0;
	float IrAt=0,UrAt=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;

	IrAt=modbusarg->irAt;
	UrAt=modbusarg->urAt;

	status=0;
	sleep(2);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x101E,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJZhengTai DTSU666 ImpEq SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZJZhengTai DTSU666 ImpEq protocol,ImpEq :read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZJZhengTai DTSU666 ImpEq data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJZhengTai DTSU666 ImpEq RECV:",com_rbuf,size);
	
	head[0]=devaddr;
	head[1]=0x03;
	head[2]=0x04;
	head[3]=0x00;
	p=memstr(com_rbuf,size,head);
	
	if((size>=9)&&(p!=NULL))
	{
		f.ch[0]=p[6];
		f.ch[1]=p[5];
		f.ch[2]=p[4];
		f.ch[3]=p[3];
		valf=f.f;
		eq=valf*(UrAt*0.1)*IrAt;
	}
	else
	{
		eq=0;
		status=1;
	}

	sleep(2);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x2012,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"ZJZhengTai DTSU666 Pt SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("ZJZhengTai DTSU666 Pt protocol,Pt :read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("ZJZhengTai DTSU666 Pt data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"ZJZhengTai DTSU666 Pt RECV:",com_rbuf,size);

	head[0]=devaddr;
	head[1]=0x03;
	head[2]=0x08;
	head[3]=0x00;
	p=memstr(com_rbuf,size,head);
	
	if((size>=13)&&(p!=NULL))
	{
		f.ch[0]=p[6];
		f.ch[1]=p[5];
		f.ch[2]=p[4];
		f.ch[3]=p[3];
		valf=f.f;
		pt=valf*(UrAt*0.1)*IrAt*0.1;
		pt=pt/1000;
	}
	else
	{
		pt=0;
		status=1;
	}

	eq*=10;
	pt*=10;

	acqdata_set_value(acq_data,"e30001",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30101",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30002",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30102",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30003",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30103",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e30004",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e30104",UNIT_KW,pt,&arg_n);
	acqdata_set_value(acq_data,"e70001",UNIT_KWH,eq,&arg_n);
	acqdata_set_value(acq_data,"e70101",UNIT_KW,pt,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data,20);
	return arg_n;
}



