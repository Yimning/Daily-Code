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

int protocol_others_LiZheng(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float valf=0;
	int i;
	long val;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	status=1;
	devaddr=0x01;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SDKaiMiSi COND SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SDKaiMiSi COND protocol,COND 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SDKaiMiSi COND data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SDKaiMiSi COND RECV1:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];

		for(i=0;i<val;i++)
			valf/=10;

		acqdata_set_value(acq_data,"wd1201",UNIT_MS_M,valf,&arg_n);
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"wd1201",UNIT_MS_M,0,&arg_n);
	}

	sleep(2);
	devaddr=0x02;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SDKaiMiSi PH SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SDKaiMiSi PH protocol,PH 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SDKaiMiSi PH data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SDKaiMiSi PH RECV2:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];

		for(i=0;i<val;i++)
			valf/=10;

		acqdata_set_value(acq_data,"wd1001",UNIT_PH,valf,&arg_n);
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"wd1001",UNIT_PH,0,&arg_n);
	}

	sleep(2);
	devaddr=0x03;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SDKaiMiSi COND SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SDKaiMiSi COND protocol,COND 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SDKaiMiSi COND data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SDKaiMiSi COND RECV3:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];

		for(i=0;i<val;i++)
			valf/=10;

		acqdata_set_value(acq_data,"wc1202",UNIT_MS_M,valf,&arg_n);
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"wc1202",UNIT_MS_M,0,&arg_n);
	}

	sleep(2);
	devaddr=0x04;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x00,0x2);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"SDKaiMiSi PH SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("SDKaiMiSi PH protocol,PH 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("SDKaiMiSi PH data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"SDKaiMiSi PH RECV4:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		valf=val;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];

		for(i=0;i<val;i++)
			valf/=10;

		acqdata_set_value(acq_data,"wc1002",UNIT_PH,valf,&arg_n);
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"wc1002",UNIT_PH,0,&arg_n);
	}
#if 0
	usleep(500);
	devaddr=0x05;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 EQ SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 EQ protocol,EQ 5:read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 EQ data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 EQ RECV5:",com_rbuf,size);
	if((size==9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		status=0;
		acqdata_set_value(acq_data,"eg0001",UNIT_KWH,f.l*0.01,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"eg0001",UNIT_KWH,0,&arg_n);
	}

	usleep(500);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0303,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 PT SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 PT protocol,PT 5:read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 PT data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 PT RECV5:",com_rbuf,size);
	if((size==19)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		status=0;
		acqdata_set_value(acq_data,"eg0101",UNIT_KW,val*0.01,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"eg0101",UNIT_KW,0,&arg_n);
	}

	usleep(500);
	devaddr=0x06;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 EQ SEND6:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 EQ protocol,EQ 6:read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 EQ data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 EQ RECV6:",com_rbuf,size);
	if((size==9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		status=0;
		acqdata_set_value(acq_data,"eg0002",UNIT_KWH,f.l*0.01,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"eg0002",UNIT_KWH,0,&arg_n);
	}

	usleep(500);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0303,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 PT SEND6:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 PT protocol,PT 6:read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 PT data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 PT RECV6:",com_rbuf,size);
	if((size==19)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		status=0;
		acqdata_set_value(acq_data,"eg0102",UNIT_KW,val*0.01,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"eg0102",UNIT_KW,0,&arg_n);
	}

	usleep(500);
	devaddr=0x07;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 EQ SEND7:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 EQ protocol,EQ 7:read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 EQ data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 EQ RECV7:",com_rbuf,size);
	if((size==9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		status=0;
		acqdata_set_value(acq_data,"eg0003",UNIT_KWH,f.l*0.01,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"eg0003",UNIT_KWH,0,&arg_n);
	}

	usleep(500);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0303,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 PT SEND7:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 PT protocol,PT 7:read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 PT data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 PT RECV7:",com_rbuf,size);
	if((size==19)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		status=0;
		acqdata_set_value(acq_data,"eg0103",UNIT_KW,val*0.01,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"eg0103",UNIT_KW,0,&arg_n);
	}

	usleep(500);
	devaddr=0x08;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0A,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 EQ SEND8:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 EQ protocol,EQ 8:read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 EQ data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 EQ RECV8:",com_rbuf,size);
	if((size==9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		status=0;
		acqdata_set_value(acq_data,"e40004",UNIT_KWH,f.l*0.01,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"e40004",UNIT_KWH,0,&arg_n);
	}

	usleep(500);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x03,0x0303,0x07);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DeLiXi DTSU6606 PT SEND8:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DeLiXi DTSU6606 PT protocol,PT 8:read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DeLiXi DTSU6606 PT data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DeLiXi DTSU6606 PT RECV8:",com_rbuf,size);
	if((size==19)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		val=com_rbuf[15];
		val<<=8;
		val+=com_rbuf[16];
		status=0;
		acqdata_set_value(acq_data,"e40104",UNIT_KW,val*0.01,&arg_n);
	}
	else
	{
		acqdata_set_value(acq_data,"e40104",UNIT_KW,0,&arg_n);
	}
#endif

	sleep(2);
	devaddr=0x0A;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03,0x00 ,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJShuiMengDianZi_SM10 flux SEND10:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJShuiMengDianZi_SM10 flux protocol 10:flux read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJShuiMengDianZi_SM10 flux protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJShuiMengDianZi_SM10 flux RECV10:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		val=f.l;
		valf=BCD32(val);
		valf/=10000;
		acqdata_set_value(acq_data,"w10001",UNIT_M3,valf,&arg_n);

		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		val=f.l;
		valf=BCD32(val);
		valf/=100000;
		valf/=3.6;
		acqdata_set_value(acq_data,"w10101",UNIT_L_S,valf,&arg_n);
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"w10001",UNIT_M3,0,&arg_n);
		acqdata_set_value(acq_data,"w10101",UNIT_L_S,0,&arg_n);
	}

	sleep(2);
	devaddr=0x0B;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03,0x00 ,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJShuiMengDianZi_SM10 flux SEND11:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJShuiMengDianZi_SM10 flux protocol 11:flux read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJShuiMengDianZi_SM10 flux protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJShuiMengDianZi_SM10 flux RECV11:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		val=f.l;
		valf=BCD32(val);
		valf/=10000;
		acqdata_set_value(acq_data,"w90002",UNIT_M3,valf,&arg_n);

		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		val=f.l;
		valf=BCD32(val);
		valf/=100000;
		valf/=3.6;
		acqdata_set_value(acq_data,"w90102",UNIT_L_S,valf,&arg_n);
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"w90002",UNIT_M3,0,&arg_n);
		acqdata_set_value(acq_data,"w90102",UNIT_L_S,0,&arg_n);
	}

	sleep(2);
	devaddr=0x0C;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03,0x00 ,0x04);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"NJShuiMengDianZi_SM10 flux SEND12:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("NJShuiMengDianZi_SM10 flux protocol 12:flux read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("NJShuiMengDianZi_SM10 flux protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"NJShuiMengDianZi_SM10 flux RECV12:",com_rbuf,size);
	if((size>=13)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		val=f.l;
		valf=BCD32(val);
		valf/=10000;
		acqdata_set_value(acq_data,"w60001",UNIT_M3,valf,&arg_n);

		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		val=f.l;
		valf=BCD32(val);
		valf/=100000;
		valf/=3.6;
		acqdata_set_value(acq_data,"w60101",UNIT_L_S,valf,&arg_n);
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"w60003",UNIT_M3,0,&arg_n);
		acqdata_set_value(acq_data,"w60103",UNIT_L_S,0,&arg_n);
	}

	sleep(2);
	devaddr=0x0D;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, 0x03,0x04FF,0x06);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"HuaXun flux SEND13:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data), com_tbuf, size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	SYSLOG_DBG("HuaXun flux protocol 13:flux read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("HuaXun flux protocol",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"HuaXun flux RECV13:",com_rbuf,size);
	if((size>=17)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x03))
	{
		f.ch[3]=com_rbuf[3];
		f.ch[2]=com_rbuf[4];
		f.ch[1]=com_rbuf[5];
		f.ch[0]=com_rbuf[6];
		acqdata_set_value(acq_data,"wb0104",UNIT_L_S,f.f,&arg_n);
		
		f.ch[3]=com_rbuf[7];
		f.ch[2]=com_rbuf[8];
		f.ch[1]=com_rbuf[9];
		f.ch[0]=com_rbuf[10];
		val=f.l*10000000;

		f.ch[3]=com_rbuf[11];
		f.ch[2]=com_rbuf[12];
		f.ch[1]=com_rbuf[13];
		f.ch[0]=com_rbuf[14];
		val+=f.l;
		valf=val*0.001;
		acqdata_set_value(acq_data,"wb0004",UNIT_M3,valf,&arg_n);
		
		status=0;
	}
	else
	{
		acqdata_set_value(acq_data,"wb0004",UNIT_M3,0,&arg_n);
		acqdata_set_value(acq_data,"wb0104",UNIT_L_S,0,&arg_n);
	}
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 3);
	return arg_n;
}



