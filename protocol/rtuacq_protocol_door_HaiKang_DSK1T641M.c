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
#include "rtuacq_protocol_door_common.h"
#include "rtuacq_protocol_error_cache.h"


int protocol_door_HaiKang_DSK1T641M(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float valf=0;
	if(!acq_data) return -1;

	SYSLOG_DBG("protocol_door_HaiKang_DSK1T641M\n");

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;	
}

static struct door_open_info doorinfo_global;

int protocol_door_HaiKang_DSK1T641M_info(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	float valf=0;
	int ret=0;
	int tm[TIME_ARRAY]={0};
	unsigned int tmlong=0;
	struct door_open_info doorinfo;
	int door_status=0;

	static int last_tm[TIME_ARRAY]={0};

	static int testid=0;

	struct acquisition_ctrl *acq_ctrl;
	struct server_cfg *scfg;
		
	if(!acq_data) return -1;
    acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

    acq_ctrl=ACQ_CTRL(acq_data);
    scfg=&acq_ctrl->scfg;

	if(acq_ctrl->doorinfo.style>0)
	{//added by miles zhang 20210524
		memcpy(doorinfo_global.code,acq_ctrl->doorinfo.code,sizeof(doorinfo_global.code));
		doorinfo_global.style=acq_ctrl->doorinfo.style;

		SYSLOG_DBG("acq_ctrl->doorinfo:%s,%d\n",acq_ctrl->doorinfo.code,acq_ctrl->doorinfo.style);
	    memset(&acq_ctrl->doorinfo,0,sizeof(struct acq_door_open_info));
	}

   
	SYSLOG_DBG("protocol_door_HaiKang_DSK1T641M_info ipaddr:%s,port:%d,uname:%s,passwd:%s\n",
		scfg->ipaddr,scfg->port,scfg->MN,scfg->password);

	memset(&doorinfo,0,sizeof(struct door_open_info));

	ret=getAcsEvent(scfg->ipaddr,scfg->port,scfg->MN,scfg->password,&doorinfo);
	SYSLOG_DBG("protocol_door_HaiKang_DSK1T641M_info ret=%d\n",ret);
	SYSLOG_DBG("doorinfo:style:%d,code:%s,name:%s,time:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
		doorinfo.style,doorinfo.code,doorinfo.name,
		doorinfo.time[5],doorinfo.time[4],doorinfo.time[3],
		doorinfo.time[2],doorinfo.time[1],doorinfo.time[0]);
#if 1
	if(ret==0 &&
	   memcmp(last_tm,doorinfo.time,sizeof(int)*6) )
	{
	    memcpy(last_tm,doorinfo.time,TIME_LEN);

	    if(doorinfo.style==1) //  1 for face recognize 
	    {
		    doorinfo.door_stat=1; // door is open
			
			acqdata_set_value_flag(acq_data,"DOORCTRL",UNIT_NONE,0,INFOR_STATUS,&arg_n);

			ret=acqdata_set_value_flag(acq_data,"i3310B",UNIT_NONE,doorinfo.style,INFOR_STATUS,&arg_n);
			ret=acqdata_set_value_flag(acq_data,"i3310D",UNIT_NONE,0,INFOR_STATUS,&arg_n);
			strcpy(acq_data->cardid,doorinfo.code);
			
			tmarray2tmlong(doorinfo.time,&tmlong);
			ret=acqdata_set_value_flag(acq_data,"i3310E",tmlong,0,INFOR_STATUS,&arg_n);

			ret=acqdata_set_value_flag(acq_data,"i3310F",UNIT_NONE,1,INFOR_STATUS,&arg_n);

			read_system_time(acq_data->acq_tm); 

			memcpy(&doorinfo_global,&doorinfo,sizeof(struct door_open_info));
	    }
		else
		{
				ret=getDoorStatus(scfg->ipaddr,scfg->port,scfg->MN,scfg->password,&door_status);
				SYSLOG_DBG("getDoorStatus1 %d,ret=%d\n",door_status,ret);
		        if(ret==0 && 
					doorinfo_global.door_stat!=door_status &&
					strlen(doorinfo_global.code)>0)
				{//door is open by remote ops from platform
						door_status=1;
						
					    doorinfo_global.door_stat=door_status; // door is open
						
						acqdata_set_value_flag(acq_data,"DOORCTRL",UNIT_NONE,0,INFOR_STATUS,&arg_n);

						ret=acqdata_set_value_flag(acq_data,"i3310B",UNIT_NONE,doorinfo_global.style,INFOR_STATUS,&arg_n);
						ret=acqdata_set_value_flag(acq_data,"i3310D",UNIT_NONE,0,INFOR_STATUS,&arg_n);
						strcpy(acq_data->cardid,doorinfo_global.code);

						read_system_time(doorinfo_global.time); 
						tmarray2tmlong(doorinfo_global.time,&tmlong);
						ret=acqdata_set_value_flag(acq_data,"i3310E",tmlong,0,INFOR_STATUS,&arg_n);
			            
						ret=acqdata_set_value_flag(acq_data,"i3310F",UNIT_NONE,door_status,INFOR_STATUS,&arg_n);

						read_system_time(acq_data->acq_tm);  		
				}
				else status=1;
		}
	}
	else
	{//if ret=2, maybe passwd error make door monitor locked. we need reboot door monitor
	    if(ret==2) SYSLOG_DBG("protocol_door_HaiKang_DSK1T641M_info maybe passwd error make door monitor locked. we need reboot door monitor\n");

		//if(ret!=0)
		{
			ret=getDoorStatus(scfg->ipaddr,scfg->port,scfg->MN,scfg->password,&door_status);
			SYSLOG_DBG("getDoorStatus2 %d,ret=%d\n",door_status,ret);
	        if(ret==0 && doorinfo_global.door_stat!=door_status)
	        {
				door_status=(door_status!=1) ? 0 : 1;
				
			    doorinfo_global.door_stat=door_status; // door is open
				
				acqdata_set_value_flag(acq_data,"DOORCTRL",UNIT_NONE,0,INFOR_STATUS,&arg_n);

				ret=acqdata_set_value_flag(acq_data,"i3310B",UNIT_NONE,doorinfo_global.style,INFOR_STATUS,&arg_n);
				ret=acqdata_set_value_flag(acq_data,"i3310D",UNIT_NONE,0,INFOR_STATUS,&arg_n);
				strcpy(acq_data->cardid,doorinfo_global.code);

				read_system_time(doorinfo_global.time); 
				tmarray2tmlong(doorinfo_global.time,&tmlong);
				ret=acqdata_set_value_flag(acq_data,"i3310E",tmlong,0,INFOR_STATUS,&arg_n);
	            
				ret=acqdata_set_value_flag(acq_data,"i3310F",UNIT_NONE,door_status,INFOR_STATUS,&arg_n);

				read_system_time(acq_data->acq_tm);  
	        }
	        else status=1;
		}
		//else status=1;
	}
#else
        testid++;
		acqdata_set_value_flag(acq_data,"DOORCTRL",UNIT_NONE,testid,INFOR_STATUS,&arg_n);

		ret=acqdata_set_value_flag(acq_data,"i3310B",UNIT_NONE,testid,INFOR_STATUS,&arg_n);
		ret=acqdata_set_value_flag(acq_data,"i3310D",UNIT_NONE,testid,INFOR_STATUS,&arg_n);
		strcpy(acq_data->cardid,"123456789");
		read_system_time(tm);
		tmarray2tmlong(tm,&tmlong);
		ret=acqdata_set_value_flag(acq_data,"i3310E",tmlong,testid,INFOR_STATUS,&arg_n);

		//ret=acqdata_set_value_flag(acq_data,"i13101",tmlong,testid,INFOR_STATUS,&arg_n);
		//ret=acqdata_set_value_flag(acq_data,"i13107",tmlong,testid,INFOR_STATUS,&arg_n);
		
		ret=acqdata_set_value_flag(acq_data,"i3310F",UNIT_NONE,testid,INFOR_STATUS,&arg_n);
		
		ret=acqdata_set_value_flag(acq_data,"i3310K",UNIT_NONE,testid,INFOR_ARGUMENTS,&arg_n);
		ret=acqdata_set_value_flag(acq_data,"i3310A",UNIT_NONE,testid,INFOR_ARGUMENTS,&arg_n);

	    read_system_time(acq_data->acq_tm); 

#endif
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;	
}


int protocol_door_HaiKang_DSK1T641M_info_all()
{
	struct door_open_info doorinfo;
	int ret=0;

    memset(&doorinfo,0,sizeof(struct door_open_info));
	ret=getAcsEvent("192.168.0.217",80,"admin","zhang1501",&doorinfo);
	return ret;
}

