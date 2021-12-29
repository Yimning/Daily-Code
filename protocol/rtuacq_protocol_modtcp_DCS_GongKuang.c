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



int protocol_modtcp_DCS_GongKuang(struct acquisition_data *acq_data)
{
    struct acquisition_ctrl  *acq_ctrl;
    char com_tbuf[]={0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x0,0x0,0x0,0x04};
    char com_rbuf[1000]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	
	union uf f;
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);

     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		//read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }

     ret=write_device(DEV_NAME(acq_data),com_tbuf,sizeof(com_tbuf));
	 SYSLOG_DBG_HEX("modbusTcp send", com_tbuf, ARRAY_SIZE(com_tbuf));
     if(ret!=ARRAY_SIZE(com_tbuf))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	    arg_n=0;	
	    printf("write modbus error %s\n",DEV_NAME(acq_data));
	    acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	    status=1;
     }

     sleep(2);
	 
     ret=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	 SYSLOG_DBG("modbusTcp read %s size=%d\n",DEV_NAME(acq_data),ret);
	 SYSLOG_DBG_HEX("modbusTcp read", com_rbuf, ret);
     if(ret>=17 && com_rbuf[0]==0 && com_rbuf[5]==0xb && com_rbuf[6]==0x1 )
     {
        float valf_th=0,valf_mw=0;

  		f.ch[0]=com_rbuf[12];
		f.ch[1]=com_rbuf[11];
		f.ch[2]=com_rbuf[10];
		f.ch[3]=com_rbuf[9];
		valf_mw=f.f;      
		
  		f.ch[0]=com_rbuf[16];
		f.ch[1]=com_rbuf[15];
		f.ch[2]=com_rbuf[14];
		f.ch[3]=com_rbuf[13];
		valf_th=f.f; 	
		
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,valf_th,&arg_n);
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,valf_mw,&arg_n);	      

     }
     else
     {
	    SYSLOG_DBG("read modbus error %s,ret=%d\n",DEV_NAME(acq_data),ret);
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
		
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,0,&arg_n);	
		
		status=1;
     }

	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    NEED_ERROR_CACHE(acq_data, 10);

     return arg_n;
}


static int modtcp_data(struct acquisition_data *acq_data,char *com_tbuf,int size,float *valf)
{
     int ret=0;
	 char com_rbuf[1000]={0};
	 union uf f;

	 if(!acq_data || !com_tbuf || size<=0 || !valf) return 1;
	 
     ret=write_device(DEV_NAME(acq_data),com_tbuf,size);
	 SYSLOG_DBG_HEX("modbusTcp send", com_tbuf, size);
     if(ret!=size)
     {
        *valf=0;
		return 1;
     }

     sleep(2);
	 
     ret=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	 SYSLOG_DBG("modbusTcp read %s size=%d\n",DEV_NAME(acq_data),ret);
	 SYSLOG_DBG_HEX("modbusTcp read", com_rbuf, ret);
     if(ret>=13 && com_rbuf[0]==0 && com_rbuf[6]==0x1 )
     {
		f.ch[0]=com_rbuf[10];
		f.ch[1]=com_rbuf[9];
		f.ch[2]=com_rbuf[12];
		f.ch[3]=com_rbuf[11];		
		*valf=f.f;      	      
     }
     else
     {
        *valf=0;
		return 1;
     }

	 return 0;
}

int protocol_modtcp_DCS_GongKuang2(struct acquisition_data *acq_data)
{
    struct acquisition_ctrl  *acq_ctrl;
    char com_tbuf[4][12]={
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x28,0x9E,0x0,0x04},// 10398
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x4B,0x1A,0x0,0x04},// 19226
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x32,0xDC,0x0,0x04},// 13020
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x4B,0x22,0x0,0x04},// 19234
	};
	float valf[4]={0};
	
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	
	union uf f;
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);

     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,0,&arg_n);
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);
		ret=acqdata_set_value(acq_data,"p10302",UNIT_MW,0,&arg_n);		
     	ret=acqdata_set_value(acq_data,"p10202",UNIT_T_H,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
     }

     for(i=0;i<4;i++)
     {
     	ret=modtcp_data(acq_data,com_tbuf[i],12,&valf[i]);
		if(ret!=0)
		{
			status=1;
		}
     }

	 if(status==0)
	 {
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,valf[0],&arg_n);
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,valf[1],&arg_n);
		ret=acqdata_set_value(acq_data,"p10302",UNIT_MW,valf[2],&arg_n);		
     	ret=acqdata_set_value(acq_data,"p10202",UNIT_T_H,valf[3],&arg_n);	 	
	 }
	 else
	 {
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,0,&arg_n);
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);
		ret=acqdata_set_value(acq_data,"p10302",UNIT_MW,0,&arg_n);		
     	ret=acqdata_set_value(acq_data,"p10202",UNIT_T_H,0,&arg_n);	 
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 }

	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    NEED_ERROR_CACHE(acq_data, 10);

     return arg_n;
}

int protocol_modtcp_DCS_GongKuang3(struct acquisition_data *acq_data)
{
    struct acquisition_ctrl  *acq_ctrl;
    char com_tbuf[4][12]={
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x3c,0x33,0x0,0x04},// 15411
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x3c,0x35,0x0,0x04},// 15413
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x3a,0xa4,0x0,0x04},// 15012
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x3c,0x38,0x0,0x04},// 15416
	};
	float valf[4]={0};
	
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	
	union uf f;
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);

     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,0,&arg_n);
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);
		ret=acqdata_set_value(acq_data,"p10302",UNIT_MW,0,&arg_n);		
     	ret=acqdata_set_value(acq_data,"p10202",UNIT_T_H,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
     }

     for(i=0;i<4;i++)
     {
     	ret=modtcp_data(acq_data,com_tbuf[i],12,&valf[i]);
		if(ret!=0)
		{
			status=1;
		}
     }

	 if(status==0)
	 {
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,valf[0],&arg_n);
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,valf[1],&arg_n);
		ret=acqdata_set_value(acq_data,"p10302",UNIT_MW,valf[2],&arg_n);		
     	ret=acqdata_set_value(acq_data,"p10202",UNIT_T_H,valf[3],&arg_n);	 	
	 }
	 else
	 {
		ret=acqdata_set_value(acq_data,"p10301",UNIT_MW,0,&arg_n);
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);
		ret=acqdata_set_value(acq_data,"p10302",UNIT_MW,0,&arg_n);		
     	ret=acqdata_set_value(acq_data,"p10202",UNIT_T_H,0,&arg_n);	 
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 }

	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    NEED_ERROR_CACHE(acq_data, 10);

     return arg_n;
}


int protocol_modtcp_DCS_GongKuang4(struct acquisition_data *acq_data)
{
    struct acquisition_ctrl  *acq_ctrl;
    char com_tbuf[1][12]={
		{0x0,0x0,0x0,0x0,0x0,0x6,0x1,0x3,0x00,0x00,0x0,0x04}
	};
	float valf[4]={0};
	
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	
	union uf f;
    int ret=0;
    int i=0,j=0;

    acq_ctrl=ACQ_CTRL(acq_data);

     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);;
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
     }

     for(i=0;i<1;i++)
     {
     	ret=modtcp_data(acq_data,com_tbuf[i],12,&valf[i]);
		if(ret!=0)
		{
			status=1;
		}
     }

	 if(status==0)
	 {
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,valf[0],&arg_n);	 	
	 }
	 else
	 {
     	ret=acqdata_set_value(acq_data,"p10201",UNIT_T_H,0,&arg_n);
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 }

	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    NEED_ERROR_CACHE(acq_data, 10);

     return arg_n;
}


