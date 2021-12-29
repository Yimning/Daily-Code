#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"


int protocol_ZONGLIANGYI_ZhuoZheng(struct acquisition_data *acq_data)
{
   int status=0;
   char com_rbuf[2048]={0}; 
   char com_tbuf[8]={0};
   int size=0;
   int arg_n=0;
   int ret=0;
   float total_flux_M3=0,speed=0;
   union uf f;
   char *pstr;
   int i=0;
   
   if(!acq_data) return -1;
	  
#if 1	  
   //01 03 00 00 00 02 C4 0B 
/*   
	memset(com_tbuf,0,sizeof(com_tbuf));
   size=modbus_pack(com_tbuf,0x1,0x3,0x00,0x2);
   size=write_device(DEV_NAME(acq_data),com_tbuf,size);
*/
	sleep(2);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data), com_rbuf, sizeof(com_rbuf)-1);
	if(size<40)
	{
	    sleep(5);
		size=read_device(DEV_NAME(acq_data), &com_rbuf[size], sizeof(com_rbuf)-size)+size;
	}

	SYSLOG_DBG_HEX("ZONGLIANGYI:",com_rbuf,size);

	if(memstr(com_rbuf,size,"##")==NULL) goto ERR_DATA;
    if(memstr(com_rbuf,size,"&&")==NULL) goto ERR_DATA;


	pstr=memstr(com_rbuf,size,"##");

    if(pstr[4]!=0x32) goto ERR_DATA;
	if(pstr[5]!=0x52) goto ERR_DATA;
	if(pstr[6]!=0x7) goto ERR_DATA;

    i=8;
	f.ch[1] = com_rbuf[i+2];
    f.ch[0] = com_rbuf[i+3];
	f.ch[3] = com_rbuf[i];
	f.ch[2] = com_rbuf[i+1];
	acqdata_set_value(acq_data,"w21001",UNIT_MG_L,f.f,&arg_n);	

    i=12;
	f.ch[1] = com_rbuf[i+2];
    f.ch[0] = com_rbuf[i+3];
	f.ch[3] = com_rbuf[i];
	f.ch[2] = com_rbuf[i+1];
	acqdata_set_value(acq_data,"w21003",UNIT_MG_L,f.f,&arg_n);

    i=16;
	f.ch[1] = com_rbuf[i+2];
    f.ch[0] = com_rbuf[i+3];
	f.ch[3] = com_rbuf[i];
	f.ch[2] = com_rbuf[i+1];
	acqdata_set_value(acq_data,"w01018",UNIT_MG_L,f.f,&arg_n);


	i=20;
	f.ch[1] = com_rbuf[i+2];
    f.ch[0] = com_rbuf[i+3];
	f.ch[3] = com_rbuf[i];
	f.ch[2] = com_rbuf[i+1];
	acqdata_set_value(acq_data,"w21011",UNIT_MG_L,f.f,&arg_n);

	i=24;
	f.ch[1] = com_rbuf[i+2];
    f.ch[0] = com_rbuf[i+3];
	f.ch[3] = com_rbuf[i];
	f.ch[2] = com_rbuf[i+1];
	acqdata_set_value(acq_data,"w00000",UNIT_L_S,f.f,&arg_n);	
	
	i=28;
	f.ch[1] = com_rbuf[i+2];
    f.ch[0] = com_rbuf[i+3];
	f.ch[3] = com_rbuf[i];
	f.ch[2] = com_rbuf[i+1];
	acqdata_set_value(acq_data,"w00001",UNIT_M3,f.f,&arg_n);
	acq_data->total=f.f;

	i=32;
	f.ch[1] = com_rbuf[i+2];
    f.ch[0] = com_rbuf[i+3];
	f.ch[3] = com_rbuf[i];
	f.ch[2] = com_rbuf[i+1];
	acqdata_set_value(acq_data,"w01001",UNIT_PH,f.f,&arg_n);	

	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
#endif

ERR_DATA:

   acqdata_set_value(acq_data,"w21001",UNIT_MG_L,0,&arg_n);
   acqdata_set_value(acq_data,"w21003",UNIT_MG_L,0,&arg_n);
   acqdata_set_value(acq_data,"w01018",UNIT_MG_L,0,&arg_n);
   acqdata_set_value(acq_data,"w21011",UNIT_MG_L,0,&arg_n);
   acqdata_set_value(acq_data,"w00000",UNIT_L_S,0,&arg_n);
   acqdata_set_value(acq_data,"w00001",UNIT_M3,0,&arg_n);
   acqdata_set_value(acq_data,"w01001",UNIT_PH,0,&arg_n);
   
   acq_data->acq_status = ACQ_ERR;

	return arg_n;	
}
