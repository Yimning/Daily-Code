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



int protocol_modtcp_NET_4021M1(struct acquisition_data *acq_data)
{//modbus tcp
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x27,0x0,0x0,0x0,0x6,0x01,0x4,0x0,0x40,0x0,0x10};
    //char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x5,0x00,0x2,0x0,0xaa,0x66};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;
    float valuef[15]={0};
    unsigned short aival[15]={0};

    acq_ctrl=ACQ_CTRL(acq_data);

     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
     	ret=acqdata_set_value(acq_data,"302b",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302c",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302d",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302e",UNIT_PH,0,&arg_n);
		
		ret=acqdata_set_value(acq_data,"302j",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302k",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302l",UNIT_PH,0,&arg_n);
		
		ret=acqdata_set_value(acq_data,"316b",UNIT_M_V,0,&arg_n);
		ret=acqdata_set_value(acq_data,"316c",UNIT_M_V,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }

#if 1
     pf=acq_data->com_f;  

     //printf("sizeof(modbus_data) = %d\n",sizeof(modbus_data));
     //ret=write_device(DEV_NAME(acq_data),modbus_data,12);
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 status=1;
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=41)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 

	   for(i=0;i<16;i++)
	   {
          aival[i] = modbus_cache[i*2+9]&0xff;
          aival[i] <<=8;
	      aival[i]+=modbus_cache[i*2+1+9]&0xff;
	   }

	   //printf("value %.4x\n",value&0xffff);

	   valuef[0]=aival[0];
	   valuef[0]=valuef[0]/0xfff*(10*2)-10;// 0~5v
	   valuef[1]=aival[1];
	   valuef[1]=valuef[1]/0xfff*(10*2)-10;// 0~5v
	   valuef[2]=aival[2];
	   valuef[2]=valuef[2]/0xfff*(10*2)-10;// 0~5v
	   valuef[3]=aival[3];
	   valuef[3]=valuef[3]/0xfff*(10*2)-10;// 0~5v
	   valuef[4]=aival[4];
	   valuef[4]=valuef[4]/0xfff*(10*2)-10;// 0~5v
	   valuef[5]=aival[5];
	   valuef[5]=valuef[5]/0xfff*(10*2)-10;// 0~5v

	   valuef[6]=aival[6];
	   valuef[6]=valuef[6]/0xfff*(10*2)-10;// 0~5v
	   valuef[7]=aival[7];
	   valuef[7]=valuef[7]/0xfff*(10*2)-10;// 0~5v
	   valuef[8]=aival[8];
	   valuef[8]=valuef[8]/0xfff*(10*2)-10;// 0~5v

	   printf("voltage1 : %.2f %.2f %.2f %.2f %.2f %.2f,%.2f %.2f %.2f\n",
	   				valuef[0],valuef[1],valuef[2],valuef[3],valuef[4],valuef[5],valuef[6],valuef[7],valuef[8]);


        valuef[0] = (valuef[0]-1)/(5-1)*14;
		valuef[1] = (valuef[1]-1)/(5-1)*14;
		valuef[2] = (valuef[2]-1)/(5-1)*14;
		valuef[3] = (valuef[3]-1)/(5-1)*14;

		valuef[4] = (valuef[4]-1)/(5-1)*3000-1500;

		valuef[5] = (valuef[5]-1)/(5-1)*14;
		valuef[6] = (valuef[6]-1)/(5-1)*14;
		valuef[7] = (valuef[7]-1)/(5-1)*14;
		
		valuef[8] = (valuef[8]-1)/(5-1)*3000-1500;
		

     	ret=acqdata_set_value(acq_data,"302b",UNIT_PH,valuef[0],&arg_n);
		ret=acqdata_set_value(acq_data,"302c",UNIT_PH,valuef[1],&arg_n);
		ret=acqdata_set_value(acq_data,"302d",UNIT_PH,valuef[2],&arg_n);
		ret=acqdata_set_value(acq_data,"302e",UNIT_PH,valuef[3],&arg_n);

		ret=acqdata_set_value(acq_data,"302j",UNIT_PH,valuef[5],&arg_n);
		ret=acqdata_set_value(acq_data,"302k",UNIT_PH,valuef[6],&arg_n);
		ret=acqdata_set_value(acq_data,"302l",UNIT_PH,valuef[7],&arg_n);
		
		ret=acqdata_set_value(acq_data,"316b",UNIT_M_V,valuef[4],&arg_n);
		ret=acqdata_set_value(acq_data,"316c",UNIT_M_V,valuef[8],&arg_n);	      

     }
     else
     {
	    printf("read modbus error %s,ret=%d\n",DEV_NAME(acq_data),ret);
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
		
     	ret=acqdata_set_value(acq_data,"302b",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302c",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302d",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302e",UNIT_PH,0,&arg_n);

		ret=acqdata_set_value(acq_data,"302j",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302k",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302l",UNIT_PH,0,&arg_n);
		
		ret=acqdata_set_value(acq_data,"316b",UNIT_M_V,0,&arg_n);
		ret=acqdata_set_value(acq_data,"316c",UNIT_M_V,0,&arg_n);
		
		status=1;
     }
#else
     	ret=acqdata_set_value(acq_data,"302b",UNIT_PH,1,&arg_n);
		ret=acqdata_set_value(acq_data,"302c",UNIT_PH,1,&arg_n);
		ret=acqdata_set_value(acq_data,"302d",UNIT_PH,1,&arg_n);
		ret=acqdata_set_value(acq_data,"302e",UNIT_PH,1,&arg_n);
		ret=acqdata_set_value(acq_data,"316b",UNIT_M_V,1,&arg_n);
		ret=acqdata_set_value(acq_data,"316c",UNIT_M_V,1,&arg_n);
#endif

	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;

}


/*
			ret=acqdata_set_value(acq_data,"302f",UNIT_PH,0,&arg_n);
			ret=acqdata_set_value(acq_data,"302g",UNIT_PH,0,&arg_n);
			ret=acqdata_set_value(acq_data,"302h",UNIT_PH,0,&arg_n);
			ret=acqdata_set_value(acq_data,"302i",UNIT_PH,0,&arg_n);
*/

int protocol_modtcp_NET_4021M2(struct acquisition_data *acq_data)
{
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x27,0x0,0x0,0x0,0x6,0x01,0x4,0x0,0x40,0x0,0x10};
    //char modbus_data[]={0x0,0x0,0x0,0x0,0x0,0x5,0x00,0x2,0x0,0xaa,0x66};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;
    float valuef[15]={0};
    unsigned short aival[15]={0};

    acq_ctrl=ACQ_CTRL(acq_data);

     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
     	ret=acqdata_set_value(acq_data,"302f",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302g",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302h",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302i",UNIT_PH,0,&arg_n);

		ret=acqdata_set_value(acq_data,"302m",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302n",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302o",UNIT_PH,0,&arg_n);
		
		ret=acqdata_set_value(acq_data,"316d",UNIT_M_V,0,&arg_n);
		ret=acqdata_set_value(acq_data,"316e",UNIT_M_V,0,&arg_n);
		acq_data->acq_status = ACQ_ERR;
		read_rtc_time(acq_data->acq_tm); 
		return arg_n;
     }

#if 1
     pf=acq_data->com_f;  

     //printf("sizeof(modbus_data) = %d\n",sizeof(modbus_data));
     //ret=write_device(DEV_NAME(acq_data),modbus_data,12);
     ret=write_device(DEV_NAME(acq_data),modbus_data,sizeof(modbus_data));
     if(ret!=sizeof(modbus_data))
     {
        pf[arg_n] = 0.0;
        acq_data->acq_status = ACQ_ERR;
	 arg_n=0;	
	 printf("write modbus error %s\n",DEV_NAME(acq_data));
	 acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
	 status=1;
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),modbus_cache,sizeof(modbus_cache));
     if(ret>=41)
     {
	   printf("read modbus : \n");
	   for(i=0;i<ret;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 

	   for(i=0;i<16;i++)
	   {
          aival[i] = modbus_cache[i*2+9]&0xff;
          aival[i] <<=8;
	      aival[i]+=modbus_cache[i*2+1+9]&0xff;
	   }

	   //printf("value %.4x\n",value&0xffff);

	   valuef[0]=aival[0];
	   valuef[0]=valuef[0]/0xfff*(10*2)-10;// 0~5v
	   valuef[1]=aival[1];
	   valuef[1]=valuef[1]/0xfff*(10*2)-10;// 0~5v
	   valuef[2]=aival[2];
	   valuef[2]=valuef[2]/0xfff*(10*2)-10;// 0~5v
	   valuef[3]=aival[3];
	   valuef[3]=valuef[3]/0xfff*(10*2)-10;// 0~5v
	   valuef[4]=aival[4];
	   valuef[4]=valuef[4]/0xfff*(10*2)-10;// 0~5v
	   valuef[5]=aival[5];
	   valuef[5]=valuef[5]/0xfff*(10*2)-10;// 0~5v

	   valuef[6]=aival[6];
	   valuef[6]=valuef[6]/0xfff*(10*2)-10;// 0~5v
	   valuef[7]=aival[7];
	   valuef[7]=valuef[7]/0xfff*(10*2)-10;// 0~5v
	   valuef[8]=aival[8];
	   valuef[8]=valuef[8]/0xfff*(10*2)-10;// 0~5v

	   printf("voltage2 : %.2f %.2f %.2f %.2f %.2f %.2f,%.2f %.2f %.2f\n",
	   				valuef[0],valuef[1],valuef[2],valuef[3],valuef[4],valuef[5],valuef[6],valuef[7],valuef[8]);


        valuef[0] = (valuef[0]-1)/(5-1)*14;
		valuef[1] = (valuef[1]-1)/(5-1)*14;
		valuef[2] = (valuef[2]-1)/(5-1)*14;
		valuef[3] = (valuef[3]-1)/(5-1)*14;

		valuef[4] = (valuef[4]-1)/(5-1)*3000-1500;

		valuef[5] = (valuef[5]-1)/(5-1)*14;
		valuef[6] = (valuef[6]-1)/(5-1)*14;
		valuef[7] = (valuef[7]-1)/(5-1)*14;
		
		valuef[8] = (valuef[8]-1)/(5-1)*3000-1500;
		

     	ret=acqdata_set_value(acq_data,"302f",UNIT_PH,valuef[0],&arg_n);
		ret=acqdata_set_value(acq_data,"302g",UNIT_PH,valuef[1],&arg_n);
		ret=acqdata_set_value(acq_data,"302h",UNIT_PH,valuef[2],&arg_n);
		ret=acqdata_set_value(acq_data,"302i",UNIT_PH,valuef[3],&arg_n);

		ret=acqdata_set_value(acq_data,"302m",UNIT_PH,valuef[5],&arg_n);
		ret=acqdata_set_value(acq_data,"302n",UNIT_PH,valuef[6],&arg_n);
		ret=acqdata_set_value(acq_data,"302o",UNIT_PH,valuef[7],&arg_n);
		
		ret=acqdata_set_value(acq_data,"316d",UNIT_M_V,valuef[4],&arg_n);
		ret=acqdata_set_value(acq_data,"316e",UNIT_M_V,valuef[8],&arg_n);	      

     }
     else
     {
	    printf("read modbus error %s,ret=%d\n",DEV_NAME(acq_data),ret);
		acq_ctrl->link_stat=ACQ_LINK_DISCONNECT;
		
     	ret=acqdata_set_value(acq_data,"302f",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302g",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302h",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302i",UNIT_PH,0,&arg_n);

		ret=acqdata_set_value(acq_data,"302m",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302n",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302o",UNIT_PH,0,&arg_n);
		
		ret=acqdata_set_value(acq_data,"316d",UNIT_M_V,0,&arg_n);
		ret=acqdata_set_value(acq_data,"316e",UNIT_M_V,0,&arg_n);
		
		status=1;
     }
#else
     	ret=acqdata_set_value(acq_data,"302b",UNIT_PH,1,&arg_n);
		ret=acqdata_set_value(acq_data,"302c",UNIT_PH,1,&arg_n);
		ret=acqdata_set_value(acq_data,"302d",UNIT_PH,1,&arg_n);
		ret=acqdata_set_value(acq_data,"302e",UNIT_PH,1,&arg_n);
		ret=acqdata_set_value(acq_data,"316b",UNIT_M_V,1,&arg_n);
		ret=acqdata_set_value(acq_data,"316c",UNIT_M_V,1,&arg_n);
#endif

	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    read_rtc_time(acq_data->acq_tm); 

     return arg_n;
}

