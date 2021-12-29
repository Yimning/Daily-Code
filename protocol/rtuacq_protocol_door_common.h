
#ifndef RTUACQ_PROTOCOL_DOOR_COMMON
#define RTUACQ_PROTOCOL_DOOR_COMMON

#define MAX_MEMORY (1024*10)

struct check_data{
	char nonce[50];
	char realm[20];
};
/*
struct user{
	int id;
	char name[20];
};

struct userInfos{
	int userNum;
	struct user userInfo[50];
};
*/
struct callback_data_struct{
	int len;
	char data[MAX_MEMORY];
};

/*
struct eventInfo{
	int openDoorType;
	int id;
};
*/

struct curl_opt{
	char ipaddr[48];
	int port;
	char uname[48];
	char passwd[48];
	char url[500];
	char uri[500];
	char condstr[200];	
	char method[20];
	long timeout;	
	
};


struct door_open_info{//rtuacquisitoin.h  struct acq_door_open_info NOTE##
/*
    {"i3310B","0.000","站房开关"},
    {"i3310B","1.000","人脸识别"},
    {"i3310B","2.000","IC卡"},
    {"i3310B","3.000","指纹"},
    {"i3310B","4.000","远程控制"},
*/
	int style; // minor
	char code[50]; // employeeNoString for face, cardNo for id card
	int time[7];// time
	char name[50]; // chinese name
	int door_stat; // door status, 1:open ,0:close
};


extern int getAcsEvent(char *ipaddr,int port,char *uname,char *passwd,struct door_open_info *doorinfo);
extern int getDoorStatus(char *ipaddr,int port,char *uname,char *passwd,int *door_status);

#endif

