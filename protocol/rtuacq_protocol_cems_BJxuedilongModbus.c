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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"
//#include "devices_mng_api.h"

/*
return : com_f arguments amount;
*/
int protocol_CEMS_BJxuedilongModbus(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	int val1,val2,val3,val4,val5,val6,val7,val8,val;
	float valf1,valf2,valf3,valf4,valf5,valf6,valf7,valf8;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 0x0FA0, 0x09);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJxuedilong CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("BJxuedilong CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJxuedilong CEMS data ",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJxuedilong CEMS RECV:",com_rbuf,size);
	
	if((size>=23)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val1=com_rbuf[3];
		val1<<=8;
		val1+=com_rbuf[4];
		valf1=(val1-5530)/22118.0*750;

		val2=com_rbuf[5];
		val2<<=8;
		val2+=com_rbuf[6];
		valf2=(val2-5530)/22118.0*750;

		val3=com_rbuf[9];
		val3<<=8;
		val3+=com_rbuf[10];
		valf3=(val3-5530)/22118.0*25;

		val4=com_rbuf[11];
		val4<<=8;
		val4+=com_rbuf[12];
		valf4=(val4-5530)/22118.0*40;

		val5=com_rbuf[13];
		val5<<=8;
		val5+=com_rbuf[14];
		valf5=(val5-5530)/22118.0*100;

		val6=com_rbuf[15];
		val6<<=8;
		val6+=com_rbuf[16];
		valf6=(val6-5530)/22118.0*40;

		val7=com_rbuf[17];
		val7<<=8;
		val7+=com_rbuf[18];
		valf7=(val7-5530)/22118.0*300;

		val8=com_rbuf[19];
		val8<<=8;
		val8+=com_rbuf[20];
		valf8=(val8-5530)/22118.0*10000-5000;

		valf2*=1.53;
		valf5=valf5*101325*(273+valf7)/(101325-valf8)/273/(1-valf4/100);;
		//valf6=0.83*0.076*sqrt(valf6*(273+valf7));
		status=0;
	}
	else
	{
		valf1=0;
		valf2=0;
		valf3=0;
		valf4=0;
		valf5=0;
		valf6=0;
		valf7=0;
		valf8=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03, 0x0F9B, 0x01);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJxuedilong status SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("BJxuedilong status protocol,status : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJxuedilong status data ",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJxuedilong status RECV:",com_rbuf,size);
	
	if((size>=7)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];

		if((val&0x0001)==0x0001) acq_data->dev_stat='N';
		if((val&0x0002)==0x0002) acq_data->dev_stat='D';
		if((val&0x0004)==0x0004) acq_data->dev_stat='C';
		if((val&0x0008)==0x0008) acq_data->dev_stat='M';
		if((val&0x0200)==0x0200) acq_data->dev_stat='d';
	}
	
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,valf1,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,valf2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,valf3,&arg_n);
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf4,&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf5,&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_PECENT,valf6,&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf7,&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf8,&arg_n);

	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_H,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_H,0,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

