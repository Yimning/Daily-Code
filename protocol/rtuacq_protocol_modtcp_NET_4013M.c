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



int protocol_modtcp_NET_4013M1(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x6,0x00,0x2,0x0,0x0,0x0,0x8};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	     	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }
	 
	 
     pf=acq_data->com_f;  
 
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 //return arg_n;
	 status=1;	
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=10)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   

	   acqdata_set_value(acq_data,"w01018",UNIT_MG_L,(float)(modbus_cache[9]^0xff),&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;

		ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}

int protocol_modtcp_NET_4013M2(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x6,0x00,0x2,0x0,0x0,0x0,0x8};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	     	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }
	 
	 
     pf=acq_data->com_f;  
 
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 //return arg_n;
	 status=1;	
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=10)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   

	   acqdata_set_value(acq_data,"w01018",UNIT_MG_L,(float)(modbus_cache[9]^0xff),&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;

		ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}

int protocol_modtcp_NET_4013M3(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x6,0x00,0x2,0x0,0x0,0x0,0x8};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	     	ret=acqdata_set_value(acq_data,"N4013M3",UNIT_NONE,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }
	 
	 
     pf=acq_data->com_f;  
 
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 //return arg_n;
	 status=1;	
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=10)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   

	   acqdata_set_value(acq_data,"N4013M3",UNIT_NONE,(float)(modbus_cache[9]^0xff),&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;

		ret=acqdata_set_value(acq_data,"N4013M3",UNIT_NONE,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}


int protocol_modtcp_NET_4013M4(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x6,0x00,0x2,0x0,0x0,0x0,0x8};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	     	ret=acqdata_set_value(acq_data,"N4013M4",UNIT_NONE,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }
	 
	 
     pf=acq_data->com_f;  
 
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 //return arg_n;
	 status=1;	
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=10)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   

	   acqdata_set_value(acq_data,"N4013M4",UNIT_NONE,(float)(modbus_cache[9]^0xff),&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;

		ret=acqdata_set_value(acq_data,"N4013M4",UNIT_NONE,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}


int protocol_modtcp_NET_4013M5(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x6,0x00,0x2,0x0,0x0,0x0,0x8};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	     	ret=acqdata_set_value(acq_data,"N4013M5",UNIT_NONE,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }
	 
	 
     pf=acq_data->com_f;  
 
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 //return arg_n;
	 status=1;	
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=10)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   

	   acqdata_set_value(acq_data,"N4013M5",UNIT_NONE,(float)(modbus_cache[9]^0xff),&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;

		ret=acqdata_set_value(acq_data,"N4013M5",UNIT_NONE,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}


int protocol_modtcp_NET_4013M6(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x6,0x00,0x2,0x0,0x0,0x0,0x8};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	     	ret=acqdata_set_value(acq_data,"N4013M6",UNIT_NONE,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }
	 
	 
     pf=acq_data->com_f;  
 
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 //return arg_n;
	 status=1;	
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=10)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   

	   acqdata_set_value(acq_data,"N4013M6",UNIT_NONE,(float)(modbus_cache[9]^0xff),&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;

		ret=acqdata_set_value(acq_data,"N4013M6",UNIT_NONE,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}


int protocol_modtcp_NET_4013M7(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x6,0x00,0x2,0x0,0x0,0x0,0x8};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	     	ret=acqdata_set_value(acq_data,"N4013M7",UNIT_NONE,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }
	 
	 
     pf=acq_data->com_f;  
 
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 //return arg_n;
	 status=1;	
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=10)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   

	   acqdata_set_value(acq_data,"N4013M7",UNIT_NONE,(float)(modbus_cache[9]^0xff),&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;

		ret=acqdata_set_value(acq_data,"N4013M7",UNIT_NONE,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}


int protocol_modtcp_NET_4013M8(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x6,0x00,0x2,0x0,0x0,0x0,0x8};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);


     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
	     	ret=acqdata_set_value(acq_data,"N4013M8",UNIT_NONE,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }
	 
	 
     pf=acq_data->com_f;  
 
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 //return arg_n;
	 status=1;	
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=10)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   

	   acqdata_set_value(acq_data,"N4013M8",UNIT_NONE,(float)(modbus_cache[9]^0xff),&arg_n);

     }
     else
     {
	     	printf("read modbus error %s\n",DEV_NAME(acq_data));
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;

		ret=acqdata_set_value(acq_data,"N4013M8",UNIT_NONE,0,&arg_n);
		
		status=1;
     }
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}








