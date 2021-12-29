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

int protocol_FLUX_MBmag(struct acquisition_data *acq_data)
{
	int status=0;
	union uf  f;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	long val=0;
	float valf=0;
	float total=0;
	float speed=0;
	int ret=0;
	int arg_n=0;
	unsigned int devaddr=0,cmd=0,startaddr=0,cnt=0;
	MODBUS_DATA_TYPE dataType;
	char *p=NULL;
	int pos=0;
	
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg = &acq_ctrl->modbusarg;

	dataType=FLOAT_ABCD;
	
	devaddr = modbusarg->devaddr&0xffff;
	cmd=0x03;
	startaddr=0x1010;
	cnt=0x12;

	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,startaddr,cnt);
	size=getHexDataPack(DEV_NAME(acq_data), com_tbuf, size, com_rbuf, sizeof(com_rbuf), "MBmag flux", 1);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, cnt);
	if(p!=NULL)
	{
		pos=get_cmd_03_04_pos(startaddr, 0x1010);
		speed=getFloatValue(p, pos, dataType);

		pos=get_cmd_03_04_pos(startaddr, 0x1012);
		val=getInt32Value(p, pos, LONG_ABCD);
		pos=get_cmd_03_04_pos(startaddr, 0x1014);
		valf=getFloatValue(p, pos, dataType);
		total=val+valf;

		pos=get_cmd_03_04_pos(startaddr, 0x1020);
		val=getUInt16Value(p, pos, INT_AB);
		switch(val)
		{
			case 0x00:	speed=speed*1000/1;							break;
			case 0x01:	speed=speed*1000/60;							break;
			case 0x02:	speed=speed*1000/3600;						break;
			case 0x03:	speed=speed*1/1;								break;
			case 0x04:	speed=speed*1/60;							break;
			case 0x05:	speed=speed*1/3600;							break;
			case 0x06:	speed=water_US_gal_to_m3(speed)*1000/60;	break;
			case 0x07:	speed=water_US_gal_to_m3(speed)*1000/3600;	break;
			case 0x08:	speed=water_UK_gal_to_m3(speed)*1000/60;	break;
			case 0x09:	speed=water_UK_gal_to_m3(speed)*1000/3600;	break;
			case 0x0A:	speed=speed*1000/1;							break;
			case 0x0B:	speed=speed*1000/60;							break;
			case 0x0C:	speed=speed*1000/3600;						break;
			case 0x0D:	speed=speed*1/1;								break;
			case 0x0E:	speed=speed*1/60;							break;
			case 0x0F:	speed=speed*1/3600;							break;
			default:		break;
			
		}

		pos=get_cmd_03_04_pos(startaddr, 0x1021);
		val=getUInt16Value(p, pos, INT_AB);
		switch(val)
		{
			case 0:	total=total/1000;					break;
			case 1:	total=total/1;						break;
			case 2:	total=water_US_gal_to_m3(total);	break;
			case 3:	total=water_UK_gal_to_m3(total);	break;
			case 4:	total=total/1000;					break;
			case 5:	total=total/1;						break;
			default:	break;
		}
		
		status=0;
	}
	else
	{
		speed=0;
		total=0;
		status=1;
	}

	acqdata_set_value(acq_data,"w00000",UNIT_L_S,speed,&arg_n);
	acqdata_set_value(acq_data,"w00001",UNIT_M3,total,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



