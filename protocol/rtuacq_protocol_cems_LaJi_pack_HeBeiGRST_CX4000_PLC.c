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


int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_PLC(struct acquisition_data *acq_data)
{
/*
"设备地址：PLC  地址：2
串口参数：485通讯，波特率9600，无校验，数据位8，停止位1，MODBUS-RTU从站
功能码： 3"					
名称	寄存器地址	量程	原始最小值	原始最大值	备 注
Ｏ2（湿基）	40010	0-25%	5530	27648	需要在数采仪内计算为干氧
烟温	40011	0－300度	5530	27648	
流速	40012	0－40m/s	5530	27648	
颗粒物(工况）	40013	0－60 mg/m3	5530	27648	需要在数采仪内计算为标干
烟压	40014	±4000pa	5530	27648	

*/
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int arg_n=0;
	//char *pcom_rbuf,*pq;
	int i=0;
	float valf=0; 
	float ai1,ai2,ai3,ai4,ai5,ai6,ai7,ai8;
	float aivalf[9]={0};
	float aival[9]={0};
	union uf f;
	int ret=0;
	float hum=0,smoke=0,smokez=0,o2=0,temp=0,pres=0;
	char devstat=0;

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int devaddr=0;

   if(!acq_data) return -1;

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;
   devaddr=modbusarg_running->devaddr;
	
   ai1=ai2=ai3=ai4=ai5=ai6=ai7=ai8=0.0;
	

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x09,0x05);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000_PLC CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("CX4000_PLC  CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CX4000_PLC CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000_PLC RECV:",com_rbuf,size);

	if(modbus_crc_check_ok(com_rbuf, size, devaddr, 0x03,0x05)!=0) goto ERROR_PACK;

 	if(size>=15 && devaddr==com_rbuf[0])
	{
        for(i=0;i<size && i<modbusarg_running->regcnt && i<8;i++)
        {
            int val=0;
			val=com_rbuf[3+i*2];
			val<<=8;
			val+=com_rbuf[4+i*2];
			aival[i]=val;

			if(aival[i]>5530)
			 aival[i]=(aival[i]-5530)*16/22118.0+4;
			else 
			 aival[i]=4;

            aivalf[i]=(aival[i]-4)/(20-4);
			SYSLOG_DBG("CX4000_PLC %d,%d,%f,%f\n",i,val,aival[i],aivalf[i]);

        }
 	}
	else
	{

ERROR_PACK:
		status=1;
	}

	for(i=0;i<modbusarg_running->array_count ;i++)
	{
	    int pos=-1;
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		//if(!strcmp(mpolcodearg->polcode,"a34013z")) continue;
		//if(!strcmp(mpolcodearg->polcode,"a34013")) continue;
		//if(!strcmp(mpolcodearg->polcode,"a19001")) continue;		
		if(mpolcodearg->enableFlag==1)
		{
		    pos=-1;
		    if(!strcmp(mpolcodearg->polcode,"a19001a"))pos=0;
		    if(!strcmp(mpolcodearg->polcode,"a01012"))pos=1;
		    if(!strcmp(mpolcodearg->polcode,"a01011"))pos=2;
		    if(!strcmp(mpolcodearg->polcode,"a34013a"))pos=3;
		    if(!strcmp(mpolcodearg->polcode,"a01013"))pos=4;
		    
			
			if(pos>=0 && pos<=7 && mpolcodearg->algorithm!=1)
			{
			    aivalf[pos] *=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			    aivalf[pos] +=mpolcodearg->alarmMin;
				SYSLOG_DBG("ai%d pos=%d, CX4000_PLC %s valf:%f,max %f,min %f\n",i,pos,
					mpolcodearg->polcode,aivalf[pos],mpolcodearg->alarmMax,mpolcodearg->alarmMin);
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,aivalf[pos],&arg_n);			
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	    
	}
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x01,0x00,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"CX4000_PLC stat CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("CX4000_PLC stat CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("CX4000_PLC stat CEMS",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"CX4000_PLC stat RECV:",com_rbuf,size);

   if(size>=4 && devaddr==com_rbuf[0])
   {
   		if((com_rbuf[3] & 0x1)==0) devstat='D';
		if(com_rbuf[3] & (0x1<<1)) devstat='M';
		if(com_rbuf[3] & (0x1<<3)) devstat='C';
   }

   acq_data->dev_stat=devstat;

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}



