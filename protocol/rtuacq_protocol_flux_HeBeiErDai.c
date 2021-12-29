
#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"


static const  char * const polcode2005HB[] = {"B01","B00"};
static const  char * const polcode2017HB[] = {"w00000","w00001"};
static const UNIT_DECLARATION const units[]={UNIT_L_S,UNIT_M3};

int protocol_FLUX_HeBeiErDai(struct acquisition_data *acq_data)
{
	int ret=0;
	int arg_n=0;
	float total;
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
		if(acqdata_get_value(acq_data,"w00001",&total)==0) //add by miles zhang 20181204
	    	acq_data->total=total;
	    arg_n=ret;
		return arg_n;
	}

	return arg_n;
}
