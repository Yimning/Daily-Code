#ifndef __NEED_ERROR_CACHE__
#define __NEED_ERROR_CACHE__


#if 0
static struct acquisition_data acq_data_bak;
static unsigned long cnt=0;
   
static void NEED_ERROR_CACHE(struct acquisition_data *acq_data,int cnt_max)  
{
   if(!acq_data) return ;

   if(cnt_max<=0 && cnt_max>=100) cnt_max=20;
   
    //  step1
	if(acq_data->acq_status==ACQ_ERR && cnt==0) return ;
    // step2
	if(acq_data->acq_status==ACQ_OK)
	{
	    cnt=1;// at fist time acq data ok
		acq_data_bak=*acq_data;
		return ;
	}
	//step3 acq_data->acq_status==ACQ_ERR 
	if(acq_data->acq_status==ACQ_ERR && cnt>0)
	{
		 if(cnt++>cnt_max)
		 {
		     cnt=1; // return to 1 and increase again
			 acq_data_bak=*acq_data;
		 }
		 else
		 {
		   *acq_data=acq_data_bak;
		   acq_data->total=0;
		   acq_data->zs_total=0;		   
		 }

		 return ;
	}

	return ;
}

#else

#if 0
static void NEED_ERROR_CACHE(struct acquisition_data *acq_data,int cnt_max)  
{

   unsigned long cnt=get_cnt_by_devname(DEV_NAME(acq_data));
   //struct acquisition_data acq_data_bak=get_acq_data_by_devname(DEV_NAME(acq_data));
   struct acquisition_data acq_data_bak;

   get_acq_data_by_devname(DEV_NAME(acq_data),&acq_data_bak);
   	
   if(!acq_data) return ;

   if(cnt_max<=0 && cnt_max>=100) cnt_max=20;
   
    //  step1
	if(acq_data->acq_status==ACQ_ERR && cnt==0) return ;
    // step2
	if(acq_data->acq_status==ACQ_OK)
	{
	    cnt=1;// at fist time acq data ok
	    set_cnt_by_devname(DEV_NAME(acq_data),cnt);
		acq_data_bak=*acq_data;
		set_acq_data_by_devname(DEV_NAME(acq_data),acq_data_bak);
		return ;
	}
	//step3 acq_data->acq_status==ACQ_ERR 
	if(acq_data->acq_status==ACQ_ERR && cnt>0)
	{
	     cnt++;
	     set_cnt_by_devname(DEV_NAME(acq_data),cnt);
		 if(cnt>cnt_max)
		 {
		     cnt=1; // return to 1 and increase again
		     set_cnt_by_devname(DEV_NAME(acq_data),cnt);
			 acq_data_bak=*acq_data;
			 set_acq_data_by_devname(DEV_NAME(acq_data),acq_data_bak);
		 }
		 else
		 {
		   //acq_data_bak=get_acq_data_by_devname(DEV_NAME(acq_data));
		   get_acq_data_by_devname(DEV_NAME(acq_data),&acq_data_bak);
		   *acq_data=acq_data_bak;
		   acq_data->total=0;
		   acq_data->zs_total=0;		   
		 }

		 return ;
	}

	return ;
}
#endif

#endif


#endif
