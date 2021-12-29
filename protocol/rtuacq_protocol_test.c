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

int protocol_test_yuiki(struct acquisition_data *acq_data)
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
	int devaddr=0;
	int i=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
#if 0
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x00,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"yuiki COD SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("yuiki COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("yuiki COD data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"yuiki COD RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[0]=com_rbuf[4];
		f.ch[1]=com_rbuf[3];
		f.ch[2]=com_rbuf[6];
		f.ch[3]=com_rbuf[5];
		total_cod=f.f;
		status=0;
	}
	else
	{
		total_cod=0;
		status=1;
	}
#endif
        memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x3,0x00,0x2);
	for(i=0;i<10;i++)
	{
	        char buff[1024]={0};
		sprintf(buff,"yuiki COD SEND%d:",(i+1));
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,buff,com_tbuf,size);
		usleep(300000);
		sprintf(buff,"yuiki COD RECV%d:",(i+1));
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,buff,com_tbuf,size);
		usleep(300000);
	}

	total_cod=rand()%10000+1000;
	//total_cod/=10;

	//ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	{
	int distat=0;
	read_device(DEV_DI3,&distat,sizeof(int));
	if(distat==1)
	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,total_cod,'N',&arg_n);
	else
        acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,total_cod,'z',&arg_n);
	}
	
        status=0;
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;


	acq_data->dev_stat=0xAA;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_test_yuiki_info(struct acquisition_data *acq_data)
{
	int status=0;
	int ret=0;
	int arg_n=0;
	float valf=0;

	time_t tm=0;

	SYSLOG_DBG("protocol_test_yuiki_info\n");

	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,0,INFOR_ARGUMENTS,&arg_n);

	valf=rand()%5;
	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,valf,INFOR_STATUS,&arg_n);
	
	//valf=rand()%2;
	//acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

	//valf=rand()%11;
	//acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,valf,INFOR_STATUS,&arg_n);
/*

	acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,500,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,20,INFOR_ARGUMENTS,&arg_n);

	time(&tm);
	acqdata_set_value_flag(acq_data,"i13101",tm,0.0,INFOR_ARGUMENTS,&arg_n);

	acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,10,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,40,INFOR_ARGUMENTS,&arg_n);

	time(&tm);
	acqdata_set_value_flag(acq_data,"i13107",tm,0.0,INFOR_ARGUMENTS,&arg_n);

	acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,50,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,60,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,30,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,70,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,200,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,100,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);

	//valf=rand()%200/10;
	acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
*/
	read_system_time(acq_data->acq_tm);

	status =0;
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_test_yuiki_info1(struct acquisition_data *acq_data)
{
	int status=0;
	int ret=0;
	int arg_n=0;
	float valf=0;

	time_t tm=0;

	SYSLOG_DBG("protocol_test_yuiki_info\n");

	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,0,INFOR_ARGUMENTS,&arg_n);
/*
	valf=rand()%5;
	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,valf,INFOR_STATUS,&arg_n);
	
	valf=rand()%2;
	acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

	valf=rand()%11;
	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,valf,INFOR_STATUS,&arg_n);


	acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,500,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,20,INFOR_ARGUMENTS,&arg_n);
*/
	//time(&tm);
	//acqdata_set_value_flag(acq_data,"i13101",tm,0.0,INFOR_ARGUMENTS,&arg_n);

	acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,10,INFOR_ARGUMENTS,&arg_n);
	//acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,40,INFOR_ARGUMENTS,&arg_n);

	//time(&tm);
	//acqdata_set_value_flag(acq_data,"i13107",tm,0.0,INFOR_ARGUMENTS,&arg_n);

/*	acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,50,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,60,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,30,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,70,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,200,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,100,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);

	//valf=rand()%200/10;
	acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
*/
	read_system_time(acq_data->acq_tm);

	status =0;
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_test_yuiki_info2(struct acquisition_data *acq_data)
{
	int status=0;
	int ret=0;
	int arg_n=0;
	float valf=0;

	time_t tm=0;

	SYSLOG_DBG("protocol_test_yuiki_info\n");

	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,0,INFOR_ARGUMENTS,&arg_n);
/*
	valf=rand()%5;
	acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,valf,INFOR_STATUS,&arg_n);
	
	valf=rand()%2;
	acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,valf,INFOR_STATUS,&arg_n);

	valf=rand()%11;
	acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,valf,INFOR_STATUS,&arg_n);


	acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,500,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,20,INFOR_ARGUMENTS,&arg_n);

	time(&tm);
	acqdata_set_value_flag(acq_data,"i13101",tm,0.0,INFOR_ARGUMENTS,&arg_n);

	acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,10,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,40,INFOR_ARGUMENTS,&arg_n);

	time(&tm);
	acqdata_set_value_flag(acq_data,"i13107",tm,0.0,INFOR_ARGUMENTS,&arg_n);
*/
/*
	acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,50,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,60,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13005",UNIT_NONE,30,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,70,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,200,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,100,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
*/
	//valf=rand()%200/10;
	acqdata_set_value_flag(acq_data,"i13003",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

	read_system_time(acq_data->acq_tm);

	status =0;
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

