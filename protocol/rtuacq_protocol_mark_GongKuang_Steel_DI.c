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

/*


*/
int protocol_mark_GongKuang_Steel_DI(struct acquisition_data *acq_data)
{
	int arg_n=0;
	int status=0;
	char com_rbuf[2048]={0}; 
	char com_tbuf[100]={0}; 
	int size=0;
	int val;
	union uf f;
	int ret=0;
	unsigned int devaddr=0;
	unsigned int cmd;
	int i=0;

   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp;
   
   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;
	

#define STEEL_POLCODE_NUM   32
   char polcodestr[STEEL_POLCODE_NUM][20]={  "SCXBH",
                                                                                      "SJSJJGK","SJSCFH","SJZCFJSD","SJFMKBD","QTZCFJDL",
											"QTSCBLQXLL","QTBTPDYXXH","LTDY","LTFY","LTFYL",
											"LTDLGFJFL","LGLS", "LGCYL","LGYQGD","LGLL",
											"BHYRQLL","BHYYL","BHYYTWD","BHYYWWD","CCBDCCQQHYC",
											"CCDCCQECDL","CCDCCQDY","CCDCCQECDY","TLXSJPHZ","TLGJLL",
											"TLGLLL","TLJYXHGYXZT","TLSFYHFJDL","TXHYJYL","HXTYL",
											"GXSJ"
											};

   UNIT_DECLARATION unitstr[STEEL_POLCODE_NUM]={UNIT_NONE,
   	                                                                                               UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_A,
													UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_PECENT,
													UNIT_NONE,UNIT_NONE,UNIT_PECENT,UNIT_NONE,UNIT_NONE,
													UNIT_NONE,UNIT_KPA,UNIT_0C,UNIT_0C,UNIT_V,
													UNIT_A,UNIT_V,UNIT_V,UNIT_NONE,UNIT_M3_H,
													UNIT_KG,UNIT_NONE,UNIT_A,UNIT_NONE,UNIT_NONE,
													UNIT_NONE
													};
   char cemspol[STEEL_POLCODE_NUM][20]={  "SCXBH",
                                                                                      "SJSJJGK","SJSCFH","SJZCFJSD","SJFMKBD","QTZCFJDL",
											"QTSCBLQXLL","QTBTPDYXXH","LTDY","LTFY","LTFYL",
											"LTDLGFJFL","LGLS", "LGCYL","LGYQGD","LGLL",
											"BHYRQLL","BHYYL","BHYYTWD","BHYYWWD","CCBDCCQQHYC",
											"CCDCCQECDL","CCDCCQDY","CCDCCQECDY","TLXSJPHZ","TLGJLL",
											"TLGLLL","TLJYXHGYXZT","TLSFYHFJDL","TXHYJYL","HXTYL",
											"GXSJ"
											};
   

	if(!acq_data) return -1;


	   SYSLOG_DBG("protocol_mark_GongKuang_Steel_DI:size=%d,%s,%s\n",STEEL_POLCODE_NUM,polcodestr[0],cemspol[0]);

        for(i=0;i<STEEL_POLCODE_NUM;i++)
        {
			  mpolcodearg_temp=find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
			   if(mpolcodearg_temp && 
			   	mpolcodearg_temp->enableFlag==1)
			   {
				       int pos=0;
				       char devname[1024]={0};
				       int curr_vol=0;
					   
				      pos=mpolcodearg_temp->regaddr;
				      if(pos<=0 || pos>DEV_DI_NUM) 
				      {
				      		ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);
					  	continue;
				      }
	                              sprintf(devname,"%s%d",DEV_DI_NAME,(pos-1));
				      ret=read_device(devname,&curr_vol,sizeof(int));

	                             if(curr_vol==0)
					  ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],1,&arg_n);
	                             else	
					  ret=acqdata_set_value(acq_data,polcodestr[i],unitstr[i],0,&arg_n);

				    SYSLOG_DBG("protocol_mark_GongKuang_Steel_DI %d polcod:%s,curr_vol=%d\n",i,polcodestr[i],curr_vol);
			   }
        }

	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;

	return arg_n;
}

