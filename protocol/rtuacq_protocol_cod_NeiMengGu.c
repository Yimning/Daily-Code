#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"


static const  char * const polcode2005HB[] = {"011"};
static const  char * const polcode2017HB[] = {"w01018"};
static const UNIT_DECLARATION const units[]={UNIT_MG_L};

int protocol_COD_NeiMengGu(struct acquisition_data *acq_data)
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

