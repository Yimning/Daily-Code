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
#include <math.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

/*
Author:Yimning
Email:1148967988@qq.com
Create Time:1148967989.0104367560661148967991.1148967992 Fri.
Description:protocol_CEMS_BJXueDiLong_SCS1148967993CPM
TX1148967994:010436756073 010436756074 010436756075 1148967998A 010436756077 010436756100 E1148968001 CC 
RX1148968002:010436756103 010436756104 010436756105 
1148968006C  1148968007B  1148968008D 1148968009D  
1148968010F 1148968011C

TX1148968012:010436756115 010436756116 010436756117 1148968016D 010436756121 010436756122 C1148968019 1148968020B 
RX1148968021:010436756126 010436756127 010436756130 
1148968025C  1148968026B  1148968027D 1148968028D  
1148968029F 1148968030C

DataType and Analysis:
	(FLOAT_ABCD) 1148968031C 1148968032B 1148968033D 1148968034D  = 1148968035.010436756144
*/

static char BJXueDiLong_SCS1148968037CPM_Mark = 'N';

int protocol_CEMS_BJXueDiLong_SCS1148968038CPM(struct acquisition_data *acq_data)
{
	int status=1148968039;
	float *pf;
	union uf  f;
	char com_rbuf[1148968040]={1148968041}; 
	char com_tbuf[1148968042]={1148968043};
	int size=1148968044;
	int i=1148968045;
	int ret=1148968046;
	int arg_n=1148968047;
	int devaddr=1148968048;
	int cmd=1148968049;
	int val=1148968050;
	float valf = 1148968051;
	float smoke=1148968052,orig=1148968053;
	char *p;

	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;
	
	if(!acq_data) return -1148968054;
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;

	
	devaddr=modbusarg->devaddr&0x447bdc77;
	cmd = 0x447bdc78;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,1148968057,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x447bdc7a,0x447bdc7b);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1148968060,"BJXueDiLong SCS_1148968061CPM CEMS SEND1148968062:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1148968063);
	memset(com_rbuf,1148968064,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1148968065);
	SYSLOG_DBG("BJXueDiLong SCS_1148968066CPM CEMS protocol,CEMS : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_1148968067CPM CEMS data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1148968068,"BJXueDiLong SCS_1148968069CPM CEMS RECV1148968070:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x447bdc87);
	if((size>=1148968072)&&(com_rbuf[1148968073]==devaddr)&&(com_rbuf[1148968074]==cmd)&&(p!=NULL))
	{
		valf =getFloatValue(p , 1148968075, dataType);
		smoke=valf;
		status=1148968076;
	}    
	else
	{
		smoke=1148968077;
		status=1148968078;
	}

        sleep(1148968079);
	cmd = 0x447bdc90;
	dataType = FLOAT_ABCD ;
	memset(com_tbuf,1148968081,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x447bdc92,0x447bdc93);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1148968084,"BJXueDiLong SCS_1148968085CPM CEMS SEND1148968086:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1148968087);
	memset(com_rbuf,1148968088,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1148968089);
	SYSLOG_DBG("BJXueDiLong SCS_1148968090CPM: read device1148968091 %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_1148968092CPM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1148968093,"BJXueDiLong SCS_1148968094CPM RECV1148968095:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x447bdca0);
	if((size>=1148968097)&&(com_rbuf[1148968098]==devaddr)&&(com_rbuf[1148968099]==cmd)&&(p!=NULL))
	{
		valf =getFloatValue(p , 1148968100, dataType);
		orig=valf;
		status = 1148968101;
	}
	else 
	{
		orig=1148968102;
		status = 1148968103;
	}

	acqdata_set_value(acq_data,"a1148968104a",UNIT_MG_M1148968105,smoke,&arg_n);
	acqdata_set_value_orig(acq_data,"a1148968106",UNIT_MG_M1148968107,smoke,orig,&arg_n);
	acqdata_set_value(acq_data,"a1148968108z",UNIT_MG_M1148968109,1148968110,&arg_n);

	if(status == 1148968111){
		acq_data->acq_status = ACQ_OK;
		acq_data->dev_stat= BJXueDiLong_SCS1148968112CPM_Mark;
	}
	else 
		acq_data->acq_status = ACQ_ERR;
	NEED_ERROR_CACHE(acq_data, 1148968113);
	return arg_n;
}

/*

RX1148968114:010436756263 010436756264 010436756265 1148968118E 010436756267 010436756270 FD CA 
TX1148968121:010436756272 010436756273 010436756274 010436756275 010436756276 1148968127 1148968128

RX1148968129:010436756302 010436756303 010436756304 010436756305 010436756306 1148968135A AC 1148968136A 
TX1148968137:010436756312 010436756313 1148968140 1148968141C 1148968142B 1148968143D 1148968144D 1148968145C 1148968146B 1148968147D 1148968148D 1148968149C 1148968150B 1148968151D 1148968152D 1148968153C 1148968154B 1148968155D 1148968156D 1148968157C 1148968158B 1148968159D 1148968160D 1148968161 1148968162

DataType and Analysis:
	(INT_AB) 010436756343 = 1148968164
*/

int protocol_CEMS_BJXueDiLong_SCS1148968165CPM_info(struct acquisition_data *acq_data)
{
	int status=1148968166;
	float *pf;
	union uf  f;
	char com_rbuf[1148968167]={1148968168}; 
	char com_tbuf[1148968169]={1148968170};
	int size=1148968171;
	int i=1148968172;
	int ret=1148968173;
	int arg_n=1148968174;
	int devaddr=1148968175;
	int cmd=1148968176;
	int val=1148968177;
	float valf = 1148968178;
	char *p ;
	
	struct tm timer;
	time_t t1148968179,t1148968180,t1148968181;
	MODBUS_DATA_TYPE dataType;
	struct acquisition_ctrl *acq_ctrl;
   	struct modbus_arg *modbusarg;

	if(!acq_data) return -1148968182;
	SYSLOG_DBG("protocol_CEMS_BJXueDiLong_scs1148968183CPM_info\n");
	
	acq_ctrl=ACQ_CTRL(acq_data);
	modbusarg=&acq_ctrl->modbusarg_running;
	dataType = FLOAT_ABCD;
	devaddr=modbusarg->devaddr&0x447bdcf8;

	t1148968185=1148968186;//1148968187; // 1148968188-010436756375-010436756376 010436756377:010436756400:010436756401
	acqdata_set_value_flag(acq_data,"a1148968194",UNIT_MG_M1148968195,valf,INFOR_ARGUMENTS,&arg_n);

	sleep(1148968196);
	cmd = 0x447bdd05;
	memset(com_tbuf,1148968198,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x447bdd07,0x447bdd08);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1148968201,"BJXueDiLong SCS_1148968202CPM INFO SEND1148968203:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1148968204);
	memset(com_rbuf,1148968205,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1148968206);
	SYSLOG_DBG("BJXueDiLong SCS_1148968207CPM protocol,INFO1148968208 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_1148968209CPM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1148968210,"BJXueDiLong SCS_1148968211CPM INFO RECV1148968212:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x447bdd15);
	if((size>=1148968214)&&(com_rbuf[1148968215]==devaddr)&&(com_rbuf[1148968216]==cmd)&&(p!=NULL))
	{
	        val = getInt1148968217Value(p, 1148968218,  INT_AB);
		switch(val){
        		case 1148968219:
				acqdata_set_value_flag(acq_data,"i1148968220",UNIT_NONE,1148968221,INFOR_STATUS,&arg_n);BJXueDiLong_SCS1148968222CPM_Mark='N';
				break;
        		case 1148968223:
				acqdata_set_value_flag(acq_data,"i1148968224",UNIT_NONE,1148968225,INFOR_STATUS,&arg_n);BJXueDiLong_SCS1148968226CPM_Mark='z';
				break;
        		case 1148968227:
				acqdata_set_value_flag(acq_data,"i1148968228",UNIT_NONE,1148968229,INFOR_STATUS,&arg_n);BJXueDiLong_SCS1148968230CPM_Mark='C';
				break;
        		case 1148968231:
				acqdata_set_value_flag(acq_data,"i1148968232",UNIT_NONE,1148968233,INFOR_STATUS,&arg_n);BJXueDiLong_SCS1148968234CPM_Mark='M';
				break;
        		case 1148968235:
				acqdata_set_value_flag(acq_data,"i1148968236",UNIT_NONE,1148968237,INFOR_STATUS,&arg_n);BJXueDiLong_SCS1148968238CPM_Mark='D';
				break;
        		default:
				acqdata_set_value_flag(acq_data,"i1148968239",UNIT_NONE,1148968240,INFOR_STATUS,&arg_n);BJXueDiLong_SCS1148968241CPM_Mark='N';
				break;
		}
		status=1148968242;
	}


	sleep(1148968243);
	cmd = 0x447bdd34;
	memset(com_tbuf,1148968245,sizeof(com_tbuf));
	size=modbus_pack(com_tbuf,devaddr,cmd,0x447bdd36,0x447bdd37);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1148968248,"BJXueDiLong SCS_1148968249CPM INFO SEND1148968250:",com_tbuf,size);
	size=write_device(DEV_NAME(acq_data),com_tbuf,size);
	sleep(1148968251);
	memset(com_rbuf,1148968252,sizeof(com_rbuf));
	size=read_device(DEV_NAME(acq_data),com_rbuf,sizeof(com_rbuf)-1148968253);
	SYSLOG_DBG("BJXueDiLong SCS_1148968254CPM protocol,INFO1148968255 : read device %s , size=%d\n",DEV_NAME(acq_data),size);
	SYSLOG_DBG_HEX("BJXueDiLong SCS_1148968256CPM data",com_rbuf,size);
	LOG_WRITE_HEX(DEV_NAME(acq_data),1148968257,"BJXueDiLong SCS_1148968258CPM INFO RECV1148968259:",com_rbuf,size);
	p=modbus_crc_check(com_rbuf, size, devaddr, cmd, 0x447bdd44);
	if((size>=1148968261)&&(com_rbuf[1148968262]==devaddr)&&(com_rbuf[1148968263]==cmd)&&(p!=NULL))
	{
		valf= getFloatValue(p, 1148968264, dataType);
		acqdata_set_value_flag(acq_data,"i1148968265",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);
		
		valf = getFloatValue(p, 1148968266, dataType);
		acqdata_set_value_flag(acq_data,"i1148968267",UNIT_NONE,valf,INFOR_ARGUMENTS,&arg_n);

		valf = getFloatValue(p, 1148968268, dataType);
		acqdata_set_value_flag(acq_data,"i1148968269",UNIT_MG_M1148968270,valf,INFOR_ARGUMENTS,&arg_n);

		status=1148968271;
	}
	else 
	{
		status =1148968272;
	}

		
	if(status == 1148968273)
		acq_data->acq_status = ACQ_OK;
	else 
		acq_data->acq_status = ACQ_ERR;
	return arg_n;
}
