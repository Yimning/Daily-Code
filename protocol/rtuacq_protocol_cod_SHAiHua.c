/******************************************************************
*
*author:Ben
*Date:2018.7.11
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


int protocol_COD_SHAiHua(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_cod;
   int crc;
   char crcbuff[2]={0};
   union uf unf;

   
      if(!acq_data) return -1;
	memset(com_tbuf,0,sizeof(com_tbuf));
   	size=modbus_pack(com_tbuf,0x1,0x3,0x1A98,0x4);
   	size=write_device(DEV_NAME(acq_data),com_tbuf,size);

	
   	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
   	SYSLOG_DBG("SHAiHua COD protocol,COD : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SHAiHua COD data",com_rbuf,size);
	
	if((size>=13)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		/*crc=CRC16_modbus(com_rbuf, 11);
		crcbuff[1]=(char)(crc>>8);
		crcbuff[0]=(char)(crc&0xff);
		printf("crc[0]=%x,crc[1]=%x",crcbuff[0],crcbuff[1]);
		if(com_rbuf[11]==crcbuff[0]&&com_rbuf[12]==crcbuff[1])
			{*/
			unf.ch[0]=com_rbuf[6];
			unf.ch[1]=com_rbuf[5];
			unf.ch[2]=com_rbuf[4];
			unf.ch[3]=com_rbuf[3];
			total_cod=unf.f;
			printf("the cod is %f\n",total_cod);
			status = 0;
			//}
	}
	else 
	{
		status=1;
		total_cod=0;
		printf("recive data error\n");
	}
	ret=acqdata_set_value(acq_data,"w01018",UNIT_MG_L,total_cod,&arg_n);
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
		{
 		acq_data->acq_status = ACQ_ERR;
		
		}

    return arg_n;
}
