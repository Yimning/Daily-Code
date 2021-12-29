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

static int SIEMENS_ELAN_pack(char *buff, unsigned int chanAddr, unsigned int comAddr, char charCMD, char hexCMD, char *data, int data_len)
{
	unsigned int crc;
	int i,j;

	if(!buff || (!data && data_len>0)) return 0; //error

	buff[0] = 0x10;
	buff[1] = 0x01;
	buff[2] =  (char)((chanAddr<<4)+comAddr);

	if(buff[2]==0x10)
	{
		buff[3]=0x10;
		j=1;
	}
	else
	{
		j=0;
	}
	
	buff[3+j]=0xD0;
	buff[4+j] =  charCMD;
	buff[5+j] =  hexCMD;
	for(i=0; i<data_len; i++)
	{
		buff[6+i+j] = data[i];
	}
	buff[6+i+j]=0x10;
	buff[7+i+j]=0x03;
	crc = CRC16_modbus(buff, 8+i+j);
	buff[8+i+j] = (char)(crc & 0xff);
	buff[9+i+j] = (char)(crc >> 8);
	
	return 10+i+j;
}

int protocol_CEMS_BJXueDiLong_U23(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0;
	int i,len; 
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	char cmd[3]={0};
	char check[7]={0};
	char *p,*q;
	char flag;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='k';
	cmd[1]=0x02;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x01,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 CEMS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(3);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("BJXueDiLong U23 CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong U23 CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 CEMS RECV:",com_rbuf,size);
	check[0]=0x10;
	check[1]=0x06;
	check[2]=0x10;
	check[3]=0x01;
	check[4]=0xD0;
	check[5]=(devaddr<<4)+0x01;
	check[6]=0x00;
	if((size>=38) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL) )
	{
		status=0;

		check[0]=0x10;
		check[1]=0x03;
		check[2]=0x00;
		q=memstr(p, com_rbuf+size-p, check);
		for(p+=2;p<q;p=p+len+5)
		{
			len=strlen(p);
			switch(p[len+3])
			{
				case 6: sscanf(p,"%f",&so2);break;
				case 7: sscanf(p,"%f",&no);break;
				case 12: sscanf(p,"%f",&o2);break;
			}
		}
		
		nox=no*46/30;

		if(com_rbuf[6]==0x00 && com_rbuf[7]==0x04)
			flag='N';
		else
		if(
			com_rbuf[7]==20 ||
			com_rbuf[7]==19 ||
			com_rbuf[7]==17 ||
			com_rbuf[7]==16 ||
			com_rbuf[7]==15 ||
			com_rbuf[7]==14 ||
			com_rbuf[7]==13 ||
			com_rbuf[7]==12 ||
			com_rbuf[7]==11 ||
			com_rbuf[7]==10 ||
			com_rbuf[7]==9 ||
			com_rbuf[7]==8 ||
			com_rbuf[7]==6 ||
			com_rbuf[7]==5 
			)
			flag='C';
		else if(com_rbuf[7]==18 ||
				com_rbuf[7]==21)
				flag='M';
		else if(com_rbuf[7]==3)
			flag='d';
		else if(com_rbuf[7]==2)
			flag='F';
		else if(com_rbuf[7]==1)
			flag='t';
		else
		{
			flag='N';
			if(com_rbuf[6]==0x18) flag='C';
			else if(com_rbuf[6]&(0x1<<1)) flag='M';
			else if(com_rbuf[6]&(0x1<<3)) flag='M';
			else if(com_rbuf[6]&(0x1<<4)) flag='C';
			else if(com_rbuf[6]!=0x00) status=1; //added by miles zhang 20210630
		}
		
	}
	else
	{
		nox=0;
		so2=0;
		o2=0;
		no=0;
		flag='D';
		status=1;
	}
	
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	

	acq_data->dev_stat=flag;
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



int protocol_CEMS_BJXueDiLong_U23_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int index=0,len=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	union uf f;
	char cmd[3]={0};
	char check[8]={0};
	char *p,*q;
	char flag;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_U23_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	check[0]=0x10;
	check[1]=0x06;
	check[2]=0x10;
	check[3]=0x01;
	check[4]=0xD0;
	check[5]=(devaddr<<4)+0x00;
	if(devaddr==0x01)
	{
		check[6]=0x10;
		check[7]=0x00;
	}
	else
	{
		check[6]=0x00;
	}

	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='s';
	cmd[1]=0x08;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x00,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 SO2 SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 SO2 RECV2:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL))
	{
		index=p[2]&0x03;
	}
	
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x02;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x00,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 SO2 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 SO2 RECV1:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL))
	{
		switch(com_rbuf[7])
		{
			case 1:	val=99;break;
			case 2:	val=99;break;
			case 3:	val=0;break;
			case 4:	val=0;break;
			case 5:	val=3;break;
			case 6:	val=1;break;
			case 7:	val=99;break;
			case 8:	val=1;break;
			case 9:	val=1;break;
			case 10:	val=1;break;
			case 11:	val=1;break;
			case 12:	val=1;break;
			case 13:	val=1;break;
			case 14:	val=3;break;
			case 15:	val=1;break;
			case 16:	val=3;break;
			case 17:	val=3;break;
			case 18:	val=5;break;
			case 19:	val=1;break;
			case 20:	val=1;break;
			case 21:	val=5;break;
			default:	val=99;break;
		}
		acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,val,INFOR_STATUS,&arg_n);
		
		if(index==0)
		{
			p+=2;
			sscanf(p,"%f",&valf);
		}
		else
		{
			p+=2;
			len=strlen(p);
			p+=(len+3);
			sscanf(p,"%f",&valf);
		}
		
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x05;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x00,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 SO2 SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 SO2 RECV2:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL))
	{
		p+=2;
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x2B;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x00,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 SO2 SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 SO2 RECV3:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL))
	{
		p+=2;
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		len=strlen(p);
		p+=(len+3);
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_U23_NOx_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int len=0,index=0;
	int devaddr=0;
	union uf f;
	char cmd[3]={0};
	char check[8]={0};
	char *p,*q;
	char flag;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_U23_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	check[0]=0x10;
	check[1]=0x06;
	check[2]=0x10;
	check[3]=0x01;
	check[4]=0xD0;
	check[5]=(devaddr<<4)+0x01;
	check[6]=0x00;

	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='s';
	cmd[1]=0x08;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x01,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 NOx SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 NOx RECV2:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL))
	{
		index=p[2]&0x03;
	}

	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x02;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x01,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 NOx SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 NOx RECV1:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL) )
	{
		if(index==0)
		{
			p+=2;
			sscanf(p,"%f",&valf);
		}
		else
		{
			p+=2;
			len=strlen(p);
			p+=(len+3);
			sscanf(p,"%f",&valf);
		}
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x05;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x01,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 NOx SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 NOx RECV2:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL) )
	{
		p+=2;
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x2B;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x01,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 NOx SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 NOx RECV3:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL) )
	{
		p+=2;
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		len=strlen(p);
		p+=(len+3);
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_U23_O2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[1024]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int devaddr=0;
	int len=0,index=0;
	union uf f;
	char cmd[10]={0};
	char check[8]={0};
	char *p,*q;
	char flag;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_U23_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	check[0]=0x10;
	check[1]=0x06;
	check[2]=0x10;
	check[3]=0x01;
	check[4]=0xD0;
	check[5]=(devaddr<<4)+0x03;
	check[6]=0x00;

	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='s';
	cmd[1]=0x08;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x03,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 O2 SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 O2 RECV2:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL))
	{
		index=p[2]&0x03;
	}
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x02;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x03,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 O2 SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 O2 RECV1:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL) )
	{
		if(index==0)
		{
			p+=2;
			sscanf(p,"%f",&valf);
		}
		else
		{
			p+=2;
			len=strlen(p);
			p+=(len+3);
			sscanf(p,"%f",&valf);
		}
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x05;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x03,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 O2 SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 O2 RECV2:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL) )
	{
		p+=2;
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13023",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	cmd[0]='w';
	cmd[1]=0x2B;
	cmd[2]=0x00;
	size=SIEMENS_ELAN_pack(com_tbuf,devaddr,0x03,cmd[0],cmd[1],NULL,0);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"BJXueDiLong U23 O2 SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"BJXueDiLong U23 O2 RECV3:",com_rbuf,size);
	if( (size>=12) && (!strncmp(com_rbuf,check,6)) && ((p=memstr(com_rbuf, size, cmd))!=NULL) )
	{
		p+=2;
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		len=strlen(p);
		p+=(len+3);
		sscanf(p,"%f",&valf);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
