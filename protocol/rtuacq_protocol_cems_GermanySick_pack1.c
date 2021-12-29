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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

#include "rtuacq_protocol_error_cache.h"

int protocol_CEMS_GermanySick_pack1(struct acquisition_data *acq_data)
{
/*
 send : #01      send hex: 23 30 31 0d
 read : >+08.074+05.794+07.228+07.748+05.763+06.812+11.576+14.668
*/
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	char *p,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float valf1,valf2,valf3,valf4,valf5,valf6,valf7,valf8,valf9;
	union uf f;
	int ret=0;
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
	SYSLOG_DBG_HEX("GermanySick_pack1 CEMS ",com_tbuf,4);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("GermanySick_pack1 CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("GermanySick_pack1 CEMS %s",com_rbuf);
/*	
	p=NULL;
	for(i=size-1;i>=0;i--)
	{
		if(com_rbuf[i]=='>')
		{
			p=com_rbuf[i];
			break;
		}
	}
*/	
 	if(size>=10)
	{
		pq=strstr(com_rbuf,">+");
		if(!pq) goto ERROR_PACK;
		else
		{
		   p=pq;
		}
		
		pq=strstr(pq++,">+");
		if(pq) p=pq;
		
		ret=sscanf(p,">+%f+%f+%f+%f+%f+%f+%f+%f",&ai1,&ai2,&ai3,&ai4,&ai5,&ai6,&ai7,&ai8);	
		if(ret!=8) goto ERROR_PACK;
		
		if(ai1>4)valf1=(ai1-4)/16.0*40;
		if(ai2>4)valf2=(ai2-4)/16.0*100; 
		if(ai3>4)valf3=(ai3-4)/16.0*25;
		if(ai4>4)valf4=(ai4-4)/16.0*150; 
		if(ai5>4)valf5=(ai5-4)/16.0*20; 
		if(ai6>4)valf6=(ai6-4)/16.0*300; 
		if(ai7>4)valf7=(ai7-4)/16.0*10000-5000; 
		if(ai8>4)
		{
			valf8=(ai8-4)/16.0*25;
			//valf8=0.821*0.076*sqrt(valf*(273+valf6));
		}

		if(valf3<19.0  && valf7<101325 && valf1<100 && valf6>-273)
		{
			valf=valf5*101325*(273+valf6)/(101325-valf7)/273/(1-valf1/100);
			valf9=valf*0.21/(0.21-valf3/100)/1.4;
		}
		else
		{
			valf9=valf5;
		}
 	}
	else
	{

ERROR_PACK:
		status=1;
		valf1=0;
		valf2=0;
		valf3=0;
		valf4=0;
		valf5=0;
		valf6=0;
		valf7=0;
		valf8=0;
		valf9=0;
	}

	acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf1,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,valf2,&arg_n); 
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,valf3,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,valf4,&arg_n);
	acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf5,&arg_n);
	acqdata_set_value(acq_data,"a01012",UNIT_0C,valf6,&arg_n);
	acqdata_set_value(acq_data,"a01013",UNIT_PA,valf7,&arg_n);
	acqdata_set_value(acq_data,"a01011",UNIT_M_S,valf8,&arg_n);

	acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,valf9,&arg_n);
#else
/*
	ret=acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,0,&arg_n);// 08
	ret=acqdata_set_value(acq_data,"a21024",UNIT_MG_M3,3.097,&arg_n);// 08
	ret=acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n); // 04
	ret=acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,0,&arg_n); // 04
	ret=acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);// 03
	ret=acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,96.638,&arg_n);// 03
	ret=acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);// 02
	ret=acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,2.36,&arg_n);// 02
	ret=acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);// 01
	ret=acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,1.61,&arg_n);// 01
	
	ret=acqdata_set_value(acq_data,"a01014",UNIT_PECENT,25.7,&arg_n);// S05
	ret=acqdata_set_value(acq_data,"a19001",UNIT_PECENT,10.0,&arg_n); // S01
*/

	ret=acqdata_set_value(acq_data,"a21024z",UNIT_MG_M3,0,&arg_n);// 08
	valf=rand()%2000;valf/=10;	
	ret=acqdata_set_value(acq_data,"a21024",UNIT_MG_M3,valf,&arg_n);// 08
	ret=acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,0,&arg_n); // 04
	valf=rand()%2000;valf/=10;		
	ret=acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,valf,&arg_n); // 04
	ret=acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);// 03
	valf=rand()%2000;valf/=10;	
	ret=acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,valf,&arg_n);// 03
	ret=acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);// 02
	valf=rand()%2000;valf/=10;	
	ret=acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,valf,&arg_n);// 02
	ret=acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);// 01
	valf=rand()%2000;valf/=10;	
	ret=acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,valf,&arg_n);// 01

	valf=rand()%1000;valf/=10;
	ret=acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf,&arg_n);// S05
	valf=(rand()%1000)/4;valf/=10;
	ret=acqdata_set_value(acq_data,"a19001",UNIT_PECENT,valf,&arg_n); // S01	
#endif	

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}

