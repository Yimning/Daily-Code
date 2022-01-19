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

int protocol_CEMS_AHWanYi_HeBei_New(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,humi=0,ai=0;
	float speed=0,temp=0,pres=0,temp2=0,pres2=0;
	float smoke=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;
	int val;
	char flag=0;
	char p[1024];

	static int O2_cnt=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;


	struct rdtu_info *rdtuinfo;

	if(!acq_data) return -1;

	acq_data->total=0;// total_flux_M3,added by miles zhang 20191211
	rdtuinfo=get_parent_rdtu_info(acq_data);

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	status=1;
	devaddr=0x01;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x012C,0x0B);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi HeBei CEMS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi HeBei CEMS protocol,CEMS 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi HeBei CEMS data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi HeBei CEMS RECV1:",com_rbuf,size);
	if(size>=55 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		so2=f.f;

		f.ch[3]=p[9];
		f.ch[2]=p[10];
		f.ch[1]=p[7];
		f.ch[0]=p[8];
		nox=f.f*46/30; // f.f * 1.53

		f.ch[3]=p[13];
		f.ch[2]=p[14];
		f.ch[1]=p[11];
		f.ch[0]=p[12];
		o2=f.f;

		f.ch[3]=p[23];
		f.ch[2]=p[24];
		f.ch[1]=p[21];
		f.ch[0]=p[22];
		ai=f.f;
		humi=(ai-4)/16*40;
		
		status=0;	
	}
	else
	{
		nox=0;
		so2=0;
		o2=0;
		humi=0;
	}

#if 1
	sleep(1);
	devaddr=0x40;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x03,0x32,0x02);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi HeBei CEMS SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi HeBei CEMS protocol,CEMS 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi HeBei CEMS data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi HeBei CEMS RECV2:",com_rbuf,size);
	if(size>=19 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x03))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		val=p[3];
		val<<=8;
		val+=p[4];
		temp=val;

		val=p[5];
		val<<=8;
		val+=p[6];
		pres=(val<0)?(0-val):val;
		
		status=0;
	}
	else
	{
		temp=0;
		pres=0;
	}


	sleep(1);
	devaddr=0x40;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x32,0x03);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi HeBei CEMS SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi HeBei CEMS protocol,CEMS 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi HeBei CEMS data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi HeBei CEMS RECV3:",com_rbuf,size);
	if(size>=23 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		val=p[3];
		val<<=8;
		val+=p[4];
		speed=val*0.1;

		val=p[5];
		val<<=8;
		val+=p[6];
		temp2=val*temp/65535;

		val=p[7];
		val<<=8;
		val+=p[8];
		pres2=(val-32767)*pres/32767;
		
		status=0;
	}
	else
	{
		speed=0;
		temp2=0;
		pres2=0;
	}


	sleep(1);
	devaddr=0x60;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x01F3,0x02);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi HeBei CEMS SEND4:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi HeBei CEMS protocol,CEMS 4: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi HeBei CEMS data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi HeBei CEMS RECV4:",com_rbuf,size);
	if(size>=19 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		smoke=f.f;
		
		status=0;
	}
	else
	{
		smoke=0;
	}

	sleep(1);
	devaddr=0x20;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x02,0x64,0x07);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi HeBei CEMS SEND5:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi HeBei CEMS protocol,CEMS 5: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi HeBei CEMS data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi HeBei CEMS RECV5:",com_rbuf,size);
	if(size>=13 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x02))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		if(p[3]==0)
		{
			sleep(1);
			devaddr=0x20;
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x34,0x01);
			LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi HeBei CEMS SEND6:",com_tbuf,size);
			size=write_device(DEV_NAME(acq_data),com_tbuf,size);
			sleep(1);
			memset(com_rbuf,0,sizeof(com_rbuf));
			size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
			SYSLOG_DBG("AHWanYi HeBei CEMS protocol,CEMS 6: read device %s , size=%d\n",DEV_NAME(acq_data),size);
			SYSLOG_DBG("AHWanYi HeBei CEMS data : %s",com_rbuf);
			LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi HeBei CEMS RECV6:",com_rbuf,size);
			if(size>=15 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
			{
				memset(p,0,sizeof(p));
				modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
				
				val=p[3];
				val<<=8;
				val+=p[4];
				switch(val)
				{
					case 0:	flag='N';break;
					//case 1:	flag='N';break;
					//case 2:	flag='N';break;
					case 1:	flag='M';break;
					case 2:	flag='z';break;
					case 3:	flag='C';break;
					case 4:	flag='D';break;
					case 5:	flag='M';break;
					default:	flag='N';break;
				}
				
				status=0;
			}
			else
			{
				flag='V';
			}
		}
		else if((p[3]&0x01)==0x01)
		{
			flag='v';
		}
		else
		{
			flag='D';
		}
		
		status=0;
	}
	//else //del by miles zhang 202107021133
	if(status==1) //added by miles zhang 202107021133
	{
		flag='V';
	}
#endif

	if(isPolcodeEnable(modbusarg, "a01011"))
	{
		acqdata_set_value(acq_data,"a00000z",UNIT_M3_S,0,&arg_n);
		acqdata_set_value(acq_data,"a00000",UNIT_M3_S,0,&arg_n);
		acqdata_set_value(acq_data,"a01011",UNIT_M_S,speed,&arg_n);
	}
	if(isPolcodeEnable(modbusarg, "a01012"))
	{
		acqdata_set_value(acq_data,"a01012",UNIT_0C,temp2,&arg_n);
	}
	if(isPolcodeEnable(modbusarg, "a01013"))
	{
		acqdata_set_value(acq_data,"a01013",UNIT_PA,pres2,&arg_n);
	}
	if(isPolcodeEnable(modbusarg, "a01014"))
	{
		acqdata_set_value(acq_data,"a01014",UNIT_PECENT,humi,&arg_n);
	}
	if(isPolcodeEnable(modbusarg, "a21002"))
	{
		acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
		acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
		acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,0,&arg_n);
	}
	if(isPolcodeEnable(modbusarg, "a21026"))
	{
		acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
		acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
		acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,0,&arg_n);
	}
	if(isPolcodeEnable(modbusarg, "a34013"))
	{
		acqdata_set_value(acq_data,"a34013a",UNIT_MG_M3,smoke,&arg_n);
		acqdata_set_value(acq_data,"a34013",UNIT_MG_M3,smoke,&arg_n);
		acqdata_set_value(acq_data,"a34013z",UNIT_MG_M3,0,&arg_n);
	}
	if(isPolcodeEnable(modbusarg, "a19001"))
	{
		acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
		acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	}

	acq_data->dev_stat=flag;
	//rdtuinfo->dev_stat=flag;

#if 0 //added by miles zhang 20210607 for pass one pack of O2 > 19,HB qing sha lv shui
        //if(o2>19 && O2_cnt==0)
        if(o2>19 && O2_cnt<20)
        {
            status=1;
        	//O2_cnt=1;
        	O2_cnt++;
        }

	    if(o2<=19) O2_cnt=0;
#endif	

	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



int protocol_CEMS_AHWanYi_HeBei_New_SO2_info(struct acquisition_data *acq_data)
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
	int alarm;
	union uf f;
	long time_val = 0;
	int year=0, mon=0, day, hour=0, min=0, sec=0;
	char p[1024];

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_AHWanYi_HeBei_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;

	devaddr=0x01;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x0134,0x15);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi SO2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi SO2 protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi SO2 data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi SO2 INFO RECV1:",com_rbuf,size);
	if(size>=95 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		
		val=p[3];
		val<<=8;
		val+=p[4];
		alarm=val;

		f.ch[3]=p[23];
		f.ch[2]=p[24];
		f.ch[1]=p[21];
		f.ch[0]=p[22];
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[35];
		f.ch[2]=p[36];
		f.ch[1]=p[33];
		f.ch[0]=p[34];
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}



	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x0190,0x14);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi SO2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi SO2 protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi SO2 data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi SO2 INFO RECV2:",com_rbuf,size);
	if(size>=91 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[9];
		f.ch[2]=p[10];
		f.ch[1]=p[7];
		f.ch[0]=p[8];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		time_val = getInt32Value(p, 27, LONG_CDAB);
		year=time_val/10000;
		mon=(time_val%10000)/100;
		day=time_val%100;
		
                time_val = getInt32Value(p, 31, LONG_CDAB);
		hour=time_val/10000;
		min=(time_val%10000)/100;
		sec=time_val%100;
		t3=getTimeValue(year, mon,  day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		time_val = getInt32Value(p, 35, LONG_CDAB);
		year=time_val/10000;
		mon=(time_val%10000)/100;
		day=time_val%100;
		
                time_val = getInt32Value(p, 39, LONG_CDAB);
		hour=time_val/10000;
		min=(time_val%10000)/100;
		sec=time_val%100;
		t3=getTimeValue(year, mon,  day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x03,0x012F,0x10);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi SO2 INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi SO2 protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi SO2 data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi SO2 INFO RECV3:",com_rbuf,size);
	if(size>=75 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x03))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[25];
		f.ch[2]=p[26];
		f.ch[1]=p[23];
		f.ch[0]=p[24];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x01C2,0x01);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi STATUS SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi STATUS protocol,STATUS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi STATUS data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi STATUS RECV:",com_rbuf,size);
	if(size>=15 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
	
		if(alarm==1)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,6,INFOR_STATUS,&arg_n);
		}
		else
		{
			val=p[3];
			val<<=8;
			val+=p[4];
			if(val==3)
			{
				acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
			}
			else
			{
				switch(val)
				{
					case 0: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n); break;
					case 1: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
					case 2: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n); break;
					default: acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n); break;
				}
				acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			}
		}
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_AHWanYi_HeBei_New_NOx_info(struct acquisition_data *acq_data)
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
	long time_val = 0;
	int year=0, mon=0, day, hour=0, min=0, sec=0;
	char p[1024];

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_AHWanYi_HeBei_NOx_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	if(!isPolcodeEnable(modbusarg, "a21002"))
		return 0;

	devaddr=0x01;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x013F,0x08);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi NOx INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi NOx protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi NOx data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi NOx INFO RECV1:",com_rbuf,size);
	if(size>=43 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[17];
		f.ch[2]=p[18];
		f.ch[1]=p[15];
		f.ch[0]=p[16];
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x0194,0x18);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi NOx INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi NOx protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi NOx data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi NOx INFO RECV2:",com_rbuf,size);
	if(size>=107 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);


		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[9];
		f.ch[2]=p[10];
		f.ch[1]=p[7];
		f.ch[0]=p[8];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		time_val = getInt32Value(p, 35, LONG_CDAB);
		year=time_val/10000;
		mon=(time_val%10000)/100;
		day=time_val%100;
		
                time_val = getInt32Value(p, 39, LONG_CDAB);
		hour=time_val/10000;
		min=(time_val%10000)/100;
		sec=time_val%100;
		t3=getTimeValue(year, mon,  day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		time_val = getInt32Value(p, 43, LONG_CDAB);
		year=time_val/10000;
		mon=(time_val%10000)/100;
		day=time_val%100;
		
                time_val = getInt32Value(p, 47, LONG_CDAB);
		hour=time_val/10000;
		min=(time_val%10000)/100;
		sec=time_val%100;
		t3=getTimeValue(year, mon,  day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x03,0x0131,0x0C);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi NOx INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi NOx protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi NOx data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi NOx INFO RECV3:",com_rbuf,size);
	if(size>=59 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x03))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[25];
		f.ch[2]=p[26];
		f.ch[1]=p[23];
		f.ch[0]=p[24];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}


int protocol_CEMS_AHWanYi_HeBei_New_O2_info(struct acquisition_data *acq_data)
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
	long time_val = 0;
	int year=0, mon=0, day, hour=0, min=0, sec=0;
	char p[1024];

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_AHWanYi_HeBei_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	if(!isPolcodeEnable(modbusarg, "a19001"))
		return 0;

	devaddr=0x01;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x0141,0x08);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi O2 INFO SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi O2 protocol,INFO 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi O2 data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi O2 INFO RECV1:",com_rbuf,size);
	if(size>=43 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13024",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[17];
		f.ch[2]=p[18];
		f.ch[1]=p[15];
		f.ch[0]=p[16];
		acqdata_set_value_flag(acq_data,"i13029",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x0198,0x1C);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi O2 INFO SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi O2 protocol,INFO 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi O2 data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi O2 INFO RECV2:",com_rbuf,size);
	if(size>=123 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);


		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13023",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[9];
		f.ch[2]=p[10];
		f.ch[1]=p[7];
		f.ch[0]=p[8];
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,f.f,INFOR_ARGUMENTS,&arg_n);

		time_val = getInt32Value(p, 43, LONG_CDAB);
		year=time_val/10000;
		mon=(time_val%10000)/100;
		day=time_val%100;
		
                time_val = getInt32Value(p, 47, LONG_CDAB);
		hour=time_val/10000;
		min=(time_val%10000)/100;
		sec=time_val%100;
		t3=getTimeValue(year, mon,  day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t3,0.0,INFOR_ARGUMENTS,&arg_n);

		time_val = getInt32Value(p, 51, LONG_CDAB);
		year=time_val/10000;
		mon=(time_val%10000)/100;
		day=time_val%100;
		
                time_val = getInt32Value(p, 55, LONG_CDAB);
		hour=time_val/10000;
		min=(time_val%10000)/100;
		sec=time_val%100;
		t3=getTimeValue(year, mon,  day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t3,0.0,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x03,0x0133,0x0C);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi O2 INFO SEND3:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi O2 protocol,INFO 3: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi O2 data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi O2 INFO RECV3:",com_rbuf,size);
	if(size>=59 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x03))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		f.ch[3]=p[5];
		f.ch[2]=p[6];
		f.ch[1]=p[3];
		f.ch[0]=p[4];
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		f.ch[3]=p[25];
		f.ch[2]=p[26];
		f.ch[1]=p[23];
		f.ch[0]=p[24];
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,f.f,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_AHWanYi_HeBei_New_TSP_info(struct acquisition_data *acq_data)
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
	long time_val = 0;
	int year=0, mon=0, day, hour=0, min=0, sec=0;
	char p[1024];

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_AHWanYi_HeBei_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	if(!isPolcodeEnable(modbusarg, "a34013"))
		return 0;

	devaddr=0x60;
	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a34013",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x01FB,0x01);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi TSP STATUS SEND1:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi TSP protocol,STATUS 1: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi TSP data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi TSP STATUS RECV1:",com_rbuf,size);
	if(size>=13 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		val=getUInt16Value(p, 3, INT_AB);
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,5,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,1,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,2,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12004",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}

		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x04,0x0160,0x01);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi TSP STATUS SEND2:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi TSP protocol,STATUS 2: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi TSP data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi TSP STATUS RECV2:",com_rbuf,size);
	if(size>=13 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x04))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);
		
		val=getUInt16Value(p, 3, INT_AB);
		if(val==0)
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		else
			acqdata_set_value_flag(acq_data,"i12005",UNIT_NONE,1,INFOR_STATUS,&arg_n);
		
		switch(val)
		{
			case 0:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
			case 1:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,4,INFOR_STATUS,&arg_n);break;
			case 2:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 3:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 4:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			case 5:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,7,INFOR_STATUS,&arg_n);break;
			case 6:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,9,INFOR_STATUS,&arg_n);break;
			default:	acqdata_set_value_flag(acq_data,"i12006",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
		}

		status=0;
	}


	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_ascii_pack(com_tbuf,devaddr,0x03,0x32,0x01);
	LOG_WRITE_STR(DEV_NAME(acq_data),0,"AHWanYi TSP INFO SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("AHWanYi TSP protocol,INFO: read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG("AHWanYi TSP data : %s",com_rbuf);
	LOG_WRITE_STR(DEV_NAME(acq_data),1,"AHWanYi TSP INFO RECV:",com_rbuf,size);
	if(size>=13 && modbus_ascii_pack_check(com_rbuf, devaddr, 0x03))
	{
		memset(p,0,sizeof(p));
		modbus_ascii_pack_to_hex_pack(com_rbuf,size,p);

		val=getUInt16Value(p, 3, INT_AB);
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,val,INFOR_ARGUMENTS,&arg_n);

		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

