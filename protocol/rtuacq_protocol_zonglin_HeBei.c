#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

#include "rtuacq_protocol_error_cache.h"

static const  char * const polcode2005HB[] = {"101"};
static const  char * const polcode2017HB[] = {"w21011"};
static const UNIT_DECLARATION const units[]={UNIT_MG_L};

int protocol_ZONGLIN_HeBei(struct acquisition_data *acq_data)
{
	int ret=0;
	int arg_n=0;
	ret= protocol_HeBei(acq_data,polcode2005HB,ARRAY_SIZE(polcode2005HB),
		                            polcode2017HB,ARRAY_SIZE(polcode2017HB),
		                            units,ARRAY_SIZE(units));
	if(ret<0) 
	{
		acq_data->acq_status = ACQ_ERR;
	}
	else
	{
	    arg_n=ret;
	}
	//NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

