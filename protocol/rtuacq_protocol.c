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

int DEBUG_PRINT_VALUE;


static struct acquisition_data acq_data_bak[ARRAY_SIZE(acq_dev_list)];
static unsigned long cnt_global[ARRAY_SIZE(acq_dev_list)];

static unsigned long cnt_global_polcode[ARRAY_SIZE(acq_dev_list)][ARGUMENT_N];

int SongXia_read_pack(char *buf, unsigned int devaddr, unsigned int reg, unsigned int cnt)
{
	unsigned int ret;
	
	if(!buf || cnt<=0)
		return 0;

	ret=hex2ascii((char)devaddr);
	buf[0]='%';
	buf[1]=(ret>>16)&0xFF;
	buf[2]=ret&0xFF;
	buf[3]='#';
	buf[4]='R';
	buf[5]='D';
	buf[6]='D';
	sprintf(&buf[7],"%05X%05X",reg,reg+cnt-1);
	buf[17]='*';
	buf[18]='*';
	buf[19]=0x0D;

	return 20;
}

float PLCtoValue(struct modbus_arg *modbusarg, int minVal, int maxVal, float val, char *polcode)
{
	struct modbus_polcode_arg *mpolcodearg;
	int i=0;
	float valf=0;

	if(!modbusarg || !polcode) return -1;

	for(i=0;i<modbusarg->array_count ;i++)
	{
		mpolcodearg=&modbusarg->polcode_arg[i];
		if(!strcmp(mpolcodearg->polcode,polcode))
		{
			if(val<minVal) return mpolcodearg->alarmMin;
			
			valf=val-minVal;
			valf/=(float)(maxVal-minVal);
			valf*=(mpolcodearg->alarmMax-mpolcodearg->alarmMin);
			valf+=mpolcodearg->alarmMin;
			return valf;
		}

	}

	return 0;
}

time_t getTimeValue(int year, int mon, int day, int hour ,int min, int sec)
{
	struct tm timer;
	time_t t;

	if(year<1900 || mon<1 || day<1)
		return 0;

	timer.tm_year=year-1900;
	timer.tm_mon=mon-1;
	timer.tm_mday=day;
	timer.tm_hour=hour;
	timer.tm_min=min;
	timer.tm_sec=sec;

	t=mktime(&timer);

	if(t<0) t=0;

	return t;
}

float getFloatValue(char *buff,int offset, MODBUS_DATA_TYPE dataType)
{
	union uf f;
	
	if(!buff)
		return 0.0;

	switch(dataType)
	{
		case FLOAT_ABCD:
			f.ch[3]=buff[offset+0];
			f.ch[2]=buff[offset+1];
			f.ch[1]=buff[offset+2];
			f.ch[0]=buff[offset+3];
			break;

		case FLOAT_BADC:
			f.ch[3]=buff[offset+1];
			f.ch[2]=buff[offset+0];
			f.ch[1]=buff[offset+3];
			f.ch[0]=buff[offset+2];
			break;

		case FLOAT_CDAB:
			f.ch[3]=buff[offset+2];
			f.ch[2]=buff[offset+3];
			f.ch[1]=buff[offset+0];
			f.ch[0]=buff[offset+1];
			break;

		case FLOAT_DCBA:
			f.ch[3]=buff[offset+3];
			f.ch[2]=buff[offset+2];
			f.ch[1]=buff[offset+1];
			f.ch[0]=buff[offset+0];
			break;
		default :
			f.f=0.0;
	}

	return f.f;
}

int32_t getInt32Value(char *buff,int offset, MODBUS_DATA_TYPE dataType)
{
	union uf f;
	
	if(!buff)
		return 0.0;

	switch(dataType)
	{
		case LONG_ABCD:
			f.ch[3]=buff[offset+0];
			f.ch[2]=buff[offset+1];
			f.ch[1]=buff[offset+2];
			f.ch[0]=buff[offset+3];
			break;

		case LONG_BADC:
			f.ch[3]=buff[offset+1];
			f.ch[2]=buff[offset+0];
			f.ch[1]=buff[offset+3];
			f.ch[0]=buff[offset+2];
			break;

		case LONG_CDAB:
			f.ch[3]=buff[offset+2];
			f.ch[2]=buff[offset+3];
			f.ch[1]=buff[offset+0];
			f.ch[0]=buff[offset+1];
			break;

		case LONG_DCBA:
			f.ch[3]=buff[offset+3];
			f.ch[2]=buff[offset+2];
			f.ch[1]=buff[offset+1];
			f.ch[0]=buff[offset+0];
			break;
		default :
			f.l=0.0;
	}

	return f.l;
}

int16_t getInt16Value(char *buff,int offset, MODBUS_DATA_TYPE dataType)
{
	union uf f;
	
	if(!buff)
		return 0;

	switch(dataType)
	{
		case INT_AB:
			f.ch[1]=buff[offset+0];
			f.ch[0]=buff[offset+1];		
			break;

		case INT_BA:
			f.ch[1]=buff[offset+1];
			f.ch[0]=buff[offset+0];		
			break;

		default :
			f.i[0]=0.0;
	}

	return f.i[0];
}

unsigned short getUInt16Value(char *buff,int offset, MODBUS_DATA_TYPE dataType)
{
	union uf f;
	
	if(!buff)
		return 0;

	switch(dataType)
	{
		case INT_AB:
			f.ch[1]=buff[offset+0];
			f.ch[0]=buff[offset+1];		
			break;

		case INT_BA:
			f.ch[1]=buff[offset+1];
			f.ch[0]=buff[offset+0];		
			break;

		default :
			f.i[0]=0.0;
	}

	return f.i[0];
}

int  get_cmd_03_04_pos(int startaddr,int addr)
{
	return (startaddr>addr) ? 0: ((addr-startaddr)*2+3);
}

int  get_cmd_10_pos(int startaddr,int addr)
{
	return (startaddr>addr) ? 0: ((addr-startaddr)*2+7);
}

/*
float getNOx2NO(float no)
{
	return no/30*46;
}
*/
float NO_to_NOx(float no)
{
	return NO_and_NO2_to_NOx(no,0);
}

float NO_and_NO2_to_NOx(float no, float no2)
{
	return no/M_NO*M_NO2+no2;
}

float gas_ppm_to_pecent(float ppm)
{
	return ppm/10000;
}

float gas_ppm_to_mg_m3(float ppm, int mr)
{
	return ppm/GAS_MOLAR_V*mr;
}

float gas_ppb_to_mg_m3(float ppb, int mr)
{
	return gas_ppm_to_mg_m3(ppb*0.001,mr);
}

float water_US_gal_to_L(float usgal)
{
	return usgal*3.785412;
}

float water_US_gal_to_m3(float usgal)
{
	return water_US_gal_to_L(usgal)*0.001;
}

float water_UK_gal_to_L(float ukgal)
{
	return ukgal*4.546092;
}

float water_UK_gal_to_m3(float ukgal)
{
	return water_US_gal_to_L(ukgal)*0.001;
}

int get_acq_data_by_devname(char *devname,struct acquisition_data *acq_data)
{
	int i=0;
	int pos=0;

	if(!devname || !acq_data) return 0;

	for(i=0;i<ARRAY_SIZE(acq_dev_list);i++)
	{
		if(!strcmp(acq_dev_list[i],devname))
		{
			pos=i;
			break;
		}
	}

	//pos=(i>=ARRAY_SIZE(acq_dev_list)) ? 0: i;

	memcpy(acq_data,&acq_data_bak[pos],sizeof(struct acquisition_data));

	return 0;
}

void set_acq_data_by_devname(char *devname,struct acquisition_data acqdata)
{
	int i=0;

	for(i=0;i<ARRAY_SIZE(acq_dev_list);i++)
	{
		if(!strcmp(acq_dev_list[i],devname))
		{
			acq_data_bak[i]=acqdata;
			break;
		}
	}
}

unsigned long get_cnt_by_devname(char *devname)
{
	int i=0;
	int pos=0;

	for(i=0;i<ARRAY_SIZE(acq_dev_list);i++)
	{
		if(!strcmp(acq_dev_list[i],devname))
		{
			pos=i;
			break;
		}
	}

	//pos=(i>=ARRAY_SIZE(acq_dev_list)) ? 0: i;

	return cnt_global[pos];
}

void set_cnt_by_devname(char *devname,unsigned long cnt)
{
	int i=0;

	for(i=0;i<ARRAY_SIZE(acq_dev_list);i++)
	{
		if(!strcmp(acq_dev_list[i],devname))
		{
			cnt_global[i]=cnt;
			break;
		}
	}
}

void NEED_ERROR_CACHE(struct acquisition_data *acq_data,int cnt_max)  
{

   unsigned long cnt=get_cnt_by_devname(DEV_NAME(acq_data));
   //struct acquisition_data acq_data_bak=get_acq_data_by_devname(DEV_NAME(acq_data));
   struct acquisition_data acq_data_bak;

   get_acq_data_by_devname(DEV_NAME(acq_data),&acq_data_bak);
   	
   if(!acq_data) return ;

   if(cnt_max<=0 && cnt_max>=100) cnt_max=20;
   
    //  step1
	if(acq_data->acq_status==ACQ_ERR && cnt==0) return ;
    // step2
	if(acq_data->acq_status==ACQ_OK)
	{
	    cnt=1;// at fist time acq data ok
	    set_cnt_by_devname(DEV_NAME(acq_data),cnt);
		acq_data_bak=*acq_data;
		set_acq_data_by_devname(DEV_NAME(acq_data),acq_data_bak);
		return ;
	}
	//step3 acq_data->acq_status==ACQ_ERR 
	if(acq_data->acq_status==ACQ_ERR && cnt>0)
	{
	     cnt++;
	     set_cnt_by_devname(DEV_NAME(acq_data),cnt);
		 if(cnt>cnt_max)
		 {
		     cnt=1; // return to 1 and increase again
		     set_cnt_by_devname(DEV_NAME(acq_data),cnt);
			 acq_data_bak=*acq_data;
			 set_acq_data_by_devname(DEV_NAME(acq_data),acq_data_bak);
		 }
		 else
		 {
		   //acq_data_bak=get_acq_data_by_devname(DEV_NAME(acq_data));
		   get_acq_data_by_devname(DEV_NAME(acq_data),&acq_data_bak);
		   *acq_data=acq_data_bak;
		   acq_data->total=0;
		   acq_data->zs_total=0;		   
		 }

		 return ;
	}

	return ;
}


static int is_polcode_value_error(struct acquisition_data *acq_data,int index)
{
	if(acq_data->flag[index]=='D' || acq_data->flag[index]=='V')
	{
		return 1;
	}

	return 0;
}

static unsigned long get_cnt_by_devname_polcode(char *devname,int index)
{
	int i=0;
	int pos=0;

	for(i=0;i<ARRAY_SIZE(acq_dev_list);i++)
	{
		if(!strcmp(acq_dev_list[i],devname))
		{
			pos=i;
			break;
		}
	}

	return cnt_global_polcode[pos][index];
}

static void set_cnt_by_devname_polcode(char *devname,int cnt,int index)
{
	int i=0;

	for(i=0;i<ARRAY_SIZE(acq_dev_list);i++)
	{
		if(!strcmp(acq_dev_list[i],devname))
		{
			cnt_global_polcode[i][index]=cnt;
			break;
		}
	}

	return;
}

static void set_one_polcode_val(struct acquisition_data *target,struct acquisition_data *source,int index)
{
	if(!source || !target)
		return;

	strcpy(target->polcode[index],source->polcode[index]);
	target->com_f[index]=source->com_f[index];
	target->unit[index]=source->unit[index];
	target->flag[index]=source->flag[index];
}

void NEED_ERROR_CACHE_ONE_POLCODE_VALUE(struct acquisition_data *acq_data,int arg_n,int cnt_max)
{
	int i=0;
	unsigned long cnt=0;
	struct acquisition_data acq_data_bak;

	if(!acq_data) return;
	if(cnt_max<=0)
		cnt_max=20;
	else if(cnt_max>=50)
		cnt_max=50;

	memset(&acq_data_bak,0,sizeof(struct acquisition_data));
	get_acq_data_by_devname(DEV_NAME(acq_data),&acq_data_bak);

	for(i=0;i<arg_n;i++)
	{
		cnt=get_cnt_by_devname_polcode(DEV_NAME(acq_data),i);

		//SYSLOG_DBG("ERROR_NEED1:%d>%s,cnt %d\n",i,acq_data->polcode[i],cnt);

		//step 1
		if(is_polcode_value_error(acq_data,i) && cnt==0)
			continue;

		//SYSLOG_DBG("ERROR_NEED2:%d>%s,cnt %d\n",i,acq_data->polcode[i],cnt);

		//step 2
		if(!is_polcode_value_error(acq_data,i))
		{
			cnt=1;// at fist time acq data ok
			set_cnt_by_devname_polcode(DEV_NAME(acq_data),cnt,i);
			set_one_polcode_val(&acq_data_bak,acq_data,i);
			//SYSLOG_DBG("ERROR_NEED3:%d>%s,cnt %d\n",i,acq_data->polcode[i],cnt);
			continue;
		}

		//step 3
		if(is_polcode_value_error(acq_data,i) && cnt>0)
		{
		    //SYSLOG_DBG("ERROR_NEED4:%d>%s,cnt %d\n",i,acq_data->polcode[i],cnt);
			cnt++;
			//set_cnt_by_devname(DEV_NAME(acq_data),cnt); //del by miles zhang 20210610
			set_cnt_by_devname_polcode(DEV_NAME(acq_data),cnt,i);
			if(cnt>cnt_max)
			{
				cnt=0;
				set_cnt_by_devname_polcode(DEV_NAME(acq_data),cnt,i);
				set_one_polcode_val(&acq_data_bak,acq_data,i);

				//SYSLOG_DBG("ERROR_NEED5:%d>%s,cnt %d\n",i,acq_data->polcode[i],cnt);
			}
			else
			{
				set_one_polcode_val(acq_data,&acq_data_bak,i);
				//SYSLOG_DBG("ERROR_NEED6:%d>%s,cnt %d\n",i,acq_data->polcode[i],cnt);
			}
			continue;
		}
	}
    //SYSLOG_DBG("ERROR_NEED7:%d>%d,%d\n",i,arg_n,cnt_max);
	set_acq_data_by_devname(DEV_NAME(acq_data),acq_data_bak);
	return;
}




PROTOCOL_T lookup_protocol(int instrument_code)
{
        int  i=0;
        for(i=0;i<ARRAY_SIZE(instrument_protocols);i++)
        {
            if(instrument_protocols[i].instrument_code==instrument_code)
                return instrument_protocols[i].protocol;
        }
        return NULL;
}

#if 0
int acqdata_set_value(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,int *arg_n)
{
        float *pf;

	if(!acq_data || !arg_n || !polcode) return -1;
	
	if(*arg_n<0 || *arg_n>=ARGUMENT_N) return -1;
	
       pf=acq_data->com_f;  
	pf[*arg_n]=value;
	strcpy(acq_data->polcode[*arg_n],polcode); 
	acq_data->unit[*arg_n]=unit;
	*arg_n += 1;
	
	return 0;
}

int acqdata_set_value_flag(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,char flag,int *arg_n)
{
	float *pf;

	if(!acq_data || !arg_n || !polcode) return -1;

	if(*arg_n<0 || *arg_n>=ARGUMENT_N) return -1;

	pf=acq_data->com_f;
	pf[*arg_n]=value;
	strcpy(acq_data->polcode[*arg_n],polcode); 
	acq_data->unit[*arg_n]=unit;
	acq_data->flag[*arg_n]=flag;
	*arg_n += 1;

	return 0;
}

int acqdata_set_value_orig(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,float orig_val,int *arg_n)
{
        float *pf;

	if(!acq_data || !arg_n || !polcode) return -1;
	
	if(*arg_n<0 || *arg_n>=ARGUMENT_N) return -1;
	
       pf=acq_data->com_f;  
	pf[*arg_n]=value;
	strcpy(acq_data->polcode[*arg_n],polcode); 
	acq_data->unit[*arg_n]=unit;
	acq_data->orig_f[*arg_n]=orig_val;
	*arg_n += 1;
	
	return 0;
}

#else

int acqdata_set_value(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,int *arg_n)
{
	float *pf;

	struct modbus_arg *modbusarg;
	struct acquisition_ctrl *acq_ctrl;

	if(!acq_data || !arg_n || !polcode) return -1;

	if(*arg_n<0 || *arg_n>=ARGUMENT_N) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, polcode))
		return 0;

	pf=acq_data->com_f;  
	pf[*arg_n]=value;
	strcpy(acq_data->polcode[*arg_n],polcode); 
	acq_data->unit[*arg_n]=unit;
	*arg_n += 1;

	return 0;
}

int acqdata_set_value_flag(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,char flag,int *arg_n)
{
	float *pf;

	struct modbus_arg *modbusarg;
	struct acquisition_ctrl *acq_ctrl;

	if(!acq_data || !arg_n || !polcode) return -1;

	if(*arg_n<0 || *arg_n>=ARGUMENT_N) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if((!isPolcodeEnable(modbusarg, polcode)) && 
		(flag!=INFOR_ALL) && 
		(flag!=INFOR_STATUS) &&
		(flag!=INFOR_ARGUMENTS) &&
		(flag!=INFOR_JPGS))
		return 0;

	pf=acq_data->com_f;
	if(isnan(value))	//add yuiki 2021-12-06
		value=0;
	pf[*arg_n]=value;
	strcpy(acq_data->polcode[*arg_n],polcode); 
	acq_data->unit[*arg_n]=unit;
	acq_data->flag[*arg_n]=flag;
	*arg_n += 1;

	return 0;
}

int acqdata_set_value_orig(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,float orig_val,int *arg_n)
{
	float *pf;

	struct modbus_arg *modbusarg;
	struct acquisition_ctrl *acq_ctrl;

	if(!acq_data || !arg_n || !polcode) return -1;

	if(*arg_n<0 || *arg_n>=ARGUMENT_N) return -1;

	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	if(!isPolcodeEnable(modbusarg, polcode))
		return 0;

	pf=acq_data->com_f;  
	pf[*arg_n]=value;
	strcpy(acq_data->polcode[*arg_n],polcode); 
	acq_data->unit[*arg_n]=unit;
	acq_data->orig_f[*arg_n]=orig_val;
	*arg_n += 1;

	return 0;
}
#endif

int acqdata_get_value(struct acquisition_data *acq_data,char *polcode,float *valf)
{
	int i=0;
	if(!acq_data || !valf || !polcode) return -1;
	
	for(i=0;i<ARGUMENT_N;i++)
	{
		if(!strcmp(acq_data->polcode[i],polcode))
		{
			*valf=acq_data->com_f[i];
			return 0;
		}
	}
	return -2;
}


int acqdata_set_rtdata(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,char flag,int *arg_n)
{
        float *pf;

	if(!acq_data || !arg_n || !polcode) return -1;
	
	if(*arg_n<0 || *arg_n>=ARGUMENT_N) return -1;

	acq_data->cn=2011;
	
    pf=acq_data->com_f;  
	pf[*arg_n]=value;
	strcpy(acq_data->polcode[*arg_n],polcode); 
	acq_data->unit[*arg_n]=unit;
	acq_data->flag[*arg_n]=flag;
	*arg_n += 1;
	
	return 0;
}

int acqdata_set_stattdata(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float avg,float min,float max,float cou,char flag,int *arg_n)
{
        float *pf;

	if(!acq_data || !arg_n || !polcode) return -1;
	
	if(*arg_n<0 || *arg_n>=ARGUMENT_N) return -1;

	
	
	acq_data->com_f[*arg_n]=avg;
	acq_data->min_f[*arg_n]=min;
	acq_data->max_f[*arg_n]=max;
	acq_data->cou_f[*arg_n]=cou;
	acq_data->flag[*arg_n]=flag;
	
	strcpy(acq_data->polcode[*arg_n],polcode); 
	acq_data->unit[*arg_n]=unit;
	*arg_n += 1;
	
	return 0;
}


int  modbus_pack(char modpack[8],char addr, char cmd, int reg, int cnt)
{
        unsigned int  crc;
        
        modpack[0] = addr;
        modpack[1] = cmd;
        modpack[2] = (char)(reg >> 8)&0xff;
        modpack[3] = (char)(reg & 0xff);
        modpack[4] = (char)(cnt >> 8)&0xff;
        modpack[5] = (char)(cnt & 0xff);
        crc = CRC16_modbus(modpack, 6);
        modpack[7] = (char)(crc >> 8);
        modpack[6] = (char)(crc & 0xff);
        return 8;
}

unsigned int hex2ascii(char hex)
{
	char hi,lo;
	unsigned int ret;

	hi=(hex>>4)&0x0F;
	lo=hex&0x0F;

	if(hi>=0x00 && hi<=0x09)
		hi+=0x30;
	else
		hi=hi-0x0A+0x41;

	if(lo>=0x00 && lo<=0x09)
		lo+=0x30;
	else
		lo=lo-0x0A+0x41;

	ret=hi;
	ret<<=8;
	ret+=lo;

	return ret;
}

char ascii2hex(char hi, char lo)
{
	char hex;

	if(hi>='0' && hi<='9')
		hi-=0x30;
	else if(hi>='A' && hi<='F')
		hi=hi-0x41+0x0A;
	else if(hi>='a' && hi<='f')
		hi=hi-0x61+0x0A;
	else
		return 0;

	if(lo>='0' && lo<='9')
		lo-=0x30;
	else if(lo>='A' && lo<='F')
		lo=lo-0x41+0x0A;
	else if(lo>='a' && lo<='f')
		lo=lo-0x61+0x0A;
	else
		return 0;

	hex=hi;
	hex<<=4;
	hex+=lo;

	return hex;
}

int modbus_ascii_pack_to_hex_pack(char *buff,int size,char *p)
{
	int i,j,len;

	len=size-2;
	for(i=1,j=0;i<len;i+=2,j++)
	{
		p[j]=ascii2hex(buff[i],buff[i+1]);
	}

	return j+1;
}

unsigned int LRC16_modbus(unsigned char* buf, int len)
{
	unsigned int sum=0;
	int i;
	char hex[1024];
	int size;
	

	if(!buf)
		return 0;

	memset(hex,0,sizeof(hex));
	size=modbus_ascii_pack_to_hex_pack(buf,len+2,hex);

	for(i=0;i<size;i++)
	{		
		sum+=hex[i];
	}

	sum%=0x100;
	sum=0x100-sum;

	return hex2ascii((char)sum);;
}

int  modbus_ascii_pack(char *modpack,char addr, char cmd, int reg, int cnt)
{
	unsigned int  lrc,ascii;

	if(!modpack)
		return 0;

	modpack[0] = 0x3A;	// ':' ascii
	
	ascii=hex2ascii(addr);
	modpack[1] = (char)((ascii >> 8) & 0xFF);
	modpack[2] = (char)(ascii & 0xFF);
	
	ascii=hex2ascii(cmd);
	modpack[3] = (char)((ascii >> 8) & 0xFF);
	modpack[4] = (char)(ascii & 0xFF);
	
	ascii=hex2ascii((char)((reg >> 8)&0xff));
	modpack[5] = (char)((ascii >> 8) & 0xFF);
	modpack[6] = (char)(ascii & 0xFF);
	
	ascii=hex2ascii((char)(reg & 0xff));
	modpack[7] = (char)((ascii >> 8) & 0xFF);
	modpack[8] = (char)(ascii & 0xFF);
	
	ascii=hex2ascii((char)((cnt >> 8)&0xff));
	modpack[9] = (char)((ascii >> 8) & 0xFF);
	modpack[10] = (char)(ascii & 0xFF);
	
	ascii=hex2ascii((char)(cnt & 0xff));
	modpack[11] = (char)((ascii >> 8) & 0xFF);
	modpack[12] = (char)(ascii & 0xFF);
	
	lrc = LRC16_modbus(modpack, 13);
	modpack[13] = (char)((lrc >> 8)& 0xFF);
	modpack[14] = (char)(lrc & 0xFF);

	modpack[15] = 0x0D;
	modpack[16] = 0x0A;
	
	return 17;
}

int modbus_ascii_pack_check(char *buff, int devaddr, int cmd)
{
	unsigned int ascii;
	char check[6];
	
	if(!buff)
		return 0;

	check[0] = 0x3A;	// ':' ascii
	ascii=hex2ascii(devaddr);
	check[1] = (char)((ascii >> 8) & 0xFF);
	check[2] = (char)(ascii & 0xFF);
	ascii=hex2ascii(cmd);
	check[3] = (char)((ascii >> 8) & 0xFF);
	check[4] = (char)(ascii & 0xFF);
	check[5] = 0x00;
	
	if(!strncmp(buff,check,5))
		return 1;
	else
		return 0;
}

char *modbus_ascii_lrc_check(char *buff,int buff_size, unsigned int devaddr, unsigned int cmd, unsigned int cnt)
{
	unsigned int ascii=0,lrc=0;
	char check[8];
	int len=0,offset=0;
	char *p=NULL;
	
	if(!buff)
		return NULL;

	check[0] = 0x3A;	// ':' ascii
	ascii=hex2ascii(devaddr);
	check[1] = (char)((ascii >> 8) & 0xFF);
	check[2] = (char)(ascii & 0xFF);
	ascii=hex2ascii(cmd);
	check[3] = (char)((ascii >> 8) & 0xFF);
	check[4] = (char)(ascii & 0xFF);
	ascii=hex2ascii(len);
	check[5] = (char)((ascii >> 8) & 0xFF);
	check[6] = (char)(ascii & 0xFF);
	check[7] = 0x00;
	
	p=memstr(buff, buff_size, check);
	if(p==NULL)
		return NULL;

	len=((cnt*2)+3)*2;
	offset=p-buff;
	if((offset+len+5)>buff_size)
		return NULL;

	lrc=LRC16_modbus(p, len+1);
	if((p[len+1] != ((lrc>>8)&0xFF)) || (p[len+2] != (lrc&0xFF)) || (p[len+3] != 0x0D) || (p[len+4] != 0x0A))
		return NULL;

	return p;
}

unsigned int SUM_CHECK(char *buff, int size)
{
	int i;
	int sum;

	if(!buff)
		return 0;
	
	sum=0;
	for(i=0;i<size;i++)
		sum+=buff[i];

	return hex2ascii((char)(sum&0xFF));
}

unsigned int SUM_CHECK_HEX(char *buff, int size)
{
	int i;
	int sum;

	if(!buff)
		return 0;
	
	sum=0;
	for(i=0;i<size;i++)
		sum+=buff[i];

	return sum&0xFF;
}

int JuGuang_ai_pack7D7B_valid(char *com_rbuf,int size)
{//7D 7B 40 80 00 00 40 80 00 00 41 12 CB 3D 41 5A 79 BF 41 1F 5D C6 40 80 00 00 40 A3 33 D4 40 80 00 00 FF 43 2E 7D 7D
//CRC2=0x2E43
    unsigned int CRC1=0;
	unsigned int CRC2=0;
	if((size<39) || (com_rbuf[0]!=0x7D) || (com_rbuf[1]!=0x7B)) return 1; // error packet

    CRC1=com_rbuf[36];
	CRC1<<=8;
	CRC1+=com_rbuf[35];
	CRC2=CRC16_modbus(&com_rbuf[2],33);

	if(CRC1!=CRC2) return 2;// error packet


	return 0;// ok packet
}

int modbus_pack_print(char *modbus_cache,int len)
{
          int i=0;
	   printf("read modbus : len=%d\n",len);
	   for(i=0;i<len;i++)
	   {
	     printf("%.2x ",modbus_cache[i]&0xff);
	   }
	   printf("\n"); 
	   
	   return 0;
}


int modbus_get_valf(char *com_rbuf,unsigned int size,unsigned int regaddr,MODBUS_DATA_TYPE datatype,float *valf)
{
    union uf  f;
	unsigned int datalen=0;
	int datapos=0;

	if(!com_rbuf || !valf) return -1;

	if(regaddr>=size) return -1;

    datalen=com_rbuf[2];
	if(regaddr>=datalen) return -2;

#define MODBUS_RECV_DATA_POS 3

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
			//[0]0x07 ,[1]0xE5 = 2021
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


struct modbus_polcode_arg * find_modbus_polcode_arg_by_polcode(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode)
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


char hex2dec(char c)
{
    char n;
    n = (c / 0x10) * 10 + c % 0x10;
   return n;
}

int HexCharToDec(char c_num)
{
   if ((c_num >= '0')&&(c_num <= '9'))
      return (c_num - '0');
   else if((c_num >= 'a')&&(c_num <= 'f'))
      return (c_num - 'a' + 10);
   else if((c_num >= 'A')&&(c_num <= 'F'))
      return (c_num - 'A' + 10);
   else
      return -1;
}

char StrHexToHex(char c)
{
	if(c >= 0x30 && c <= 0x39)//0~9
   {
   		c -= 0x30;
   }
   else if(c >= 0x41 && c <= 0x46)//A~F
   {
   		c -= 0x37;
   }
   else if(c >= 0x61 && c <= 0x66)//a~f
   {
   		c -= 0x57;
   }
   else
   {
   		c = 0;
   }

   return c;
}

static unsigned char LRC(char chkdata[], unsigned int chklen)
{
	unsigned char lrcValue;
	unsigned int i;

   lrcValue = 0;
   for (i = 1; i < chklen; i++)
   {
   		lrcValue += chkdata[i];
   }
   lrcValue = (unsigned char)(-((char)lrcValue));
   return lrcValue;
}

int modbusAsciiPacket(char modpack[17],char addr,char cmd,int reg,int cnt)
{
	char sendData[10];
   union char2int intchr;

   memset(sendData, 0x00, sizeof(sendData));

   sendData[1] = addr;
   sendData[2] = cmd;
   intchr.i = reg;
   sendData[3] = intchr.ch[1];
   sendData[4] = intchr.ch[0];
   intchr.i = cnt;
   sendData[5] = intchr.ch[1];
   sendData[6] = intchr.ch[0];
   sendData[7] = LRC(sendData, 7);
   sprintf(modpack, ":%02X%02X%02X%02X%02X%02X%02X",
   sendData[1], sendData[2], sendData[3], sendData[4], sendData[5], sendData[6], sendData[7]);
   modpack[15] = 0x0D;
   modpack[16] = 0x0A;

	return 17;
}


void SYSLOG_DBG_HEX(char *str,char *hex,int hex_size)
{
   char strcache[1024]={0};
   int i=0;

   if(!hex || hex_size<=0) return ;
   
   memset(strcache,0,sizeof(strcache));
   for(i=0;(i<hex_size) && (i<200);i++)
   	 sprintf(&strcache[strlen(strcache)],"%.2x ",hex[i]);
   
   SYSLOG_DBG("%s size=%d,%s\n",str,hex_size,strcache);
}

void LOG_WRITE_HEX(char *devname,int send_or_recv,char *str,char *hex,int hex_size)
{//send =0 , recv =1 
   char strcache[1024]={0};
   int serno=0;
   int i=0;

   if(!hex || hex_size<0) return ;
   
   memset(strcache,0,sizeof(strcache));
   for(i=0;(i<hex_size) && (i<200);i++)
   	 sprintf(&strcache[strlen(strcache)],"%.2x ",hex[i]);

   if(devname)
   {
		sscanf(devname,"serial%d",&serno);
   }
   
   write_instrument_log(serno,send_or_recv,"%s size=%d,%s\n",str,hex_size,strcache);
}

void LOG_WRITE_STR(char *devname,int send_or_recv,char *str,char *hex,int hex_size)
{//send =0 , recv =1 
   char strcache[2048]={0};
   int serno=0;
   int i=0;

   if(!hex || hex_size<0 || hex_size>=2040) return ;
   
   //memset(strcache,0,sizeof(strcache));
   //for(i=0;(i<hex_size) && (i<200);i++)
   //  sprintf(&strcache[strlen(strcache)],"%.2x ",hex[i]);

   hex_size=(hex_size>=500) ? 500 : hex_size;

   memcpy(strcache,hex,hex_size);

   if(devname)
   {
  	sscanf(devname,"serial%d",&serno);
   }
   
   write_instrument_log(serno,send_or_recv,"%s size=%d,%s\n",str,hex_size,strcache);
}

int songxia(char *ppack,int size,float *fvalue,int model,int *status)
{
	int multiple=1,tem=0;
	char pch[50]={0};
	if(!ppack||!fvalue||!status) return -1;
	if((size>=29)&&(ppack[0]=='%')&&(ppack[3]=='$')&&(ppack[4]=='R'))
	 {
		strncpy(pch,&ppack[18],8);
		tem=atoi(pch);
		switch(model){
			case 1:multiple=10;
				break;
			case 2:multiple=100;
				break;
			case 3:multiple=1000;
				break;
			case 4:multiple=10000;
				break;
			default:return -2;
		}
		*fvalue=((float)tem)/multiple;
		/*printf("ppack:%s\n",ppack);
		printf("p:%s\n",pch);
		printf("value=%d\n",tem);*/
		printf("value=%f\n",*fvalue);
		*status=0;
		return 0;
	}
	else
	{
		*fvalue=0;
		*status=1;
		//printf("ppack:%s\n",ppack);
		return -3;
	}

	return 0;
}


#if 1
int protocol_hjt212_test(struct acquisition_data *acq_data)
{
    int status=0;
    float *pf;
    char com_rbuf[2048]; 
    char modpack[8];
    int size=0;
    union uf  f;
    int arg_n=0;
    float month=0,day=0,hours=0,minutes=0;
    struct tm tm_struct;
    time_t time_long;
    int time_array[7]={0};
#if 0
    //time(&time_long);
    //gmtime_r(&time_long,&tm_struct);
    //memcpy(&tm_struct,localtime(&time_long),sizeof(struct tm));
    read_system_time(time_array);
	
    month = time_array[4];//tm_struct.tm_mon+1;
    day =time_array[3];// tm_struct.tm_mday;
    hours =time_array[2];// tm_struct.tm_hour;
    minutes = time_array[1];//tm_struct.tm_min;

    SYSLOG_DBG("month:%f,day:%f,hours:%f,minutes:%f\n",month,day,hours,minutes);
    
    pf=acq_data->com_f;  
    pf[arg_n]=100+hours*10+hours/10;//rand()%15+5;
    strcpy(acq_data->polcode[arg_n],"01"); 
    acq_data->unit[arg_n]=UNIT_MG_M3;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=rand()%25+5;
    strcpy(acq_data->polcode[arg_n],"01z"); 
    acq_data->unit[arg_n]=UNIT_MG_M3;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=100+hours*10+hours/10;//rand()%5+5;
    strcpy(acq_data->polcode[arg_n],"02"); 
    acq_data->unit[arg_n]=UNIT_MG_M3;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=rand()%15+5;
    strcpy(acq_data->polcode[arg_n],"02z"); 
    acq_data->unit[arg_n]=UNIT_MG_M3;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=100+hours*10+hours/10;//rand()%10+5;
    strcpy(acq_data->polcode[arg_n],"03"); 
    acq_data->unit[arg_n]=UNIT_MG_M3;
    status=0;
    arg_n++;
    
        pf=acq_data->com_f;  
    pf[arg_n]=rand()%15+5;
    strcpy(acq_data->polcode[arg_n],"03z"); 
    acq_data->unit[arg_n]=UNIT_MG_M3;
    status=0;
    arg_n++;
    
        pf=acq_data->com_f;  
    pf[arg_n]=100+hours*10+hours/10;//rand()%10+5;
    strcpy(acq_data->polcode[arg_n],"04"); 
    acq_data->unit[arg_n]=UNIT_MG_M3;
    status=0;
    arg_n++;
    
        pf=acq_data->com_f;  
    pf[arg_n]=rand()%15+5;
    strcpy(acq_data->polcode[arg_n],"04z"); 
    acq_data->unit[arg_n]=UNIT_MG_M3;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=9+hours/2;//rand()%15+5;
    strcpy(acq_data->polcode[arg_n],"S01"); 
    acq_data->unit[arg_n]=UNIT_PECENT;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=0+minutes*10+minutes/10;//rand()%25+5;
    strcpy(acq_data->polcode[arg_n],"S02"); 
    acq_data->unit[arg_n]=UNIT_M_S;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=day+40;//rand()%5+5;
    //pf[arg_n]=7;
    strcpy(acq_data->polcode[arg_n],"S03"); 
    acq_data->unit[arg_n]=UNIT_0C;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=minutes*10;//rand()%15+5;
    //pf[arg_n]=16;
    strcpy(acq_data->polcode[arg_n],"S04"); 
    acq_data->unit[arg_n]=UNIT_PA;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=day+40;//rand()%10+5;
    //pf[arg_n]=13;
    strcpy(acq_data->polcode[arg_n],"S05"); 
    acq_data->unit[arg_n]=UNIT_PECENT;
    status=0;
    arg_n++;
    
        pf=acq_data->com_f;  
    pf[arg_n]=minutes*10;//rand()%15+5;
    //pf[arg_n]=7;
    strcpy(acq_data->polcode[arg_n],"S08"); 
    acq_data->unit[arg_n]=UNIT_PA;
    status=0;
    arg_n++;

            pf=acq_data->com_f;  
    pf[arg_n]=100+hours*10+hours/10;//rand()%50+25;
    //pf[arg_n]=69;
    strcpy(acq_data->polcode[arg_n],"B02"); 
    acq_data->unit[arg_n]=UNIT_M3_H;
    status=0;
    arg_n++;

    
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
#endif
    return arg_n;
}
#endif


int protocol_temperature(struct acquisition_data *acq_data)
{
    char rbuff[100]={0},rbuff2[100]={0};
    int len=0,len2=0;
    float *pf=acq_data->com_f; 
    static float temperature=0;
    int arg_n=0;
    int ret=0;
    int status=0;

    len=write_device(DEV_NAME(acq_data),"temperature",sizeof("temperature"));
    if(len>0)
    {
	    sleep(5);
	    memset(rbuff,0,sizeof(rbuff));
	    len=read_device(DEV_NAME(acq_data),rbuff,sizeof(rbuff)-1);
	    if(len>0)
	    {
	        SYSLOG_DBG("protocol_temperature : %s\n",rbuff);
	        
	        ret=sscanf(rbuff,"current temperature : %f",&pf[arg_n]);
	        if(ret==1)
	        {
	            temperature = pf[arg_n];
	            strcpy(acq_data->polcode[arg_n],"301"); 
	            acq_data->unit[arg_n]=UNIT_0C;
	            status=0;
	            arg_n++;
	        }
	        else 
	        {
	                temperature = pf[arg_n]=0;
	                strcpy(acq_data->polcode[arg_n],"301"); 
	                acq_data->unit[arg_n]=UNIT_0C;
	                status=1;
	                arg_n++;
	        }
	    }
	    else
	    {
	        pf[arg_n]=0;
	        strcpy(acq_data->polcode[arg_n],"301"); 
	        acq_data->unit[arg_n]=UNIT_0C;
	        status=1;
	        arg_n++;     
	    }
    }
    else
	{
		 pf[arg_n]=0;
	        strcpy(acq_data->polcode[arg_n],"301"); 
	        acq_data->unit[arg_n]=UNIT_0C;
	        status=1;
	        arg_n++;     
	}
    
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;

}


int protocol_RONGYUAN_RYIS01501(struct acquisition_data *acq_data)
{
    int status=0;
    float *pf;
    char com_rbuf[2048]; 
    char modpack[8];
    int size=0;
    union uf  f;
    int arg_n=0;
    float month=0,day=0,hours=0,minutes=0;
    struct tm tm_struct;
    time_t time_long;
    int time_array[7]={0};
    int i=0;
    int temp,o2_val;
    float ftemp,fo2_val;

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=modbus_pack(com_rbuf,0x1,0x3,0x1000,0x2);
	size=write_device(DEV_NAME(acq_data),com_rbuf,size);
	
	for(i=0;i<size;i++)
	{
		printf("tx:%x\n",com_rbuf[i]);
	}
	sleep(2);

	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	printf("rx len=%d\n",size);
	for(i=0;i<size;i++)
	{
		printf("rx:%x\n",com_rbuf[i]);
	}

	if(size!=9)
	{

	     acqdata_set_value(acq_data,"315",UNIT_MG_L,0,&arg_n);
	     acqdata_set_value(acq_data,"301",UNIT_0C,0,&arg_n);
	     acq_data->acq_status = ACQ_ERR;

             return arg_n;//no polcode
	}
	
	temp = com_rbuf[3];
	temp = ((temp<<8)&0xff00) | com_rbuf[4];
	ftemp = temp;
	ftemp/=10;
	
	o2_val = com_rbuf[5];
	o2_val = ((o2_val<<8)&0xff00) | com_rbuf[6];
	fo2_val = o2_val;
	fo2_val/=100;
	
    //time(&time_long);
    //gmtime_r(&time_long,&tm_struct);
    //memcpy(&tm_struct,localtime(&time_long),sizeof(struct tm));
    read_system_time(time_array);
	
    month = time_array[4];//tm_struct.tm_mon+1;
    day =time_array[3];// tm_struct.tm_mday;
    hours =time_array[2];// tm_struct.tm_hour;
    minutes = time_array[1];//tm_struct.tm_min;
    
    pf=acq_data->com_f;  
    pf[arg_n]=fo2_val;//rand()%15+5;
    strcpy(acq_data->polcode[arg_n],"315"); 
    acq_data->unit[arg_n]=UNIT_MG_L;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=ftemp;
    strcpy(acq_data->polcode[arg_n],"301"); 
    acq_data->unit[arg_n]=UNIT_0C;
    status=0;
    arg_n++;

    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    return arg_n;
	
}

int protocol_WATER_TEST(struct acquisition_data *acq_data)
{
    int status=0;
    float *pf;
    char com_rbuf[2048]; 
    char modpack[8];
    int size=0;
    union uf  f;
    int arg_n=0;
    float month=0,day=0,hours=0,minutes=0;
    struct tm tm_struct;
    time_t time_long;
    int time_array[7]={0};
    int i=0;
    int temp,o2_val;
    float ftemp,fo2_val;

   srand((unsigned)time(0));
#if 0   
   acqdata_set_value(acq_data,"302",UNIT_PH,(float)(rand()%10)/10+6.7,&arg_n);
   acqdata_set_value(acq_data,"301",UNIT_0C,(float)(rand()%10)/10+20.6,&arg_n);
   acqdata_set_value(acq_data,"315",UNIT_MG_L,(float)(rand()%10)/10+6.8,&arg_n);
   acqdata_set_value(acq_data,"463",UNIT_US_CM,(float)(rand()%10)+2674,&arg_n);
   acqdata_set_value(acq_data,"316",UNIT_M_V,(float)(rand()%10)+50,&arg_n);
   acqdata_set_value(acq_data,"465",UNIT_NTU,(float)(rand()%10)+812,&arg_n);
   acqdata_set_value(acq_data,"466",UNIT_MG_L,(float)(rand()%10)/10+3.18,&arg_n);
   acqdata_set_value(acq_data,"467",UNIT_CELLS_ML,(float)(rand()%10+500),&arg_n);
   acqdata_set_value(acq_data,"468",UNIT_MG_L,(float)(rand()%10)/10+7.18,&arg_n);
   acqdata_set_value(acq_data,"B01",UNIT_M3_H,(float)(rand()%10)/10+26.8,&arg_n);
#endif
	
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;
}


int protocol_AGRICLTURE_TEST(struct acquisition_data *acq_data)
{
    int status=0;
    float *pf;
    char com_rbuf[2048]; 
    char modpack[8];
    int size=0;
    union uf  f;
    int arg_n=0;
    float month=0,day=0,hours=0,minutes=0;
    struct tm tm_struct;
    time_t time_long;
    int time_array[7]={0};
    int i=0;
    int temp,o2_val;
    float ftemp,fo2_val;

   acqdata_set_value(acq_data,"at1",UNIT_0C,(float)(32.1-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"at2",UNIT_0C,(float)(33.5-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"ah1",UNIT_PRH,(float)(73.6-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"ah2",UNIT_PRH,(float)(70.7-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"co21",UNIT_PPM,(float)(266-rand()%3),&arg_n);

    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

#if 1
int protocol_DODI_TEST(struct acquisition_data *acq_data)
{
    int status=0;
    float *pf;
    char com_rbuf[2048]; 
    char modpack[8];
    int size=0;
    union uf  f;
    int arg_n=0;
    float month=0,day=0,hours=0,minutes=0;
    struct tm tm_struct;
    time_t time_long;
    int time_array[7]={0};

    //time(&time_long);
    //gmtime_r(&time_long,&tm_struct);
    //memcpy(&tm_struct,localtime(&time_long),sizeof(struct tm));
    read_system_time(time_array);
	
    month = time_array[4];//tm_struct.tm_mon+1;
    day =time_array[3];// tm_struct.tm_mday;
    hours =time_array[2];// tm_struct.tm_hour;
    minutes = time_array[1];//tm_struct.tm_min;

    //SYSLOG_DBG("month:%f,day:%f,hours:%f,minutes:%f\n",month,day,hours,minutes);
    
    pf=acq_data->com_f;  
    pf[arg_n]=get_one_do_level(0);
    strcpy(acq_data->polcode[arg_n],"DO1"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=get_one_do_level(1);
    strcpy(acq_data->polcode[arg_n],"DO2"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=get_one_do_level(2);
    strcpy(acq_data->polcode[arg_n],"DO3"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=get_one_do_level(3);
    strcpy(acq_data->polcode[arg_n],"DO4"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=get_one_do_level(4);
    strcpy(acq_data->polcode[arg_n],"DO5"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;
    
        pf=acq_data->com_f;  
    pf[arg_n]=get_one_do_level(5);
    strcpy(acq_data->polcode[arg_n],"DO6"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;
    
        pf=acq_data->com_f;  
    pf[arg_n]=(get_one_di_level(0)>0)?0:1;
    strcpy(acq_data->polcode[arg_n],"DI1"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;
    
        pf=acq_data->com_f;  
    pf[arg_n]=(get_one_di_level(1)>0)?0:1;
    strcpy(acq_data->polcode[arg_n],"DI2"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=(get_one_di_level(2)>0)?0:1;
    strcpy(acq_data->polcode[arg_n],"DI3"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=(get_one_di_level(3)>0)?0:1;
    strcpy(acq_data->polcode[arg_n],"DI4"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

        pf=acq_data->com_f;  
    pf[arg_n]=(get_one_di_level(4)>0)?0:1;
    strcpy(acq_data->polcode[arg_n],"DI5"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=(get_one_di_level(5)>0)?0:1;
    strcpy(acq_data->polcode[arg_n],"DI6"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;

    pf=acq_data->com_f;  
    pf[arg_n]=(get_one_di_level(6)>0)?0:1;
    strcpy(acq_data->polcode[arg_n],"DI7"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;
    
        pf=acq_data->com_f;  
    pf[arg_n]=(get_one_di_level(7)>0)?0:1;
    strcpy(acq_data->polcode[arg_n],"DI8"); 
    acq_data->unit[arg_n]=UNIT_NONE;
    status=0;
    arg_n++;


    
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;
}
#endif

int protocol_DODI_ALL_TEST(struct acquisition_data *acq_data)
{
    int status=0;
    int arg_n=0;
	unsigned char di_val=0,do_val=0;
    int i=0;

	for(i=5;i>=0;i--)
	{
		do_val<<=1;
		if(get_one_do_level(i))do_val|=1;
	}
	for(i=7;i>=0;i--)
	{
		di_val<<=1;
		if(get_one_di_level(i)==0)di_val|=1;
	}
   acqdata_set_value(acq_data,"NDIM",UNIT_NONE,(float)di_val,&arg_n);
   acqdata_set_value(acq_data,"NDOM",UNIT_NONE,(float)do_val,&arg_n);

    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;
}

int protocol_BATTERY_VOLTAGE(struct acquisition_data *acq_data)
{ 
    int status=0;
    int arg_n=0;
    float vol=0;

    k37_k37a_battery_quantity(&vol);
	
    acqdata_set_value(acq_data,"330",UNIT_V,vol,&arg_n);
	
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;
}



static  float wind2valuef(float val)
{
    if((val)<4.5 && (val)>=3.5) return 4;
    else if((val)<5.5 && (val)>=4.5) return 5;
    else if((val)<6.5 && (val)>=5.5) return 6;
    else if((val)<7.5 && (val)>=6.5) return 7;
    else if((val)<8.5 && (val)>=7.5) return 8;
    else if((val)<9.5 && (val)>=8.5) return 9;
    else if((val)<10.5 && (val)>=9.5) return 10;
    else if((val)<11.5 && (val)>=10.5) return 11;
    else if((val)<12.5 && (val)>=11.5) return 12;
    else if((val)<13.5 && (val)>=12.5) return 13;
     else if((val)<14.5 && (val)>=13.5) return 14;
     else if((val)<15.5 && (val)>=14.5) return 15;
     else if((val)<16.5 && (val)>=15.5) return 16;
    else if((val)<17.5 && (val)>=16.5) return 17;
    else if((val)<18.5 && (val)>=17.5) return 18;
    else if((val)<19.5 && (val)>=18.5) return 19;
    return 0;
}

#ifndef PROTOCOL_HJT2017
#else


int protocol_ai(struct acquisition_data *acq_data)
{
	char buf[20]={0};
	float *pf;
	int arg_n=0;
	int size=0;
	int status=-1;
	int i=0;

	unsigned long curr_time ;	
	static unsigned long total_flux_time=0;
	static float total_flux_M3=0.0;

	static float total_sum=0;
	//static char total_polcode[10]={0};
	static char total_polcode[CODE_LEN]={0}; //20210308

	if(total_flux_time==0) 
	{
		total_flux_time = get_current_time()-3;
		total_flux_M3=0.0;
	}

	pf=acq_data->com_f;  
	size=read_device(DEV_NAME(acq_data),buf,sizeof(buf)-1);
	if(size>0)
	{
		pf[arg_n] = atof(buf);
		status=0;

		if(!strcmp(DEV_NAME(acq_data),"ai0"))
		{
			total_sum=pf[arg_n];
			strcpy(total_polcode,acq_data->polcode[0]);
		}
		else if(!strcmp(DEV_NAME(acq_data),"ai1")&& !strcmp(acq_data->polcode[0],total_polcode))
		{
			pf[arg_n]+=total_sum;
			total_sum=0;
		}

	}
	else
	{
		pf[arg_n] = 0.0;
		status=-1;
	}
	arg_n++;



	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;


	if(!strcmp("a00000",acq_data->polcode[0]) || !strcmp("a00000z",acq_data->polcode[0]))
		    acq_data->unit[0]=UNIT_M3_S;//UNIT_M3_H;
	else  if(!strcmp("a01017",acq_data->polcode[0]) ||
			!strcmp("a01013",acq_data->polcode[0]) ||
			!strcmp("a01006",acq_data->polcode[0]))
		acq_data->unit[0]=UNIT_KPA;//UNIT_PA;    
	else  if(!strcmp("w00000",acq_data->polcode[0]))
	{
		acq_data->unit[0]=UNIT_L_S;
	}
	else 
		acq_data->unit[0]=polcode2unit(acq_data->polcode[0]);

	//printf("polcode:%s,unit:%d\n",acq_data->polcode[0],acq_data->unit[0]);


	//add zs polcode
	for(i=1;(strlen(acq_data->polcode[i])>0) && (i<20);i++)
	{        
		if(!strcmp("a00000",acq_data->polcode[i]) || !strcmp("a00000z",acq_data->polcode[i]))
			acq_data->unit[i]=UNIT_M3_S;//UNIT_M3_H;

	else  if(/*!strcmp("a01017",acq_data->polcode[0]) ||*/
			//!strcmp("a01013",acq_data->polcode[0]) ||
			!strcmp("a01006",acq_data->polcode[0]))
		acq_data->unit[0]=UNIT_KPA;//UNIT_PA;    
	else if(!strcmp("a01017",acq_data->polcode[0]) || // for XuZhou MingXuan 20210426 in formula_a01017ToS02() Pa 
		!strcmp("a01013",acq_data->polcode[0]) // for XuZhou MingXuan 20210426 in formula_a01017ToS02() Pa 
	     ) //added by miles zhang 20210426
		acq_data->unit[0]=UNIT_PA;//UNIT_PA;

		else  if(!strcmp("w00000",acq_data->polcode[i]))
			acq_data->unit[i]=UNIT_L_S;        

		else if(!strcmp("w00001",acq_data->polcode[i]))
		{
			acq_data->unit[i]=UNIT_M3;//UNIT_L;
		}        
		else 
		{
			acq_data->unit[i]=polcode2unit(acq_data->polcode[i]);        
		}

		if(!strcmp("a01011",acq_data->polcode[0]) || !strcmp("a01011",acq_data->polcode[i]))
			acq_data->com_f[i]=-1;//zs polcode zs  value
		else 
			acq_data->com_f[i]=acq_data->com_f[0];

		if(!strcmp("w00001",acq_data->polcode[i]))
		{
			curr_time = get_current_time();
			if(curr_time > total_flux_time)
			{
				total_flux_M3 +=acq_data->com_f[0]*(float)(curr_time-total_flux_time);// L unit
				acq_data->com_f[i] = total_flux_M3/1000;// L==> m3
				//acq_data->total = total_flux_M3/1000; //	
				acq_data->total = 0; //must be 0	
			}
			total_flux_time=curr_time;
		}

		if(!strncmp(acq_data->polcode[i],"we0001",4))
		{
			acq_data->com_f[i]=acq_data->com_f[0]/100*2.4;
		}

		if(!strcmp(acq_data->polcode[i],"a34013a") ||
			!strcmp(acq_data->polcode[i],"a21026a") ||
			!strcmp(acq_data->polcode[i],"a21002a") ||
			!strcmp(acq_data->polcode[i],"a21005a") ||
			!strcmp(acq_data->polcode[i],"a21028a") ||
			!strcmp(acq_data->polcode[i],"a21018a") ||
			!strcmp(acq_data->polcode[i],"a21017a") ||
			!strcmp(acq_data->polcode[i],"a21024a") ||
			!strcmp(acq_data->polcode[i],"a24088a") ||
			!strcmp(acq_data->polcode[i],"a19001a")			
			)
		{
			acq_data->com_f[i]=acq_data->com_f[0];
			acq_data->unit[i]=acq_data->unit[0];
		}
		
		arg_n++;
	}


#if 0 //added by miles zhang 20210511 for gongkuang 

    acq_data->acq_status = ACQ_OK;

	if(status == 0)
	{
	//{1,2,3,4,5,6}
	//{C,K,T,M,D,P}
	    int curr_vol=0;
	    int ret=0;
	    acq_data->dev_stat='N';
		ret=read_device(DEV_DI0,&curr_vol,sizeof(int));//C
		if(curr_vol==0) acq_data->dev_stat='C';	//C
		ret=read_device(DEV_DI1,&curr_vol,sizeof(int));//K
		if(curr_vol==0) acq_data->dev_stat='K';	//K
		ret=read_device(DEV_DI2,&curr_vol,sizeof(int));//T
		if(curr_vol==0) acq_data->dev_stat='T';	//T
		ret=read_device(DEV_DI3,&curr_vol,sizeof(int));//M
		if(curr_vol==0) acq_data->dev_stat='M';	//M
		ret=read_device(DEV_DI4,&curr_vol,sizeof(int));//D
		if(curr_vol==0) acq_data->dev_stat='D';	//D
		ret=read_device(DEV_DI5,&curr_vol,sizeof(int));//P
		if(curr_vol==0) acq_data->dev_stat='P'; //P
		ret=read_device(DEV_DI7,&curr_vol,sizeof(int));//P
		if(curr_vol==0) acq_data->dev_stat='v'; //P		
	}
	else
	{
		acq_data->dev_stat='V';
	}
	
#endif

	return arg_n;
}
#endif



static char *devCodeStr[]={"t1","t2","t3","mA","mB","mC","mD","mE","mF","mG"};

char *getDevCode(int code)
{
	if(code<0 || code>=ARRAY_SIZE(devCodeStr)) return "";

	return devCodeStr[code];
}


int protocol_electric_meter_TEST(struct acquisition_data *acq_data)
{
    int status=0;
    int arg_n=0;

#define ELMETER_POLCODE_NUM 10

	char polcode_prefix[10]={0};
	char polcode_suffix[10]={0};    
	char polcodes[ELMETER_POLCODE_NUM][100]={"mAIa01","mAIb01","mAIc01","mAUa01","mAUb01","mAUc01",
		                   "mAPw01","mAQw01","mATe01","mAPW01"};
    char pcodes[ELMETER_POLCODE_NUM][20]={"Ia","Ib","Ic","Ua","Ub","Uc",
		                   "Pw","Qw","Te","PW"};
	char elmeter_addr[20]={0};

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   
   int i=0;
   int ret=0;

   if(!acq_data) return -1;

   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;

   sprintf(elmeter_addr,"%.6x%.6x",modbusarg_running->devaddr,modbusarg_running->devfun);
   strcpy(polcode_prefix,getDevCode(modbusarg_running->devstaddr));
   sprintf(polcode_suffix,"%.2x",modbusarg_running->regcnt);

   SYSLOG_DBG("electric_meter %s,%s,%s",elmeter_addr,polcode_prefix,polcode_suffix);

   for(i=0;i<ELMETER_POLCODE_NUM;i++)
   {
   	  sprintf(polcodes[i],"%s%s%s",polcode_prefix,pcodes[i],polcode_suffix);
   }

   
#if 0
   acqdata_set_value(acq_data,"mAIa01",UNIT_A,(float)(32.1-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"mAIb01",UNIT_A,(float)(33.5-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"mAIc01",UNIT_A,(float)(73.6-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"mAUa01",UNIT_V,(float)(32.1-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"mAUb01",UNIT_V,(float)(33.5-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"mAUc01",UNIT_V,(float)(73.6-rand()%3*0.1),&arg_n); 
   acqdata_set_value(acq_data,"mAPw01",UNIT_KW,(float)(70.7-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"mAQw01",UNIT_KWH,(float)(70.7-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"mATe01",UNIT_NONE,(float)(10-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,"mAPW01",UNIT_KVAR,(float)(10-rand()%3*0.1),&arg_n);
#else
   acqdata_set_value(acq_data,polcodes[0],UNIT_A,(float)(32.1-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,polcodes[1],UNIT_A,(float)(33.5-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,polcodes[2],UNIT_A,(float)(73.6-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,polcodes[3],UNIT_V,(float)(32.1-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,polcodes[4],UNIT_V,(float)(33.5-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,polcodes[5],UNIT_V,(float)(73.6-rand()%3*0.1),&arg_n); 
   acqdata_set_value(acq_data,polcodes[6],UNIT_KW,(float)(70.7-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,polcodes[7],UNIT_KWH,(float)(70.7-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,polcodes[8],UNIT_NONE,(float)(10-rand()%3*0.1),&arg_n);
   acqdata_set_value(acq_data,polcodes[9],UNIT_KVAR,(float)(10-rand()%3*0.1),&arg_n);
#endif

    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;

    return arg_n;
}



int debug_acq_protocol_print(int debug_print)
{
	DEBUG_PRINT_VALUE=debug_print;// 0 close, 1 open debub
}


struct rdtu_info * get_parent_rdtu_info(struct acquisition_data *acq_data)
{
    struct rdtu_info * rdtuinfo;
	struct acquisition_all *acqall;

	if(!acq_data) return NULL;

    acqall=ACQ_PARENT(acq_data);

	return &acqall->rdtuinfo;
}





PROTOCOL_T lookup_protocol_info(int instrument_code,int index)
{
        int  i=0;
        for(i=0;i<ARRAY_SIZE(instrinfo_protocols);i++)
        {
            if(instrinfo_protocols[i].instrument_code==instrument_code &&
				instrinfo_protocols[i].index==index)
                return instrinfo_protocols[i].protocol;
        }
        return NULL;
}


int instrument_protocols_sel(PROTOCOLS_SEL_T func,struct acquisition_data *acq_data,unsigned int devaddr,void *data,int runId,int funcId)
{
	if(!func || !acq_data || !data)
		return -1;
	
	if(runId==funcId)
		return func(acq_data,devaddr,data);
	
	return 0;
}

unsigned char cal_bcc(unsigned char *buf, int len)
{
	unsigned char xor;
	int i;
	
	if(!buf) return 0;

	for(i=1,xor=buf[0];i<len;i++)
		xor^=buf[i];

	return xor;
}

char * modbus_crc_check(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt)
{
	char *p;
	char check[4];
	unsigned int crc;
	int len;

	if(!data || data_len<(cnt*2+5))
		return NULL;

	check[0]=devaddr;
	check[1]=cmd;
	check[2]=(cnt*2)&0xFF;
	check[3]=0;

	p=memstr(data, data_len, check);

	if(p==NULL)
		return NULL;

	len=cnt*2+3;
	crc=CRC16_modbus(p, len);

	if((p[len]!=((crc>>0)&0xFF)) || (p[len+1]!=((crc>>8)&0xFF)))
		return NULL;

	return p;
}

char * modbus_crc_check_write(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int reg,unsigned int cnt)
{
	char *p;
	char check[8];
	unsigned int crc;
	int len;

	if(!data || data_len<(cnt*2+9))
		return NULL;

	check[0]=devaddr;
	check[1]=cmd;
	check[2]=(reg>>8)&0xFF;
	check[3]=reg&0xFF;
	check[4]=(cnt>>8)&0xFF;
	check[5]=cnt&0xFF;
	check[6]=(cnt*2)&0xFF;

	p=memstrhex(data, data_len, check,7);

	if(p==NULL)
		return NULL;

	len=cnt*2+7;
	crc=CRC16_modbus(p, len);

	if((p[len]!=((crc>>0)&0xFF)) || (p[len+1]!=((crc>>8)&0xFF)))
		return NULL;

	return p;
}

char * modbus_crc_check_coil(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt)
{
	char *p;
	char check[4];
	unsigned int crc;
	int len;
	int size=0;

	size=((cnt%8)==0)?(cnt/8):(cnt/8+1);

	if(!data || data_len<(size+5))
		return NULL;

	check[0]=devaddr;
	check[1]=cmd;
	check[2]=size&0xFF;
	check[3]=0;

	p=memstr(data, data_len, check);

	if(p==NULL)
		return NULL;

	len=size+3;
	crc=CRC16_modbus(p, len);

	if((p[len]!=((crc>>0)&0xFF)) || (p[len+1]!=((crc>>8)&0xFF)))
		return NULL;

	return p;
}

char * modbus_tcp_crc_check(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt)
{
	char *p;
	char check[10];
	unsigned int crc;
	int len;

	if(!data || data_len<(cnt*2+9))
		return NULL;

	len=cnt*2+3;

	check[0]=0x00;
	check[1]=0x01;
	check[2]=0x00;
	check[3]=0x00;
	check[4]=(len>>8)&0xFF;
	check[5]=len&0xFF;
	check[6]=devaddr;
	check[7]=cmd;
	check[8]=(cnt*2)&0xFF;
	check[9]=0;

	p=memstrhex(data, data_len, check,9);

	if(p==NULL)
		return NULL;

	return p;
}

char * modbus_tcp_crc_check_coil(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt)
{
	char *p;
	char check[10];
	unsigned int crc;
	int len;
	int size=0;

	size=((cnt%8)==0)?(cnt/8):(cnt/8+1);

	if(!data || data_len<(size+9))
		return NULL;

	len=size+3;

	check[0]=0x00;
	check[1]=0x01;
	check[2]=0x00;
	check[3]=0x00;
	check[4]=(len>>8)&0xFF;
	check[5]=len&0xFF;
	check[6]=devaddr;
	check[7]=cmd;
	check[8]=size&0xFF;
	check[9]=0;

	p=memstrhex(data, data_len, check,9);

	if(p==NULL)
		return NULL;

	return p;
}

int modbus_crc_check_ok(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt)
{
	char *p;
	char check[4];
	unsigned int crc;
	int len;

	if(!data || data_len<(cnt*2+5))
		return 1; // crc error

	p=data;

	if(p==NULL)
		return 1; // crc error

	len=cnt*2+3;
	crc=CRC16_modbus(p, len);

	if((p[len]!=((crc>>0)&0xFF)) || (p[len+1]!=((crc>>8)&0xFF)))
		return 1; // crc error

	return 0;// crc ok
}


int getDataPack(char *dev_name, char *tbuf, int tbuf_size, char *rbuf, int rbuf_max_size, char *text, int wait_time, int tbuf_hex_or_str, int rbuf_hex_or_str)
{
	int size;
	char str[200];
	int i;

	if(!dev_name || !rbuf || !text)
		return 0;

	if(tbuf!=NULL)
	{
		size=write_device(dev_name,tbuf,tbuf_size);
		if(tbuf_hex_or_str==0)
		{
			if(size<=200)
			{
				memset(str,0,sizeof(str));
				sprintf(str,"%s SEND:",text);
				LOG_WRITE_HEX(dev_name,0,str,tbuf,size);
				SYSLOG_DBG("%s device %s , size=%d\n",str,dev_name,size);
				SYSLOG_DBG_HEX(str,tbuf,size);
			}
			else
			{
				SYSLOG_DBG("%s SEND device %s , size=%d\n",text,dev_name,size);
				for(i=1;size>i*200;i++)
				{
					memset(str,0,sizeof(str));
					sprintf(str,"%s SEND%d:",text,i);
					LOG_WRITE_HEX(dev_name,0,str,&tbuf[(i-1)*200],200);
					SYSLOG_DBG_HEX(str,&tbuf[(i-1)*200],200);
				}
				memset(str,0,sizeof(str));
				sprintf(str,"%s SEND%d:",text,i);
				LOG_WRITE_HEX(dev_name,0,str,&tbuf[(i-1)*200],size-(i-1)*200);
				SYSLOG_DBG_HEX(str,&tbuf[(i-1)*200],size-(i-1)*200);
			}
		}
		else
		{
			if(size<=500)
			{
				memset(str,0,sizeof(str));
				sprintf(str,"%s SEND:",text);
				LOG_WRITE_STR(dev_name,0,str,tbuf,size);
				SYSLOG_DBG("%s device %s , size=%d\n",str,dev_name,size);
				SYSLOG_DBG("%s %s\n",str,tbuf);
			}
			else
			{
				SYSLOG_DBG("%s SEND device %s , size=%d\n",text,dev_name,size);
				for(i=1;size>i*500;i++)
				{
					memset(str,0,sizeof(str));
					sprintf(str,"%s SEND%d:",text,i);
					LOG_WRITE_STR(dev_name,0,str,&tbuf[(i-1)*500],500);
					SYSLOG_DBG("%s %s\n",str,&tbuf[(i-1)*500]);
				}
				memset(str,0,sizeof(str));
				sprintf(str,"%s SEND%d:",text,i);
				LOG_WRITE_STR(dev_name,0,str,&tbuf[(i-1)*500],size-(i-1)*500);
				SYSLOG_DBG("%s %s\n",str,&tbuf[(i-1)*500]);
			}
		}
	}
	
	sleep(wait_time);
	memset(rbuf,0,rbuf_max_size);
	size=read_device(dev_name,rbuf,rbuf_max_size-1);
	if(rbuf_hex_or_str==0)
	{
		if(size<=200)
		{
			memset(str,0,sizeof(str));
			sprintf(str,"%s RECV:",text);
			LOG_WRITE_HEX(dev_name,1,str,rbuf,size);
			SYSLOG_DBG("%s device %s , size=%d\n",str,dev_name,size);
			SYSLOG_DBG_HEX(str,rbuf,size);
		}
		else
		{
			SYSLOG_DBG("%s RECV device %s , size=%d\n",text,dev_name,size);
			for(i=1;size>i*200;i++)
			{
				memset(str,0,sizeof(str));
				sprintf(str,"%s RECV%d:",text,i);
				LOG_WRITE_HEX(dev_name,1,str,&rbuf[(i-1)*200],200);
				SYSLOG_DBG_HEX(str,&rbuf[(i-1)*200],200);
			}
			memset(str,0,sizeof(str));
			sprintf(str,"%s RECV%d:",text,i);
			LOG_WRITE_HEX(dev_name,1,str,&rbuf[(i-1)*200],size-(i-1)*200);
			SYSLOG_DBG_HEX(str,&rbuf[(i-1)*200],size-(i-1)*200);
		}
	}
	else
	{
		if(size<=500)
		{
			memset(str,0,sizeof(str));
			sprintf(str,"%s RECV:",text);
			LOG_WRITE_STR(dev_name,1,str,rbuf,size);
			SYSLOG_DBG("%s device %s , size=%d\n",str,dev_name,size);
			SYSLOG_DBG("%s %s\n",str,rbuf);
		}
		else
		{
			SYSLOG_DBG("%s RECV device %s , size=%d\n",text,dev_name,size);
			for(i=1;size>i*500;i++)
			{
				memset(str,0,sizeof(str));
				sprintf(str,"%s RECV%d:",text,i);
				LOG_WRITE_STR(dev_name,1,str,&rbuf[(i-1)*500],500);
				SYSLOG_DBG("%s %s\n",str,&rbuf[(i-1)*500]);
			}
			memset(str,0,sizeof(str));
			sprintf(str,"%s RECV%d:",text,i);
			LOG_WRITE_STR(dev_name,1,str,&rbuf[(i-1)*500],size-(i-1)*500);
			SYSLOG_DBG("%s %s\n",str,&rbuf[(i-1)*500]);
		}
	}

	return size;
}

int getHexDataPack(char *dev_name, char *tbuf, int tbuf_size, char *rbuf, int rbuf_max_size, char *text, int wait_time)
{
	return getDataPack(dev_name, tbuf, tbuf_size, rbuf, rbuf_max_size, text, wait_time, 0, 0);
}

int getStrDataPack(char *dev_name, char *tbuf, int tbuf_size, char *rbuf, int rbuf_max_size, char *text, int wait_time)
{
	return getDataPack(dev_name, tbuf, tbuf_size, rbuf, rbuf_max_size, text, wait_time, 1, 1);
}


int isPolcodeEnable(struct modbus_arg *modbusarg, char *polcode)
{
	struct modbus_polcode_arg *mpolcodearg;
	int i=0;

	if(!modbusarg || !polcode) return 0;

	for(i=0;i<modbusarg->array_count ;i++)
	{
		mpolcodearg=&modbusarg->polcode_arg[i];
		if(!strcmp(mpolcodearg->polcode,polcode))
		{
			if(mpolcodearg->enableFlag==1)
				return 1; //enable
		}

	}

	return 0; //disable 
}


int acqdataIsPolcodeEnable(struct acquisition_data *acq_data, char *polcode)
{//added by miles zhang 20211116
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return 0;
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	return isPolcodeEnable(modbusarg, polcode);
}

char* memstrhex(char* buffer, int buff_len, char* subhex, int hex_len)
{
	int sublen,bufflen;
	int i=0;

	if(buffer == NULL || buff_len <= 0 || subhex == NULL || hex_len <= 0) {
		return NULL;
	}

	sublen = hex_len;
	bufflen = buff_len-sublen + 1;

	for (i = 0; i < bufflen; i++) 
	{
		if (buffer[i] == subhex[0] && !memcmp(&buffer[i], subhex, sublen)) 
			return &buffer[i];
	}

	return NULL;
}

int modbus_pack_tcp(char *modpack,char addr, char cmd, int reg, int cnt)
{
	modpack[0] = 0x00;
	modpack[1] = 0x01;
	modpack[2] = 0x00;
	modpack[3] = 0x00;
	modpack[4] = 0x00;
	modpack[5] = 0x06;
	modpack[6] = addr;
	modpack[7] = cmd;
	modpack[8] = (char)(reg >> 8)&0xff;
	modpack[9] = (char)(reg & 0xff);
	modpack[10] = (char)(cnt >> 8)&0xff;
	modpack[11] = (char)(cnt & 0xff);
	return 12;
}

