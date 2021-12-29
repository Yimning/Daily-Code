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

int protocol_TSP_HBManDeKe_PLC(struct acquisition_data *acq_data)
{
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
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	
	return arg_n;
}