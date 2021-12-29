/******************************************************************
*
*author:Ben
*Date:2018.7.12
*debug_date:2018.7.13
*********************************************************************/
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


int protocol_COD_DaoJin4100CN(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[100]={0};
   char buf1[100]={0},buf2[100]={0};
   int tem1,tem2;
   int size=0;
   int arg_n=0;
   int ret=0,ret1=0;
   float total_cod=0;
   int i=0;
   char pch[100]={0};
      if(!acq_data) return -1;

	memset(com_tbuf,0,sizeof(com_tbuf));
		com_tbuf[0]=0x23;//init the com_tbuf to###001R1***
		com_tbuf[1]=0x23;
		com_tbuf[2]=0x23;
		com_tbuf[3]=0x30;
		com_tbuf[4]=0x30;
		com_tbuf[5]=0x31;
		com_tbuf[6]=0x52;
		com_tbuf[7]=0x31;
		com_tbuf[8]=0x2A;
		com_tbuf[9]=0x2A;
		com_tbuf[10]=0x2A;
	size=11;
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	
   	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	for(i=0;i<3;i++)
	{
   		    size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	        SYSLOG_DBG("DaoJin4100CN COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	        SYSLOG_DBG("DaoJin4100CN COD data %s\n",com_rbuf);
	
			if((size>=40)&&(com_rbuf[0]=='#')&&(com_rbuf[1]=='#')&&(com_rbuf[2]=='#'))
				{
					
					strncpy(pch,&com_rbuf[24],20);
					printf("the copy string is %s\n",pch);
					ret=sscanf(pch,"%[^.].%[^.].",buf1,buf2);
					printf("the buf1 is %s,the buf2 is %s\n",buf1,buf2);
					status=0;
					if(ret1!=2)
					  {
							printf("sscanf error,%s\n",pch);
							status=1;
						}
					if(strlen(buf2)<4)
					  {
							printf("error,%d\n",tem2);
							status=1;
						}
					tem1=atoi(buf1);
					buf2[3]=0;
					tem2=atoi(buf2);
					total_cod=(float)tem1+((float)tem2)/1000;
					printf("the cod is %f\n",total_cod);
					break;
				}
				
			else 
				 {
			 		sleep(1);
					printf("error,recive data is %s\n",com_rbuf);
					status=1;
				}
		}
		if(status==1)
		{
			total_cod=0;
		}
		ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else {
 		acq_data->acq_status = ACQ_ERR;
		
		}

    return arg_n;
}
