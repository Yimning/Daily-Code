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

static const  char * const polcode2005HB_WATER[] = {
	"B01","011","060","101","065",
	"024","028","023","026","022",
	"032","031","027","030","029",
	"070","110"
};

static const  char * const polcode2017HB_WATER[] ={
	"w00000","w01018","w21003","w21011","w21001",
	"w20117","w20121","w20116","w20119","w20115",
	"w20125","w20124","w20120","w20123","w20122",
	"w21016","w23002"
};

static const UNIT_DECLARATION const units_WATER[]={
	UNIT_L_S,	UNIT_MG_L,	UNIT_MG_L,	UNIT_MG_L,	UNIT_MG_L,
	UNIT_MG_L,	UNIT_MG_L,	UNIT_MG_L,	UNIT_MG_L,	UNIT_MG_L,
	UNIT_MG_L,	UNIT_MG_L,	UNIT_MG_L,	UNIT_MG_L,	UNIT_MG_L,
	UNIT_MG_L,	UNIT_MG_L
};

int protocol_WATER_HeBei(struct acquisition_data *acq_data)
{
	int ret=0;
	int arg_n=0;
	ret= protocol_HeBei(acq_data,polcode2005HB_WATER,ARRAY_SIZE(polcode2005HB_WATER),
								polcode2017HB_WATER,ARRAY_SIZE(polcode2017HB_WATER),
								units_WATER,ARRAY_SIZE(units_WATER));
	
	//NEED_ERROR_CACHE(acq_data, 10);
	if(ret<0) 
	{
		acq_data->acq_status = ACQ_ERR;
		return arg_n;
	}
	else
	{
		arg_n=ret;
		return arg_n;
	}

	return arg_n;
}

