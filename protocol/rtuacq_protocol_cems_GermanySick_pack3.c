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

#include "rtuacq_protocol_error_cache.h"

extern int t901_val;
int t901_val_temp;

int protocol_CEMS_GermanySick_pack3(struct acquisition_data *acq_data)
{
/*

recv hex: 3e 2b 31 36 2e 34 35 34 2b 31 36 2e 30 39 31 2b 31 36 2e 38 32 38 2b 31 36 2e 36 35 33 2b 31 36 2e 32 39 35 2b 31 36 2e 38 32 36 2b 31 36 2e 36 37 30 2d 30 30 2e 30 30 31 0d
recv char:>+04.753+03.994+04.940+04.187+13.813+11.184+05.496-00.001
*/
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float valf=0; 
   float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
   float valf1,valf2,valf3,valf4,valf5,valf6,valf7,valf8;
   union uf f;
      if(!acq_data) return -1;
	  
	  ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;
	  valf1=valf2=valf3=valf4=valf5=valf6=valf7=valf8=0;
#if 0	  
	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x23;
	com_tbuf[1]=0x30;
	com_tbuf[2]=0x31;
	com_tbuf[3]=0x0d;
	size=4;
	SYSLOG_DBG_HEX("GermanySick_pack3 CEMS ",com_tbuf,4);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("GermanySick_pack3 CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("GermanySick_pack3 CEMS ",com_rbuf,size);
	
	if((size>=9)&&(com_rbuf[0]=='>'))
	{
	  //">+05.103+03.994+08.360+04.262+13.739+11.536+05.376-00.001"
	  sscanf(com_rbuf,">+%f+%f+%f+%f+%f+%f+%f-%f",&ai1,&ai2,&ai3,&ai4,&ai5,&ai6,&ai7,&ai8);	
	  if(ai1>4)valf1=(ai1-4)/16*1200; //i33312
	  if(ai2>4)valf2=(ai2-4)/16*1200; //i33313
	  if(ai3>4)valf3=(ai3-4)/16*1200; //i33311
	  if(ai4>4)valf4=(ai4-4)/16*1200; //i33322
	  if(ai5>4)valf5=(ai5-4)/16*1200; //i33323
	  if(ai6>4)valf6=(ai6-4)/16*1200; //i33321
	  if(ai7>4)valf7=(ai7-4)/16*1200; //i33330

	  SYSLOG_DBG("i33312:%f,i33313:%f,i33311:%f,i33322:%f,i33323:%f,i33321:%f,i33330:%f\n",
	  	valf1,valf2,valf3,valf4,valf5,valf6,valf7);

		ret=acqdata_set_value(acq_data,"i33312",UNIT_0C,valf1,&arg_n);// i33312 // T12
		ret=acqdata_set_value(acq_data,"i33313",UNIT_0C,valf2,&arg_n); // i33313 // T13
		ret=acqdata_set_value(acq_data,"i33311",UNIT_0C,valf3,&arg_n);// i33311 // T11
		ret=acqdata_set_value(acq_data,"i33322",UNIT_0C,valf4,&arg_n);// i33322 // T22
		ret=acqdata_set_value(acq_data,"i33323",UNIT_0C,valf5,&arg_n);// i33323 // T23
		ret=acqdata_set_value(acq_data,"i33321",UNIT_0C,valf6,&arg_n); // i33321 // T21
		ret=acqdata_set_value(acq_data,"i33330",UNIT_0C,valf7,&arg_n);// i33330 // T30		
	}
	else
	{
		status=1;
		ret=acqdata_set_value(acq_data,"i33311",UNIT_0C,0,&arg_n);// T11
		ret=acqdata_set_value(acq_data,"i33312",UNIT_0C,0,&arg_n); // T12
		ret=acqdata_set_value(acq_data,"i33313",UNIT_0C,0,&arg_n);// T13
		ret=acqdata_set_value(acq_data,"i33321",UNIT_0C,0,&arg_n);// T21
		ret=acqdata_set_value(acq_data,"i33322",UNIT_0C,0,&arg_n);// T22
		ret=acqdata_set_value(acq_data,"i33323",UNIT_0C,0,&arg_n); // T23
		ret=acqdata_set_value(acq_data,"i33330",UNIT_0C,0,&arg_n);// T30
	}

t901_val+=valf3;//977.4; //i33311
t901_val+=valf1;//942.3; //i33312
t901_val+=valf2;//943.2; //i33313
t901_val+=valf6;//0; //i33321
t901_val+=valf4;//953.1; //i33322
t901_val+=valf5;//967.58; //i33323
t901_val+=valf7;//951; //i33330

#else

/*
	ret=acqdata_set_value(acq_data,"i33311",UNIT_0C,valf,&arg_n);// T11
ret=acqdata_set_value(acq_data,"i33312",UNIT_0C,942.3,&arg_n); // T12
ret=acqdata_set_value(acq_data,"i33313",UNIT_0C,943.2,&arg_n);// T13
ret=acqdata_set_value(acq_data,"i33321",UNIT_0C,0,&arg_n);// T21
ret=acqdata_set_value(acq_data,"i33322",UNIT_0C,953.1,&arg_n);// T22
ret=acqdata_set_value(acq_data,"i33323",UNIT_0C,967.58,&arg_n); // T23
ret=acqdata_set_value(acq_data,"i33330",UNIT_0C,951,&arg_n);// T30
t901_val+=977.4; //i33311
t901_val+=942.3; //i33312
t901_val+=943.2; //i33313
t901_val+=0; //i33321
t901_val+=953.1; //i33322
t901_val+=967.58; //i33323
t901_val+=951; //i33330
*/

valf=rand()%2000;t901_val+=valf;
	ret=acqdata_set_value(acq_data,"i33311",UNIT_0C,valf,&arg_n);// T11
	valf=rand()%2000;t901_val+=valf;
ret=acqdata_set_value(acq_data,"i33312",UNIT_0C,valf,&arg_n); // T12
valf=rand()%2000;t901_val+=valf;
ret=acqdata_set_value(acq_data,"i33313",UNIT_0C,valf,&arg_n);// T13
valf=rand()%2000;t901_val+=valf;
ret=acqdata_set_value(acq_data,"i33321",UNIT_0C,valf,&arg_n);// T21
valf=rand()%2000;t901_val+=valf;
ret=acqdata_set_value(acq_data,"i33322",UNIT_0C,valf,&arg_n);// T22
valf=rand()%2000;t901_val+=valf;
ret=acqdata_set_value(acq_data,"i33323",UNIT_0C,valf,&arg_n); // T23
valf=rand()%2000;t901_val+=valf;
ret=acqdata_set_value(acq_data,"i33330",UNIT_0C,valf,&arg_n);// T30
#endif


	if(status == 0)
	{
	    t901_val_temp=0;
		t901_val_temp+=valf3;//977.4; //i33311
		t901_val_temp+=valf1;//942.3; //i33312
		t901_val_temp+=valf2;//943.2; //i33313
		t901_val_temp+=valf6;//0; //i33321
		t901_val_temp+=valf4;//953.1; //i33322
		t901_val_temp+=valf5;//967.58; //i33323
		t901_val_temp+=valf7;//951; //i33330
   		acq_data->acq_status = ACQ_OK;
	}
	else 
	{
	    t901_val+=t901_val_temp;
 		acq_data->acq_status = ACQ_ERR;
	}

	NEED_ERROR_CACHE(acq_data,20);

	if(acq_data->acq_status == ACQ_ERR) t901_val_temp=0;//added by miles zhang 20200107s

    return arg_n;
}

