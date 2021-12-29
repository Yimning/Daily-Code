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

int t901_val;

int protocol_CEMS_GermanySick_pack2(struct acquisition_data *acq_data)
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
	SYSLOG_DBG_HEX("GermanySick_pack2 CEMS ",com_tbuf,4);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
   	SYSLOG_DBG("GermanySick_pack2 CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("GermanySick_pack2 CEMS ",com_rbuf,size);
	
	if((size>=9)&&(com_rbuf[0]=='>'))
	{
	  //">+05.103+03.994+08.360+04.262+13.739+11.536+05.376-00.001"
	  sscanf(com_rbuf,">+%f+%f+%f+%f+%f+%f+%f+%f",&ai1,&ai2,&ai3,&ai4,&ai5,&ai6,&ai7,&ai8);	
	  if(ai1>4)valf1=(ai1-4)/16*40; //a01011  S02
	  if(ai2>4)valf2=(ai2-4)/16*300; //S03 //a01012
	  //if(ai3>4)valf3=(ai3-4)/16*10000; //S08 10kPa //a01013
	  if(ai3>4)valf3=(ai3-4)/16*20000-10000; //S08 -10kPa~+10kPa //a01013
	  if(ai4>4)valf4=(ai4-4)/16*1200; //902 //a01902
	  if(ai5>4)valf5=(ai5-4)/16*1200; //T10 //i33310
	  if(ai6>4)valf6=(ai6-4)/16*1200; //T20 //i33320
	  if(ai7>4)valf7=(ai7-4)/16*1200; //i33342
	  if(ai8>4)valf8=(ai8-4)/16*1200; //i33343

	  SYSLOG_DBG("a01011:%f,S03:%f,S08:%f,902:%f,T10:%f,T20:%f,i33342:%f,i33343:%f\n",
	  	valf1,valf2,valf3,valf4,valf5,valf6,valf7,valf8);

	ret=acqdata_set_value(acq_data,"a01011",UNIT_M_S,valf1,&arg_n);// a01011
	ret=acqdata_set_value(acq_data,"a01012",UNIT_0C,valf2,&arg_n); // S03
	ret=acqdata_set_value(acq_data,"a01013",UNIT_PA,valf3,&arg_n);// S08
	ret=acqdata_set_value(acq_data,"a01902",UNIT_0C,valf4,&arg_n);// 902
	ret=acqdata_set_value(acq_data,"i33310",UNIT_0C,valf5,&arg_n);// T10
	ret=acqdata_set_value(acq_data,"i33320",UNIT_0C,valf6,&arg_n); // T20
	ret=acqdata_set_value(acq_data,"i33342",UNIT_0C,valf7,&arg_n);// i33342
	ret=acqdata_set_value(acq_data,"i33343",UNIT_0C,valf8,&arg_n);// i33343
	}
	else
	{
		status=1;
ret=acqdata_set_value(acq_data,"a01011",UNIT_M_S,0,&arg_n);// a01011
ret=acqdata_set_value(acq_data,"a01012",UNIT_0C,0,&arg_n); // S06
ret=acqdata_set_value(acq_data,"a01013",UNIT_PA,0,&arg_n);// S08
ret=acqdata_set_value(acq_data,"a01902",UNIT_0C,0,&arg_n);// 902
ret=acqdata_set_value(acq_data,"i33310",UNIT_0C,0,&arg_n);// T10
ret=acqdata_set_value(acq_data,"i33320",UNIT_0C,0,&arg_n); // T20
ret=acqdata_set_value(acq_data,"i33342",UNIT_0C,0,&arg_n);// i33342
ret=acqdata_set_value(acq_data,"i33343",UNIT_0C,0,&arg_n);// i33343
	}
	
ret=acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);// a00000z	

t901_val+=valf5;//977.48; // i33310 T10
t901_val+=valf6;//972.67; // i33320 T20

t901_val/=9;
ret=acqdata_set_value(acq_data,"a01901",UNIT_0C,t901_val,&arg_n);// 901
t901_val=0;
	
#else
/*
t901_val+=977.48; // i33310
t901_val+=972.67; // i33320

t901_val/=9;

ret=acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);// a00000z
ret=acqdata_set_value(acq_data,"a01901",UNIT_0C,t901_val,&arg_n);// 901

ret=acqdata_set_value(acq_data,"a01011",UNIT_M_S,10.33,&arg_n);// a01011
ret=acqdata_set_value(acq_data,"a01012",UNIT_0C,143.8,&arg_n); // S06
ret=acqdata_set_value(acq_data,"a01013",UNIT_PA,-0.69,&arg_n);// S08
ret=acqdata_set_value(acq_data,"a01902",UNIT_0C,1026.08,&arg_n);// 902
ret=acqdata_set_value(acq_data,"i33310",UNIT_0C,977.48,&arg_n);// T10
ret=acqdata_set_value(acq_data,"i33320",UNIT_0C,972.67,&arg_n); // T20
ret=acqdata_set_value(acq_data,"i33342",UNIT_0C,942.38,&arg_n);// i33342
ret=acqdata_set_value(acq_data,"i33343",UNIT_0C,1026.45,&arg_n);// i33343

t901_val=0;
*/




ret=acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);// a00000z

valf=rand()%2000;valf/=10;
ret=acqdata_set_value(acq_data,"a01011",UNIT_M_S,valf,&arg_n);// a01011
valf=rand()%2000;valf/=10;
ret=acqdata_set_value(acq_data,"a01012",UNIT_0C,valf,&arg_n); // S06
valf=rand()%2000-1000;valf/=10;
ret=acqdata_set_value(acq_data,"a01013",UNIT_PA,valf,&arg_n);// S08
valf=rand()%2000;
ret=acqdata_set_value(acq_data,"a01902",UNIT_0C,valf,&arg_n);// 902
valf=rand()%2000;
ret=acqdata_set_value(acq_data,"i33310",UNIT_0C,valf,&arg_n);// T10
valf=rand()%2000;
ret=acqdata_set_value(acq_data,"i33320",UNIT_0C,valf,&arg_n); // T20
valf=rand()%2000;t901_val+=valf;
ret=acqdata_set_value(acq_data,"i33342",UNIT_0C,valf,&arg_n);// i33342
valf=rand()%2000;t901_val+=valf;
ret=acqdata_set_value(acq_data,"i33343",UNIT_0C,valf,&arg_n);// i33343
t901_val/=9;
ret=acqdata_set_value(acq_data,"a01901",UNIT_0C,t901_val,&arg_n);// 901


t901_val=0;

#endif


	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}

