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


int protocol_FLUX_GuangHua(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float speed=0,total_flux_M3=0,multiple=1;
   int temp=0,point=0,unit_n=0;
   UNIT_DECLARATION funit_s,funit_f;
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x2a;
	com_tbuf[1]=0x01;
	com_tbuf[2]=0x00;
	com_tbuf[3]=0x2e;
	size=4;
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   SYSLOG_DBG("FLUX_GuangHua protocol,FLUX : read device %s , size=%d\n",DEV_NAME(acq_data),size);
   SYSLOG_DBG_HEX("FLUX_GuangHua",com_rbuf,size);
	
	if((size>=10)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x00))
	{
		temp=(com_rbuf[4]>>4)*10+(com_rbuf[4]&0x0f);
		speed=(speed+temp)*100;
		//printf("temp=%d,speed=%f\n",temp,speed);
		temp=(com_rbuf[3]>>4)*10+(com_rbuf[3]&0x0f);
		speed=(speed+temp)*100;
		temp=(com_rbuf[2]>>4)*10+(com_rbuf[2]&0x0f);
		speed=speed+temp;
		point=com_rbuf[5]-5;
		//printf("temp=%d,speed=%f\n,point=%d",temp,speed,point);
		switch(point){
			case -5:multiple=1/100000.0;
				break;
			case -4:multiple=1/10000.0;
				break;
			case -3:multiple=1/1000.0;
				break;
			case -2:multiple=1/100.0;
				break;
			case -1:multiple=1/10.0;
				break;
			case 0:multiple=1.0;
				break;
			case 1:multiple=10;
				break;
			case 2:multiple=100;
				break;
			case 3:multiple=1000;
				break;
			case 4:multiple=10000;
				break;
			case 5:multiple=100000;
				break;
			default:return -1;	
		}
		//printf(",speed=%f,multiple=%f\n",speed,multiple);
		speed=speed*multiple;
		//printf(",speed=%f,multiple=%f\n",speed,multiple);
		unit_n=com_rbuf[6];
		switch(unit_n){
			case 0:funit_s=UNIT_M3_S;
				break;
			case 1:funit_s=UNIT_M3_M;
				break;
			case 2:funit_s=UNIT_M3_H;
				break;
			/*case 3:funit_s=UNIT_M3_D;//UNIT_M3_H isn't exist
				break;*/
			case 4:funit_s=UNIT_L_S;
				break;
			case 5:funit_s=UNIT_L_M;
				break;
			case 6:funit_s=UNIT_L_H;
				break;
			/*case 7:funit_s=UNIT_L_D;//UNIT_L_D isn't exist
				break;*/
			case 8:funit_s=UNIT_T_S;
				break;
			case 9:funit_s=UNIT_T_M;
				break;
			case 10:funit_s=UNIT_T_H;
				break;
			/*case 11:funit_s=UNIT_T_D;//UNIT_XX_XX  isn't exist
				break;
			case 12:funit_s=UNIT_KG_S;
				break;
			case 13:funit_s=UNIT_KG_M;
				break;
			case 14:funit_s=UNIT_KG_H;
				break;
			case 15:funit_s=UNIT_KG_D:
				break;*/
			default:return -1;
		}
		/*the floating direction
		if((com_rbuf[7]&0x01)==1)
		{
			speed=speed*(-1);
		}
		*/
		status=0;
		printf("speed=%f\n",speed);
	}
	else{
		status=1;
		speed=0;
		funit_s=UNIT_L_S;
	}
	//printf("status_1=%d\n",status);
	
	sleep(1);
	if(status==0)
	{
		multiple=1;
		memset(com_tbuf,0,sizeof(com_tbuf));
		com_tbuf[0]=0x2a;
		com_tbuf[1]=0x01;
		com_tbuf[2]=0x04;
		com_tbuf[3]=0x2e;
		size=4;
		size=write_device(DEV_NAME(acq_data), com_tbuf, size);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
		if((size>=10)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x04))
			{
			temp=(com_rbuf[6]<<4)*10+(com_rbuf[6]&0x0f);
			total_flux_M3=(total_flux_M3+temp)*100;
			temp=(com_rbuf[5]<<4)*10+(com_rbuf[5]&0x0f);
			total_flux_M3=(total_flux_M3+temp)*100;
			temp=(com_rbuf[4]<<4)*10+(com_rbuf[4]&0x0f);
			total_flux_M3=(total_flux_M3+temp)*100;
			temp=(com_rbuf[3]<<4)*10+(com_rbuf[3]&0x0f);
			total_flux_M3=(total_flux_M3+temp)*100;
			temp=(com_rbuf[2]<<4)*10+(com_rbuf[2]&0x0f);
			total_flux_M3=(total_flux_M3+temp);
			point=com_rbuf[7]%4;
			printf("temp=%d,total_flux=%f\n",temp,total_flux_M3);
			switch(point)
			{
				case 0:multiple=1/1000.0;
					break;
				case 1:multiple=1/100.0;
					break;
				case 2:multiple=1/10.0;
					break;
				case 3:multiple=1;
					break;
				default:return -2;
			}
			//printf("point=%d,total_flux=%f,multiple=%f\n",point,total_flux,multiple);
			total_flux_M3=total_flux_M3*multiple;
			//printf("point=%d,total_flux=%f,multiple=%f\n",point,total_flux,multiple);
			unit_n=com_rbuf[7]/4;
			switch(unit_n)
			{
				case 0:funit_f=UNIT_L;
					break;
				case 1:funit_f=UNIT_M3;
					break;
				case 2:funit_f=UNIT_KG;
					break;
				case 3:funit_f=UNIT_T;
					break;
				default:return -2;
			}
		
			printf("total_flux=%f\n",total_flux_M3);
			status=0;	

			acq_data->total=total_flux_M3;
		}
		else
		{
			status=1;
			total_flux_M3=0;
			speed=0;
			funit_s=UNIT_L_S;
			funit_f=UNIT_M3;
		}
		
		//printf("speed=%f\n,status=%d\n",speed,status);
		acqdata_set_value(acq_data,"w00000",funit_s,speed,&arg_n);
		acqdata_set_value(acq_data,"w00001",funit_f,total_flux_M3,&arg_n);
		printf("speed=%f,total_flux=%f\n",speed,total_flux_M3);
	}
	else
	{
		status=1;
		total_flux_M3=0;
		speed=0;
		acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
		acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	}
	//printf("status_1=%d\n",status);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
    return arg_n;
}

