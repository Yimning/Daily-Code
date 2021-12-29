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

int protocol_others_vocs_pid(struct acquisition_data *acq_data)
{
/*
TX:01 03 00 08 00 02 45 C9 
RX:01 03 04 00 00 09 5E 7D 9B
*/
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	time_t timer;
	struct tm *t;
	unsigned int crc;
    static float total_flux_M3=0;
	float valf=0;

	if(!acq_data) return -1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x08, 0x02);	
	
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	SYSLOG_DBG("vocs_pid : send device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("vocs_pid send data",com_tbuf,size);
	//LOG_WRITE_HEX(DEV_NAME(acq_data),0,"vocs_pid SEND:",com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("vocs_pid protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("vocs_pid read data",com_rbuf,size);
	//LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tengentTx RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		int val=0;
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val<<=8;
		val+=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		valf=val;
		status=0;
	}
	else
	{
		valf=0;
		status=1;
	}

    total_flux_M3+=1;

	ret=acqdata_set_value(acq_data,"ag0606",UNIT_PPM,valf,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;

}


int protocol_others_vocs_pid_div10(struct acquisition_data *acq_data)
{
/*
TX:01 03 00 08 00 02 45 C9 
RX:01 03 04 00 00 09 5E 7D 9B
*/
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	time_t timer;
	struct tm *t;
	unsigned int crc;
    static float total_flux_M3=0;
	float valf=0;

	if(!acq_data) return -1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x08, 0x02);	
	
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	SYSLOG_DBG("vocs_pid : send device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("vocs_pid send data",com_tbuf,size);
	//LOG_WRITE_HEX(DEV_NAME(acq_data),0,"vocs_pid SEND:",com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("vocs_pid protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("vocs_pid read data",com_rbuf,size);
	//LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tengentTx RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		int val=0;
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val<<=8;
		val+=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		valf=val;
		valf/=10;
		status=0;
	}
	else
	{
		valf=0;
		status=1;
	}

    total_flux_M3+=1;

	ret=acqdata_set_value(acq_data,"ag0606",UNIT_PPM,valf,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;

}

int protocol_others_vocs_pid_div1000(struct acquisition_data *acq_data)
{
/*
TX:01 03 00 08 00 02 45 C9 
RX:01 03 04 00 00 09 5E 7D 9B
*/
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	time_t timer;
	struct tm *t;
	unsigned int crc;
    static float total_flux_M3=0;
	float valf=0;

	if(!acq_data) return -1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x08, 0x02);	
	
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	SYSLOG_DBG("vocs_pid : send device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("vocs_pid send data",com_tbuf,size);
	//LOG_WRITE_HEX(DEV_NAME(acq_data),0,"vocs_pid SEND:",com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("vocs_pid protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("vocs_pid read data",com_rbuf,size);
	//LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tengentTx RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		int val=0;
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val<<=8;
		val+=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		valf=val;
		valf/=1000;
		status=0;
	}
	else
	{
		valf=0;
		status=1;
	}

    total_flux_M3+=1;

	ret=acqdata_set_value(acq_data,"ag0606",UNIT_MG_M3,valf,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;

}


int protocol_others_vocs_pid_mgm3(struct acquisition_data *acq_data)
{
/*
TX:01 03 00 08 00 02 45 C9 
RX:01 03 04 00 00 09 5E 7D 9B
*/
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	time_t timer;
	struct tm *t;
	unsigned int crc;
    static float total_flux_M3=0;
	float valf=0;

	if(!acq_data) return -1;
	
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, 0x01, 0x03, 0x08, 0x02);	
	
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	SYSLOG_DBG("vocs_pid : send device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("vocs_pid send data",com_tbuf,size);
	//LOG_WRITE_HEX(DEV_NAME(acq_data),0,"vocs_pid SEND:",com_tbuf,size);
	sleep(1);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("vocs_pid protocol : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("vocs_pid read data",com_rbuf,size);
	//LOG_WRITE_HEX(DEV_NAME(acq_data),1,"tengentTx RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==0x01)&&(com_rbuf[1]==0x03))
	{
		int val=0;
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val<<=8;
		val+=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		valf=val;
		valf/=1000;
		valf*=0.53;
		status=0;
	}
	else
	{
		valf=0;
		status=1;
	}

    total_flux_M3+=1;

	ret=acqdata_set_value(acq_data,"ag0606",UNIT_MG_M3,valf,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	NEED_ERROR_CACHE(acq_data,10);

	return arg_n;

}



