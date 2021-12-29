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
#include <time.h>s
#include <sys/time.h>
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:2021.07.06 Tues.
Description:protocol_PLC_CEMS_ZhuoMeiKeJi
TX:01 04 00 00 00 08 F1 CC
RX:01 04 10 
67 71 
FF FE 
00 02 
00 01 
00 01 
00 01 
00 01 
00 01 
9C BA
DataType and Analysis:
	(int) 00 01 = 1
*/
int protocol_PLC_CEMS_ZhuoMeiKeJi(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	int val;
	float valf2;
	float valf[5];
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
	size=modbus_pack(com_tbuf,devaddr,0x04,0x00,0x08);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"protocol_PLC_CEMS_ZhuoMeiKeJi PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("protocol_PLC_CEMS_ZhuoMeiKeJi PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("protocol_PLC_CEMS_ZhuoMeiKeJi PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"protocol_PLC_CEMS_ZhuoMeiKeJi PLC RECV:",com_rbuf,size);
	if((size>=21)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
	
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		//SO2 value
		valf2=PLCtoValue(modbusarg, 0, 32768, val,"a21026");
		acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,valf2,&arg_n);
		acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,valf2,&arg_n);
		acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,valf2,&arg_n);

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		//NO value
		valf2=PLCtoValue(modbusarg, 0, 32768, val,"a21003");
		acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,valf2,&arg_n);
		valf2=NO_to_NOx(valf2);
		acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,valf2,&arg_n);
		acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,valf2,&arg_n);
		acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,valf2,&arg_n);

		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		//O2 value
		valf2=PLCtoValue(modbusarg, 0, 32768, val,"a19001");
		acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,valf2,&arg_n);
		acqdata_set_value(acq_data,"a19001",UNIT_PECENT,valf2,&arg_n);

		val=com_rbuf[9];
		val<<=8;
		val+=com_rbuf[10];
		//smoke dust value  //flue gas original value
		valf2=PLCtoValue(modbusarg, 0, 32768, val,"a34013");
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,valf2,&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf2,&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,valf2,&arg_n);


		//flue gas velocity
		val=com_rbuf[11];
		val<<=8;
		val+=com_rbuf[12];
		valf[2]=PLCtoValue(modbusarg, 0, 32768, val,"a01011");

		val=com_rbuf[13];
		val<<=8;
		val+=com_rbuf[14];
		//flue gas temperature
		valf[0]=PLCtoValue(modbusarg, 0, 32768, val,"a01012");

		
		//flue gas static pressure
		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		valf[1]=PLCtoValue(modbusarg, 0, 32768, val,"a01013");


		//flue gas humidity
		val=com_rbuf[17];
		val<<=8;
		val+=com_rbuf[18];
		valf[3]=PLCtoValue(modbusarg, 0, 32768, val,"a01014");


		if(PTC>0 && (valf[0]+273)>0 && (valf[1]+atm_press)>0)
			speed=PTC*valf[2]*sqrt(((valf[0]+273)/273)*(101325/(valf[1]+atm_press))*(2/1.2928));
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
		status=1;

		acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,0,&arg_n);
		acqdata_set_value(acq_data,"a19001",UNIT_PECENT,0,&arg_n);
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	}
	
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf[0],&arg_n);

	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf[1],&arg_n);
	
	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf[3],&arg_n);


	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


