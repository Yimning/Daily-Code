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

int protocol_others_tengentRx(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float val=0,valf=0;
	int ret=0;
	int arg_n=0;
	unsigned int crc;
	static float total_flux_M3=0;

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

	if(!acq_data) return -1;
	
   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;


   	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("tengentRx protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("tengentRx read data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tengentRx RECV:",com_rbuf,size);
	
#if 0   
#if 0
	memset(com_tbuf,0,sizeof(com_tbuf));

    com_tbuf[0] = modbusarg_running->devaddr&0xffff;
    com_tbuf[1] = modbusarg_running->devfun&0xffff;
    com_tbuf[2] = (modbusarg_running->devstaddr&0xffff) >> 8;
    com_tbuf[3] = modbusarg_running->devstaddr&0xff;
    com_tbuf[4] = (char)(modbusarg_running->regcnt >> 8)&0xff;
    com_tbuf[5] = (char)(modbusarg_running->regcnt & 0xff);
    crc = CRC16_modbus(com_tbuf, 6);
    com_tbuf[6] = (char)(crc & 0xff);
    com_tbuf[7] = (char)(crc >> 8);

	size=8;
	
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	SYSLOG_DBG("tengentTx2 protocol : send2 device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("tengentTx2 send2 data",com_tbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"tengentTx SEND2:",com_tbuf,size);
	
	sleep(1);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("tengentRx2 protocol : read2 device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("tengentRx2 read2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tengentRx RECV2:",com_rbuf,size);
	
#else
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("tengentRx protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("tengentRx read data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tengentRx RECV:",com_rbuf,size);
	
	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		//f.ch[0]=com_rbuf[3];
		//f.ch[1]=com_rbuf[4];
		//val=f.i[0];
		memcpy(&val,&com_rbuf[3],4);
		status=0;
	}
	else
	{
		val=-10;
		status=1;
	}

/*
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	valf = val+1;
	
	com_tbuf[0] = 0x01;
	com_tbuf[1] = 0x03;
	com_tbuf[2] = 0x04;
	//com_tbuf[3] = f.ch[0];
	//com_tbuf[4] = f.ch[1];
	memcpy(&com_tbuf[3],&valf,4);
	crc = CRC16_modbus(com_tbuf,7);
	com_tbuf[7] = (char)(crc >> 8);
	com_tbuf[8] = (char)(crc & 0xFF);
	
	size=9;

	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"tengentRx SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	SYSLOG_DBG("tengentRx protocol : send device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("tengentRx send data",com_tbuf,size);
*/	
#endif

    //total_flux_M3+=1;
	ret=acqdata_set_value(acq_data,"w00000",UNIT_L_S,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w21003",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w21011",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w21001",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20117",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20121",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20116",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20119",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20115",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20125",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20124",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20120",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20123",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w20122",UNIT_MG_L,val,&arg_n);
	ret=acqdata_set_value(acq_data,"w21016",UNIT_MG_L,val,&arg_n);	
#endif

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
