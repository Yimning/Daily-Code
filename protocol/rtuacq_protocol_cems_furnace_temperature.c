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


int protocol_CEMS_furnace_temperature(struct acquisition_data *acq_data)
{
    int status=0;
	int arg_n=0;

    acqdata_set_value(acq_data,"a01902",UNIT_0C,1,&arg_n);
	acqdata_set_value(acq_data,"a01901",UNIT_0C,2,&arg_n);
	acqdata_set_value(acq_data,"i33310",UNIT_0C,3,&arg_n);
	acqdata_set_value(acq_data,"i33311",UNIT_0C,4,&arg_n);
	acqdata_set_value(acq_data,"i33312",UNIT_0C,5,&arg_n);
	acqdata_set_value(acq_data,"i33320",UNIT_0C,6,&arg_n);
	acqdata_set_value(acq_data,"i33321",UNIT_0C,7,&arg_n);
	acqdata_set_value(acq_data,"i33322",UNIT_0C,8,&arg_n);
	acqdata_set_value(acq_data,"i33330",UNIT_0C,9,&arg_n);
	acqdata_set_value(acq_data,"i33331",UNIT_0C,10,&arg_n);
	acqdata_set_value(acq_data,"i33332",UNIT_0C,11,&arg_n);
	acqdata_set_value(acq_data,"i33340",UNIT_0C,12,&arg_n);
	acqdata_set_value(acq_data,"i33341",UNIT_0C,13,&arg_n);
	acqdata_set_value(acq_data,"i33342",UNIT_0C,14,&arg_n);


	
	if(status == 0)
   		acq_data->acq_status = ACQ_OK;
	else 
 		acq_data->acq_status = ACQ_ERR;

    return arg_n;
}


