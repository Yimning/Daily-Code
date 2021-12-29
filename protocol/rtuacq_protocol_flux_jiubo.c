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
*return : com_f arguments amount;
*/
#if 0
int protocol_FLUX_jiubo(struct acquisition_data *acq_data)
{
    float *pf;
    int status=0;
    char cmd=0x11;
    char com_rbuf[2048];    
    int arg_n=0;
    int size=0;
    static float total_volume=0;
    static int total=0;
       
    if(!acq_data) return -1;
    
#if 1
    size=write_device(DEV_NAME(acq_data),&cmd,sizeof(cmd));
    sleep(1);
    
    memset(com_rbuf,0,sizeof(com_rbuf));
    size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
    SYSLOG_DBG("jiubo protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG("data:%s\n",com_rbuf);
    
    pf=acq_data->com_f;    
    if(size >= 48)
    {
                char *temptr;
                temptr = strchr(&com_rbuf[10],'Q');
                if(temptr != NULL)
                {
                        temptr=strchr(temptr,'=');
                        if(temptr != NULL)
                        {
                                temptr ++;
                                if(temptr!=NULL)
                                {
                                        pf[0] = atof(temptr);/*current instantaneous flow*/
                                        strcpy(acq_data->polcode[arg_n],"w00000");    //?
                                        acq_data->unit[arg_n]=UNIT_L_S;
                                        arg_n++;
                                }
                        }
                }
                else
                {
                        pf[0]=0.0;
                        status++;
                }

                temptr=strchr(&com_rbuf[10],'M');
                if(temptr!=NULL)
                {
                        temptr=strchr(temptr,'=');
                        if(temptr != NULL)
                        {
                                temptr ++;
                                if(temptr != NULL)
								pf[arg_n]=atof(temptr);
								strcpy(acq_data->polcode[arg_n],"w00001");
								acq_data->unit[arg_n]=UNIT_M3;
								arg_n++;
                                acq_data->total= atof(temptr);/*total flow*/            
                        }
                }
                else
                {
                        acq_data->total=0.0;
                        status++;     
                        
                        total_volume += pf[0];
                        pf[1] = total_volume;
                        strcpy(acq_data->polcode[arg_n],"w00001"); 
                        acq_data->unit[arg_n]=UNIT_L;     
                        arg_n++;
						
                }
                
    }
    else
    {
            pf[0]=0.0;
            strcpy(acq_data->polcode[arg_n],"w00000"); 
            acq_data->unit[arg_n]=UNIT_L_S;
            acq_data->total=0.0;
            arg_n++;
            
            total_volume += pf[0];
            pf[1] = total_volume;
            strcpy(acq_data->polcode[arg_n],"w00001"); 
            acq_data->unit[arg_n]=UNIT_L;
            arg_n++;
            
            status=2;
    }
#else
/*
      pf=acq_data->com_f;
      pf[0] = 17.3;
      strcpy(acq_data->polcode[arg_n],"w00000"); 
      acq_data->unit[arg_n]=UNIT_M3_H;
      arg_n=1;

      total_volume += pf[0];
      pf[1] = total_volume;
      strcpy(acq_data->polcode[arg_n],"w00001"); 
      acq_data->unit[arg_n]=UNIT_M3;
      arg_n=2;
      acq_data->total= 34.6;
*/      
      status=0;
#endif   
    
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    //read_rtc_time(acq_data->acq_tm); 
	read_system_time(acq_data->acq_tm); 

    return arg_n;
}

#else

int protocol_FLUX_jiubo_LS(struct acquisition_data *acq_data)
{
    int status=0;
    char cmd=0x11;
    char com_rbuf[2048];    
    int arg_n=0;
    int size=0;
	char *ppack;
	int ret=0;
	float speed=0,total_flux_M3=0;
	int i=0;
    if(!acq_data) return -1;

	sleep(1);

    LOG_WRITE_HEX(DEV_NAME(acq_data),0,"jiubo  SEND:",&cmd,1);
	SYSLOG_DBG("jiubo protocol send : %x\n",cmd);
	size=write_device(DEV_NAME(acq_data), &cmd, sizeof(cmd));
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
    SYSLOG_DBG("jiubo protocol LS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG("data:%s\n",com_rbuf);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"jiubo  RECV:",com_rbuf,size);
	
	ppack=strstr(com_rbuf,"Q =");
	if(ppack==NULL) goto ERROR_PROTOTOCL;
	ret=sscanf(ppack,"Q =%f(",&speed);
	if(ret != 1) goto ERROR_PROTOTOCL;

	ppack=strstr(com_rbuf,"M =");
	if(ppack==NULL) goto ERROR_PROTOTOCL;
	ret=sscanf(ppack,"M =%f",&total_flux_M3);
	if(ret != 1) goto ERROR_PROTOTOCL;


	status=0;
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	//acq_data->total=total_flux_M3;
	acq_data->total=0;
	if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;

ERROR_PROTOTOCL:
		acqdata_set_value(acq_data,"w00000",UNIT_L_S,0,&arg_n);
		acqdata_set_value(acq_data,"w00001",UNIT_M3,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
}

int protocol_FLUX_jiubo_M3H(struct acquisition_data *acq_data)
{
    int status=0;
    char cmd=0x11;
    char com_rbuf[2048];    
    int arg_n=0;
    int size=0;
	char *ppack;
	int ret=0;
	float speed=0,total_flux_M3=0;
	int i=0;
    if(!acq_data) return -1;

	size=write_device(DEV_NAME(acq_data), &cmd, sizeof(cmd));
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
    SYSLOG_DBG("jiubo protocol M3H : read device %s , size=%d\n",DEV_NAME(acq_data),size);
    SYSLOG_DBG("data:%s\n",com_rbuf);
	
	//ppack=strstr(com_rbuf,"Q=");
	ppack=strstr(com_rbuf,"Q =");
	if(ppack==NULL) goto ERROR_PROTOTOCL;
	//ret=sscanf(ppack,"Q=%fm3/h",&speed);
	ret=sscanf(ppack,"Q =%f(",&speed);
	if(ret != 1) goto ERROR_PROTOTOCL;

	//ppack=strstr(com_rbuf,"M=");
	ppack=strstr(com_rbuf,"M =");
	if(ppack==NULL) goto ERROR_PROTOTOCL;
	//ret=sscanf(ppack,"M=%fM3",&total_flux_M3);
	ret=sscanf(ppack,"M =%f",&total_flux_M3);
	if(ret != 1) goto ERROR_PROTOTOCL;


	status=0;
	acqdata_set_value(acq_data,"w00000",UNIT_M3_H,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total_flux_M3,&arg_n);
	//acq_data->total=total_flux_M3;
	acq_data->total=0;
	if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;

ERROR_PROTOTOCL:
		acqdata_set_value(acq_data,"w00000",UNIT_M3_H,0,&arg_n);
		acqdata_set_value(acq_data,"w00001",UNIT_M3,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
}


/*
int protocol_jiubo(struct acquisition_data *acq_data)
{
    int status=0;
    float *pf;
    char com_rbuf[2048]; 
    char modpack[8];
    int size=0;
    union uf  f;
    int arg_n=0;
    static int total=0;

    pf=acq_data->com_f;  
    pf[arg_n]=10;
    strcpy(acq_data->polcode[arg_n],"w00000"); 
    acq_data->unit[arg_n]=UNIT_L_S;
    status=0;
    arg_n++;
    
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    total+=10;
    acq_data->total +=total;
    pf[1] = acq_data->total;
    strcpy(acq_data->polcode[arg_n],"w00001"); 
    acq_data->unit[arg_n]=UNIT_L;
    arg_n=2;

    read_rtc_time(acq_data->acq_tm); 

    return arg_n;
}
*/
#endif

