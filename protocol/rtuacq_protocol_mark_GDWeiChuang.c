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


static int datatype2valf_buff(char *com_rbuf,MODBUS_DATA_TYPE datatype,float *valf)
{
    union uf f;
	
    switch(datatype)
    {
        case INT_AB:
		//case INT_BA:
			f.s=com_rbuf[0];
		    f.s<<=8;
			f.s+=com_rbuf[1];
			*valf=f.s;
		break;
		case INT_BA:
		//case INT_AB:
			//f.ch[0]=com_rbuf[1];
			//f.ch[1]=com_rbuf[0];
			f.s=com_rbuf[0];
		    f.s<<=8;
			f.s+=com_rbuf[1];
			*valf=f.s;
			*valf/=100;
		break;
        case LONG_ABCD:
		//case LONG_DCBA:
			//f.ch[0]=com_rbuf[0];
			//f.ch[1]=com_rbuf[1];
			//f.ch[2]=com_rbuf[2];
			//f.ch[3]=com_rbuf[3];
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];

			*valf=f.l;
		break;
		case LONG_CDAB:
		//case LONG_BADC:
			f.l=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];
		    f.l<<=8;
			f.l+=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];	
			*valf=f.l;
		break;
        case LONG_BADC:
		//case LONG_CDAB:
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];
			*valf=f.l/100;
		break;
		case LONG_DCBA:
		//case LONG_ABCD:
			f.l=com_rbuf[0];
		    f.l<<=8;
			f.l+=com_rbuf[1];
		    f.l<<=8;
			f.l+=com_rbuf[2];
		    f.l<<=8;
			f.l+=com_rbuf[3];	
			*valf=f.l/1000;
		break;		
		//case FLOAT_ABCD:
		case FLOAT_DCBA:
			f.ch[0]=com_rbuf[0];
			f.ch[1]=com_rbuf[1];
			f.ch[2]=com_rbuf[2];
			f.ch[3]=com_rbuf[3];
			*valf=f.f;
		break;			
		//case FLOAT_CDAB:
		case FLOAT_BADC:
			f.ch[0]=com_rbuf[2];
			f.ch[1]=com_rbuf[3];
			f.ch[2]=com_rbuf[0];
			f.ch[3]=com_rbuf[1];	
			*valf=f.f;
		break;
        //case FLOAT_BADC:
        case FLOAT_CDAB:
			f.ch[0]=com_rbuf[1];
			f.ch[1]=com_rbuf[0];
			f.ch[2]=com_rbuf[3];
			f.ch[3]=com_rbuf[2];
			*valf=f.f;
		break;
		//case FLOAT_DCBA:
		case FLOAT_ABCD:
			f.ch[0]=com_rbuf[3];
			f.ch[1]=com_rbuf[2];
			f.ch[2]=com_rbuf[1];
			f.ch[3]=com_rbuf[0];
			//printf("datapos %d,%x,%x,%x,%x:%f\n",datapos,
			//	f.ch[0]&0xff,f.ch[1]&0xff,f.ch[2]&0xff,f.ch[3]&0xff,
			//	f.f);

			*valf=f.f;
		break;		

		default:
			*valf=0;
			return -10;
    }
	
	return 0;
}

static int mark_data_modbus_buff(struct acquisition_data *acq_data,char *com_tbuf,int size,char *com_rbuf,int *len)
{
     int ret=0;
	 union uf f;
	 char buff[100]={0};

	 if(!acq_data || !com_tbuf || size<=0 || !com_rbuf ) return 1;

	 ret=config_device(DEV_NAME(acq_data),SERIAL_TCIOFLUSH_DATA,NULL); //clear buffer
	 
     ret=write_device(DEV_NAME(acq_data),com_tbuf,size);
	 SYSLOG_DBG_HEX(buff, com_tbuf, size);
	 LOG_WRITE_HEX(DEV_NAME(acq_data),0,"txtx",com_tbuf,size);

     sleep(2);
	 
     ret=read_device(DEV_NAME(acq_data),com_rbuf,*len);
	 SYSLOG_DBG_HEX(buff, com_rbuf, ret);
	 LOG_WRITE_HEX(DEV_NAME(acq_data),1,"rxrx",com_rbuf,ret);
	 if(ret>0)
	 {
		*len=ret;
		return 0;
	 }
	 
     *len=0;
	 return -1;
}



static struct modbus_polcode_arg * mark_find_modbus_polcode_arg_by_polcode(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode)
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

static int is_existed(int curr_dev_addr[100],int dev_addr_cnt,int devaddr)
{
	int i=0;
	for(i=0;i<dev_addr_cnt;i++)
	{
		if(curr_dev_addr[i]==devaddr) return 1; // existed
	}

	return 0;// non-existence
}

static void save_dev_addr(int curr_dev_addr[100],int *dev_addr_cnt,int devaddr)
{
    if(*dev_addr_cnt>=100)return ;
	
    curr_dev_addr[*dev_addr_cnt]=devaddr;
	*dev_addr_cnt+=1;
}

static int get_valf_by_pos(char *com_rbuf,int size , int pos,MODBUS_DATA_TYPE datatype,float *valf)
{
    if(pos<=0 )  
	{
		*valf=0;
		return -1;
    }
	
	datatype2valf_buff(&com_rbuf[3+(pos-1)*4],datatype,valf);

	return 0;
}

int protocol_MARK_GDWeiChuang(struct acquisition_data *acq_data)
{
#define MARK_MODBUS_POLCODE_NUM 20
   char *polcodestr[MARK_MODBUS_POLCODE_NUM]={"g10201","g10101","g10102","g10103","g10104",
   "g10202","g10105","g10106","g10107","g10108",
   "g10203","g10109","g10110","g10111","g10112",
   "g10204","g10113","g10114","g10115","g10116"};
   UNIT_DECLARATION unitstr[MARK_MODBUS_POLCODE_NUM]={UNIT_A,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,
   	UNIT_A,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,
   	UNIT_A,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE,
   	UNIT_A,UNIT_NONE,UNIT_NONE,UNIT_NONE,UNIT_NONE};
   int posnum[MARK_MODBUS_POLCODE_NUM]={
   	1,2,3,4,5,
   	1,2,3,4,5,
   	1,2,3,4,5,
   	1,2,3,4,5,
   	1,2,3,4,5
   	};
   
    char com_tbuf[500]={0};
	char com_rbuf[2049]={0};
	float valf=0;
    int arg_n=0;
    int size=0,len=0;
    int status=0;	
    int ret=0;
    int i=0,j=0;
   struct acquisition_ctrl *acq_ctrl;
   struct modbus_arg *modbusarg_running;
   struct modbus_polcode_arg *mpolcodearg,*mpolcodearg_temp,*mpolcodearg_temp2;
   int curr_dev_addr[100]={0};
   int dev_addr_cnt=0;
   
   acq_data->total=0;// total_flux_M3,added by miles zhang 20191211

   acq_ctrl=ACQ_CTRL(acq_data);
   modbusarg_running=&acq_ctrl->modbusarg_running;
   mpolcodearg=modbusarg_running->polcode_arg;


     for(i=0;i<MARK_MODBUS_POLCODE_NUM;i++)
     {
        mpolcodearg_temp=mark_find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[i]);
		if(!mpolcodearg_temp) continue;
		if(mpolcodearg_temp->enableFlag==0) continue;
		
		
		if(is_existed(curr_dev_addr,dev_addr_cnt,mpolcodearg_temp->regaddr)==0 // such devaddr is not yet operated sending modbus cmd
			)
		{
		    save_dev_addr(curr_dev_addr,&dev_addr_cnt,mpolcodearg_temp->regaddr);
			
			memset(com_tbuf,0,sizeof(com_tbuf));
			size=modbus_pack(com_tbuf, mpolcodearg_temp->regaddr&0xffff, modbusarg_running->devfun,0, 0xa);
			len=sizeof(com_rbuf);
	     	ret=mark_data_modbus_buff(acq_data,com_tbuf,size,com_rbuf,&len);
			if(ret!=0 || com_rbuf[0]!=mpolcodearg_temp->regaddr || com_rbuf[1]!=modbusarg_running->devfun)
			{
			    ret=acqdata_set_value_flag(acq_data,polcodestr[i],unitstr[i],0,'D',&arg_n);
				
	            for(j=i+1;j<MARK_MODBUS_POLCODE_NUM;j++)
	            {
			        mpolcodearg_temp2=mark_find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[j]);
					if(!mpolcodearg_temp2) continue;
					if(mpolcodearg_temp2->enableFlag==0) continue;   
                    if(mpolcodearg_temp2->regaddr==mpolcodearg_temp->regaddr)
                    {
			    		ret=acqdata_set_value_flag(acq_data,polcodestr[j],unitstr[j],0,'D',&arg_n);
                    }
	            }
				continue;
			}
			
            get_valf_by_pos(com_rbuf,len,posnum[i],mpolcodearg_temp->datatype,&valf);
			ret=acqdata_set_value_flag(acq_data,polcodestr[i],unitstr[i],valf,'N',&arg_n);

            for(j=i+1;j<MARK_MODBUS_POLCODE_NUM;j++)
            {
		        mpolcodearg_temp2=mark_find_modbus_polcode_arg_by_polcode(mpolcodearg,modbusarg_running->array_count,polcodestr[j]);
				if(!mpolcodearg_temp2) continue;
				if(mpolcodearg_temp2->enableFlag==0) continue;   

				if(mpolcodearg_temp2->regaddr==mpolcodearg_temp->regaddr)
				{
				    get_valf_by_pos(com_rbuf,len,posnum[j],mpolcodearg_temp2->datatype,&valf);
		    	    ret=acqdata_set_value_flag(acq_data,polcodestr[j],unitstr[j],valf,'N',&arg_n);
				}
            }

		}

     }

    acq_data->dev_stat=0xaa;
	 
    if(status == 0)
        acq_data->acq_status = ACQ_OK;
    else 
        acq_data->acq_status = ACQ_ERR;
    
    NEED_ERROR_CACHE(acq_data, 10);

     return arg_n;
}


