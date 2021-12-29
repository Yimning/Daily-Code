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



int protocol_modtcp_C2000_SAX0800_1(struct acquisition_data *acq_data)
{
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x01,0x0,0x0,0x0,0x6,0xff,0x3,0x2,0x58,0x0,0x10};
    char modbus_cache[1000]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;
    float valuef[8]={0};
    unsigned int aival[8]={0};

    acq_ctrl=ACQ_CTRL(acq_data);

     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
     	ret=acqdata_set_value(acq_data,"302b",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302c",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302d",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302e",UNIT_PH,0,&arg_n);
		
		ret=acqdata_set_value(acq_data,"302j",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302k",UNIT_PH,0,&arg_n);
		
		ret=acqdata_set_value(acq_data,"AI7",UNIT_NONE,0,&arg_n);
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

	   for(i=0;i<8;i++)
	   {
          aival[i] = modbus_cache[i*4+2+9]&0xff;
          aival[i] <<=8;
	      aival[i]+=modbus_cache[i*4+3+9]&0xff;

		  aival[i] <<=16;
		  
          aival[i] += (modbus_cache[i*4+9]&0xff)<<8;
	      aival[i] +=modbus_cache[i*4+1+9]&0xff;

		  memcpy(&valuef[i],&aival[i],4);
		  valuef[i]/=1000; // 20000/1000 = 20ma
	   }

	   printf("current : %x %x %x %x %x %x %x %x \n",
	   				aival[0],aival[1],aival[2],aival[3],aival[4],aival[5],aival[6],aival[7]);

	   printf("current : %.2f %.2f %.2f %.2f %.2f %.2f,%.2f %.2f \n",
	   				valuef[0],valuef[1],valuef[2],valuef[3],valuef[4],valuef[5],valuef[6],valuef[7]);


        valuef[0] = (valuef[0]-4)/(20-4)*14;
		valuef[1] = (valuef[1]-4)/(20-4)*14;
		valuef[2] = (valuef[2]-4)/(20-4)*14;
		valuef[3] = (valuef[3]-4)/(20-4)*14;

		valuef[4] = (valuef[4]-4)/(20-4)*14;
		valuef[5] = (valuef[5]-4)/(20-4)*14;

		valuef[6] = (valuef[6]-4)/(20-4);
		
		valuef[7] = (valuef[7]-4)/(20-4)*3000-1500;
		

     	ret=acqdata_set_value(acq_data,"302b",UNIT_PH,valuef[0],&arg_n);
		ret=acqdata_set_value(acq_data,"302c",UNIT_PH,valuef[1],&arg_n);
		ret=acqdata_set_value(acq_data,"302d",UNIT_PH,valuef[2],&arg_n);
		ret=acqdata_set_value(acq_data,"302e",UNIT_PH,valuef[3],&arg_n);

		ret=acqdata_set_value(acq_data,"302j",UNIT_PH,valuef[4],&arg_n);
		ret=acqdata_set_value(acq_data,"302k",UNIT_PH,valuef[5],&arg_n);
		
		ret=acqdata_set_value(acq_data,"AI7",UNIT_NONE,valuef[6],&arg_n);
		ret=acqdata_set_value(acq_data,"316c",UNIT_M_V,valuef[7],&arg_n);	      

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
		
		ret=acqdata_set_value(acq_data,"AI7",UNIT_NONE,0,&arg_n);
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

int protocol_modtcp_C2000_SAX0800_2(struct acquisition_data *acq_data)
{
    struct acquisition_ctrl  *acq_ctrl;
    char modbus_data[]={0x0,0x01,0x0,0x0,0x0,0x6,0xff,0x3,0x2,0x58,0x0,0x10};
    char modbus_cache[100]={0};
    float *pf; 
    int arg_n=0;
    int size=0;
    int status=0;	 
    int ret=0;
    int i=0,j=0;
    float valuef[8]={0};
    unsigned int aival[8]={0};

    acq_ctrl=ACQ_CTRL(acq_data);

     if(acq_ctrl->link_stat != ACQ_LINK_CONNECTED)
     {
     	ret=acqdata_set_value(acq_data,"302f",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302g",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302h",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302i",UNIT_PH,0,&arg_n);
		
		ret=acqdata_set_value(acq_data,"302m",UNIT_PH,0,&arg_n);
		ret=acqdata_set_value(acq_data,"302n",UNIT_PH,0,&arg_n);
		
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

	   for(i=0;i<8;i++)
	   {
          aival[i] = modbus_cache[i*4+2+9]&0xff;
          aival[i] <<=8;
	      aival[i]+=modbus_cache[i*4+3+9]&0xff;

		  aival[i] <<=16;
		  
          aival[i] += (modbus_cache[i*4+9]&0xff)<<8;
	      aival[i] +=modbus_cache[i*4+1+9]&0xff;

		  memcpy(&valuef[i],&aival[i],4);
		  valuef[i]/=1000; // 20000/1000 = 20ma
	   }


	   printf("current : %.2f %.2f %.2f %.2f %.2f %.2f,%.2f %.2f \n",
	   				valuef[0],valuef[1],valuef[2],valuef[3],valuef[4],valuef[5],valuef[6],valuef[7]);


        valuef[0] = (valuef[0]-4)/(20-4)*14;
		valuef[1] = (valuef[1]-4)/(20-4)*14;
		valuef[2] = (valuef[2]-4)/(20-4)*14;
		valuef[3] = (valuef[3]-4)/(20-4)*14;

		valuef[4] = (valuef[4]-4)/(20-4)*3000-1500;

		valuef[5] = (valuef[5]-4)/(20-4)*14;
		valuef[6] = (valuef[6]-4)/(20-4)*14;
		
		valuef[7] = (valuef[7]-4)/(20-4)*3000-1500;
		

     	ret=acqdata_set_value(acq_data,"302f",UNIT_PH,valuef[0],&arg_n);
		ret=acqdata_set_value(acq_data,"302g",UNIT_PH,valuef[1],&arg_n);
		ret=acqdata_set_value(acq_data,"302h",UNIT_PH,valuef[2],&arg_n);
		ret=acqdata_set_value(acq_data,"302i",UNIT_PH,valuef[3],&arg_n);

		ret=acqdata_set_value(acq_data,"302m",UNIT_PH,valuef[5],&arg_n);
		ret=acqdata_set_value(acq_data,"302n",UNIT_PH,valuef[6],&arg_n);
		
		ret=acqdata_set_value(acq_data,"316d",UNIT_M_V,valuef[4],&arg_n);
		ret=acqdata_set_value(acq_data,"316e",UNIT_M_V,valuef[7],&arg_n);	      

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



