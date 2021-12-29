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

/*
曼德克PLCPLC通讯协议(Modbus_Rtu)(1).docx
*/

int protocol_TSP_HBManDeKe_PLC_S7_200(struct acquisition_data *acq_data)
{
/*//miles zhang 20210712
//9600	8	1	奇校验（ODD）	无	01H
序号	寄存器地址	数据类型	字节长度	读写	单位	量程	说明
1	40001	Int	2	读	℃	0-300	烟气温度
2	40002	Int	2	读	Kpa	-10-10	烟气压力（静压）
3	40003	Int	2	读	m/s	0-40	流速
4	40004	Int	2	读	%	0-40	湿度
5	40005	Int 	2	读	mg/m3	0-100	颗粒物
6	00006	Binary	1	读			“1”系统故障状态
7	00007	Binary	1	读			“1”系统校准状态
8	00008	Binary	1	读			“1”系统维护状态
注：西门子S7-200温度、压力、流速、湿度、颗粒物（AD量程对应于6400-32000）
西门子Smart200温度、压力、流速、湿度、颗粒物（AD量程对应于5530-27648）

例：
PLC型号为S7-200，读取到温度值为27B5，解析后数据是10165，用6400-32000，对应温度量程0-300℃。计算方法如下：
(300-0)*（10165-6400)/(32000-6400)+0=44.12℃.
*/
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf=0.0;
	int val=0,i=0,j=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

   
#define POLCODE_NUM 4
	char *polcode[POLCODE_NUM]={
		"a01012","a01013","a01011","a01014"
	};

	UNIT_DECLARATION unit[POLCODE_NUM]={
		UNIT_0C,		UNIT_KPA,	UNIT_M_S,	UNIT_PECENT,	UNIT_MG_M3
	};

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg->polcode_arg;

	devaddr=modbusarg->devaddr&0xffff;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe PLC SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe PLC protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe PLC data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe PLC RECV:",com_rbuf,size);
	if((size>=15)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
			val=com_rbuf[3+i*2];
			val<<=8;
			val+=com_rbuf[4+i*2];

			for(j=0;j<modbusarg->array_count;j++)
			{
				mpolcodearg=&modbusarg->polcode_arg[j];
				if(strcmp(polcode[i],mpolcodearg->polcode))
					continue;	//polcode[i]  !=  mpolcodearg->polcode
                val=(val<6400) ? 6400 : val;
				valf=(val-6400)*(mpolcodearg->alarmMax-mpolcodearg->alarmMin)/(32000-6400)+mpolcodearg->alarmMin;
				break;
			}

	        mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,polcode[i]);
	        if(mpolcodearg_temp!=NULL &&
				mpolcodearg_temp->enableFlag==1)
	        {
	        	acqdata_set_value(acq_data,polcode[i],unit[i],valf,&arg_n);

				if(!strcmp(polcode[i],"a01011"))
				{
					acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
					acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);			
				}				
	        }
		}
		
		status=0;
	}
	else
	{
		for(i=0;i<POLCODE_NUM;i++)
		{
	        mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(modbusarg->polcode_arg,modbusarg->array_count,polcode[i]);
	        if(mpolcodearg_temp!=NULL &&
				mpolcodearg_temp->enableFlag==1)
	        {
	        	acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);

				if(!strcmp(polcode[i],"a01011"))
				{
					acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
					acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);			
				}				
	        }
		}
		status=1;
	}

	//TX:01 01 00 05 00 03 crc
	//RX:01 01 01 07 crc

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x01,0x05,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HBManDeKe PLC stat SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("HBManDeKe PLC stat protocol,PLC : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HBManDeKe PLC stat data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HBManDeKe PLC stat RECV:",com_rbuf,size);

	if((size>=6)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x01))
	{
		if(com_rbuf[3]&(0x1<<2)) acq_data->dev_stat='M';
		if(com_rbuf[3]&(0x1<<1)) acq_data->dev_stat='C';
		if(com_rbuf[3]&0x1) acq_data->dev_stat='D';
		
	}
	else
	{
			status=1;
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	
	return arg_n;
}
