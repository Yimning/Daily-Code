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


int protocol_FLUX_AnJun(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float speed=0,total_flux_M3=0;
   int temp_s=0,temp_f=0,multiple=1;
   UNIT_DECLARATION funit_s,funit_f;
   char point=0,unit=0;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,0x01,0x04,0x0A,0x04);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   SYSLOG_DBG("FLUX_AnJun protocol,FLUX : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG_HEX("FLUX_AnJun",com_rbuf,size);
   
	if((size>=13)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x04))
		{
		temp_s=com_rbuf[3]*256+com_rbuf[4];
		point=(com_rbuf[5]>>4)&0x07;
		switch(point){
			case 0:multiple=100000;
					break;
			case 1:multiple=10000;
					break;
			case 2:multiple=1000;
					break;
			case 3:multiple=100;
					break;
			case 4:multiple=10;
					break;
			case 5:multiple=1;
					break;
			default:return -1;
			}
		speed=(float)temp_s/multiple;
		/*if(com_rbuf[5]&0x80!=0)
			{
			temp_s=temp_s*(-1);
		}
		else
			{
			temp_s=temp_s;
		}*/
		unit=com_rbuf[5]&0x07;
		switch(unit){
			case 0:funit_s=UNIT_L_S;
				break;
			case 1:funit_s=UNIT_L_M;
				break;
			case 2:funit_s=UNIT_L_H;
				break;
			case 3:funit_s=UNIT_M3_S;
				break;
			case 4:funit_s=UNIT_M3_M;
				break;
			case 5:funit_s=UNIT_M3_H;
				break;
			case 6:funit_s=UNIT_T_S;
				break;
			case 7:funit_s=UNIT_T_M;
				break;
			case 8:funit_s=UNIT_T_H;
				break;
			default:return -1;	
		}
		point=0;
		unit=0;
		multiple=1;
		temp_f=(com_rbuf[6]*256+com_rbuf[7])*256+com_rbuf[8];
		point=com_rbuf[10]%4;
		switch(point)
			{
			case 0:multiple=1000;
				break;
			case 1:multiple=100;
				break;
			case 2:multiple=10;
				break;
			case 3:multiple=1;
				break;
			default:return -1;		
		}
		total_flux_M3=(float)temp_f/multiple;
		unit=com_rbuf[10]/4;
		switch(unit){
			case 0:funit_f=UNIT_L;
				break;
			case 1:funit_f=UNIT_M3;
				break;
			case 2:funit_f=UNIT_T;
				break;
			default:return -1;	
		}
		acqdata_set_value(acq_data,"w00000",funit_s,speed,&arg_n);
		acqdata_set_value(acq_data,"w00001",funit_f,total_flux_M3,&arg_n);
		printf("speed=%f,total=%f\n",speed,total_flux_M3);
		status=0;

		acq_data->total=total_flux_M3;
	}
	else
		{
		status=1;
		speed=0;
		total_flux_M3=0;
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);	
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	}
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

