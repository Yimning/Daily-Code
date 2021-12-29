#include <unistd.h>
#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"



int protocol_RAINTECH_RY_FSX485(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int ret=0;
	unsigned short value;
	float valuef;

	if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x1,0x4,0x01,0x2);
	modbus_pack_print(com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("RY_FSX485 protocol,read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("data:%s\n",com_rbuf);

      modbus_pack_print(com_rbuf,size);

     if(size>=9)
     {
          value = com_rbuf[5];
          value <<=8;
	   value+=com_rbuf[6];	  
	  acqdata_set_value(acq_data,"W01",UNIT_NONE,value,&arg_n);
          value = com_rbuf[3];
          value <<=8;
	   value+=com_rbuf[4];	  
	   valuef =(float)value/10;
	  acqdata_set_value(acq_data,"WS01",UNIT_M_S,valuef,&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
			
		ret=acqdata_set_value(acq_data,"W01",UNIT_NONE,0,&arg_n);
		ret=acqdata_set_value(acq_data,"WS01",UNIT_M_S,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 
	
    return arg_n;

}
