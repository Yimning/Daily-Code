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

#define MODBUS_RECV_DATA_POS 3

static int modbus_get_valf(char *com_rbuf,unsigned int size,unsigned int regaddr,MODBUS_DATA_TYPE datatype,float *valf)
{
    union uf  f;
	unsigned int datalen=0;
	int datapos=0;

	if(!com_rbuf || !valf) return -1;

	if(regaddr>=size) return -1;

    datalen=com_rbuf[2];
	if(regaddr>=datalen) return -2;

	//datapos=MODBUS_RECV_DATA_POS+regaddr;
	datapos=MODBUS_RECV_DATA_POS+regaddr*2;
	if(datapos>(size-4)) return -3;
	
    switch(datatype)
    {
        //case INT_AB:
		case INT_BA:
			f.ch[0]=com_rbuf[0+datapos];
			f.ch[1]=com_rbuf[1+datapos];
			*valf=f.s;
		break;
		//case INT_BA:
		case INT_AB:
			f.ch[0]=com_rbuf[1+datapos];
			f.ch[1]=com_rbuf[0+datapos];
			*valf=f.s;
		break;
        //case LONG_ABCD:
		case LONG_DCBA:
			f.ch[0]=com_rbuf[0+datapos];
			f.ch[1]=com_rbuf[1+datapos];
			f.ch[2]=com_rbuf[2+datapos];
			f.ch[3]=com_rbuf[3+datapos];
			*valf=f.l;
		break;
		//case LONG_CDAB:
		case LONG_BADC:
			f.ch[0]=com_rbuf[2+datapos];
			f.ch[1]=com_rbuf[3+datapos];
			f.ch[2]=com_rbuf[0+datapos];
			f.ch[3]=com_rbuf[1+datapos];	
			*valf=f.l;
		break;
        //case LONG_BADC:
		case LONG_CDAB:
			f.ch[0]=com_rbuf[1+datapos];
			f.ch[1]=com_rbuf[0+datapos];
			f.ch[2]=com_rbuf[3+datapos];
			f.ch[3]=com_rbuf[2+datapos];
			*valf=f.l;
		break;
		//case LONG_DCBA:
		case LONG_ABCD:
			f.ch[0]=com_rbuf[3+datapos];
			f.ch[1]=com_rbuf[2+datapos];
			f.ch[2]=com_rbuf[1+datapos];
			f.ch[3]=com_rbuf[0+datapos];	
			*valf=f.l;
		break;		
		//case FLOAT_ABCD:
		case FLOAT_DCBA:
			f.ch[0]=com_rbuf[0+datapos];
			f.ch[1]=com_rbuf[1+datapos];
			f.ch[2]=com_rbuf[2+datapos];
			f.ch[3]=com_rbuf[3+datapos];
			*valf=f.f;
		break;			
		//case FLOAT_CDAB:
		case FLOAT_BADC:
			f.ch[0]=com_rbuf[2+datapos];
			f.ch[1]=com_rbuf[3+datapos];
			f.ch[2]=com_rbuf[0+datapos];
			f.ch[3]=com_rbuf[1+datapos];	
			*valf=f.f;
		break;
        //case FLOAT_BADC:
        case FLOAT_CDAB:
			f.ch[0]=com_rbuf[1+datapos];
			f.ch[1]=com_rbuf[0+datapos];
			f.ch[2]=com_rbuf[3+datapos];
			f.ch[3]=com_rbuf[2+datapos];
			*valf=f.f;
		break;
		//case FLOAT_DCBA:
		case FLOAT_ABCD:
			f.ch[0]=com_rbuf[3+datapos];
			f.ch[1]=com_rbuf[2+datapos];
			f.ch[2]=com_rbuf[1+datapos];
			f.ch[3]=com_rbuf[0+datapos];
			//printf("datapos %d,%x,%x,%x,%x:%f\n",datapos,
			//	f.ch[0]&0xff,f.ch[1]&0xff,f.ch[2]&0xff,f.ch[3]&0xff,
			//	f.f);

			*valf=f.f;
		break;		

		default:
			return -10;
    }

	return 0;
}
/*
static struct modbus_polcode_arg * find_modbus_polcode_arg_by_polcode(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode)
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
*/
/*
return : com_f arguments amount;
*/
int protocol_modbus_gas(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float speed=0,total_flux_M3=0,cod=0,andan=0,zlin=0,zdan=0;
	unsigned int devaddr=0,cmd=0,startaddr=0,cnt=0;
	char *p=NULL;
	int len=0;

#define MODBUS_POLCODE_NUM 20
	char *polcodestr[MODBUS_POLCODE_NUM]={
		"a21003",		"a21004",		"a21005",		"a21005z",
		"a21026",		"a21026z",	"a21002",		"a21002z",
		"a34013",		"a34013z",	"a01011",		"a19001",
		"a01012",		"a01013",		"a00000",		"a01014",
		"a21024",		"a21024z",	"a05001",		"a00000z"
	};
	UNIT_DECLARATION unitstr[MODBUS_POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_M_S,		UNIT_PECENT,
		UNIT_0C,			UNIT_PA,			UNIT_M3_H,		UNIT_PECENT,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_M3_H
	};


	struct acquisition_ctrl *acq_ctrl;
	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;

	int i=0;
	int ret=0;

	if(!acq_data) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	mpolcodearg=modbusarg_running->polcode_arg;

	devaddr=modbusarg_running->devaddr&0xFF;
	cmd=modbusarg_running->devfun&0xFF;
	startaddr=modbusarg_running->devstaddr&0xFFFF;
	cnt=modbusarg_running->regcnt&0xFFFF;

	SYSLOG_DBG("modbus protocol modbusarg : %x, %x,%x,%x,%d\n",devaddr,cmd,startaddr,cnt,modbusarg_running->array_count);
	for(i=0;i<modbusarg_running->array_count && i<MODBUS_POLCODE_MAX;i++)
	{
		SYSLOG_DBG("modbus protocol modbusarg  polcode  %d, %s,%x,%d\n",i,
		mpolcodearg[i].polcode,mpolcodearg[i].regaddr&0xffff,mpolcodearg[i].datatype);
	}

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, startaddr, cnt);
	SYSLOG_DBG_HEX("modbus protocol send data",com_tbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"modbus SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("modbus protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("modbus protocol recv data",com_rbuf,size);	
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"modbus RECV:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			if(mpolcodearg_temp)
			{
				float valf=0.0;
				len=p-com_rbuf;
				len=size-len;
				ret=modbus_get_valf(p,len,mpolcodearg_temp->regaddr,mpolcodearg_temp->datatype,&valf);
				SYSLOG_DBG("modbus protocol modbus_get_valf %s ,%f,%d,%d,ret=%d\n",polcodestr[i],valf,
				mpolcodearg_temp->regaddr&0xffff,mpolcodearg_temp->datatype,ret);
				if(ret<0) 
					acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
				else
				{
					//if((!strcmp(polcodestr[i],"a21002")) && (mpolcodearg_temp->algorithm == 1))
					//	valf*=1.53;
					acqdata_set_value(acq_data,polcodestr[i],unitstr[i],valf,&arg_n);
				}
			}
		}
	}
	else
	{
		status=1;
		for(i=0;i<MODBUS_POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
		}
	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;
}



