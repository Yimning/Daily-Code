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

char CEMS_JuGuang_HeBei_STATUS;

static int JuGuang_datatype2valf(char *com_rbuf,MODBUS_DATA_TYPE datatype,float *valf)
{
    union uf f;
	
    switch(datatype)
    {
        case INT_AB:
		//case INT_BA:
			f.s=com_rbuf[0];
		    f.s<<=8;
			f.s+=com_rbuf[1];
			*valf=f.s;
		break;
		case INT_BA:
		//case INT_AB:
			//f.ch[0]=com_rbuf[1];
			//f.ch[1]=com_rbuf[0];
			f.s=com_rbuf[0];
		    f.s<<=8;
			f.s+=com_rbuf[1];
			*valf=f.s;
			*valf/=100;
		break;
        case LONG_ABCD:
		//case LONG_DCBA:
			//f.ch[0]=com_rbuf[0];
			//f.ch[1]=com_rbuf[1];
			//f.ch[2]=com_rbuf[2];
			//f.ch[3]=com_rbuf[3];
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];

			*valf=f.l;
		break;
		case LONG_CDAB:
		//case LONG_BADC:
			f.l=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];
		    f.l<<=8;
			f.l+=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];	
			*valf=f.l;
		break;
        case LONG_BADC:
		//case LONG_CDAB:
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];
			*valf=f.l/100;
		break;
		case LONG_DCBA:
		//case LONG_ABCD:
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];	
			*valf=f.l/1000;
		break;		
		//case FLOAT_ABCD:
		case FLOAT_DCBA:
			f.ch[0]=com_rbuf[0];
			f.ch[1]=com_rbuf[1];
			f.ch[2]=com_rbuf[2];
			f.ch[3]=com_rbuf[3];
			*valf=f.f;
		break;			
		//case FLOAT_CDAB:
		case FLOAT_BADC:
			f.ch[0]=com_rbuf[2];
			f.ch[1]=com_rbuf[3];
			f.ch[2]=com_rbuf[0];
			f.ch[3]=com_rbuf[1];	
			*valf=f.f;
		break;
        //case FLOAT_BADC:
        case FLOAT_CDAB:
			f.ch[0]=com_rbuf[1];
			f.ch[1]=com_rbuf[0];
			f.ch[2]=com_rbuf[3];
			f.ch[3]=com_rbuf[2];
			*valf=f.f;
		break;
		//case FLOAT_DCBA:
		case FLOAT_ABCD:
			f.ch[0]=com_rbuf[3];
			f.ch[1]=com_rbuf[2];
			f.ch[2]=com_rbuf[1];
			f.ch[3]=com_rbuf[0];
			//printf("datapos %d,%x,%x,%x,%x:%f\n",datapos,
			//	f.ch[0]&0xff,f.ch[1]&0xff,f.ch[2]&0xff,f.ch[3]&0xff,
			//	f.f);

			*valf=f.f;
		break;		

		default:
			return -10;
    }

}


static int JuGuang_data_modbus(struct acquisition_data *acq_data,char *com_tbuf,int size,MODBUS_DATA_TYPE datatype,float *valf,char *polcode)
{
     int ret=0;
	 char com_rbuf[1000]={0};
	 union uf f;
	 char buff[100]={0};

	 if(!acq_data || !com_tbuf || size<=0 || !valf) return 1;

	 //ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	 
     ret=write_device(DEV_NAME(acq_data),com_tbuf,size);
	 
	 sprintf(buff,"JuGuang %s send:",polcode);
	 SYSLOG_DBG_HEX(buff, com_tbuf, size);
	 LOG_WRITE_HEX(DEV_NAME(acq_data),0,buff,com_tbuf,size);
     if(ret!=size)
     {
        *valf=0;
		return 1;
     }

     sleep(1);
	 
     ret=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	 SYSLOG_DBG("JuGuang read %s %s size=%d\n",polcode, DEV_NAME(acq_data),ret);
	 
	 sprintf(buff,"JuGuang %s recv:",polcode);
	 SYSLOG_DBG_HEX(buff, com_rbuf, ret);
	 LOG_WRITE_HEX(DEV_NAME(acq_data),1,buff,com_rbuf,ret);
     if(ret>=7 && com_rbuf[0]==com_tbuf[0] && com_rbuf[1]==com_tbuf[1] )
     {
		JuGuang_datatype2valf(&com_rbuf[3],datatype,valf);
		SYSLOG_DBG("JuGuang_datatype=%d,%s valf:%f\n",datatype,polcode,*valf);
		     	      
     }
     else
     {
        *valf=0;
		return 1;
     }

	 return 0;
}


int protocol_CEMS_JuGuang_HeBei(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float nox;
	int index=0;
	float valf=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
#if 0
#define CEMS_POLCODE_NUM 10
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026","a21003","a19001","a21005","a99051",
		"a01014","a21004","a21001","a21022","a21024"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_PECENT,	UNIT_MG_M3,		UNIT_MG_M3, 
		UNIT_PECENT,	UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3
	};

	int r_addr[CEMS_POLCODE_NUM]={
		307,		347,		427,		467,		477,
		487,		497,		507,		517,		527
	};
#else
#define CEMS_POLCODE_NUM 4
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026a","a21002a","a19001a","a01014"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_PECENT,  UNIT_PECENT
	};

	int r_addr[CEMS_POLCODE_NUM]={
		307,		347,		427,	487		
	};
#endif

	if(!acq_data) return -1;

    //ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer 20210604
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	sleep(1);

	memset(com_tbuf,0,sizeof(com_tbuf));
    size=modbus_pack(com_tbuf,devaddr,0x04,0x0132,0x02); //mies zhang 20210510 for read 30531~30533 fro status
 	ret=JuGuang_data_modbus(acq_data,com_tbuf,size,FLOAT_CDAB,&valf,"a21026a");
	if(ret==0)
	{
		acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,valf,&arg_n);
	}
	else
	{
		status=1;
		acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,0,&arg_n);
	}
	
#if 1
	memset(com_tbuf,0,sizeof(com_tbuf));
    size=modbus_pack(com_tbuf,devaddr,0x04,0x015A,0x02); //mies zhang 20210510 for read 30531~30533 fro status
 	ret=JuGuang_data_modbus(acq_data,com_tbuf,size,FLOAT_CDAB,&valf,"a21002a");
	if(ret==0)
	{
		acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,valf,&arg_n);  //Added  by Yimning 20211113
		acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,valf,&arg_n);
		
	    	valf*= 1.53;
		acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,valf,&arg_n);
	}
	else
	{
		status=1;
		acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,0,&arg_n);
		acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,0,&arg_n);	
	}	
	
	memset(com_tbuf,0,sizeof(com_tbuf));
    size=modbus_pack(com_tbuf,devaddr,0x04,0x01AA,0x02); //mies zhang 20210510 for read 30531~30533 fro status
 	ret=JuGuang_data_modbus(acq_data,com_tbuf,size,FLOAT_CDAB,&valf,"a19001a");
	if(ret==0)
	{
	
		acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,valf,&arg_n);
	}
	else
	{
		status=1;
		acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,0,&arg_n);	
	}

	memset(com_tbuf,0,sizeof(com_tbuf));
    size=modbus_pack(com_tbuf,devaddr,0x04,0x01E6,0x02); //mies zhang 20210510 for read 30531~30533 fro status
 	ret=JuGuang_data_modbus(acq_data,com_tbuf,size,FLOAT_CDAB,&valf,"a01014");
	if(ret==0)
	{
		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,valf,&arg_n);	
	}
	else
	{
		status=1;
		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,0,&arg_n);
	}
#endif

	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,0,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);

    if(status == 0)
    {
    	float stat=0;
		memset(com_tbuf,0,sizeof(com_tbuf));
	    size=modbus_pack(com_tbuf,devaddr,0x04,0x0212,0x02); //mies zhang 20210510 for read 30531~30533 fro status
	 	ret=JuGuang_data_modbus(acq_data,com_tbuf,size,FLOAT_CDAB,&valf,"status");		
        if(ret==0)
        {
			stat=valf;

			if((stat>=1.9999) && (stat<=2.0001) || 
				(stat>=2.9999) && (stat<=3.0001)) acq_data->dev_stat='C';
			if((stat>=3.9999) && (stat<=4.0001) || 
				(stat>=7.9999) && (stat<=8.0001) ||
				(stat>=8.9999) && (stat<=9.0001) ||
				(stat>=9.9999) && (stat<=10.0001)) acq_data->dev_stat='D';		
			if((stat>=10.9999) && (stat<=11.0001) ) acq_data->dev_stat='F';	
			if((stat>=4.9999) && (stat<=5.0001))  acq_data->dev_stat='M';
			if(	(stat>=5.9999) && (stat<=6.0001) ||
				(stat>=6.9999) && (stat<=7.0001) ) acq_data->dev_stat='z';	// z for fan Chui
        }
		else
		{
			status=1;
		}

		CEMS_JuGuang_HeBei_STATUS=acq_data->dev_stat;
    }
   

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;


	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

#if 0
int protocol_CEMS_JuGuang_HeBei(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	float nox;
	int index=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
#if 0
#define CEMS_POLCODE_NUM 10
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026","a21003","a19001","a21005","a99051",
		"a01014","a21004","a21001","a21022","a21024"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_PECENT,	UNIT_MG_M3,		UNIT_MG_M3, 
		UNIT_PECENT,	UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3
	};

	int r_addr[CEMS_POLCODE_NUM]={
		307,		347,		427,		467,		477,
		487,		497,		507,		517,		527
	};
#else
#define CEMS_POLCODE_NUM 4
	char *polcode[CEMS_POLCODE_NUM]={
		"a21026a","a21002a","a19001a","a01014"
	};
	UNIT_DECLARATION unit[CEMS_POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_PECENT,  UNIT_PECENT
	};

	int r_addr[CEMS_POLCODE_NUM]={
		307,		347,		427,	487		
	};
#endif

	//float valf[CEMS_POLCODE_NUM]={0};

	if(!acq_data) return -1;

    ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer 20210604
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
    sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	//size=modbus_pack(com_tbuf,devaddr,0x04,0x0132,0xDE);
	size=modbus_pack(com_tbuf,devaddr,0x04,0x0132,0xE2); //mies zhang 20210510 for read 30531~30533 fro status
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang HeBeiModbus CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	//sleep(1);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang HeBeiModbus CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang HeBeiModbus CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang HeBeiModbus CEMS RECV1:",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang HeBeiModbus CEMS RECV2:",&com_rbuf[200],size-200);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang HeBeiModbus CEMS RECV3:",&com_rbuf[400],size-400);
	if((size>=449)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			index=(r_addr[i]-307)*2+3;
			
			f.ch[0]=com_rbuf[index+1];
			f.ch[1]=com_rbuf[index+0];
			f.ch[2]=com_rbuf[index+3];
			f.ch[3]=com_rbuf[index+2];
			//if(!strcmp(polcode[i],"a21002a")) f.f *= 1.53;
			
			//valf[i]=f.f;
			acqdata_set_value(acq_data,polcode[i],unit[i],f.f,&arg_n);
		}
		status=0;
	}
	else
	{
		for(i=0;i<CEMS_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcode[i],unit[i],0,&arg_n);
		}
		status=1;
	}
	//nox=valf[1]*1.53+valf[6];
	//nox=valf[1]*1.53;

	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,0,&arg_n);

    if(status == 0)
    {
    	float stat=0;
		index=(531-307)*2+3;
		f.ch[0]=com_rbuf[index+1];
		f.ch[1]=com_rbuf[index+0];
		f.ch[2]=com_rbuf[index+3];
		f.ch[3]=com_rbuf[index+2];

		stat=f.f;

		if((stat>=1.9999) && (stat<=2.0001) || 
			(stat>=2.9999) && (stat<=3.0001)) acq_data->dev_stat='C';
		if((stat>=3.9999) && (stat<=4.0001) || 
			(stat>=7.9999) && (stat<=8.0001) ||
			(stat>=8.9999) && (stat<=9.0001) ||
			(stat>=9.9999) && (stat<=10.0001)) acq_data->dev_stat='D';		
		if((stat>=10.9999) && (stat<=11.0001) ) acq_data->dev_stat='F';	
		if((stat>=4.9999) && (stat<=5.0001) || 
			(stat>=5.9999) && (stat<=6.0001) ||
			(stat>=6.9999) && (stat<=7.0001) ) acq_data->dev_stat='M';	

		CEMS_JuGuang_HeBei_STATUS=acq_data->dev_stat;
    }
   

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;


	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}
#endif

int protocol_CEMS_JuGuang_HeBei_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_JuGuang_HeBei_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x0212,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang CEMS STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang CEMS STATUS protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang CEMS STATUS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang CEMS STATUS RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		val=f.f;
		switch(val)
		{
			case 1:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
			case 2:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
			case 3:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
			case 4:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				break;
			case 5:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
			case 6:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
			case 7:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,5,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
			case 8:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,4,INFOR_STATUS,&arg_n);
				break;
			case 9:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n);
				break;
			case 10:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				break;
			case 11:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
			default:	
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				break;
		}
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x012C,0x12);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang SO2 INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang SO2 protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang SO2 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang SO2 INFO RECV:",com_rbuf,size);
	if((size>=41)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);
	
		f.ch[3]=com_rbuf[9];
		f.ch[2]=com_rbuf[10];
		f.ch[1]=com_rbuf[7];
		f.ch[0]=com_rbuf[8];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[21];
		f.ch[2]=com_rbuf[22];
		f.ch[1]=com_rbuf[19];
		f.ch[0]=com_rbuf[20];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[25];
		f.ch[2]=com_rbuf[26];
		f.ch[1]=com_rbuf[23];
		f.ch[0]=com_rbuf[24];
		//acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n); //mod miles zhang 20210726

		f.ch[3]=com_rbuf[29];
		f.ch[2]=com_rbuf[30];
		f.ch[1]=com_rbuf[27];
		f.ch[0]=com_rbuf[28];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[33];
		f.ch[2]=com_rbuf[34];
		f.ch[1]=com_rbuf[31];
		f.ch[0]=com_rbuf[32];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[37];
		f.ch[2]=com_rbuf[38];
		f.ch[1]=com_rbuf[35];
		f.ch[0]=com_rbuf[36];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[41];
		f.ch[2]=com_rbuf[42];
		f.ch[1]=com_rbuf[39];
		f.ch[0]=com_rbuf[40];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
    sleep(1);
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_JuGuang_HeBei_NOx_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	union uf f;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_JuGuang_HeBei_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x0154,0x12);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"JuGuang NOx INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("JuGuang NOx protocol,INFO : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("JuGuang NOx data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"JuGuang NOx INFO RECV:",com_rbuf,size);
	if((size>=41)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		f.ch[3]=com_rbuf[5];
		f.ch[2]=com_rbuf[6];
		f.ch[1]=com_rbuf[3];
		f.ch[0]=com_rbuf[4];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);
	
		f.ch[3]=com_rbuf[9];
		f.ch[2]=com_rbuf[10];
		f.ch[1]=com_rbuf[7];
		f.ch[0]=com_rbuf[8];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[21];
		f.ch[2]=com_rbuf[22];
		f.ch[1]=com_rbuf[19];
		f.ch[0]=com_rbuf[20];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[25];
		f.ch[2]=com_rbuf[26];
		f.ch[1]=com_rbuf[23];
		f.ch[0]=com_rbuf[24];
		//acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13011",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);//mod miles zhang 20210726

		f.ch[3]=com_rbuf[29];
		f.ch[2]=com_rbuf[30];
		f.ch[1]=com_rbuf[27];
		f.ch[0]=com_rbuf[28];
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[33];
		f.ch[2]=com_rbuf[34];
		f.ch[1]=com_rbuf[31];
		f.ch[0]=com_rbuf[32];
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[37];
		f.ch[2]=com_rbuf[38];
		f.ch[1]=com_rbuf[35];
		f.ch[0]=com_rbuf[36];
		acqdata_set_value_flag(acq_data,"i13022",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=com_rbuf[41];
		f.ch[2]=com_rbuf[42];
		f.ch[1]=com_rbuf[39];
		f.ch[0]=com_rbuf[40];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}
    sleep(1);
	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


