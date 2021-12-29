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

static const int NMGas_polcodes_num=16;
static const  char  const polcode2005NMGas[][20] = {"02","02z","03","03z","01","01z",
                                                   "S02","S01","S03","S08","B02","S05",
                                                   "10","10z","S04","S07"};
static const  char const polcode2017NMGas[][20] = {"a21026","a21026z","a21002","a21002z","a34013","a34013z",
	                                               "a01011","a19001","a01012","a01013","a00000","a01014",
	                                               "a21001","a21001z","a01017","a01016"};
static const UNIT_DECLARATION const unitsNMGas[]={UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
	                                               UNIT_M_S,UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_H,UNIT_PECENT,
	                                               UNIT_MG_M3,UNIT_MG_M3,UNIT_PA,UNIT_M2};

static int NMGas_rtdata(struct acquisition_data *acq_data,int *argn)
{
#if 0
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[50]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float temp=0;
   time_t tmlong=0;

   if(!acq_data || !argn) return -1;
   
   acqdata_set_rtdata(acq_data,"w00000",UNIT_L_S,10+rand()%15,'N',&arg_n);
   acqdata_set_rtdata(acq_data,"w00001",UNIT_M3,100+rand()%15,'N',&arg_n);
   acqdata_set_rtdata(acq_data,"w01018",UNIT_MG_L,80+rand()%15,'N',&arg_n);
   tmlong=get_current_time();
   tmlong=(tmlong/30)*30;
   tmlong2tmarray(acq_data->acq_tm,&tmlong);
   acq_data->cn=2011;

   *argn=arg_n;

   return status;
#else
    int status=0;
	int arg_n=0;
	arg_n= HeBei_rtdata(acq_data,polcode2005NMGas,NMGas_polcodes_num,
		                            polcode2017NMGas,NMGas_polcodes_num,
		                            unitsNMGas,ARRAY_SIZE(unitsNMGas));
	status=acq_data->acq_status;
	
	if(arg_n<0) 
	{
		status = ACQ_ERR;
		*argn=0;
	}
	else
	{
	    *argn=arg_n;
	}
	return status;

#endif
}

static int NMGas_statdata_minute(struct acquisition_data *acq_data,int *argn)
{
#if 0
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[50]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float temp=0;
   time_t tmlong=0;

   if(!acq_data || !argn) return -1;
   
   acqdata_set_stattdata(acq_data,"w00000",UNIT_L_S,10+rand()%15,10,25,1,'N',&arg_n);
   acqdata_set_stattdata(acq_data,"w00001",UNIT_M3,100+rand()%15,100,115,10,'N',&arg_n);
   acqdata_set_stattdata(acq_data,"w01018",UNIT_MG_L,80+rand()%15,80,95,8,'N',&arg_n);
   tmlong=get_current_time();
   tmlong=(tmlong/60)*60;
   tmlong2tmarray(acq_data->acq_tm,&tmlong);
   acq_data->cn=2051;

   *argn=arg_n;

   return status;   
#else
    int status=0;
	int arg_n=0;
	arg_n= HeBei_stattdata(2051,acq_data,polcode2005NMGas,NMGas_polcodes_num,
		                            polcode2017NMGas,NMGas_polcodes_num,
		                            unitsNMGas,ARRAY_SIZE(unitsNMGas));
	status=acq_data->acq_status;
	
	if(arg_n<0) 
	{
		status = ACQ_ERR;
		*argn=0;
	}
	else
	{
	    *argn=arg_n;
	}
	return status;
#endif
}


static int NMGas_statdata_hour(struct acquisition_data *acq_data,int *argn)
{
#if 0
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[50]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float temp=0;
   time_t tmlong=0;

   if(!acq_data || !argn) return -1;
   
   acqdata_set_stattdata(acq_data,"w00000",UNIT_L_S,10+rand()%15,10,25,1,'N',&arg_n);
   acqdata_set_stattdata(acq_data,"w00001",UNIT_M3,100+rand()%15,100,115,10,'N',&arg_n);
   acqdata_set_stattdata(acq_data,"w01018",UNIT_MG_L,80+rand()%15,80,95,8,'N',&arg_n);
   read_system_time(acq_data->acq_tm);
   acq_data->acq_tm[1]=acq_data->acq_tm[0]=0;
   acq_data->cn=2061;

   *argn=arg_n;

   return status;   
#else
    int status=0;
	int arg_n=0;
	arg_n= HeBei_stattdata(2061,acq_data,polcode2005NMGas,NMGas_polcodes_num,
		                            polcode2017NMGas,NMGas_polcodes_num,
		                            unitsNMGas,ARRAY_SIZE(unitsNMGas));
	status=acq_data->acq_status;
	
	if(arg_n<0) 
	{
		status = ACQ_ERR;
		*argn=0;
	}
	else
	{
	    *argn=arg_n;
	}
	return status;
#endif
}


static int NMGas_statdata_day(struct acquisition_data *acq_data,int *argn)
{
#if 0
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[50]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float temp=0;
   time_t tmlong=0;

   if(!acq_data || !argn) return -1;
   
   acqdata_set_stattdata(acq_data,"w00000",UNIT_L_S,10+rand()%15,10,25,1,'N',&arg_n);
   acqdata_set_stattdata(acq_data,"w00001",UNIT_M3,100+rand()%15,100,115,10,'N',&arg_n);
   acqdata_set_stattdata(acq_data,"w01018",UNIT_MG_L,80+rand()%15,80,95,8,'N',&arg_n);
   read_system_time(acq_data->acq_tm);
   acq_data->acq_tm[2]=acq_data->acq_tm[1]=acq_data->acq_tm[0]=0;
   acq_data->cn=2031;

   *argn=arg_n;

   return status;  
#else
    int status=0;
	int arg_n=0;
	arg_n= HeBei_stattdata(2031,acq_data,polcode2005NMGas,NMGas_polcodes_num,
		                            polcode2017NMGas,NMGas_polcodes_num,
		                            unitsNMGas,ARRAY_SIZE(unitsNMGas));
	status=acq_data->acq_status;
	
	if(arg_n<0) 
	{
		status = ACQ_ERR;
		*argn=0;
	}
	else
	{
	    *argn=arg_n;
	}
	return status;
#endif
}


int protocol_direct_NMGas(struct acquisition_data *acq_data)
{
   int status=0;
   int arg_n=0;

   static unsigned int hjt212sel=0;
   
   if(!acq_data) return -1;


   hjt212sel++;
   hjt212sel = (hjt212sel>=4) ? 0 : hjt212sel;

   switch (hjt212sel)
   {
	   case 0://rtdata
	   {
           status=NMGas_rtdata(acq_data,&arg_n);
	   }
	   break;
	   case 1://minute
	   {
           status=NMGas_statdata_minute(acq_data,&arg_n);
	   }
	   break;
	   case 2://hour
	   {
          status=NMGas_statdata_hour(acq_data,&arg_n);
	   }
	   break;
	   case 3://day
	   {
          status=NMGas_statdata_day(acq_data,&arg_n);
	   }
	   break;
   }


   SYSLOG_DBG("NMGas direct hjt212sel=%d cn=%d argn=%d, acqtime:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
   	   hjt212sel,acq_data->cn,arg_n,
   	   acq_data->acq_tm[5],acq_data->acq_tm[4],acq_data->acq_tm[3],
   	   acq_data->acq_tm[2],acq_data->acq_tm[1],acq_data->acq_tm[0]);
   
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

   return arg_n;
}

