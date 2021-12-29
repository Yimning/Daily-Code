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

int protocol_dianbiao_DGYongBang(struct acquisition_data *acq_data)
{
	int status=0;
	int val=0;
	union uf  f;
	union uf64 f64;
	char com_rbuf[2048]={0}; 
	char com_tbuf[8]={0};
	int size=0;
	float ia=0,ib=0,ic=0,power=0,pa=0,pb=0,pc=0;
	int ret=0;
	int arg_n=0;
	int devaddr=0;

	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	devaddr=modbusarg->devaddr&0xffff;

	status=0;
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x03,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DGYongBang ec SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DGYongBang ec protocol,ec : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DGYongBang ec data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DGYongBang ec RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		ia=val*0.1;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		ib=val*0.1;
		
		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		ic=val*0.1;
	}
	else
	{
		ia=0;
		ib=0;
		ic=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x08,0x03);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DGYongBang pt SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DGYongBang pt protocol,pt : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DGYongBang pt data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DGYongBang pt RECV:",com_rbuf,size);
	if((size>=11)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		pa=(float)val/1000;

		val=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		pb=(float)val/1000;
		
		val=com_rbuf[7];
		val<<=8;
		val+=com_rbuf[8];
		pc=(float)val/1000;
	}
	else
	{
		pa=0;
		pb=0;
		pc=0;
		status=1;
	}

	sleep(1);
	memset(com_tbuf,0,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,0x04,0x1D,0x02);
	LOG_WRITE_HEX(DEV_NAME(acq_data),0,"DGYongBang power SEND:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1);
	memset(com_rbuf,0,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1);
	SYSLOG_DBG("DGYongBang power protocol,power : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("DGYongBang pt data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1,"DGYongBang power RECV:",com_rbuf,size);
	if((size>=9)&&(com_rbuf[0]==devaddr)&&(com_rbuf[1]==0x04))
	{
		val=com_rbuf[3];
		val<<=8;
		val+=com_rbuf[4];
		val<<=8;
		val+=com_rbuf[5];
		val<<=8;
		val+=com_rbuf[6];
		power=val*0.01;
	}
	else
	{
		power=0;
		status=1;
	}

	acqdata_set_value(acq_data,"e30201",UNIT_A,ia,&arg_n);
	acqdata_set_value(acq_data,"e30202",UNIT_A,ib,&arg_n);
	acqdata_set_value(acq_data,"e30203",UNIT_A,ic,&arg_n);
	acqdata_set_value(acq_data,"e30101",UNIT_KW,pa,&arg_n);
	acqdata_set_value(acq_data,"e30102",UNIT_KW,pb,&arg_n);
	acqdata_set_value(acq_data,"e30103",UNIT_KW,pc,&arg_n);
	acqdata_set_value(acq_data,"e30001",UNIT_KWH,power,&arg_n);
	acqdata_set_value(acq_data,"e30204",UNIT_A,ia,&arg_n);
	acqdata_set_value(acq_data,"e30205",UNIT_A,ib,&arg_n);
	acqdata_set_value(acq_data,"e30206",UNIT_A,ic,&arg_n);
	acqdata_set_value(acq_data,"e30104",UNIT_KW,pa,&arg_n);
	acqdata_set_value(acq_data,"e30105",UNIT_KW,pb,&arg_n);
	acqdata_set_value(acq_data,"e30106",UNIT_KW,pc,&arg_n);
	acqdata_set_value(acq_data,"e30004",UNIT_KWH,power,&arg_n);
	acqdata_set_value(acq_data,"e30207",UNIT_A,ia,&arg_n);
	acqdata_set_value(acq_data,"e30208",UNIT_A,ib,&arg_n);
	acqdata_set_value(acq_data,"e30209",UNIT_A,ic,&arg_n);
	acqdata_set_value(acq_data,"e30107",UNIT_KW,pa,&arg_n);
	acqdata_set_value(acq_data,"e30108",UNIT_KW,pb,&arg_n);
	acqdata_set_value(acq_data,"e30109",UNIT_KW,pc,&arg_n);
	acqdata_set_value(acq_data,"e30007",UNIT_KWH,power,&arg_n);
	acqdata_set_value(acq_data,"e30210",UNIT_A,ia,&arg_n);
	acqdata_set_value(acq_data,"e30211",UNIT_A,ib,&arg_n);
	acqdata_set_value(acq_data,"e30212",UNIT_A,ic,&arg_n);
	acqdata_set_value(acq_data,"e30110",UNIT_KW,pa,&arg_n);
	acqdata_set_value(acq_data,"e30111",UNIT_KW,pb,&arg_n);
	acqdata_set_value(acq_data,"e30112",UNIT_KW,pc,&arg_n);
	acqdata_set_value(acq_data,"e30010",UNIT_KWH,power,&arg_n);
	acqdata_set_value(acq_data,"e70203",UNIT_A,ia,&arg_n);
	acqdata_set_value(acq_data,"e70204",UNIT_A,ib,&arg_n);
	acqdata_set_value(acq_data,"e70205",UNIT_A,ic,&arg_n);
	acqdata_set_value(acq_data,"e70103",UNIT_KW,pa,&arg_n);
	acqdata_set_value(acq_data,"e70104",UNIT_KW,pb,&arg_n);
	acqdata_set_value(acq_data,"e70105",UNIT_KW,pc,&arg_n);
	acqdata_set_value(acq_data,"e70003",UNIT_KWH,power,&arg_n);
	
	if(status == 0)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 10);
	return arg_n;
}



