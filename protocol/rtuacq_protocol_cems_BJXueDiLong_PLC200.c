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

static float setValue_TSP_BJXueDiLong_200(struct modbus_arg *modbusarg, int val, char *polcode)
{
	struct modbus_polcode_arg *mpolcodearg;
	int i=0;
	float valf=0;

	if(!modbusarg || !polcode) return -1;

	for(i=0;i<modbusarg->array_count ;i++)
	{
		mpolcodearg=&modbusarg->polcode_arg[i];
		if(!strcmp(mpolcodearg->polcode,polcode))
		{
			if(val<6400) return mpolcodearg->alarmMin;
		
			valf=(val-6400.0)/(32000-6400);
			valf*=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			valf+=mpolcodearg->alarmMin;
			return valf;
		}

	}

	return 0;
}

int protocol_PLC_BJXueDiLong_200(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf[6];
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float speed,atm_press,PTC;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	rdtuinfo=get_parent_rdtu_info(acq_data);
	devaddr=modbusarg->devaddr&0xffff;
	PTC=rdtuinfo->PTC;
	atm_press=rdtuinfo->atm_press*1000;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x09,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong 200 PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong 200 PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong 200 PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong 200 PLC RECV:",com_rbuf,size);
	if((size>=17)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf[0]=setValue_TSP_BJXueDiLong_200(modbusarg,val,"a01014");
#if 0
		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		valf[1]=setValue_TSP_BJXueDiLong_200(modbusarg,val,"a34013");
#endif
		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		valf[2]=setValue_TSP_BJXueDiLong_200(modbusarg,val,"a01011");

		val=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		valf[3]=setValue_TSP_BJXueDiLong_200(modbusarg,val,"a01012");

		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		valf[4]=setValue_TSP_BJXueDiLong_200(modbusarg,val,"a01013");

		val=com_rbuf[13];
		val<<=8;
		val+=com_rbuf[14];
		valf[5]=setValue_TSP_BJXueDiLong_200(modbusarg,val,"a21001");

		if(PTC>0 && (valf[3]+273)>0 && (valf[4]+atm_press)>0)
			speed=PTC*valf[2]*sqrt(((valf[3]+273)/273)*(101325/(valf[4]+atm_press))*(2/1.2928));
		else
			speed=0;
		
		status=0;
	}
	else
	{
		valf[0]=0;
		valf[1]=0;
		speed=0;
		valf[3]=0;
		valf[4]=0;
		valf[5]=0;
		status=1;
	}
	
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[0],&arg_n);
	//acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf[1],&arg_n); //del by miles zhang conflict with 8009 TL_PMM180
	//acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);//del by miles zhang conflict with 8009 TL_PMM180
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[3],&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[4],&arg_n);
	acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a21001a",UNIT_MG_M3,valf[5],&arg_n);
	acqdata_set_value(acq_data,"a21001",UNIT_MG_M3,valf[5],&arg_n);
	acqdata_set_value(acq_data,"a21001z",UNIT_MG_M3,0,&arg_n);

    if(status==0)
    {
		memset(com_tbuf,0,sizeof(com_tbuf));
		size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x01);
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong2 200 PLC SEND:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data),com_tbuf,size);
		sleep(3);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
		SYSLOG_DBG("BJXueDiLong2 200 PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("BJXueDiLong2 200 PLC data",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong2 200 PLC RECV:",com_rbuf,size);	

		if((size>=5)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
		{
			if(com_rbuf[3]&0x1)acq_data->dev_stat='M';
			if(com_rbuf[3]&(0x1<<1))acq_data->dev_stat='z';
			if(com_rbuf[3]&(0x1<<2))acq_data->dev_stat='C';
			if(com_rbuf[3]&(0x1<<3))acq_data->dev_stat='C';
			if(com_rbuf[3]&(0x1<<4))acq_data->dev_stat='M';
			if(com_rbuf[3]&(0x1<<5))acq_data->dev_stat='C';
			if(com_rbuf[3]&(0x1<<6))acq_data->dev_stat='D';
			if(com_rbuf[3]&(0x1<<7))acq_data->dev_stat='N';
		}
    }
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

