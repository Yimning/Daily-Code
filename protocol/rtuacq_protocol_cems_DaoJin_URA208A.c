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

#define SO2 0
#define NOx 1
#define O2 2
#define CO 3
static struct hebei_dynamic_info_gas URA208A_info[4];

static pthread_mutex_t URA208A_mute;
static int URA208A_mute_flag=0;

#define CHANNEL_NUM 12
static float URA208A_valf[CHANNEL_NUM];
static float URA208A_span[CHANNEL_NUM];
static int URA208A_status;

extern char SHIMADZU_STATUS;

static void set_URA208A_data(float valf, float span, int index)
{
	if(URA208A_mute_flag==0)
	{
		URA208A_mute_flag=1;
		pthread_mutex_init(&URA208A_mute, NULL);
	}

	pthread_mutex_lock(&URA208A_mute);
	URA208A_valf[index]=valf;
	URA208A_span[index]=span;
	pthread_mutex_unlock(&URA208A_mute);
}

static void get_URA208A_data(float *valf, float *span, int index)
{
	if(URA208A_mute_flag==0)
	{
		URA208A_mute_flag=1;
		pthread_mutex_init(&URA208A_mute, NULL);
	}

	pthread_mutex_lock(&URA208A_mute);
	*valf=URA208A_valf[index];
	*span=URA208A_span[index];
	pthread_mutex_unlock(&URA208A_mute);
}

static int getDaoJinDataPack(const char *dev_name, char *pack, char *cmd, char *log)
{
#define MAX_RECV_NUM 12

	char com_tbuf[100];
	char com_rbuf[200]; 
	char str[100];
	int len;
	int size,size2;
	int i=0;

	if(!dev_name || !pack || !log || !cmd || strlen(cmd)!=2) return -1;


//----------------- init of transmission----------------------
	com_tbuf[0]=0x04;
	sleep(1);
	memset(str,0,sizeof(str));
	sprintf(str,"%s init SEND:",log);
	LOG_WRITE_HEX(dev_name,0,str,com_tbuf,1);
	write_device(dev_name,com_tbuf,1);
	sleep(1);
	size=read_device(dev_name,com_rbuf,sizeof(com_rbuf)-1);
	memset(str,0,sizeof(str));
	sprintf(str,"%s init RECV:",log);
	LOG_WRITE_HEX(dev_name,1,str,com_tbuf,size);
//--------------------------------------------------------

	memset(com_tbuf,0,sizeof(com_tbuf));
	com_tbuf[0]=0x05;
	com_tbuf[1]=0x02;
	com_tbuf[2]=cmd[0];
	com_tbuf[3]=cmd[1];
	com_tbuf[4]=0x03;
	com_tbuf[5]=cal_bcc(&com_tbuf[2], 3);
	com_tbuf[6]=0x04;
	size=7;

	memset(str,0,sizeof(str));
	sprintf(str,"%s SEND1:",log);
	LOG_WRITE_HEX(dev_name,0,str,com_tbuf,size);
	size=write_device(dev_name,com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(dev_name,com_rbuf,sizeof(com_rbuf)-1);
	memset(str,0,sizeof(str));
	sprintf(str,"%s RECV1:",log);
	LOG_WRITE_HEX(dev_name,1,str,com_rbuf,size);
	if((size<3) || (com_rbuf[0]!=0x06) || (com_rbuf[1]!=0x06) || (com_rbuf[2]!=0x05))
		return 0;

	com_tbuf[0]=0x06;
	len=0;
	for(i=2;i<MAX_RECV_NUM;i++)
	{
		sleep(1);
		memset(str,0,sizeof(str));
		sprintf(str,"%s SEND%d:",log,i);
		LOG_WRITE_HEX(dev_name,0,str,com_tbuf,1);
		write_device(dev_name,com_tbuf,1);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(dev_name,com_rbuf,sizeof(com_rbuf)-1);
		memset(str,0,sizeof(str));
		sprintf(str,"%s RECV%d:",log,i);
		LOG_WRITE_HEX(dev_name,1,str,com_rbuf,size);
		if((size>3) && (com_rbuf[0]==0x02))
		{
			if((com_rbuf[size-2]!=0x17) && (com_rbuf[size-2]!=0x03))
			{
				size2=read_device(dev_name,&com_rbuf[size],sizeof(com_rbuf)-1-size);
				memset(str,0,sizeof(str));
				sprintf(str,"%s RECV%d again:",log,i);
				LOG_WRITE_HEX(dev_name,1,str,&com_rbuf[size],size2);
				size+=size2;
			}

			memcpy(&pack[len],&com_rbuf[1],size-3);
			len+=(size-3);
		}
		else	
			break;	//error

		if((size>3) && (com_rbuf[size-2]==0x03))
		{
			sleep(1);
			write_device(dev_name,com_tbuf,1);
			break;	//end
		}
	}

//----------------- end of transmission----------------------
	com_tbuf[0]=0x04;
	sleep(1);
	memset(str,0,sizeof(str));
	sprintf(str,"%s end SEND:",log);
	LOG_WRITE_HEX(dev_name,0,str,com_tbuf,1);
	write_device(dev_name,com_tbuf,1);
	sleep(1);
	size=read_device(dev_name,com_rbuf,sizeof(com_rbuf)-1);
	memset(str,0,sizeof(str));
	sprintf(str,"%s end RECV:",log);
	LOG_WRITE_HEX(dev_name,1,str,com_tbuf,size);
//--------------------------------------------------------

	for(i=0;len>i*500;i++)
	{
		memset(str,0,sizeof(str));
		sprintf(str,"%s RECV data%d:",log,i+1);
		LOG_WRITE_STR(dev_name,1,str,&pack[i*500],len-i*500);
	}
#undef MAX_RECV_NUM
	return len;
}


int protocol_CEMS_DaoJin_URA208A(struct acquisition_data *acq_data)
{
#define CHANNEL_NUM 12

	int status=0;
	char data[2048]={0}; 
	int size=0;
	int i=0,offset=0,pos=0;
	int ret=0;
	int arg_n=0;
	float valf;
	float span;
	int val;
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;

	status=URA208A_status;
	
	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		if(mpolcodearg->enableFlag==1)
		{
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=11 && mpolcodearg->algorithm!=1)
			{
				get_URA208A_data(&valf, &span, pos);
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,valf,&arg_n);

				if((!strcmp(mpolcodearg->polcode,"a21026a")) || (!strcmp(mpolcodearg->polcode,"a21026")))
					URA208A_info[SO2].i13013=span;
				else if((!strcmp(mpolcodearg->polcode,"a21002a")) || (!strcmp(mpolcodearg->polcode,"a21002")))
					URA208A_info[NOx].i13013=span;
				else if((!strcmp(mpolcodearg->polcode,"a19001a")) || (!strcmp(mpolcodearg->polcode,"a19001")))
					URA208A_info[O2].i13013=span;
				else if((!strcmp(mpolcodearg->polcode,"a21005a")) || (!strcmp(mpolcodearg->polcode,"a21005")))
					URA208A_info[CO].i13013=span;
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	}

	if(status == 0)
	{
		if(SHIMADZU_STATUS == 0)
		{
			acq_data->acq_status = ACQ_OK;
		}
		else
		{
			acq_data->dev_stat = SHIMADZU_STATUS;
			acq_data->acq_status = ACQ_OK;
		}
	}
	else 
	{
		acq_data->acq_status = ACQ_ERR;
	}

	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_CEMS_DaoJin_URA208A_data(struct acquisition_data *acq_data)
{
	int status=0;
	char data[2048]={0}; 
	int size=0;
	int i=0,offset=0,pos=0;
	int ret=0;
	int arg_n=0;
	float valf;
	float span;
	int val;
	char *p;
	static unsigned int cnt;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;

	sleep(1);
	memset(data,0,sizeof(data));

	size=getDaoJinDataPack(DEV_NAME(acq_data), data, "DT", "DaoJin URA-208A CEMS");
	if((size>=70) && (!strncmp(data,"0,DT,",5)))
	{
		for(i=0;i<CHANNEL_NUM;i++)
		{
			offset=i/5;
			if(memchr(&data[8+i*22+offset*2],'*',6)!=NULL)
			{
				set_URA208A_data(0, 0, i);
				continue;
			}
			
			if(memchr(&data[8+i*22+offset*2],'.',6)==NULL)
			{
				sscanf(&data[8+i*22+offset*2],"%d/",&val);
				valf=val;
			}
			else
			{
				sscanf(&data[8+i*22+offset*2],"%f/",&valf);
			}

			if(memchr(&data[15+i*22+offset*2],'.',5)==NULL)
			{
				sscanf(&data[15+i*22+offset*2],"%d/",&val);
				span=val;
			}
			else
			{
				sscanf(&data[15+i*22+offset*2],"%f/",&span);
			}

			set_URA208A_data(valf, span, i);
		}
		
		URA208A_status=0;
	}
	else
	{
		URA208A_status=1;
	}
	
	return 0;
}


int protocol_CEMS_DaoJin_URA208A_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char data[2048]={0}; 
	int size=0;
	int i=0,offset=0,pos=0;
	int val;
	int polNo,index,zflag;
	int year, mon, day, hour, min;
	float valf;
	int ret=0;
	int arg_n=0;
	time_t t;
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg;

	SYSLOG_DBG("protocol_CEMS_DaoJin_URA208A_SO2_info\n");

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;


	memset(data,0,sizeof(data));
	sleep(1);
	size=getDaoJinDataPack(DEV_NAME(acq_data), data, "DD", "DaoJin URA-208A CEMS INFO");
	if((size>=70) && (!strncmp(data,"0,DD,",5)))
	{
		for(i=0;i<10;i++)
		{
			if((48+i*69)>size)
				break;
		
			polNo=0;
			//sscanf(&data[5+i*138],"%02d/",&polNo);
			sscanf(&data[5+i*69],"%02d/%01d",&polNo,&zflag);

			switch(polNo)
			{
				case 1:	index=SO2;break;
				case 2:	index=NOx;break;
				case 3:	index=CO;break;
				case 5:	index=O2;break;
				default :	index=-1;break;
			}

			if(index!=-1)
			{
#if 0				
				if(memchr(&data[12+i*138],'.',5)==NULL)
				{
					sscanf(&data[12+i*138],"%d/",&val);
					URA208A_info[index].i13023=val;
				}
				else
				{
					sscanf(&data[12+i*138],"%f/",&(URA208A_info[index].i13023));
				}
				
				sscanf(&data[20+i*138],"%02d-%02d-%02d/%02d:%02d/",&mon,&day,&year,&hour,&min);
				t=getTimeValue(year+2000, mon, day, hour, min, 0);
				URA208A_info[index].i13021=t;
				
				if(memchr(&data[48+i*138],'.',6)==NULL)
				{
					sscanf(&data[48+i*138],"%d/",&val);
					URA208A_info[index].i13025=val;
				}
				else
				{
					sscanf(&data[48+i*138],"%f/",&(URA208A_info[index].i13025));
				}

				
			
				if(memchr(&data[81+i*138],'.',5)==NULL)
				{
					sscanf(&data[81+i*138],"%d/",&val);
					URA208A_info[index].i13028=val;
				}
				else
				{
					sscanf(&data[81+i*138],"%f/",&(URA208A_info[index].i13028));
				}
				
				sscanf(&data[89+i*138],"%02d-%02d-%02d/%02d:%02d/",&mon,&day,&year,&hour,&min);
				t=getTimeValue(year+2000, mon, day, hour, min, 0);
				URA208A_info[index].i13027=t;
				
				if(memchr(&data[117+i*138],'.',6)==NULL)
				{
					sscanf(&data[117+i*138],"%d/",&val);
					URA208A_info[index].i13010=val;
				}
				else
				{
					sscanf(&data[117+i*138],"%f/",&(URA208A_info[index].i13010));
				}
#else
				if(zflag==0)
				{
					if(memchr(&data[12+i*69],'.',5)==NULL)
					{
						sscanf(&data[12+i*69],"%d/",&val);
						URA208A_info[index].i13023=val;
					}
					else
					{
						sscanf(&data[12+i*69],"%f/",&(URA208A_info[index].i13023));
					}
					
					sscanf(&data[20+i*69],"%02d-%02d-%02d/%02d:%02d/",&mon,&day,&year,&hour,&min);
					t=getTimeValue(year+2000, mon, day, hour, min, 0);
					URA208A_info[index].i13021=t;
					
					if(memchr(&data[48+i*69],'.',6)==NULL)
					{
						sscanf(&data[48+i*69],"%d/",&val);
						URA208A_info[index].i13025=val;
					}
					else
					{
						sscanf(&data[48+i*69],"%f/",&(URA208A_info[index].i13025));
					}
				}
				else if(zflag==1)
				{
					if(memchr(&data[12+i*69],'.',5)==NULL)
					{
						sscanf(&data[12+i*69],"%d/",&val);
						URA208A_info[index].i13028=val;
					}
					else
					{
						sscanf(&data[12+i*69],"%f/",&(URA208A_info[index].i13028));
					}
					
					sscanf(&data[20+i*69],"%02d-%02d-%02d/%02d:%02d/",&mon,&day,&year,&hour,&min);
					t=getTimeValue(year+2000, mon, day, hour, min, 0);
					URA208A_info[index].i13027=t;
					
					if(memchr(&data[48+i*69],'.',6)==NULL)
					{
						sscanf(&data[48+i*69],"%d/",&val);
						URA208A_info[index].i13010=val;
					}
					else
					{
						sscanf(&data[48+i*69],"%f/",&(URA208A_info[index].i13010));
					}
				}
#endif

				URA208A_info[index].status=1;
			}
		}
	}

	if(!isPolcodeEnable(modbusarg_running, "a21026"))
		return 0;

	if(URA208A_info[SO2].status==1)
	{
		acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);
	
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,URA208A_info[SO2].i13013,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,URA208A_info[SO2].i13023,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,URA208A_info[SO2].i13025,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13021",URA208A_info[SO2].i13021,0.0,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,URA208A_info[SO2].i13028,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,URA208A_info[SO2].i13010,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13027",URA208A_info[SO2].i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	}
	

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_DaoJin_URA208A_NOx_info(struct acquisition_data *acq_data)
{
	int arg_n=0;

	SYSLOG_DBG("protocol_CEMS_DaoJin_URA208A_NOx_info\n");
	if(!acq_data) return -1;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	
	if(!isPolcodeEnable(modbusarg_running, "a21002"))
		return 0;


	if(URA208A_info[NOx].status==1)
	{
		acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);
	
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,URA208A_info[NOx].i13013,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,URA208A_info[NOx].i13023,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,URA208A_info[NOx].i13025,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13021",URA208A_info[NOx].i13021,0.0,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,URA208A_info[NOx].i13028,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,URA208A_info[NOx].i13010,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13027",URA208A_info[NOx].i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	}

	read_system_time(acq_data->acq_tm);

	acq_data->acq_status = ACQ_OK;

	return arg_n;
}


int protocol_CEMS_DaoJin_URA208A_O2_info(struct acquisition_data *acq_data)
{
	int arg_n=0;

	SYSLOG_DBG("protocol_CEMS_DaoJin_URA208A_O2_info\n");
	if(!acq_data) return -1;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	
	if(!isPolcodeEnable(modbusarg_running, "a19001"))
		return 0;

	if(URA208A_info[O2].status==1)
	{
		acqdata_set_value_flag(acq_data,"a19001",UNIT_PECENT,0,INFOR_ARGUMENTS,&arg_n);
	
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,URA208A_info[O2].i13013,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13023",UNIT_PECENT,URA208A_info[O2].i13023,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,URA208A_info[O2].i13025,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13021",URA208A_info[O2].i13021,0.0,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,URA208A_info[O2].i13028,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,URA208A_info[O2].i13010,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13027",URA208A_info[O2].i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	}

	read_system_time(acq_data->acq_tm);

	acq_data->acq_status = ACQ_OK;

	return arg_n;
}


int protocol_CEMS_DaoJin_URA208A_CO_info(struct acquisition_data *acq_data)
{
	int arg_n=0;

	

	SYSLOG_DBG("protocol_CEMS_DaoJin_URA208A_CO_info\n");
	if(!acq_data) return -1;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	
	if(!isPolcodeEnable(modbusarg_running, "a21005"))
		return 0;

	if(URA208A_info[CO].status==1)
	{
		acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);
	
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,URA208A_info[CO].i13013,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,URA208A_info[CO].i13023,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,URA208A_info[CO].i13025,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13021",URA208A_info[CO].i13021,0.0,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,URA208A_info[CO].i13028,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,URA208A_info[CO].i13010,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13027",URA208A_info[CO].i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	}

	read_system_time(acq_data->acq_tm);

	acq_data->acq_status = ACQ_OK;

	return arg_n;
}


int protocol_CEMS_DaoJin_URA209(struct acquisition_data *acq_data)
{
//#define URA209_CHANNEL_NUM 12

	int status=0;
	char data[2048]={0}; 
	int size=0;
	int i=0,offset=0,pos=0;
	int ret=0;
	int arg_n=0;
	float valf;
	float span;
	int val;
	float nox=0;
	char *p;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;
	struct modbus_polcode_arg *mpolcodearg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	
	status=URA208A_status;
	
	for(i=0;i<modbusarg_running->array_count ;i++)
	{
		mpolcodearg=&modbusarg_running->polcode_arg[i];
		if(mpolcodearg->enableFlag==1)
		{
			if((!strcmp(mpolcodearg->polcode,"a21002a")) || (!strcmp(mpolcodearg->polcode,"a21002")))
				continue;
		
			pos=mpolcodearg->regaddr-1;
			if(pos>=0 && pos<=11 && mpolcodearg->algorithm!=1)
			{
				get_URA208A_data(&valf, &span, pos);
				
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,valf,&arg_n);

				if((!strcmp(mpolcodearg->polcode,"a21026a")) || (!strcmp(mpolcodearg->polcode,"a21026")))
					URA208A_info[SO2].i13013=span;
				else if((!strcmp(mpolcodearg->polcode,"a21003a")) || (!strcmp(mpolcodearg->polcode,"a21003")))
				{
					URA208A_info[NOx].i13013=span;
					nox=NO_to_NOx(valf);
				}
				else if((!strcmp(mpolcodearg->polcode,"a19001a")) || (!strcmp(mpolcodearg->polcode,"a19001")))
					URA208A_info[O2].i13013=span;
				else if((!strcmp(mpolcodearg->polcode,"a21005a")) || (!strcmp(mpolcodearg->polcode,"a21005")))
					URA208A_info[CO].i13013=span;
			}
			else
			{
				acqdata_set_value(acq_data,mpolcodearg->polcode,mpolcodearg->unit,0,&arg_n);
			}
		}
	}

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);

	if(status == 0)
	{
		if(SHIMADZU_STATUS == 0)
		{
			acq_data->acq_status = ACQ_OK;
		}
		else
		{
			acq_data->dev_stat = SHIMADZU_STATUS;
			acq_data->acq_status = ACQ_OK;
		}
	}
	else 
	{
		acq_data->acq_status = ACQ_ERR;
	}

	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
	
}

int protocol_CEMS_DaoJin_URA209_NO_info(struct acquisition_data *acq_data)
{
	int arg_n=0;

	SYSLOG_DBG("protocol_CEMS_DaoJin_URA209_NO_info\n");
	if(!acq_data) return -1;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg_running;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg_running=&acq_ctrl->modbusarg_running;
	
	if(!isPolcodeEnable(modbusarg_running, "a21003"))
		return 0;


	if(URA208A_info[NOx].status==1)
	{
		acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,0,INFOR_ARGUMENTS,&arg_n);
	
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,URA208A_info[NOx].i13013,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,URA208A_info[NOx].i13023,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,URA208A_info[NOx].i13025,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13021",URA208A_info[NOx].i13021,0.0,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,URA208A_info[NOx].i13028,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,URA208A_info[NOx].i13010,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13027",URA208A_info[NOx].i13027,0.0,INFOR_ARGUMENTS,&arg_n);
	}

	read_system_time(acq_data->acq_tm);

	acq_data->acq_status = ACQ_OK;

	return arg_n;
}

#undef SO2
#undef NOx
#undef O2 
#undef CO

