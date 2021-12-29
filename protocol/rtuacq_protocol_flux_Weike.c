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

static float total_flux_M3;

int protocol_FLUX_Weike(struct acquisition_data *acq_data)
{
    int status=0;
    char com_rbuf[2048]; 
    char modpack[8];
    int size=0;
    float flux=0.0;
	UNIT_DECLARATION funit;
	float multiple;
	char unit,point;
    int arg_n=0;
    
    if(!acq_data) return -1;
#if 1

    size=modbus_pack(modpack,0x01,0x04,0x10,0x04);
    size=write_device(DEV_NAME(acq_data),modpack,size);
	if(DEBUG_PRINT_VALUE)
		print_hex(modpack,size);
    sleep(2);
	

    memset(com_rbuf,0,sizeof(com_rbuf));
    size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
    SYSLOG_DBG("DLfulang FLUX protocol,FLUX : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG("data:%s\n",com_rbuf);
    if(DEBUG_PRINT_VALUE)
		print_hex(com_rbuf,size);
    
    if( (size >= 13)&&(com_rbuf[0] == 0x01)&&(com_rbuf[1] == 0x04))
    {
                
		//f.f=0.1;
		flux = com_rbuf[3]*256+com_rbuf[4];
		point=(com_rbuf[5]>>4)&0x7;
		switch(point){
			case 0:
				multiple=100000;
				break;
			case 1:
				multiple=10000;
				break;
			case 2:
				multiple=1000;
				break;
			case 3:
				multiple=100;
				break;
			case 4:
				multiple=10;
				break;
			case 5:
				multiple=1;
				break;
			default :
				multiple=1;
		}
		flux=flux/multiple;
		
		unit=com_rbuf[5]&0x7;
		switch(unit){
			case 0:
				funit=UNIT_L_S;
				break;
			case 1:
				funit=UNIT_L_M;
				break;
			case 2:
				funit=UNIT_L_H;
				break;
			case 3:
				funit=UNIT_M3_S;
				break;
			case 4:
				funit=UNIT_M3_M;
				break;
			case 5:
				funit=UNIT_M3_H;
				break;
			case 6:
				funit=UNIT_T_S;
				break;
			case 7:
				funit=UNIT_T_M;
				break;
			case 8:
				funit=UNIT_T_H;
				break;
			default:
				funit=UNIT_L_S;
		}
		if(DEBUG_PRINT_VALUE) printf("flux=%f,%d,%f\n",flux,funit,multiple);
		acqdata_set_value(acq_data,"w00000",funit,(float)(flux),&arg_n);
		//total_flux+=f.f;
		total_flux_M3 = com_rbuf[6]*256*256*256+com_rbuf[7]*256*256+com_rbuf[8]*256+com_rbuf[9];		
		switch(com_rbuf[10]){
			case 0:
				multiple=1000;
				funit=UNIT_L;
				break;
			case 1:
				multiple=100;
				funit=UNIT_L;
				break;
			case 2:
				multiple=10;
				funit=UNIT_L;
				break;
			case 3:
				multiple=1;
				funit=UNIT_L;
				break;
			case 4:
				multiple=1000;
				funit=UNIT_M3;
				break;
			case 5:
				multiple=100;
				funit=UNIT_M3;
				break;
			case 6:
				multiple=10;
				funit=UNIT_M3;
				break;
			case 7:
				multiple=1;
				funit=UNIT_M3;
				break;
			case 8:
				multiple=1000;
				funit=UNIT_T;
				break;
			case 9:
				multiple=100;
				funit=UNIT_T;
				break;	
			case 10:
				multiple=10;
				funit=UNIT_T;
				break;	
			case 11:
				multiple=1;
				funit=UNIT_T;
				break;					
			default:
				multiple=1;
				funit=UNIT_M3;
		}
		
		total_flux_M3 = total_flux_M3/multiple;
		if(DEBUG_PRINT_VALUE) printf("total_flux=%f,%d,%f\n",total_flux_M3,funit,multiple);
		acqdata_set_value(acq_data,"w00001",funit,(float)(total_flux_M3),&arg_n);

		acq_data->total=total_flux_M3;
		
		read_system_time(acq_data->acq_tm);    
    }
    else
    {
		acqdata_set_value(acq_data,"w00000",UNIT_L_S,(float)(0.0),&arg_n);	
		acqdata_set_value(acq_data,"w00001",UNIT_M3,(float)(0.0),&arg_n);
		status = 1;
    }

	if(status == 0)
 		acq_data->acq_status = ACQ_OK;
	else 
   		acq_data->acq_status = ACQ_ERR;
#endif
 	return arg_n;
}





