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
#include "rtuacq_protocol_rdtu_t36z.h"

int protocol_rdtu_t36z(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int ret=0;
	unsigned short value;
	float valuef;

	if(!acq_data) return -1;

		if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

int protocol_rdtu_t36z_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int ret=0;
	unsigned short value;
	float valuef;

   struct rdtu_info *rdtuinfo;

   if(!acq_data) return -1;

   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

   rdtuinfo=get_parent_rdtu_info(acq_data);

	read_system_time(acq_data->acq_tm);

#if 0
    acqdata_set_value_flag(acq_data,"RDTU",UNIT_NONE,0,INFOR_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"i22001",UNIT_NONE,rdtuinfo->status,INFOR_STATUS,&arg_n);
	valuef=0;
	if(rdtuinfo->COD_stat!=0)valuef=1;
	if(rdtuinfo->ANDAN_stat!=0)valuef=2;
	if(rdtuinfo->ZONGLIN_stat!=0)valuef=3;
	if(rdtuinfo->ZONGDAN_stat!=0)valuef=4;	
	if(rdtuinfo->SO2_stat!=0)valuef=5;
	if(rdtuinfo->NOx_stat!=0)valuef=6;
	if(rdtuinfo->O2_stat!=0)valuef=7;
	if(rdtuinfo->FENCHENG_stat!=0)valuef=8;	
	acqdata_set_value_flag(acq_data,"i22002",UNIT_NONE,valuef,INFOR_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"i22003",UNIT_NONE,rdtuinfo->alarm,INFOR_STATUS,&arg_n);
	
	//acqdata_set_value_flag(acq_data,"i22002",UNIT_NONE,rdtuinfo->COD_stat,INFOR_STATUS,&arg_n);
	//acqdata_set_value_flag(acq_data,"w01018-i22004",UNIT_NONE,rdtuinfo->COD_stat,INFOR_STATUS,&arg_n);
	//acqdata_set_value_flag(acq_data,"w21003-i22004",UNIT_NONE,rdtuinfo->ANDAN_stat,INFOR_STATUS,&arg_n);
    acqdata_set_value_flag(acq_data,"a21026-i22004",UNIT_NONE,rdtuinfo->SO2_stat,INFOR_STATUS,&arg_n);
	acqdata_set_value_flag(acq_data,"a21002-i22004",UNIT_NONE,rdtuinfo->NOx_stat,INFOR_STATUS,&arg_n);
	//acqdata_set_value_flag(acq_data,"i22005",UNIT_NONE,rdtuinfo->alarm,INFOR_STATUS,&arg_n);
#else
    acqdata_set_value_flag(acq_data,"RDTU",UNIT_NONE,0,INFOR_ARGUMENTS,&arg_n);
	acqdata_set_value_flag(acq_data,"i22001",UNIT_NONE,rdtuinfo->status,INFOR_ARGUMENTS,&arg_n);
	
	valuef=0;	
	if(rdtuinfo->COD_stat!=0)valuef=1;
	if(rdtuinfo->ANDAN_stat!=0)valuef=2;
	if(rdtuinfo->ZONGLIN_stat!=0)valuef=3;
	if(rdtuinfo->ZONGDAN_stat!=0)valuef=4;	
	if(rdtuinfo->SO2_stat!=0)valuef=5;
	if(rdtuinfo->NOx_stat!=0)valuef=6;
	if(rdtuinfo->O2_stat!=0)valuef=7;
	if(rdtuinfo->FENCHENG_stat!=0)valuef=8;	
	//acqdata_set_value_flag(acq_data,"i22002",UNIT_NONE,valuef,INFOR_ARGUMENTS,&arg_n);
/*	
	acqdata_set_value_flag(acq_data,"i22003",UNIT_NONE,rdtuinfo->alarm,INFOR_ARGUMENTS,&arg_n);
*/

	//acqdata_set_value_flag(acq_data,"i22002",UNIT_NONE,rdtuinfo->COD_stat,INFOR_STATUS,&arg_n);

	acqdata_set_value_flag(acq_data,"i22005",UNIT_NONE,rdtuinfo->alarm,INFOR_ARGUMENTS,&arg_n);

    if(rdtuinfo->poll_syscode!=32)
    {
             if(rdtuinfo->SO2_stat>=0)
	    acqdata_set_value_flag(acq_data,"a21026-i22004",UNIT_NONE,rdtuinfo->SO2_stat,INFOR_ARGUMENTS,&arg_n);
	    if(rdtuinfo->NOx_stat>=0)
		acqdata_set_value_flag(acq_data,"a21002-i22004",UNIT_NONE,rdtuinfo->NOx_stat,INFOR_ARGUMENTS,&arg_n);
	    if(rdtuinfo->FENCHENG_stat>=0)
		acqdata_set_value_flag(acq_data,"a34013-i22004",UNIT_NONE,rdtuinfo->FENCHENG_stat,INFOR_ARGUMENTS,&arg_n);
    }
	else
	{
	        if(rdtuinfo->COD_stat>=0)
		acqdata_set_value_flag(acq_data,"w01018-i22004",UNIT_NONE,rdtuinfo->COD_stat,INFOR_ARGUMENTS,&arg_n);
		if(rdtuinfo->ANDAN_stat>=0)
		acqdata_set_value_flag(acq_data,"w21003-i22004",UNIT_NONE,rdtuinfo->ANDAN_stat,INFOR_ARGUMENTS,&arg_n);
		if(rdtuinfo->ZONGLIN_stat>=0)
		acqdata_set_value_flag(acq_data,"w21011-i22004",UNIT_NONE,rdtuinfo->ZONGLIN_stat,INFOR_ARGUMENTS,&arg_n);
		if(rdtuinfo->ZONGDAN_stat>=0)
		acqdata_set_value_flag(acq_data,"w21001-i22004",UNIT_NONE,rdtuinfo->ZONGDAN_stat,INFOR_ARGUMENTS,&arg_n);	
	}

    if(rdtuinfo->poll_syscode!=32)
    {
		acqdata_set_value_flag(acq_data,"a01016",UNIT_M2,rdtuinfo->gas_acreage,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i23002",UNIT_PECENT,rdtuinfo->O2_base,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i23003",UNIT_NONE,rdtuinfo->vfc,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i23004",UNIT_NONE,rdtuinfo->PTC,INFOR_ARGUMENTS,&arg_n);
		if(rdtuinfo->gas_flux_range>0) //added by miles zhang 20211126
			acqdata_set_value_flag(acq_data,"i23005",UNIT_PECENT,rdtuinfo->gas_humidity_range,INFOR_ARGUMENTS,&arg_n);
		if(rdtuinfo->gas_temp_max!=0 && rdtuinfo->gas_temp_min!=rdtuinfo->gas_temp_max)
		{//added by miles zhang 20211126
			acqdata_set_value_flag(acq_data,"i23006",UNIT_0C,rdtuinfo->gas_temp_max,INFOR_ARGUMENTS,&arg_n);
			acqdata_set_value_flag(acq_data,"i23007",UNIT_0C,rdtuinfo->gas_temp_min,INFOR_ARGUMENTS,&arg_n);
		}
		if(rdtuinfo->gas_pressure_max!=rdtuinfo->gas_pressure_min)
		{//added by miles zhang 20211126
			acqdata_set_value_flag(acq_data,"i23008",UNIT_KPA,rdtuinfo->gas_pressure_max,INFOR_ARGUMENTS,&arg_n);
			acqdata_set_value_flag(acq_data,"i23009",UNIT_KPA,rdtuinfo->gas_pressure_min,INFOR_ARGUMENTS,&arg_n);
		}
		if(rdtuinfo->gas_flux_range>0)//added by miles zhang 20211126
			acqdata_set_value_flag(acq_data,"i23010",UNIT_M_S,rdtuinfo->gas_flux_range,INFOR_ARGUMENTS,&arg_n);
		
		acqdata_set_value_flag(acq_data,"i23011",UNIT_PA,rdtuinfo->atm_press*1000,INFOR_ARGUMENTS,&arg_n);
		acqdata_set_value_flag(acq_data,"i23012",UNIT_NONE,rdtuinfo->standard_alpha,INFOR_ARGUMENTS,&arg_n);
    }
	
#endif

	status =0;
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

