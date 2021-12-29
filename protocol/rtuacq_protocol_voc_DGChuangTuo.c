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

int protocol_VOCs_DGChuangTuo(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float speed=0,temp=0,humi=0,pres=0,voc=0,flow=0;
	float eq1,eq2,eq3,eq4,eq5,pt1,pt2,pt3,pt4,pt5;
	int val;
	int ret=0;
	int arg_n=0;
	char *p;
	char head[4];
	int i=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	head[0]=modbusarg->devaddr&0xffff;
	head[1]=modbusarg->devfun&0xffff;
	head[2]=(modbusarg->regcnt&0xffff)*2;
	head[3]=0x00;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,modbusarg->devaddr&0xffff,modbusarg->devfun&0xffff,modbusarg->devstaddr&0xffff,modbusarg->regcnt&0xffff);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DGChuangTuo voc SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(5);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DGChuangTuo voc protocol,voc : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DGChuangTuo voc data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DGChuangTuo voc RECV:",com_rbuf,size);

	p=memstr(com_rbuf,size,head);
	if((size>=(5+(modbusarg->regcnt&0xffff)*2))&&(p!=NULL))
	{
		f.ch[0]=p[6];
		f.ch[1]=p[5];
		f.ch[2]=p[4];
		f.ch[3]=p[3];
		voc=f.f;

		f.ch[0]=p[10];
		f.ch[1]=p[9];
		f.ch[2]=p[8];
		f.ch[3]=p[7];
		temp=f.f;

		f.ch[0]=p[14];
		f.ch[1]=p[13];
		f.ch[2]=p[12];
		f.ch[3]=p[11];
		humi=f.f;

		f.ch[0]=p[18];
		f.ch[1]=p[17];
		f.ch[2]=p[16];
		f.ch[3]=p[15];
		speed=f.f;

		f.ch[0]=p[22];
		f.ch[1]=p[21];
		f.ch[2]=p[20];
		f.ch[3]=p[19];
		pres=f.f;

		f.ch[0]=p[26];
		f.ch[1]=p[25];
		f.ch[2]=p[24];
		f.ch[3]=p[23];
		eq1=f.f;

		f.ch[0]=p[30];
		f.ch[1]=p[29];
		f.ch[2]=p[28];
		f.ch[3]=p[27];
		eq2=f.f;

		f.ch[0]=p[34];
		f.ch[1]=p[33];
		f.ch[2]=p[32];
		f.ch[3]=p[31];
		eq3=f.f;

		f.ch[0]=p[38];
		f.ch[1]=p[37];
		f.ch[2]=p[36];
		f.ch[3]=p[35];
		eq4=f.f;

		f.ch[0]=p[42];
		f.ch[1]=p[41];
		f.ch[2]=p[40];
		f.ch[3]=p[39];
		eq5=f.f;

		f.ch[0]=p[46];
		f.ch[1]=p[45];
		f.ch[2]=p[44];
		f.ch[3]=p[43];
		pt1=f.f;

		f.ch[0]=p[50];
		f.ch[1]=p[49];
		f.ch[2]=p[48];
		f.ch[3]=p[47];
		pt2=f.f;

		f.ch[0]=p[54];
		f.ch[1]=p[53];
		f.ch[2]=p[52];
		f.ch[3]=p[51];
		pt3=f.f;

		f.ch[0]=p[58];
		f.ch[1]=p[57];
		f.ch[2]=p[56];
		f.ch[3]=p[55];
		pt4=f.f;

		f.ch[0]=p[62];
		f.ch[1]=p[61];
		f.ch[2]=p[60];
		f.ch[3]=p[59];
		pt5=f.f;

		f.ch[0]=p[66];
		f.ch[1]=p[65];
		f.ch[2]=p[64];
		f.ch[3]=p[63];
		flow=f.f;
		
		status=0;
	}
	else
	{
		speed=0;
		temp=0;
		humi=0;
		pres=0;
		voc=0;
		status=1;
	}

	acqdata_set_value(acq_data,"g20201",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"g28001",UNIT_0C,temp,&arg_n);
	acqdata_set_value(acq_data,"g28101",UNIT_PECENT,humi,&arg_n);
	acqdata_set_value(acq_data,"g28201",UNIT_PA,pres,&arg_n);
	acqdata_set_value(acq_data,"g29001",UNIT_MG_M3,voc,&arg_n);
	acqdata_set_value(acq_data,"g20101",UNIT_M3_S,flow,&arg_n);
	acqdata_set_value(acq_data,"e60201",UNIT_A,eq1,&arg_n);
	acqdata_set_value(acq_data,"e70201",UNIT_A,eq1,&arg_n);
	acqdata_set_value(acq_data,"e80201",UNIT_A,eq1,&arg_n);
	acqdata_set_value(acq_data,"e30101",UNIT_KW,pt1,&arg_n);
	acqdata_set_value(acq_data,"e60202",UNIT_A,eq2,&arg_n);
	acqdata_set_value(acq_data,"e70202",UNIT_A,eq2,&arg_n);
	acqdata_set_value(acq_data,"e80202",UNIT_A,eq2,&arg_n);
	acqdata_set_value(acq_data,"e30102",UNIT_KW,pt2,&arg_n);
	acqdata_set_value(acq_data,"e60203",UNIT_A,eq3,&arg_n);
	acqdata_set_value(acq_data,"e70203",UNIT_A,eq3,&arg_n);
	acqdata_set_value(acq_data,"e80203",UNIT_A,eq3,&arg_n);
	acqdata_set_value(acq_data,"e30103",UNIT_KW,pt3,&arg_n);
	acqdata_set_value(acq_data,"e60204",UNIT_A,eq4,&arg_n);
	acqdata_set_value(acq_data,"e70204",UNIT_A,eq4,&arg_n);
	acqdata_set_value(acq_data,"e80204",UNIT_A,eq4,&arg_n);
	acqdata_set_value(acq_data,"e30104",UNIT_KW,pt4,&arg_n);
	acqdata_set_value(acq_data,"e60205",UNIT_A,eq5,&arg_n);
	acqdata_set_value(acq_data,"e70205",UNIT_A,eq5,&arg_n);
	acqdata_set_value(acq_data,"e80205",UNIT_A,eq5,&arg_n);
	acqdata_set_value(acq_data,"e30105",UNIT_KW,pt5,&arg_n);
	acqdata_set_value(acq_data,"g20202",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"g28002",UNIT_0C,temp,&arg_n);
	acqdata_set_value(acq_data,"g28102",UNIT_PECENT,humi,&arg_n);
	acqdata_set_value(acq_data,"g28202",UNIT_PA,pres,&arg_n);
	acqdata_set_value(acq_data,"g29002",UNIT_MG_M3,voc,&arg_n);
	acqdata_set_value(acq_data,"g20102",UNIT_M3_S,flow,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 30);
	return arg_n;
}



