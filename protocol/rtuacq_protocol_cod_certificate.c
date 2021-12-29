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
//#include "devices_mng_api.h"

/*
return : com_f arguments amount;
*/
int protocol_COD_Certificate(struct acquisition_data *acq_data)
{
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0x01,0x03,0x00,0x00,0x00,0x14,0x45,0xc5};
   int size=8;
   unsigned int value;
   
   int arg_n=0;
   int status=0;
    
   if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x3,0x0000,0x0014);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
   if(size!=8)
   {
   		printf("write dev %s size=%d error ,not 8\n",DEV_NAME(acq_data),size);
		return 0;
   }
   sleep(1);

   	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("certificate COD protocol,read device %s , size=%d\n",DEV_NAME(acq_data),size);
	//SYSLOG_DBG("data:%s\n",com_rbuf);
	print_hex(com_rbuf,size);
    
   if(size>=45)
   {       
	    char valch[4]={0};
	    float fval=0;   
   		int acqtime[TIME_ARRAY]={0};
		int i=0;
		unsigned short workstat;
		float fval_range=0;
		float ftemperature=0;
		short timepass=0;
		float farea=0;
		float fkk=0;
		float flimit=0;
		
   	    //fval=27.64;
		//memcpy(valch,&fval,4);
		//print_hex(valch,4);
		
   		valch[0]=com_rbuf[4];
		valch[1]=com_rbuf[3];
		valch[2]=com_rbuf[6];
		valch[3]=com_rbuf[5];

		memcpy(&fval,valch,4);
		printf("COD fval=%f\n",fval);
		acqdata_set_value(acq_data,"w01018",UNIT_MG_L,(float)(fval),&arg_n);

		for(i=0;i<6;i++)
		{
			acqtime[5-i]=com_rbuf[7+i*2];
			acqtime[5-i]<<=8;
			acqtime[5-i]+=com_rbuf[8+i*2];
		}
		printf("sample time : %s\n",time_format(acqtime));
		memcpy(acq_data->acq_tm,acqtime,TIME_LEN);

		workstat=com_rbuf[19];
		workstat<<=8;
		workstat=com_rbuf[20];

		valch[0]=com_rbuf[22];
		valch[1]=com_rbuf[21];
		valch[2]=com_rbuf[24];
		valch[3]=com_rbuf[23];
		memcpy(&fval_range,valch,4);

		valch[0]=com_rbuf[26];
		valch[1]=com_rbuf[25];
		valch[2]=com_rbuf[28];
		valch[3]=com_rbuf[27];
		memcpy(&ftemperature,valch,4);

		timepass=com_rbuf[29];
		timepass<<=8;
		timepass=com_rbuf[30];

		valch[0]=com_rbuf[32];
		valch[1]=com_rbuf[31];
		valch[2]=com_rbuf[34];
		valch[3]=com_rbuf[33];
		memcpy(&farea,valch,4);

		valch[0]=com_rbuf[36];
		valch[1]=com_rbuf[35];
		valch[2]=com_rbuf[38];
		valch[3]=com_rbuf[37];
		memcpy(&fkk,valch,4);		


		valch[0]=com_rbuf[40];
		valch[1]=com_rbuf[39];
		valch[2]=com_rbuf[42];
		valch[3]=com_rbuf[41];
		memcpy(&flimit,valch,4);	

		printf("workstat=%d,fval_range=%f,ftemperature=%f,timepass=%d,farea=%f,fkk=%f,flimit=%f",
			workstat,fval_range,ftemperature,timepass,farea,fkk,flimit);
		
    }
    else
    {
		acqdata_set_value(acq_data,"w01018",UNIT_MG_L,0.0,&arg_n);
		read_system_time(acq_data->acq_tm); 
		status = 1;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}


