#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"

/*
static const  char * const polcode2005HB[] = {"ZB2","ZD8","ZD5","ZB3","ZD9",
										"ZD6","ZA9","ZD7","ZD4","ZB0",
										"ZB4","ZD1","ZB1","ZB6","ZD3"};


static const  char * const polcode2017HB[] = {"a21026","a21026z","a00LD5","a21002","a21002z",
										"a00LD6","a34013","a34013z","a00LD4","a01011",
										"a19001","a01012","a01013","a00000","a00LD3"};

static const UNIT_DECLARATION const units[]={UNIT_MG_M3,UNIT_MG_M3,UNIT_KG,UNIT_MG_M3,UNIT_MG_M3,
										UNIT_KG,UNIT_MG_M3,UNIT_MG_M3,UNIT_KG,UNIT_M_S,
										UNIT_PECENT,UNIT_0C,UNIT_PA,UNIT_M3_H,UNIT_M3};
*/

int protocol_CEMS_XinJiang(struct acquisition_data *acq_data)
{
#define POLCODE_NUM 16

	static char *polcode2005[POLCODE_NUM] = {
		" 02",	" 02",	" 03",	" 03",	" 01",
		" 01",	"QX8",	"QX8",	"QX9",	"QX9",
		"S02",	"S01",	"S03",	"S08",	"B02",
		"S05",
	};

	static char *polcode2017[POLCODE_NUM] = {
		"a21026",		"a21026z",	"a21002",		"a21002z",	"a34013",
		"a34013z",	"a21003",		"a21003z",	"a21004",		"a21004z",
		"a01011",		"a19001",		"a01012",		"a01013",		"a00000",
		"a01014"
	};

	static UNIT_DECLARATION units[POLCODE_NUM]={
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,		UNIT_MG_M3,
		UNIT_M_S,		UNIT_PECENT,	UNIT_0C,			UNIT_PA,			UNIT_M3_H,
		UNIT_PECENT
	};
	
	int arg_n=0;
	arg_n=protocol_XinJiang(acq_data,polcode2005,polcode2017,units,POLCODE_NUM);

	NEED_ERROR_CACHE(acq_data, 60);

	return arg_n;
#undef POLCODE_NUM
}

