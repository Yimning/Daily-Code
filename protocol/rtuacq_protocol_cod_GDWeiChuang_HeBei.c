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

static struct hebei_dynamic_info_water GDWeiChuang_COD_info;

int protocol_COD_GDWeiChuang_HeBei(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 1
#define INFOPOL_NUM 22

	int ret=0;
	struct hebei_dynamic_info_water *info_data;

	char polcodestr[POLCODE_NUM][20]={"w01018"};
	UNIT_DECLARATION unitstr[POLCODE_NUM]={UNIT_MG_L};

	void *infoVal[INFOPOL_NUM];
	char infopol[INFOPOL_NUM][20]={
		"i12101","i12102","i12103","i13101","i13007",
		"i13104","i13105","i13106","i13107","i13108",
		"i13110","i13112","i13003","i13116","i13008",
		"i13119","i13120","i13005","i13004","i13128",
		"i13129","i13130"
	};

	SYSLOG_DBG("protocol_COD_GDWeiChuang_HeBei:size=%d,%s\n",POLCODE_NUM,polcodestr[0]);
	info_data=&GDWeiChuang_COD_info;

	infoVal[0]=VOID_ADDR(info_data->i12101);
	infoVal[1]=VOID_ADDR(info_data->i12102);
	infoVal[2]=VOID_ADDR(info_data->i12103);
	infoVal[3]=VOID_ADDR(info_data->i13101);
	infoVal[4]=VOID_ADDR(info_data->i13007);
	infoVal[5]=VOID_ADDR(info_data->i13104);
	infoVal[6]=VOID_ADDR(info_data->i13105);
	infoVal[7]=VOID_ADDR(info_data->i13106);
	infoVal[8]=VOID_ADDR(info_data->i13107);
	infoVal[9]=VOID_ADDR(info_data->i13108);
	infoVal[10]=VOID_ADDR(info_data->i13110);
	infoVal[11]=VOID_ADDR(info_data->i13112);
	infoVal[12]=VOID_ADDR(info_data->i13003);
	infoVal[13]=VOID_ADDR(info_data->i13116);
	infoVal[14]=VOID_ADDR(info_data->i13008);
	infoVal[15]=VOID_ADDR(info_data->i13119);
	infoVal[16]=VOID_ADDR(info_data->i13120);
	infoVal[17]=VOID_ADDR(info_data->i13005);
	infoVal[18]=VOID_ADDR(info_data->i13004);
	infoVal[19]=VOID_ADDR(info_data->i13128);
	infoVal[20]=VOID_ADDR(info_data->i13129);
	infoVal[21]=VOID_ADDR(info_data->i13130);

	info_data->status = 0;
	ret=protocol_HeBei_Dynamic(acq_data,POLCODE_NUM,polcodestr,unitstr,INFOPOL_NUM,infopol,infoVal,&(info_data->status));

	NEED_ERROR_CACHE(acq_data,120);

	return ret;
}

int protocol_COD_GDWeiChuang_HeBei_info(struct acquisition_data *acq_data)
{
	int arg_n=0;
	struct hebei_dynamic_info_water *info_data;
	static int num=0;

	SYSLOG_DBG("protocol_COD_GDWeiChuang_HeBei_info\n");

	info_data=&GDWeiChuang_COD_info;

	acqdata_set_value_flag(acq_data,"w01018",UNIT_MG_L,0,INFOR_ARGUMENTS,&arg_n);
	if(num>15)
	{
		acqdata_set_value_flag(acq_data,"i12101",UNIT_NONE,info_data->i12101,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12102",UNIT_NONE,info_data->i12102,INFOR_STATUS,&arg_n);
		acqdata_set_value_flag(acq_data,"i12103",UNIT_NONE,info_data->i12103,INFOR_STATUS,&arg_n);

		acqdata_set_value_flag(acq_data,"i13007",UNIT_NONE,info_data->i13007,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13104",UNIT_MG_L,info_data->i13104,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13105",UNIT_NONE,info_data->i13105,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13106",UNIT_PECENT,info_data->i13106,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13108",UNIT_MG_L,info_data->i13108,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13110",UNIT_NONE,info_data->i13110,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13112",UNIT_PECENT,info_data->i13112,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13003",UNIT_MINUTE,info_data->i13003,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13116",UNIT_MG_L,info_data->i13116,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13008",UNIT_NONE,info_data->i13008,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13119",UNIT_NONE,info_data->i13119,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13120",UNIT_NONE,info_data->i13120,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13005",UNIT_MINUTE,info_data->i13005,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13004",UNIT_0C,info_data->i13004,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13129",UNIT_PECENT,info_data->i13129,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13130",UNIT_MG_L,info_data->i13130,INFOR_ARGUMENTS,&arg_n);

		acqdata_set_value_flag(acq_data,"i13101",info_data->i13101,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13107",info_data->i13107,0,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i13128",info_data->i13128,0,INFOR_ARGUMENTS,&arg_n);
	}

	read_system_time(acq_data->acq_tm);

	num++;
	
	if(info_data->status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;	
}

