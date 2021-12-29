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

int protocol_FLUX_SHGuangHua(struct acquisition_data *acq_data)
{
	int status=0;
	float *pf;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	union uf  f;
	int arg_n=0;
	int val[4]={0};
	float valf=0;
	float total=0;
	float speed=0;
	unsigned int devaddr=0,cmd=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;

	dataType=FLOAT_CDAB;
	
	devaddr = modbusarg->devaddr&0xffff;
	cmd=0x03;
	cnt=0x0F;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf, devaddr, cmd, 0x01, cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "SHGuangHua flux", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		speed=getInt32Value(p, 3, LONG_ABCD);
		valf=getInt16Value(p, 7, INT_AB);
		while(valf>1)
			valf*=0.1;
		speed+=valf;
		
		val[0]=getInt16Value(p, 27, INT_AB);
		val[1]=getInt16Value(p, 29, INT_AB);
		val[2]=getInt16Value(p, 31, INT_AB);
		val[0]=val[0]%10000;
		val[1]=val[1]%10000;
		val[2]=val[2]%10000;
		val[3]=val[2]%100;
		val[2]=val[2]/100;

		total=(val[0]*10000+val[1])*100+val[2];
		switch(val[3])
		{
			case 0:	total = total/1000/1000;					break;
			case 1:	total = total/1000/100;						break;
			case 2:	total = total/1000/10;						break;
			case 3:	total = total/1000/1;						break;
			case 4:	total = total/1/1000;						break;
			case 5:	total = total/1/100;						break;
			case 6:	total = total/1/10;							break;
			case 7:	total = total/1/1;							break;
			case 8:	total = water_US_gal_to_m3(total)/1000;	break;
			case 9:	total = water_US_gal_to_m3(total)/100;	break;
			case 10:	total = water_US_gal_to_m3(total)/10;		break;
			case 11:	total = water_US_gal_to_m3(total)/1;		break;
			case 12:	total = water_UK_gal_to_m3(total)/1000;	break;
			case 13:	total = water_UK_gal_to_m3(total)/100;	break;
			case 14:	total = water_UK_gal_to_m3(total)/10;		break;
			case 15:	total = water_UK_gal_to_m3(total)/1;		break;
			case 16:	total = total/1000/1000;					break;
			case 17:	total = total/1000/100;						break;
			case 18:	total = total/1000/10;						break;
			case 19:	total = total/1000/1;						break;
			case 20:	total = total/1/1000;						break;
			case 21:	total = total/1/100;						break;
			case 22:	total = total/1/10;							break;
			case 23:	total = total/1/1;							break;
		}
			
		status=0;
	}
	else
	{
		speed=0;
		total=0;
		
		status=1;

	}
	
	acqdata_set_value(acq_data,"w00000",UNIT_M3_H,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else
		acq_data->acq_status = ACQ_ERR;
	
	NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

