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

#include "rtuacq_protocol_error_cache.h"


static int HeBeiGRST_cems_get_valf(char *com_rbuf,unsigned int size,unsigned int regaddr,float *valf)
{
	union uf  f;
	unsigned int datalen=0;
	int datapos=0;

	if(!com_rbuf || !valf) return -1;

	if(regaddr<0) return -1;

	datapos=7+(regaddr/2)*4;

	f.ch[0]=com_rbuf[1+datapos];
	f.ch[1]=com_rbuf[0+datapos];
	f.ch[2]=com_rbuf[3+datapos];
	f.ch[3]=com_rbuf[2+datapos];
	
	*valf=f.f;

	SYSLOG_DBG("HeBeiGRST_cems_get_valf %d,%x,%f\n",regaddr,f.l&0xffffffff,f.f);
		
	return 0;
}

static struct modbus_polcode_arg * HeBeiGRST_cems_find_arg_by_polcode(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode)
{
	int i=0;

	if(!mpolcodearg ||num<=0 || !polcode) return NULL;

	for(i=0;i<num;i++)
	{
		if(!strcmp(mpolcodearg[i].polcode,polcode)) 
			return &mpolcodearg[i];
	}

	return NULL;
}


int protocol_CEMS_LaJi_pack_HeBeiGRST(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char *pcom_rbuf=NULL;
   char com_tbuf[8]={0};
   char *pcomrbuff,*pcomrbuff2;
   int size=0;
   int arg_n=0;
   int ret=0;
   float valf=0; 
   union uf f;

#define HeBeiGRST_CEMS_POLCODE_NUM 12
	char *polcodestr[HeBeiGRST_CEMS_POLCODE_NUM]={
		"a01901","a01902","i33310","i33311","i33312",
		"i33320","i33321","i33322","i33330","i33331",
		"i33332","i33300"
	};
	UNIT_DECLARATION unitstr[HeBeiGRST_CEMS_POLCODE_NUM]={
		UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,
		UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,UNIT_0C,
		UNIT_0C,UNIT_0C
	};

	int r_addr[HeBeiGRST_CEMS_POLCODE_NUM]={
		0,	2,	4,	6,  8,
		10,	12,	14,	16,18,	
		20,	22
	};

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int devaddr=0,devfun=0;
   
   int i=0;

   if(!acq_data) return -1;

   acq_data->total=0;

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg; 
   devaddr=modbusarg_running->devaddr;
   devfun=modbusarg_running->devfun;

	//memset(com_tbuf,0,sizeof(com_tbuf));
	//size=modbus_pack(com_tbuf, devaddr, devfun, modbusarg_running->devstaddr&0xff, modbusarg_running->regcnt&0xff);
	//size=modbus_pack(com_tbuf, devaddr, devfun, modbusarg_running->devstaddr&0xff,33);
	//SYSLOG_DBG_HEX("HeBeiGRST CEMS data send",com_tbuf,size);
	//size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	if(size>6)
	{
	    sleep(3);
		size=read_device(DEV_NAME(acq_data),&com_rbuf[size], 2048-size)+size;
	}

	SYSLOG_DBG("HeBeiGRST CEMS protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HeBeiGRST CEMS data RECV",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBeiGRST CEMS data1 RECV:",com_rbuf,size);


	if(size>=(modbusarg_running->regcnt*2) //&& com_rbuf[0]==devaddr && com_rbuf[1]==devfun //del by miles zhang 20210722
		)
	{//error packet HeBeiGRST CEMS data2 RECV: size=50,01 10 00 00 00 18 30 14 8d 43 27 0c e6 42 01 10 00 00 00 18 30 11 f0 43 27 c8 f3 42 0b ae e1 42 0a 7f b6 42 07 50 d6 42 0a c8 f3 42 0b 3b d9 42 08 7a
	    unsigned crc=0;
        char buffstr[6]={0};
		buffstr[0]=devaddr;
		buffstr[1]=devfun;
		buffstr[2]=0x00;
		buffstr[3]=0x00;
		buffstr[4]=0x00;

		if(size>1960) size=size-70;//added by miles zhang 20210722
		
	    pcomrbuff=memstrlen(com_rbuf,size,buffstr,5); 
		if(pcomrbuff==NULL) goto ERROR_PACK;
		
		pcomrbuff2=memstrlen(pcomrbuff,55,buffstr,5); 
		if(pcomrbuff2!=NULL && pcomrbuff2[2]==0 && pcomrbuff2[3]==0 && pcomrbuff2[4]==0) 
			pcomrbuff=pcomrbuff2; //added by miles zhang 20210724
		
		if(pcomrbuff[2]!=0 || pcomrbuff[3]!=0 ||pcomrbuff[4]!=0) goto ERROR_PACK;
		crc=CRC16_modbus(pcomrbuff, 0x30+0x07); //added by miles zhang 20210724
	    if((pcomrbuff[0x30+0x07]!=((crc>>0)&0xFF)) || (pcomrbuff[0x30+0x07+1]!=((crc>>8)&0xFF)))
		goto ERROR_PACK; //crc error

		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBeiGRST CEMS data2 RECV:",pcomrbuff,70); //added by miles zhang 20210722
	
		for(i=0;i<HeBeiGRST_CEMS_POLCODE_NUM;i++)
		{
			mpolcodearg_temp=HeBeiGRST_cems_find_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			if(mpolcodearg_temp)
			{
				valf=0.0;
				ret=HeBeiGRST_cems_get_valf(pcomrbuff,size,(r_addr[i]),&valf);
				
				if(ret<0) 
				{
					valf=0.0;
				}
			}


			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
		}
	}
    else
    {
    
ERROR_PACK:    
	    ret=acqdata_set_value(acq_data,"a01901",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"a01902",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33332",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33331",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33330",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33312",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33311",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33310",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33322",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33321",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33320",UNIT_0C,0,&arg_n);

		ret=acqdata_set_value(acq_data,"i33342",UNIT_0C,0,&arg_n);
		ret=acqdata_set_value(acq_data,"i33341",UNIT_0C,0,&arg_n);	
		ret=acqdata_set_value(acq_data,"i33300",UNIT_0C,0,&arg_n);	
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}


int protocol_CEMS_LaJi_pack_HeBeiGRST_GongKuang(struct acquisition_data *acq_data)
{
/*
RX:01 10 00 18 00 10 20 00 00 00 00 00 00 42 F0 2B 3C 40 7A 00 50 3D C3 D8 DD 41 00 67 B4 44 E1 00 00 00 00 00 00 00 00 76 D6 01 10 00 18 00 10 20 00 00 00 00 00 00 42 F0 2B 3C 40 7A 00 50 3D C3 D8 DD 41 00 67 B4 44 E1 00 00 00 00 00 00 00 00 76 D6 01 10 00 18 00 10 20 00 00 00 00 00 00 42 F0 2B 3C 40 7A 00 50 3D C3 D8 DD 41 00 67 B4 44 E1 00 00 00 00 00 00 00 00 76 D6
*/
   int status=0;
   char com_rbuf[2048]={0}; 
   char *pcom_rbuf=NULL;
   char com_tbuf[8]={0};
   char *pcomrbuff,*pcomrbuff2;
   int size=0;
   int arg_n=0;
   int ret=0;
   float valf=0; 
   union uf f;

#define HeBeiGRST_CEMS_POLCODE_GONGKUANG_NUM 6
	char *polcodestr[HeBeiGRST_CEMS_POLCODE_GONGKUANG_NUM]={
		"CSXTLJCLL","TXSSTXHYJYL","TLSSBGFSHJYL","TLSSGFSHSYL","HXTTJXTHXTYL",
		"CCSSDSCCQJCKYC"
	};
	UNIT_DECLARATION unitstr[HeBeiGRST_CEMS_POLCODE_GONGKUANG_NUM]={
		UNIT_NONE,UNIT_NONE,UNIT_KG,UNIT_KG,UNIT_KG,
		UNIT_V
	};

	int r_addr[HeBeiGRST_CEMS_POLCODE_GONGKUANG_NUM]={
		0,	2,	4,	6,  8,
		10
	};

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   int devaddr=0,devfun=0;
   
   int i=0;

   if(!acq_data) return -1;

   acq_data->total=0;

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg; 
   devaddr=modbusarg_running->devaddr;
   devfun=modbusarg_running->devfun;

	//memset(com_tbuf,0,sizeof(com_tbuf));
	//size=modbus_pack(com_tbuf, devaddr, devfun, modbusarg_running->devstaddr&0xff, modbusarg_running->regcnt&0xff);
	//size=modbus_pack(com_tbuf, devaddr, devfun, modbusarg_running->devstaddr&0xff,33);
	//SYSLOG_DBG_HEX("HeBeiGRST CEMS data send",com_tbuf,size);
	//size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	if(size>6)
	{
	    sleep(3);
		size=read_device(DEV_NAME(acq_data),&com_rbuf[size], 2048-size)+size;
	}

	SYSLOG_DBG("HeBeiGRST CEMS gk protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HeBeiGRST CEMS gk data RECV",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBeiGRST CEMS gk data1 RECV:",com_rbuf,size);


	if(size>=(modbusarg_running->regcnt*2) //&& com_rbuf[0]==devaddr && com_rbuf[1]==devfun //del by miles zhang 20210722
		)
	{//RX:01 10 00 18 00 10 20 00 00 00 00 00 00 42 F0 2B 3C 40 7A 00 50 3D C3 D8 DD 41 00 67 B4 44 E1 00 00 00 00 00 00 00 00 76 D6 01 10 00 18 00 10 20 00 00 00 00 00 00 42 F0 2B 3C 40 7A 00 50 3D C3 D8 DD 41 00 67 B4 44 E1 00 00 00 00 00 00 00 00 76 D6 01 10 00 18 00 10 20 00 00 00 00 00 00 42 F0 2B 3C 40 7A 00 50 3D C3 D8 DD 41 00 67 B4 44 E1 00 00 00 00 00 00 00 00 76 D6
	    unsigned crc=0;
        char buffstr[6]={0};
		buffstr[0]=0x01;
		buffstr[1]=0x10;
		buffstr[2]=0x00;

		if(size>1960) size=size-70;//added by miles zhang 20210722
		
	    pcomrbuff=memstr(com_rbuf,size,buffstr); 
		if(pcomrbuff==NULL) goto ERROR_PACK;
		
		pcomrbuff2=memstr(pcomrbuff,30,buffstr); 
		if(pcomrbuff2!=NULL && pcomrbuff2[2]==0 && pcomrbuff2[3]==0x18 && pcomrbuff2[4]==0) 
			pcomrbuff=pcomrbuff2; //added by miles zhang 20210724
		
		if(pcomrbuff[2]!=0 || pcomrbuff[3]!=0x18 ||pcomrbuff[4]!=0) goto ERROR_PACK;
		crc=CRC16_modbus(pcomrbuff, 0x20+0x07); //added by miles zhang 20210724
	    if((pcomrbuff[0x20+0x07]!=((crc>>0)&0xFF)) || (pcomrbuff[0x20+0x07+1]!=((crc>>8)&0xFF)))
		goto ERROR_PACK; //crc error

		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HeBeiGRST CEMS gk data2 RECV:",pcomrbuff,70); //added by miles zhang 20210722
	
		for(i=0;i<HeBeiGRST_CEMS_POLCODE_GONGKUANG_NUM;i++)
		{
			mpolcodearg_temp=HeBeiGRST_cems_find_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			if(mpolcodearg_temp)
			{
				valf=0.0;
				ret=HeBeiGRST_cems_get_valf(pcomrbuff,size,(r_addr[i]),&valf);
				
				if(ret<0) 
				{
					valf=0.0;
				}
				
				acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
			}
		}
	}
    else
    {
    
ERROR_PACK:    
	    ret=acqdata_set_value(acq_data,"CSXTLJCLL",UNIT_NONE,0,&arg_n);
		ret=acqdata_set_value(acq_data,"TXSSTXHYJYL",UNIT_NONE,0,&arg_n);
		ret=acqdata_set_value(acq_data,"TLSSBGFSHJYL",UNIT_KG,0,&arg_n);
		ret=acqdata_set_value(acq_data,"TLSSGFSHSYL",UNIT_KG,0,&arg_n);
		ret=acqdata_set_value(acq_data,"HXTTJXTHXTYL",UNIT_KG,0,&arg_n);
		ret=acqdata_set_value(acq_data,"CCSSDSCCQJCKYC",UNIT_V,0,&arg_n);
		status=1;
	}

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,20);

    return arg_n;
}




