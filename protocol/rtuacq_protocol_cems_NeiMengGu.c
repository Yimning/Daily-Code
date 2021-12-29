#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"


static const  char * const polcode2005HB[] = {"S03","S08","S05","S01","S02",
					"01","01z","02","02z","03","03z","04","04z","B02"};

static const  char * const polcode2017HB[] ={"a01012","a01013","a01014","a19001",
					"a01011","a34013","a34013z","a21026","a21026z","a21002",
					"a21002z","a21005","a21005z","a00000"};

static const UNIT_DECLARATION const units[]={UNIT_0C,UNIT_KPA,UNIT_PECENT,UNIT_PECENT,
					UNIT_M_S,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,
					UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_MG_M3,UNIT_M3_H};

int protocol_CEMS_NeiMengGu(struct acquisition_data *acq_data)
{
	int ret=0;
	int arg_n=0;
	ret= protocol_HeBei(acq_data,polcode2005HB,ARRAY_SIZE(polcode2005HB),
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
