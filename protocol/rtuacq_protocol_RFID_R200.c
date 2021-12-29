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

/*
Author:Yimning
Time:2021.10.23 Sun.
Description:protocol_RFID_R200

*/


int checksum(char buff[],int size)
{
	int sum = 0;
	char *p = NULL;
	int i = 0;
	int res = 0;
	char sumHex[4] = {0} ;
	
	p = buff;
	
	if(p == NULL)  
		return 0;
	
	p = buff+1;
	for(i = 1; i < size-2 ;i++,p++)
	{
		sum = sum + *p;
	}
	sprintf(sumHex,"%04X",sum);
	sscanf(&sumHex[2],"%X",&res);
	
	if(res == (int)(*p))  
		res = 1;
	else res = 0;

	return res;
}

static unsigned long long int valID1;	
static int di0 = 0;
static int info_flag1 = 0;

static unsigned long long int valID2;	
static int di1 = 0;
static int info_flag2 = 0;

static unsigned long long int valID3;	
static int di2 = 0;
static int info_flag3 = 0;

static unsigned long long int valID4;	
static int di3 = 0;
static int info_flag4 = 0;

int protocol_RFID_R200_16bytes(struct acquisition_data *acq_data)
{
#define MAXLEN 12 //card id 12 bytes

#define POLCODE_NUM 8
	char *polcodeCid[POLCODE_NUM]={
		"RCID1","RCID2","RCID3","RCID4","RCID5","RCID6","RCID7","RCID8"
	};

	char *polcodeSta[POLCODE_NUM]={
		"RDI1","RDI2","RDI3","RDI4","RDI5","RDI6","RDI7","RDI8"
	};

	UNIT_DECLARATION unit[0]={UNIT_NONE};


	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char buffTX[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	float ph = 0,temp = 0;
	int i = 0;
	char cardID[50] = {0};

	static  int valID;	
	static int di = 0;


	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x00;
	regpos = 0x00;
	regcnt = 0x00;
	dataType = FLOAT_ABCD ;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));

	com_tbuf[0] = 0xBB;
	com_tbuf[1] = 0x01;
	com_tbuf[2] = 0xC1;
	com_tbuf[3] = 0x00;
	com_tbuf[4] = 0x01;
	com_tbuf[5] = 0x00;
	com_tbuf[6] = 0xC3;
	com_tbuf[7] = 0x7E;
	
	//TX:BB 01 C1 00 01 00 C3 7E
	//RX:BB 01 C1 00 01 00 C3 7E
	size = 8;
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"RFID R200 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("RFID R200 protocol, R200 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("RFID R200 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"RFID R200 RECV:",com_rbuf,size);
	if(!strcmp(com_rbuf,com_tbuf))
	{
		//TX:BB 00 22 00 00 22 7E 
		//RX:BB 02 22 00 11 C4 30 00 E2 00 00 1D 73 15 01 65 21 40 8F C6 B8 3E C2 7E      //exists.
		//RX:BB 01 FF 00 01 15 16 7E        //read error or the card does not  exists.
		
		memset(com_tbuf,0,sizeof(com_tbuf));
		com_tbuf[0] = 0xBB;
		com_tbuf[1] = 0x00;
		com_tbuf[2] = 0x22;
		com_tbuf[3] = 0x00;
		com_tbuf[4] = 0x00;
		com_tbuf[5] = 0x22;
		com_tbuf[6] = 0x7E;

		//size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
		size=7;
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"RFID R200 SEND1:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
		SYSLOG_DBG("RFID R200 protocol, R200: read device1 %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("RFID R200 data1",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"RFID R200 RECV1:",com_rbuf,size);
		//p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
		//if(p!= NULL)

		ret = checksum(com_rbuf,size);  //CheckSum
		
		if((size>=24)&&(com_rbuf[0]==0xBB)&&(com_rbuf[1]==0x02)&&(ret == 1)&&(com_rbuf[size-1] == 0x7E))
		{
			for(i = 0; i < MAXLEN;i++)
			{
				sprintf(&cardID[i*2],"%02X",com_rbuf[8+i]);    //get CardID and save to array.
			}

			SYSLOG_DBG("cardID----- = %s\n",cardID);

			p = cardID;

			while((*p) == '0' && (*p )!='\0') p++;   //remove '0'

			strcpy(cardID,p);  

			sscanf(cardID,"%d",&valID);

			for(i=0;i<POLCODE_NUM;i++)
			{
			        char buffdi[20]={0};
				acqdata_set_value(acq_data,polcodeCid[i],UNIT_NONE,valID,&arg_n);

				sprintf(buffdi,"%s%d",DEV_DI_NAME,i);

				size=read_device(buffdi,(char *)(&di),4);
				di=(di!=0)?0:1;
				SYSLOG_DBG("RFID R200  DI protocol,RFID R200  DI: read device %s ,size=%d, di=%d\n",buffdi,size,di);
				LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DI %s RFID R200  DI RECV:size=%d, di=%d",buffdi,size,di);
				
				acqdata_set_value(acq_data,polcodeSta[i],UNIT_NONE,di,&arg_n);
			}

			status=0;

		}
		else
		{
			status = 0;
			valID = -1;  //do not read card.
			di = 0;
			for(i=0;i<POLCODE_NUM;i++)
			{
				acqdata_set_value(acq_data,polcodeCid[i],UNIT_NONE,valID,&arg_n);
				acqdata_set_value(acq_data,polcodeSta[i],UNIT_NONE,di,&arg_n);
			}
		}

	}
	else
	{
		status=1;  //read error.
		          
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcodeCid[i],UNIT_NONE,0,&arg_n);
			acqdata_set_value(acq_data,polcodeSta[i],UNIT_NONE,0,&arg_n);
		}
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}





int protocol_RFID_R200_8bytes(struct acquisition_data *acq_data)
{
#define MAXLEN 8   //card id 8 bytes

#define POLCODE_NUM 8
	char *polcodeCid[POLCODE_NUM]={
		"RCID1","RCID2","RCID3","RCID4","RCID5","RCID6","RCID7","RCID8"
	};

	char *polcodeSta[POLCODE_NUM]={
		"RDI1","RDI2","RDI3","RDI4","RDI5","RDI6","RDI7","RDI8"
	};

	UNIT_DECLARATION unit[0]={UNIT_NONE};


	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	char buffTX[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int cmd = 0,regpos = 0,regcnt = 0;
	char *p=NULL;
	char flag=0;
	int val = 0;
	float valf = 0;
	float ph = 0,temp = 0;
	int i = 0;
	char cardID[50] = {0};

	static int valID;	
	static int di = 0;


	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	cmd = 0x00;
	regpos = 0x00;
	regcnt = 0x00;
	dataType = FLOAT_ABCD ;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	devaddr=modbusarg->devaddr&0xffff;
	
	memset(com_tbuf,0,sizeof(com_tbuf));

	com_tbuf[0] = 0xBB;
	com_tbuf[1] = 0x01;
	com_tbuf[2] = 0xC1;
	com_tbuf[3] = 0x00;
	com_tbuf[4] = 0x01;
	com_tbuf[5] = 0x00;
	com_tbuf[6] = 0xC3;
	com_tbuf[7] = 0x7E;
	
	//TX:BB 01 C1 00 01 00 C3 7E
	//RX:BB 01 C1 00 01 00 C3 7E
	size = 8;
	
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"RFID R200 SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("RFID R200 protocol, R200 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("RFID R200 data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"RFID R200 RECV:",com_rbuf,size);
	if(!strcmp(com_rbuf,com_tbuf))
	{
		//TX:BB 00 22 00 00 22 7E 
		//RX:BB 02 22 00 11 C4 30 00 E2 00 00 1D 73 15 01 65 21 40 8F C6 B8 3E C2 7E      //exists.
		//RX:BB 01 FF 00 01 15 16 7E        //read error or the card does not  exists.
		
		memset(com_tbuf,0,sizeof(com_tbuf));
		com_tbuf[0] = 0xBB;
		com_tbuf[1] = 0x00;
		com_tbuf[2] = 0x22;
		com_tbuf[3] = 0x00;
		com_tbuf[4] = 0x00;
		com_tbuf[5] = 0x22;
		com_tbuf[6] = 0x7E;

		//size=modbus_pack(com_tbuf, devaddr, cmd, regpos, regcnt);
		size=7;
		LOG_WRITE_HEX(DEV_NAME(acq_data),0,"RFID R200 SEND1:",com_tbuf,size);
		size=write_device(DEV_NAME(acq_data), com_tbuf ,size);
		sleep(1);
		memset(com_rbuf,0,sizeof(com_rbuf));
		size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
		SYSLOG_DBG("RFID R200 protocol, R200: read device1 %s , size=%d\n",DEV_NAME(acq_data),size);
		SYSLOG_DBG_HEX("RFID R200 data1",com_rbuf,size);
		LOG_WRITE_HEX(DEV_NAME(acq_data),1,"RFID R200 RECV1:",com_rbuf,size);
		//p = modbus_crc_check(com_rbuf,size, devaddr, cmd, regcnt);
		//if(p!= NULL)

		ret = checksum(com_rbuf,size);  //CheckSum
		
		if((size>=20)&&(com_rbuf[0]==0xBB)&&(com_rbuf[1]==0x02)&&(ret == 1)&&(com_rbuf[size-1] == 0x7E))
		{
			for(i = 0; i < MAXLEN;i++)
			{
				sprintf(&cardID[i*2],"%02X",com_rbuf[8+i]);    //get CardID and save to array.
			}

			SYSLOG_DBG("cardID----- = %s\n",cardID);

			p = cardID;

			while((*p) == '0' && (*p )!='\0') p++;   //remove '0'

			strcpy(cardID,p);  

			sscanf(cardID,"%d",&valID);

			for(i=0;i<POLCODE_NUM;i++)
			{
			        char buffdi[20]={0};
				acqdata_set_value(acq_data,polcodeCid[i],UNIT_NONE,valID,&arg_n);

				sprintf(buffdi,"%s%d",DEV_DI_NAME,i);

				size=read_device(buffdi,(char *)(&di),4);
				di=(di!=0)?0:1;
				SYSLOG_DBG("RFID R200  DI protocol,RFID R200  DI: read device %s ,size=%d, di=%d\n",buffdi,size,di);
				LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DI %s RFID R200  DI RECV:size=%d, di=%d",buffdi,size,di);
				
				acqdata_set_value(acq_data,polcodeSta[i],UNIT_NONE,di,&arg_n);
			}

			status=0;

		}
		else
		{
			status = 0;
			valID = -1;  //do not read card.
			di = 0;
			for(i=0;i<POLCODE_NUM;i++)
			{
				acqdata_set_value(acq_data,polcodeCid[i],UNIT_NONE,valID,&arg_n);
				acqdata_set_value(acq_data,polcodeSta[i],UNIT_NONE,di,&arg_n);
			}
		}

	}
	else
	{
		status=1;  //read error.
		          
		for(i=0;i<POLCODE_NUM;i++)
		{
			acqdata_set_value(acq_data,polcodeCid[i],UNIT_NONE,0,&arg_n);
			acqdata_set_value(acq_data,polcodeSta[i],UNIT_NONE,0,&arg_n);
		}
	}


	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	//NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}


