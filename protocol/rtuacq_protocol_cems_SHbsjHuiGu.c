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


static int getPolcodeValf(char *valstr,char *cemspol,float *valf)
{
/*
"STX,15,20100602122233,ZA9,11.23,ZB0,22.23,ZB1,33.23,ZB2,44.23,ZB3,55.23,ZB4,66.23,ZD1,77.23,ZD3,88.23,ZD4,99.23,ZD5,105.23,ZD6,100.23,ZD7,101.23,ZD8,102.23,ZD9,103.23,ZB6,145.30,ETX
"
*/
   int ret=0;
   char *polstr;

   if(!valstr || !cemspol || !valf) return -1;

   polstr=strstr(valstr,cemspol);
   
   if(polstr && valf)
   {
     char valfstr[200]={0};
	 memset(valfstr,0,sizeof(valfstr));
	 strcpy(valfstr,cemspol);
	 strcat(valfstr,",%f,");
     ret=sscanf(polstr,valfstr,valf);

	 if(ret!=1) return -2;

	 return 0;
   }

   return -1;
}

int protocol_CEMS_SHbsjHuiGu(struct acquisition_data *acq_data)
{
	int status=0;
   float *pf;
   char com_rbuf[2048]={0}; 
   char com_tbuf[255]={0};
   int size=0;
   union uf  f;
   int arg_n=0;
   int i=0,j=0;
   int ret=0;
   
/*
{8002,a21026,0,1,UNIT_MG_M3,1000,0,1000,0,0},
{8002,a21026z,0,1,UNIT_MG_M3,1000,0,1000,0,0},
{8002,a00LD5,0,1,UNIT_KG,100000,0,100000,0,0},
{8002,a21002,0,1,UNIT_MG_M3,1000,0,1000,0,0},
{8002,a21002z,0,1,UNIT_MG_M3,1000,0,1000,0,0},
{8002,a00LD6,0,1,UNIT_KG,100000,0,100000,0,0},
{8002,a34013,0,1,UNIT_MG_M3,1000,0,1000,0,0},
{8002,a34013z,0,1,UNIT_MG_M3,1000,0,1000,0,0},
{8002,a00LD4,0,1,UNIT_KG,100000,0,100000,0,0},
{8002,a01011,0,1,UNIT_M_S,1000,0,1000,0,0},
{8002,a19001,0,1,UNIT_PECENT,100,0,100,0,0},
{8002,a01012,0,1,UNIT_0C,1000,0,1000,0,0},
{8002,a01013,0,1,UNIT_KPA,1000,0,1000,0,0},
{8002,a00000,0,1,UNIT_M3_S,10000000,0,10000000,0,0},
{8002,a00LD3,0,1,UNIT_M3,10000000,0,10000000,0,0},
*/
#define CEMSPOL_NUM 15

   char *polcodestr[]={"a21026","a21026z","a00LD5","a21002","a21002z","a00LD6",
   "a34013","a34013z","a00LD4","a01011","a19001","a01012","a01013","a00000","a00LD3"};

   UNIT_DECLARATION unitstr[CEMSPOL_NUM]={UNIT_MG_M3,UNIT_MG_M3,UNIT_KG,UNIT_MG_M3,UNIT_MG_M3,UNIT_KG,
   	UNIT_MG_M3,UNIT_MG_M3,UNIT_KG,UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_H,UNIT_M3};

   //char *cemspol[]={"ZB2","ZD8","ZD5","ZB3","ZD9","ZD6","ZA9","ZD7","ZD4",
   //	"ZB0","ZB4","ZA8","ZB1","ZB6","ZD3"};
   
	   char *cemspol[]={"ZB2","ZD8","ZD5","ZB3","ZD9","ZD6","ZA9","ZD7","ZD4",
   	"ZB0","ZB4","ZD1","ZB1","ZB6","ZD3"};

       float cemvalf[CEMSPOL_NUM]={0};
	   
	   float cemvalf_num=0;
	   
   if(!acq_data) return -1;

   memset(cemvalf,0,sizeof(float)*CEMSPOL_NUM);
   cemvalf_num=0;
   
	for(i=0;i<10;i++) //need 30 seconds running
	{// ERROR , NOTE : for loop 30 seconds will make error , in rtu_statistics.c
	 //error log : "save_statistics_records , incompleted"
	 
		memset(com_tbuf,0,sizeof(com_tbuf));
	    strcpy(com_tbuf,"STX,READ DATA,ETX");
		size=strlen(com_tbuf);
	    size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	    sleep(2);

	   	memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	   	SYSLOG_DBG("SHbsjHuiGu CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG("SHbsjHuiGu CEMS size=%d,data:%s",size,com_rbuf);   

		if(size >= 10 && !strncmp(com_rbuf,"STX",3))
		{
		  	 float valf=0.0;
		     for(j=0;j<CEMSPOL_NUM;j++)
		     {
			       ret=getPolcodeValf(com_rbuf,cemspol[j],&valf);
				   if(ret!=0)
				   {
				    	valf=0;
				   }
				   if(strlen(polcodestr[j])>0)
				   {
				     cemvalf[j]+=valf;
				   }
		     }	

			 cemvalf_num++;
		}
		
		sleep(1);
	}



    if(cemvalf_num>0)
    {
        char strvalf[400]={0};
		memset(strvalf,0,sizeof(strvalf));
		
		for(i=0;i<CEMSPOL_NUM;i++)
		{
		   cemvalf[i]/=cemvalf_num;
		}

		for(i=0;i<CEMSPOL_NUM;i++)
		{
		  sprintf(&strvalf[strlen(strvalf)],"%s,%.2f,",cemspol[i],cemvalf[i]);
		}
		
		sprintf(com_rbuf,"STX,15,20100602122233,%sETX",strvalf);
		size=strlen(com_rbuf);
    }
	else
	{
	  memset(com_rbuf,0,sizeof(com_rbuf));
	  size=0;
	}


/*
"STX,15,20100602122233,ZA9,11.23,ZB0,22.23,ZB1,33.23,ZB2,44.23,ZB3,55.23,ZB4,66.23,ZD1,77.23,ZD3,88.23,ZD4,99.23,ZD5,105.23,ZD6,100.23,ZD7,101.23,ZD8,102.23,ZD9,103.23,ZB6,145.30,ETX
"
*/
//   strcpy(com_rbuf,"STX,15,20100602122233,ZA9,2.46,ZB0,11.73,ZB1,2.23,ZB2,2.48,ZB3,11.58,ZB4,17.08,ZD3,29163.11,ZD4,99.23,ZD5,105.23,ZD6,100.23,ZD7,8.99,ZD8,10.96,ZD9,44.30,ZB6,29163.11,ZD1,118.52,ETX");
//   size=strlen("STX,15,20100602122233,ZA9,2.46,ZB0,11.73,ZB1,2.23,ZB2,2.48,ZB3,11.58,ZB4,17.08,ZD3,29163.11,ZD4,99.23,ZD5,105.23,ZD6,100.23,ZD7,8.99,ZD8,10.96,ZD9,44.30,ZB6,29163.11,ZD1,118.52,ETX");

   if(size >= 10 && !strncmp(com_rbuf,"STX",3))
   {	 
     for(i=0;i<CEMSPOL_NUM;i++)
     {
	       ret=getPolcodeValf(com_rbuf,cemspol[i],&f.f);
		   if(ret!=0)
		   {
		    	f.f=0;
		   }
		   if(strlen(polcodestr[i])>0)
		        acqdata_set_value(acq_data,polcodestr[i],unitstr[i],f.f,&arg_n);
     }	
   }
    else
    {
        int i=0;
		read_system_time(acq_data->acq_tm);
		for(i=0;i<CEMSPOL_NUM;i++)
		{
		   if(strlen(polcodestr[i])>0)
		       acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0.0,&arg_n);
		}		

		if(size == 0)// stop product
			status = 0;
		else
			status = 1;
		//status = 0;
    }

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

