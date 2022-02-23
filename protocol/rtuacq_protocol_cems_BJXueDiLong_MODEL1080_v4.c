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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

static float get_BJXueDiLong_MODEL1080_v4_bias(float valf, float range,float point, int unitId, int mr)
{
	float bias=0;

	switch(unitId)
	{
		case 2:
			bias=valf*point;
			bias=gas_ppm_to_mg_m3(bias, mr);
			bias=bias*100/range;
			break;

		case 9:
			bias=valf*point;
			bias=bias*100/range;
			break;
			
		case 10:
			bias=valf*0.01;
			break;

		default:
			bias=valf;
			break;
	}

	return bias;
}

static int get_BJXueDiLong_MODEL1080_v4_pos(struct modbus_arg *modbusarg,char *data, int polId, int groups, int reg_type)
{
	int num=0,o2_num=0;
	int pos=0;
	int i=0;

	if(!modbusarg || !data)
		return 0;

	if(isPolcodeEnable(modbusarg, "a19001"))
		num=groups-1;
	o2_num=groups-num;

	switch(reg_type)
	{
		case 1:
			pos=(1+groups*2)*2+3;
			break;
			
		case 2:
			pos=(1+groups*2+2)*2+3;
			for(i=0;i<num;i++)
			{
				if(data[pos+i*8+3]==polId)
					break;
			}
			SYSLOG_DBG("YUIKI:pos=%d,polId=%d,data[pos+i*8+3]=%d,groups=%d,num=%d,o2_num=%d\n",pos,polId,data[pos+i*8+3],groups,num,o2_num);
			pos=pos+i*8;
			break;
			
		case 3:
			pos=(1+groups*2+2+num*4)*2+3;
			break;
		
		case 4:
			pos=(1+groups*2+2+num*4+o2_num*7)*2+3;
			for(i=0;i<groups;i++)
			{
				if(data[pos+i*18+7]==polId)
					break;
			}
			pos=pos+i*18;
			break;
			
		default:
			pos=0;
			break;
	}

	return pos;
}

static int get_BJXueDiLong_MODEL1080_v4_cnt(struct modbus_arg *modbusarg, int groups)
{
	int num=0,o2_num=0;
	int cnt;

	if(!modbusarg)
		return 0;

	if(isPolcodeEnable(modbusarg, "a19001"))
		num=groups-1;
	o2_num=groups-num;

	cnt=1+groups*2+2+num*4+o2_num*7+groups*9;

	return cnt;
}

static int get_BJXueDiLong_MODEL1080_v4_groups(struct modbus_arg *modbusarg)
{
	int num;

	if(!modbusarg)
		return 0;

	num=0;
	if(isPolcodeEnable(modbusarg, "a21026"))
		num++;
	if(isPolcodeEnable(modbusarg, "a21003"))
		num++;
	if(isPolcodeEnable(modbusarg, "a19001"))
		num++;
	if(isPolcodeEnable(modbusarg, "a05002"))
		num++;
	if(isPolcodeEnable(modbusarg, "a21005"))
		num++;
	if(isPolcodeEnable(modbusarg, "a05001"))
		num++;

	return num;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v4(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0};
	int size=0;
	float nox=0,so2=0,o2=0,no=0,ch4=0,co=0,co2=0;
	int val=0;
	int i=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,cnt=0,staddr=0;
	char *p;
	int groups=0;
	float point=0;
	char flag=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	groups=get_BJXueDiLong_MODEL1080_v4_groups(modbusarg);

	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	staddr=0x00;
	cnt=1+groups*2;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,staddr,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v4.0 CEMS ", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		if((p[4]&0x08)==0x08)
			point=0.1;
		else
			point=1;
	
		for(i=0;i<groups;i++)
		{
			val=getUInt16Value(p, 5+i*4, INT_AB);
			switch(p[8+i*4])
			{
				case 2:
					if(p[7+i*4]==2)
						co=gas_ppm_to_mg_m3(val, M_CO)*point;
					else if(p[7+i*4]==10)
						co=gas_ppm_to_mg_m3(val*0.01*10000.0, M_CO);
					else
						co=val*point;
					break;

				case 3:
					if(p[7+i*4]==2)
						co2=gas_ppm_to_mg_m3(val, M_CO2)*point;
					else
						co2=val*point;
					break;

				case 4:
					if(p[7+i*4]==2)
						ch4=gas_ppm_to_mg_m3(val, M_CH4)*point;
					else
						ch4=val*point;
					break;
				
				case 6:
					if(p[7+i*4]==2)
						so2=gas_ppm_to_mg_m3(val, M_SO2)*point;
					else
						so2=val*point;
					break;
					
				case 7:
					if(p[7+i*4]==2)
						no=gas_ppm_to_mg_m3(val, M_NO)*point;
					else
						no=val*point;
					break;
					
				case 12:
					if(p[7+i*4]==2)
						o2=val/10000.0*point;
					else
						o2=val*0.01;
					break;
			}
		}

		nox=NO_to_NOx(no);

		val=p[4]&0x0F;
		switch(val)
		{
			case 1:	flag='N';break;
			case 3:	flag='N';break;
			case 4:	flag='N';break;
			case 5:	flag='C';break;
			case 6:	flag='C';break;
			default:	flag=0;break;
		}
		
		status=0;
	}
	else
	{
		nox=0;
		so2=0;
		o2=0;
		no=0;
		co=0;
		co2=0;
		ch4=0;
		status=1;
	}

	acqdata_set_value(acq_data,"a21002a",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21002z",UNIT_MG_M3,nox,&arg_n);
	acqdata_set_value(acq_data,"a21003a",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21003z",UNIT_MG_M3,no,&arg_n);
	acqdata_set_value(acq_data,"a21026a",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a21026z",UNIT_MG_M3,so2,&arg_n);
	acqdata_set_value(acq_data,"a19001a",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a19001",UNIT_PECENT,o2,&arg_n);
	acqdata_set_value(acq_data,"a05001a",UNIT_MG_M3,co2,&arg_n);
	acqdata_set_value(acq_data,"a05001",UNIT_MG_M3,co2,&arg_n);
	acqdata_set_value(acq_data,"a05001z",UNIT_MG_M3,co2,&arg_n);
	acqdata_set_value(acq_data,"a21005a",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value(acq_data,"a21005",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value(acq_data,"a21005z",UNIT_MG_M3,co,&arg_n);
	acqdata_set_value(acq_data,"a05002",UNIT_MG_M3,ch4,&arg_n);
	
	if(status == 0)
	{
		acq_data->dev_stat = flag;
		acq_data->acq_status = ACQ_OK;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v4_SO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;
	int groups=0;
	int pos=0;
	int i=0;
	int polId=0;
	float range=0;
	float point=0;
	int mr=0;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v4_SO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a21026"))
		return 0;

	polId=6;
	mr=M_SO2;
	groups=get_BJXueDiLong_MODEL1080_v4_groups(modbusarg);

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=get_BJXueDiLong_MODEL1080_v4_cnt(modbusarg, groups);

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21026",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v4 SO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		point=((p[4]&0x08)==0x08)?0.1:1;
	
		val=p[3]&0x01;
		if(val!=0)
		{
			acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,2,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,1,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,99,INFOR_STATUS,&arg_n);
		}
		else
		{
			val=p[4]&0x07;
			switch(val)
			{
				case 0:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 1:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
				case 3:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 4:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,0,INFOR_STATUS,&arg_n);break;
				case 5:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,3,INFOR_STATUS,&arg_n);break;
				case 6:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,6,INFOR_STATUS,&arg_n);break;
				default:	acqdata_set_value_flag(acq_data,"i12007",UNIT_NONE,99,INFOR_STATUS,&arg_n);break;
			}
			acqdata_set_value_flag(acq_data,"i12008",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			acqdata_set_value_flag(acq_data,"i12009",UNIT_NONE,0,INFOR_STATUS,&arg_n);
		}
		

		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 4);
		
		valf=getUInt16Value(p, pos+12, dataType)*point;
		range=(p[pos+14]==0x02) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,range,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(p, pos+4, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+6], mr);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, pos+8, dataType)*point;
		valf=(p[pos+10]==2) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos+16, dataType);
		valf=val*0.01;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 1);
		
		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 2);
		
		valf=getInt16Value(p, pos, dataType);
		SYSLOG_DBG("YUIKI:groups=%d,pos=%d,valf=%.4f,range=%.4f,p[pos+2]=%d\n",groups,pos,valf,range,p[pos+2]);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+2], mr);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v4_NO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;
	int groups=0;
	int pos=0;
	int i=0;
	int polId=0;
	float range=0;
	float point=0;
	int mr=0;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v4_NO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a21003"))
		return 0;

	polId=7;
	mr=M_NO;
	groups=get_BJXueDiLong_MODEL1080_v4_groups(modbusarg);

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=get_BJXueDiLong_MODEL1080_v4_cnt(modbusarg, groups);

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21003",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v4 NO INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		point=((p[4]&0x08)==0x08)?0.1:1;
	
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 4);
		
		valf=getUInt16Value(p, pos+12, dataType)*point;
		range=(p[pos+14]==0x02) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,range,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(p, pos+4, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+6], mr);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, pos+8, dataType)*point;
		valf=(p[pos+10]==2) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos+16, dataType);
		valf=val*0.01;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 1);
		
		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 2);
		
		valf=getInt16Value(p, pos, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+2], mr);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v4_O2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;
	int groups=0;
	int pos=0;
	int i=0;
	int polId=0;
	float range=0;
	float point=0;
	int mr=0;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v4_O2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a19001"))
		return 0;

	polId=12;
	groups=get_BJXueDiLong_MODEL1080_v4_groups(modbusarg);

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=get_BJXueDiLong_MODEL1080_v4_cnt(modbusarg, groups);

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a19001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v4 O2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		point=((p[4]&0x08)==0x08)?0.1:1;
	
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 4);
		
		valf=getUInt16Value(p, pos+12, dataType);
		range=(p[pos+14]==0x02) ? gas_ppm_to_pecent(valf*point) : valf*0.01;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_PECENT,range,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(p, pos+4, dataType);
		valf=(p[pos+6]==2)?(gas_ppm_to_pecent(valf*point)*100/range):(valf*0.01);;
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, pos+8, dataType);
		valf=(p[pos+10]==2) ? gas_ppm_to_pecent(valf*point) : valf*0.01;
		acqdata_set_value_flag(acq_data,"i13028",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos+16, dataType);
		valf=val*0.01;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 3);
		
		val=getUInt16Value(p, pos+6, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+8];
		min=p[pos+9];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		SYSLOG_DBG("YUIKI O2:pos=%d,p=0x%X 0x%X 0x%X 0x%X,time=%04d%02d%02d%02d%02d%02d\n",
			pos,p[pos+6],p[pos+7],p[pos+8],p[pos+9],year, mon, day, hour, min, sec);
		
		valf=getInt16Value(p, pos+10, dataType);
		valf=(p[pos+12]==2)?(gas_ppm_to_pecent(valf*point)*100/range):(valf*0.01);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v4_CO_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;
	int groups=0;
	int pos=0;
	int i=0;
	int polId=0;
	float range=0;
	float point=0;
	int mr=0;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v4_CO_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a21005"))
		return 0;

	polId=2;
	mr=M_CO;
	groups=get_BJXueDiLong_MODEL1080_v4_groups(modbusarg);

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=get_BJXueDiLong_MODEL1080_v4_cnt(modbusarg, groups);

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a21005",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v4 CO INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		point=((p[4]&0x08)==0x08)?0.1:1;
	
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 4); //pos = 81
		
		valf=getUInt16Value(p, pos+12, dataType)*point; // 93rd byte  
		range=(p[pos+14]==0x02) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,range,INFOR_ARGUMENTS,&arg_n);

		SYSLOG_DBG("==>pos=%d,point=%f,range=%f,valf=%f\n",pos,point,range,valf);

		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(p, pos+4, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+6], mr);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, pos+8, dataType)*point;
		valf=(p[pos+10]==2) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos+16, dataType);
		valf=val*0.01;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 1);
		
		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 2);
		
		valf=getInt16Value(p, pos, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+2], mr);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v4_CO2_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;
	int groups=0;
	int pos=0;
	int i=0;
	int polId=0;
	float range=0;
	float point=0;
	int mr=0;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v4_CO2_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a05001"))
		return 0;

	polId=3;
	mr=M_CO2;
	groups=get_BJXueDiLong_MODEL1080_v4_groups(modbusarg);

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=get_BJXueDiLong_MODEL1080_v4_cnt(modbusarg, groups);

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a05001",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v4 CO2 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		point=((p[4]&0x08)==0x08)?0.1:1;
		
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 4);
		
		valf=getUInt16Value(p, pos+12, dataType)*point;
		range=(p[pos+14]==0x02) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,range,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(p, pos+4, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+6], mr);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, pos+8, dataType)*point;
		valf=(p[pos+10]==2) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos+16, dataType);
		valf=val*0.01;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 1);
		
		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 2);
		
		valf=getInt16Value(p, pos, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+2], mr);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_CEMS_BJXueDiLong_MODEL1080_v4_CH4_info(struct acquisition_data *acq_data)
{
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[20]={0};
	int size=0;
	int ret=0;
	int arg_n=0;
	float valf=0;
	int val=0;
	int year=0,mon=0,day=0,hour=0,min=0,sec=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	union uf f;
	char *p;
	int groups=0;
	int pos=0;
	int i=0;
	int polId=0;
	float range=0;
	float point=0;
	int mr=0;

	struct tm timer;
	time_t t1,t2,t3;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_MODEL1080_v4_CH4_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, "a05002"))
		return 0;

	polId=4;
	mr=M_CH4;
	groups=get_BJXueDiLong_MODEL1080_v4_groups(modbusarg);

	dataType=INT_AB;
	devaddr=modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=get_BJXueDiLong_MODEL1080_v4_cnt(modbusarg, groups);

	t1=0;//1577808000; // 2020-01-01 00:00:00
	acqdata_set_value_flag(acq_data,"a05002",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);
	status=1;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x00,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "BJXueDiLong MODEL1080 v4 CH4 INFO", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		point=((p[4]&0x08)==0x08)?0.1:1;
		
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 4);
		
		valf=getUInt16Value(p, pos+12, dataType)*point;
		range=(p[pos+14]==0x02) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13013",UNIT_MG_M3,range,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13027",t1,0,INFOR_ARGUMENTS,&arg_n);

		valf=getInt16Value(p, pos+4, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+6], mr);
		acqdata_set_value_flag(acq_data,"i13010",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);

		valf=getUInt16Value(p, pos+8, dataType)*point;
		valf=(p[pos+10]==2) ? gas_ppm_to_mg_m3(valf,mr) : valf;
		acqdata_set_value_flag(acq_data,"i13028",UNIT_MG_M3,valf,INFOR_ARGUMENTS,&arg_n);

		val=getUInt16Value(p, pos+16, dataType);
		valf=val*0.01;
		acqdata_set_value_flag(acq_data,"i13026",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);


		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 1);
		
		val=getUInt16Value(p, pos, dataType);
		year=(val>>9)&0x7F;
		year+=2000;
		mon=(val>>5)&0x0F;
		day=val&0x1F;
		hour=p[pos+2];
		min=p[pos+3];
		sec=0;
		t1=getTimeValue(year, mon, day, hour, min, sec);
		acqdata_set_value_flag(acq_data,"i13021",t1,0,INFOR_ARGUMENTS,&arg_n);
		
		pos=get_BJXueDiLong_MODEL1080_v4_pos(modbusarg, p, polId, groups, 2);
		
		valf=getInt16Value(p, pos, dataType);
		valf=get_BJXueDiLong_MODEL1080_v4_bias(valf, range, point, p[pos+2], mr);
		acqdata_set_value_flag(acq_data,"i13025",UNIT_PECENT,valf,INFOR_ARGUMENTS,&arg_n);
		
		status=0;
	}

	read_system_time(acq_data->acq_tm);

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}
