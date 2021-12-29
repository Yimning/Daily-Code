#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"


static const  char * const polcode2005HB[] = {"ZB2","ZD8","ZD5","ZB3","ZD9",
										"ZD6","ZA9","ZD7","ZD4","ZB0",
										"ZB4","ZD1","ZB1","ZB6","ZD3"};

static const  char * const polcode2017HB[] = {"a21026","a21026z","a00LD5","a21002","a21002z",
										"a00LD6","a34013","a34013z","a00LD4","a01011",
										"a19001","a01012","a01013","a00000","a00LD3"};

static const UNIT_DECLARATION const units[]={UNIT_MG_M3,UNIT_MG_M3,UNIT_KG,UNIT_MG_M3,UNIT_MG_M3,
										UNIT_KG,UNIT_MG_M3,UNIT_MG_M3,UNIT_KG,UNIT_M_S,
										UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_H,UNIT_M3};

int protocol_CEMS_XinJiang(struct acquisition_data *acq_data)
{
	int ret=0;
	int arg_n=0;
	ret= protocol_XinJiang(acq_data,polcode2005HB,ARRAY_SIZE(polcode2005HB),
		                            polcode2017HB,ARRAY_SIZE(polcode2017HB),
		                            units,ARRAY_SIZE(units));
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

