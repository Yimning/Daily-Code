#include <stdio.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <openssl/md5.h>

#include "common.h"
#include "rtuacquisition.h"
#include "rtuacq_protocol.h"
#include "rtuacq_protocol_error_cache.h"
#include "rtuacq_protocol_door_common.h"

static size_t body_callback(void *buffer,size_t size,size_t nmemb,void *arg)

{
	int len;
	struct callback_data_struct *data;

	data=(struct callback_data_struct *)arg;
	len=data->len+size*nmemb>MAX_MEMORY?MAX_MEMORY-len:size*nmemb;
	memcpy(&(data->data[data->len]),(char *)buffer,len);

	data->len=data->len+len;

	//SYSLOG_DBG("body_callback size=%d,nmemb=%d:%s\n",size,nmemb,(char *)buffer);
	
	return size*nmemb;

}

static size_t header_callback(void *buffer,size_t size,size_t nmemb,void *arg)
{
	char *data;
	char *p,*q;
	int len;

	p=NULL;
	q=NULL;
	if(!strncmp((char *)buffer,"WWW-Authenticate:",16))
	{
		data=&(((char *)buffer)[18]);
		p=strstr(data,"nonce");
		if(p==NULL) return size*nmemb;

		p=p+7;
		q=strchr(p,'"');
		if(q==NULL) return size*nmemb;
		len=q-p;
		strncpy(((struct check_data *)arg)->nonce,p,len);

		p=NULL;
		q=NULL;
		p=strstr(data,"realm");
		if(p==NULL) return size*nmemb;

		p=p+7;
		q=strchr(p,'"');
		if(q==NULL) return size*nmemb;
		len=q-p;
		strncpy(((struct check_data *)arg)->realm,p,len);
	}

	//printf("callback %s\n",(char *)buffer);

	return size*nmemb;
}


static int curl_certification(struct check_data *checkdata,struct curl_opt *opt,char *respdata)
{
	int i;
	struct callback_data_struct callbackData;
	char *url=opt->url;
	char *method=opt->method;
	long timeout=opt->timeout;
	char *uri=opt->uri;
	char *condstr=opt->condstr;

	CURL *curl;
	CURLcode res;
	long httpCode=0;
	int ret=0;
	struct curl_slist *http_header = NULL;
	struct check_data *checkData=checkdata;

    memset(&callbackData,0,sizeof(struct callback_data_struct));

	curl = curl_easy_init();
	if(!curl)
	{
		printf("curl init failed\n");
		return -2;
	}

	curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,header_callback);
	curl_easy_setopt(curl,CURLOPT_HEADERDATA,checkData);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,body_callback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&callbackData);
	curl_easy_setopt(curl,CURLOPT_URL,url);
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,timeout);
	curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1L);
	curl_easy_setopt(curl, CURLOPT_INTERFACE, "eth0");
	
	if(!strcmp(method,"POST"))
	{
		curl_easy_setopt(curl,CURLOPT_POSTFIELDS,condstr); 
		curl_easy_setopt(curl,CURLOPT_POST,1);
	}

	curl_easy_setopt(curl,CURLOPT_HTTPHEADER,http_header);
 
	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		switch(res)
		{
			case CURLE_COULDNT_RESOLVE_HOST:
				ret=-3;
				//printf("acq curl error : could not resolve host\n");break;
				SYSLOG_DBG("acq curl error : could not resolve host\n");break;
			case CURLE_UNSUPPORTED_PROTOCOL:
				ret=-4;
				SYSLOG_DBG("acq curl error : unsupported protocol\n");break;
			case CURLE_COULDNT_CONNECT:
				ret=-5;
				SYSLOG_DBG("acq curl error : could not connect\n");break;
			case CURLE_HTTP_RETURNED_ERROR:
				ret=-6;
				SYSLOG_DBG("acq curl error : HTTP return error\n");break;
			case CURLE_READ_ERROR:
				ret=-7;
				SYSLOG_DBG("acq curl error : read error\n");break;
			default:
				ret=-8;
				SYSLOG_DBG("acq curl error : unknow error return:%d\n",res);break;
		}
		
	    curl_slist_free_all(http_header);
	    curl_easy_cleanup(curl);
		return ret;
	}

	res=curl_easy_getinfo(curl,CURLINFO_HTTP_CODE,&httpCode);
	curl_slist_free_all(http_header);
	curl_easy_cleanup(curl);

	//printf("%s\n%d\n",callbackData.data,callbackData.len);
	SYSLOG_DBG("callbackData len=%d: %s\n",callbackData.len,callbackData.data);

	if(res==CURLE_OK && httpCode==401)
	{
                return 0;
	}

	if(res!=CURLE_OK) ret = -20;
	else if(httpCode!=401) ret = -21;

	return ret;
}

static int curl_info(struct check_data *checkdata,struct curl_opt *opt,char *respdata,int *resplen)
{
	struct callback_data_struct callbackData;
	char *url=opt->url;
	char *method=opt->method;
	long timeout=opt->timeout;
	char *uri=opt->uri;
	char *condstr=opt->condstr;
	
	CURL *curl;
	CURLcode res;
	long httpCode=0;
	int ret=0;
	struct curl_slist *http_header = NULL;
	int i;

	unsigned char md[16];
	char Auth1[33]={'\0'};
	char Auth2[33]={'\0'};
	char response[33]={'\0'};
	char tmp[200]={'\0'};
	char Authorization[800]={0};
	struct check_data *checkData=checkdata;

	if(!opt || !respdata || !resplen) return -1;

	curl = curl_easy_init();
	if(!curl)
	{
		printf("curl init failed\n");
		return -2;
	}

	callbackData.len=0;

	curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,header_callback);
	curl_easy_setopt(curl,CURLOPT_HEADERDATA,checkData);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,body_callback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&callbackData);
	curl_easy_setopt(curl,CURLOPT_URL,url);
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,timeout);
	curl_easy_setopt(curl, CURLOPT_INTERFACE, "eth0");
	
	if(!strcmp(method,"POST"))
	{
		curl_easy_setopt(curl,CURLOPT_POSTFIELDS,condstr); 
		curl_easy_setopt(curl,CURLOPT_POST,1);
	}

	if(!strcmp(method,"PUT") )
	{
		curl_easy_setopt(curl,CURLOPT_POSTFIELDS,condstr); 
		curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,"PUT");
	}
	
	//curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
	curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1L);
	//http_header = curl_slist_append(http_header,"User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.116 Safari/537.36");
	//http_header = curl_slist_append(http_header,"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
	
	//if(checkFlag==1)
	{
        //snprintf(tmp,180,"admin:%s:abc12345",checkData->realm);
        snprintf(tmp,180,"%s:%s:%s",opt->uname,checkData->realm,opt->passwd);
        MD5(tmp,strlen(tmp),md);
		for( i=0; i<16; i++ )
		{
              sprintf(&Auth1[strlen(Auth1)],"%02x",md[i]);
		}

        snprintf(tmp,180,"%s:%s",method,uri);
        MD5(tmp,strlen(tmp),md);
		for( i=0; i<16; i++ ){
              sprintf(&Auth2[strlen(Auth2)],"%02x",md[i]);
		}

		snprintf(tmp,180,"%s:%s:00000003:dcdc1919a7f43849:auth:%s",Auth1,checkData->nonce,Auth2);
        MD5(tmp,strlen(tmp),md);
		for( i=0; i<16; i++ )
		{
              sprintf(&response[strlen(response)],"%02x",md[i]);
		}
		
		snprintf(Authorization,780,"Authorization: Digest username=\"admin\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\", qop=auth, nc=00000003, cnonce=\"dcdc1919a7f43849\"",
                         checkData->realm,checkData->nonce,uri,response);	

		//printf("%s",Authorization);
		
		http_header = curl_slist_append(http_header,Authorization);
	}

	curl_easy_setopt(curl,CURLOPT_HTTPHEADER,http_header);
 
	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		switch(res)
		{
			case CURLE_COULDNT_RESOLVE_HOST:
				SYSLOG_DBG("acq curl error : could not resolve host\n");break;
			case CURLE_UNSUPPORTED_PROTOCOL:
				SYSLOG_DBG("acq curl error : unsupported protocol\n");break;
			case CURLE_COULDNT_CONNECT:
				SYSLOG_DBG("acq curl error : could not connect\n");break;
			case CURLE_HTTP_RETURNED_ERROR:
				SYSLOG_DBG("acq curl error : HTTP return error\n");break;
			case CURLE_READ_ERROR:
				printf("acq curl error : read error\n");break;
			default:
				SYSLOG_DBG("acq curl error : unknow error return:%d\n",res);break;
		}
		ret=-1;
	}

	res=curl_easy_getinfo(curl,CURLINFO_HTTP_CODE,&httpCode);
	curl_slist_free_all(http_header);
	curl_easy_cleanup(curl);

	memcpy(respdata,callbackData.data,callbackData.len);
	*resplen=callbackData.len;

	SYSLOG_DBG("respdata len=%d: %s\n",callbackData.len,callbackData.data);

	return ret;
}


static int http_get_respond_data(struct curl_opt *opt,char *respdata,int *resplen)
{
    struct check_data checkData;
	int ret=0;

    if(!opt || !respdata ||!resplen) return -1100;
	
    memset(&checkData,0,sizeof(struct check_data));
    ret=curl_certification(&checkData,opt,respdata);
    if(ret==0)
    {
           ret=curl_info(&checkData,opt,respdata,resplen);
           //printf("curl_info:%s\n",respdata);
           if(ret!=0) ret=-1200+ret;
    }
    else
    {
          //printf("curl certification error\n");
		  ret=-1100+ret;
    }	

	return ret;
}

char* strnstr(const char*s1,const char*s2,int cmplen)
{//cmplen from ¡¡s1[0] to s1[cmplen-1] 
     const char *p=s1;
     size_t len = 0;
	 int index=0;

	 if(!s1 || !s2 || cmplen==0) return NULL;
	 len= strlen(s2);
	 
     for(;(p=strchr(p,*s2))!=0;p++)
     {
       if(strncmp(p,s2,len)==0)
       {
          index=p-s1;
		  if(index>cmplen) return NULL;
       	  return (char*)p;
       }
     }
     return (0);
}

char *strgetstr(const char *s1,const char *s2)
{
    char *pstr=NULL;
	pstr=strstr(s1,s2);
	if(!pstr) return NULL;
	pstr=strnstr(pstr,":",20);
	if(!pstr) return NULL;
	pstr=strnstr(pstr,"\"",20);
    if(!pstr) return NULL;
	
	return &pstr[1];
}

int strgetint(const char *s1,const char *s2)
{
    char *pstr=NULL;
	pstr=strstr(s1,s2);
	if(!pstr) return NULL;
	pstr=strnstr(pstr,":",20);
	if(!pstr) return NULL;
	
	return atoi(&pstr[1]);
}

static char *is_exist_minor_75_1(struct door_open_info *doorinfo,char *respdata,int resplen)
{
    char *pstr=NULL;
	int ret=0;
	int i=0;
	
	pstr=strstr(respdata,"\"minor\":");
		
	for(i=0;i<30 && pstr!=NULL;i++)
	{// find "minor":75,  and "minor":1,

        if(strnstr(pstr,"75,",30)!=NULL)
        {
        	doorinfo->style=1;// get face recognition
        	return pstr;//exist
        }
		else if(strnstr(pstr,"1,",30)!=NULL)
		{
			doorinfo->style=2;//get ic car recognition
			return pstr; //exist
		}
	
		pstr=strstr(&pstr[1],"\"minor\":");
	}	


	return NULL; // non-existence;
}

static int http_parse_event_respond_data(struct door_open_info *doorinfo,char *respdata,int resplen)
{
        char *pstr=NULL;
		char *pstr2=NULL;
		int ret=0;
		//int i=0;
		//char buff[100]={0};

		if(!doorinfo || !respdata || resplen<=0) return -1;

		if((pstr=strstr(respdata,"\"minor\":"))!=NULL)
		{
			pstr=is_exist_minor_75_1(doorinfo,respdata,resplen);
			if(pstr==NULL)
			{//non-existence and return			
				if((pstr=strstr(respdata,"\"responseStatusStrg\":"))!=NULL )
				{
					ret=0;
					return 1; // ok no more respond data           	
				}

				return 3;
			}
		}
		else if((pstr=strstr(respdata,"\"responseStatusStrg\":"))!=NULL )
		{
			ret=0;
			return 1; // ok no more respond data
		}
		else
		{
			//get more respond data 
			return 2; //maybe password error make door monitor locked
			//continue;
		}
		

		pstr2=strgetstr(pstr,"\"time\"");
		if(pstr2)
		ret=sscanf(pstr2,"%d-%d-%dT%d:%d:%d+08:00",
			             &doorinfo->time[5],&doorinfo->time[4],&doorinfo->time[3],
			             &doorinfo->time[2],&doorinfo->time[1],&doorinfo->time[0]);

        if(doorinfo->style==1)
        {
			pstr2=strgetstr(pstr,"\"name\"");
            if(pstr2) ret=sscanf(pstr2,"%[^\"]\",",doorinfo->name);
			
        	pstr2=strgetstr(pstr,"\"employeeNoString\"");
			if(pstr2)ret=sscanf(pstr2,"%[^\"]\",",doorinfo->code);
        }
		if(doorinfo->style==2)
		{
			pstr2=strgetstr(pstr,"\"cardNo\"");
			if(pstr2) ret=sscanf(pstr2,"%[^\"]\",",doorinfo->code);

			pstr2=strgetstr(pstr,"\"name\"");
			if(pstr2) ret=sscanf(pstr2,"%[^\"]\",",doorinfo->name);	
		}

		return 0; // ok get one door info
}

int getAcsEvent(char *ipaddr,int port,char *uname,char *passwd,struct door_open_info *doorinfo)
{
	static char respdata[MAX_MEMORY]={0};
	int resplen=0;
	//char getEventUri[]="ISAPI/AccessControl/AcsEvent?format=json"; //error
	char getEventUri[]="/ISAPI/AccessControl/AcsEvent?format=json"; //can not put / befor ISAPI
    struct curl_opt opt;
	int sttm[TIME_ARRAY],endtm[TIME_ARRAY];
	int eventNum=0;
	int eventNumOnce=6;
	char *pstr=NULL;
	int ret=0;
	int i=0;

	if(!doorinfo || !ipaddr || port<1) return -1;


	//curl_global_init(CURL_GLOBAL_ALL); //if not set in init_all_http_operate(), we must set curl_global_init(CURL_GLOBAL_ALL)

	memset(&opt,0,sizeof(struct curl_opt));

	strcpy(opt.ipaddr,ipaddr);
	opt.port=port;
	strncpy(opt.uname,uname,40);
	strncpy(opt.passwd,passwd,40);

	sprintf(opt.url,"http://%s:%d%s",ipaddr,port,getEventUri);
	strcpy(opt.uri,getEventUri);
	strcpy(opt.method,"POST");
	opt.timeout=5; // 5 seconds timout for curl

   	read_system_time(endtm);
	//time_decrease(endtm,sttm,1800);// half an hour , 1800 seconds
	//time_decrease(endtm,sttm,120);// half an hour , 120 seconds
	//time_decrease(endtm,sttm,300);// half an hour , 120 seconds
	time_decrease(endtm,sttm,60);
		
	for(eventNum=0;eventNum<3;eventNum+=eventNumOnce)
	{
		sprintf(opt.condstr,"{\"AcsEventCond\":{\"searchID\":\"T36\",\"searchResultPosition\":%d,\"maxResults\":%d,\"major\":5,\"minor\":0,\"startTime\":\"%04d-%02d-%02dT%02d:%02d:%02d+08:00\",\"endTime\":\"%04d-%02d-%02dT%02d:%02d:%02d+08:00\"}}",
			eventNum,eventNumOnce,sttm[5],sttm[4],sttm[3],sttm[2],sttm[1],sttm[0],
			endtm[5],endtm[4],endtm[3],endtm[2],endtm[1],endtm[0]);	
	
		memset(respdata,0,MAX_MEMORY);
		ret=http_get_respond_data(&opt,respdata,&resplen);
        if(ret!=0) 
		{
			SYSLOG_DBG("http_get_respond_data error ret=%d\n",ret);
			break;
        }

if(DEBUG_PRINT_VALUE)
{
		for(i=0;i<resplen;i+=200)
		{
		    char buff[300]={0};
			memset(buff,0,sizeof(buff));
			strncpy(buff,&respdata[i],202);
			//printf("gtnt:%d:%d>%s\n",eventNum,i,buff);
			SYSLOG_DBG("gtnt:%d:%d>%s\n",eventNum,i,buff);
			write_system_error_log(ACQ_SYSLOG_LOG_TYPE,"gtnt:%d:%d>%s\n",eventNum,i,buff);
		}
}
 		if(strstr(respdata,"Unauthorized") &&
			strstr(respdata,"401"))
		{
			ret=401;
			break;
		}      

		ret=http_parse_event_respond_data(doorinfo,respdata,resplen);
		if(ret==0 || ret==1)
		{	
			break;
		}
		sleep(1);
	}

    //curl_global_cleanup(); // curl_global_init(CURL_GLOBAL_ALL)
	return ret;
}


static int http_parse_door_status_respond_data(int *door_status,char *respdata,int resplen)
{
/*
{
        "AcsWorkStatus":        {
                "doorLockStatus":       [0],
                "doorStatus":   [4],
                "magneticStatus":       [1],
                "powerSupplyStatus":    "ACPowerSupply",
                "multiDoorInterlockStatus":     "close",
                "antiSneakStatus":      "close",
                "hostAntiDismantleStatus":      "close",
                "cardReaderOnlineStatus":       [1],
                "cardReaderAntiDismantleStatus":        [],
                "cardReaderVerifyMode": [25, 3],
                "cardNum":      1
        }
}
*/

        char *pstr=NULL;
		char *pstr2=NULL;
		int ret=0;
		int door_stat=0;


		if(!door_status || !respdata || resplen<=0) return -1;

		if((pstr=strstr(respdata,"magneticStatus"))!=NULL)
		{
		     pstr2=strstr(pstr,"[");
			 if(pstr2!=NULL)
			 {
			 	ret=sscanf(pstr2,"[%d],",&door_stat);
				if(ret!=1) return -1;

				*door_status=door_stat;
			 }
		}

		return 0;
}

int getDoorStatus(char *ipaddr,int port,char *uname,char *passwd,int *door_status)
{

	static char respdata[MAX_MEMORY]={0};
	int resplen=0;
	//char getEventUri[]="ISAPI/AccessControl/AcsEvent?format=json"; //error
	char getEventUri[]="/ISAPI/AccessControl/AcsWorkStatus?format=json"; //can not put / befor ISAPI
    struct curl_opt opt;	
	int ret=0;

	if(!door_status || !ipaddr || port<1) return -1;


	//curl_global_init(CURL_GLOBAL_ALL); //if not set in init_all_http_operate(), we must set curl_global_init(CURL_GLOBAL_ALL)

	memset(&opt,0,sizeof(struct curl_opt));

	strcpy(opt.ipaddr,ipaddr);
	opt.port=port;
	strncpy(opt.uname,uname,40);
	strncpy(opt.passwd,passwd,40);

	sprintf(opt.url,"http://%s:%d%s",ipaddr,port,getEventUri);
	strcpy(opt.uri,getEventUri);
	strcpy(opt.method,"GET");
	opt.timeout=5; // 5 seconds timout for curl

	memset(respdata,0,MAX_MEMORY);
	ret=http_get_respond_data(&opt,respdata,&resplen);
    if(ret!=0) 
	{
		SYSLOG_DBG("http_get_respond_data error ret=%d\n",ret);
		return -2;
    }

	SYSLOG_DBG("getDoorStatus:%s\n",respdata);

	ret=http_parse_door_status_respond_data(door_status,respdata,resplen);
	if(ret!=0)
	{
		SYSLOG_DBG("parse door status err ret=%d\n",ret);
	}


	return ret;
	
}

/*
void init_door_info()
{
	curl_global_init(CURL_GLOBAL_ALL);
}
*/
