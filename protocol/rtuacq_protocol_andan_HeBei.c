#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

#include "rtuacq_protocol_error_cache.h"

static const  char * const polcode2005HB[] = {"060"};
static const  char * const polcode2017HB[] = {"w21003"};
static const UNIT_DECLARATION const units[]={UNIT_MG_L};

int protocol_ANDAN_HeBei(struct acquisition_data *acq_data)
{
	int ret=0;
	int arg_n=0;
	arg_n= protocol_HeBei(acq_data,polcode2005HB,ARRAY_SIZE(polcode2005HB),
		                            polcode2017HB,ARRAY_SIZE(polcode2017HB),
		                            units,ARRAY_SIZE(units));

	//NEED_ERROR_CACHE(acq_data,10);
	return arg_n;
}

