#ifndef __COMMON_PRO__
#define __COMMON_PRO__

#include "rtu_log_common.h"

extern int DEBUG_PRINT_VALUE;

union	uf
{
        float f;
        char ch[4];
        unsigned int i[2];
        unsigned long l;
	 unsigned short s;
};

union   uf64
{
        float f;
        char ch[8];
        unsigned int i[2];
        unsigned long long l;
        double d;
};

union char2int
{
	char ch[2];
	unsigned int i;
};

#define VOID_ADDR(x) ((void *)(&(x)))

struct hebei_dynamic_info_water{
	float i12101;
	float i12102;
	float i12103;

	float i13007;
	float i13103;
	float i13104;
	float i13105;
	float i13106;
	float i13108;
	float i13109;
	float i13110;
	float i13111;
	float i13112;
	float i13113;
	float i13003;
	float i13115;
	float i13116;
	float i13008;
	float i13118;
	float i13119;
	float i13120;
	float i13005;
	float i13004;
	float i13123;
	float i13124;
	float i13125;
	float i13126;
	float i13127;
	float i13129;
	float i13130;

	time_t i13101;
	time_t i13107;
	time_t i13128;

	int status;
};

struct hebei_dynamic_info_gas{
	float i12107;
	float i12108;
	float i12109;
	float i12104;
	float i12105;
	float i12106;

	float i13022;
	float i13023;
	float i13024;
	float i13025;
	float i13026;
	float i13028;
	float i13029;
	float i13010;
	float i13011;
	float i13012;
	float i13013;
	float i13014;
	float i13015;
	float i13016;
	float i13017;
	float i13018;
	float i13019;
	float i13020;

	time_t i13021;
	time_t i13027;

	int status;
};


extern int DEBUG_PRINT_VALUE;

//calculate CRC correct value
static const char aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
    0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
    0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
    0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
    0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81,
    0x40
};

static const char aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB,
    0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE,
    0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2,
    0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E,
    0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B,
    0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27,
    0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD,
    0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8,
    0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4,
    0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94,
    0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59,
    0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D,
    0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80,
    0x40
};

/*******************************************************************************
calcaute modbus CRC correct value
*******************************************************************************/
static unsigned cal_crc( unsigned char * pucFrame, unsigned char usLen )
{
    unsigned char           ucCRCHi;
    unsigned char           ucCRCLo;
    unsigned             iIndex;

    ucCRCHi = 0xFF;
    ucCRCLo = 0xFF;

    if( !pucFrame || usLen<0) return 0;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( unsigned char )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( unsigned )( ucCRCHi << 8 | ucCRCLo );
}

static unsigned CRC16_modbus(unsigned char* buf, int len)
{
    return cal_crc(buf,len);
}

static char* memstr(char* buffer, int buff_len, char* substr)
{
    int sublen,bufflen;
	int i=0;

    if (buffer == NULL || buff_len <= 0 || substr == NULL) {
        return NULL;
    }

    if (*substr == '\0') {
        return NULL;
    }

   	sublen = strlen(substr);
    bufflen = buff_len-sublen + 1;
	
    for (i = 0; i < bufflen; i++) 
	{
        if (buffer[i] == substr[0] && 
			!memcmp(&buffer[i], substr, sublen)) 
                return &buffer[i];
    }

    return NULL;
}


static char* memstrlen(char* buffer, int buff_len, char* substr,int sublen)
{
    int bufflen;
	int i=0;

    if (buffer == NULL || buff_len <= 0 || substr == NULL) {
        return NULL;
    }

    if (*substr == '\0') {
        return NULL;
    }

   if(sublen<5) return NULL;
   if(buff_len<sublen) return NULL;
	
    for (i = 0; i < (buff_len-sublen+1); i++) 
{
	        if (buffer[i] == substr[0]  &&
			buffer[i+1] == substr[1]  &&
			buffer[i+2] == substr[2]  &&
			buffer[i+3] == substr[3]  &&
			buffer[i+4] == substr[4] 
			) 
	        {
/*	        
	                int others=0;
			int j=0;
			
			for(j=5;j<sublen;j++)
			{
				if(buffer[i+j] != substr[j])  break;
			}
*/			
	                return &buffer[i];
	        }
    }

    return NULL;
}


#if 0
static void print_hex(char *data,int len)
{
   int i=0,j=0;

   if(!data) return ;

   for(i=0;i<(len/10);i++)
   {
	printf("%d>",i);
	for(j=0;j<10;j++)
		printf("%.2x ",data[i*10+j]&0xff);
	printf("\n");
   }

   if(len%10!=0)
   {
	printf("%d>",i);
	for(j=i*10;j<len;j++)
		printf("%.2x ",data[j]&0xff);
	printf("\n");
	return ;
   }
}
#endif

typedef int (*PROTOCOLS_SEL_T)(struct acquisition_data *acq_data,int devaddr,void *data);
int instrument_protocols_sel(PROTOCOLS_SEL_T func,struct acquisition_data *acq_data,unsigned int devaddr,void *data,int runId,int funcId);

typedef int (*PROTOCOL_T)(struct acquisition_data *acq_data);
struct k37_instrument_protocols{
    const int const instrument_code;
    const PROTOCOL_T protocol;
};

struct k37_dev_instrument{
    char dev_name[NAME_LEN];
    int instrument_code;
};

//int modbus_get_valf(char *com_rbuf,unsigned int size,unsigned int regaddr,MODBUS_DATA_TYPE datatype,float *valf);
//struct modbus_polcode_arg * find_modbus_polcode_arg_by_polcode(struct modbus_polcode_arg *mpolcodearg,int num,char *polcode);


int getDataPack(char *dev_name, char *tbuf, int tbuf_size, char *rbuf, int rbuf_max_size, char *text, int wait_time, int tbuf_hex_or_str, int rbuf_hex_or_str);
int getHexDataPack(char *dev_name, char *tbuf, int tbuf_size, char *rbuf, int rbuf_max_size, char *text, int wait_time);
int getStrDataPack(char *dev_name, char *tbuf, int tbuf_size, char *rbuf, int rbuf_max_size, char *text, int wait_time);

float getFloatValue(char *buff,int offset, MODBUS_DATA_TYPE dataType);
time_t getTimeValue(int year, int mon, int day, int hour ,int min, int sec);
int32_t getInt32Value(char *buff,int offset, MODBUS_DATA_TYPE dataType);
int16_t getInt16Value(char *buff,int offset, MODBUS_DATA_TYPE dataType);
unsigned short getUInt16Value(char *buff,int offset, MODBUS_DATA_TYPE dataType);

int  get_cmd_03_04_pos(int startaddr,int addr);
int  get_cmd_10_pos(int startaddr,int addr);

char * modbus_crc_check(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt);
char * modbus_crc_check_coil(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt);
char * modbus_crc_check_write(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int reg,unsigned int cnt);
char * modbus_tcp_crc_check(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt);
char * modbus_tcp_crc_check_coil(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt);
char *modbus_ascii_lrc_check(char *buff,int buff_size, unsigned int devaddr, unsigned int cmd, unsigned int cnt);
int modbus_crc_check_ok(char *data, int data_len, unsigned int devaddr, unsigned int cmd, unsigned int cnt);
float NO_to_NOx(float no);
float NO_and_NO2_to_NOx(float no, float no2);
float gas_ppm_to_mg_m3(float ppm, int mr);
float gas_ppb_to_mg_m3(float ppb, int mr);
float gas_ppm_to_pecent(float ppm);
float water_US_gal_to_L(float usgal);
float water_US_gal_to_m3(float usgal);
float water_UK_gal_to_L(float usgal);
float water_UK_gal_to_m3(float usgal);
float PLCtoValue(struct modbus_arg *modbusarg, int minVal, int maxVal, float val, char *polcode);
int isPolcodeEnable(struct modbus_arg *modbusarg, char *polcode);
int acqdataIsPolcodeEnable(struct acquisition_data *acq_data, char *polcode);
char* memstrhex(char* buffer, int buff_len, char* subhex, int hex_len);

unsigned char cal_bcc(unsigned char *buf, int len);
unsigned int SUM_CHECK(char *buff, int size);
unsigned int SUM_CHECK_HEX(char *buff, int size);
unsigned int hex2ascii(char hex);
extern char hex2dec(char c);
extern int HexCharToDec(char c_num);
extern char StrHexToHex(char c);
extern int modbusAsciiPacket(char modpack[17],char addr,char cmd,int reg,int cnt);
extern int modbus_pack_tcp(char *modpack,char addr, char cmd, int reg, int cnt);
extern int SongXia_read_pack(char *buf, unsigned int devaddr, unsigned int reg, unsigned int cnt);
PROTOCOL_T lookup_protocol(int instrument_code);
PROTOCOL_T lookup_protocol_info(int instrument_code,int index);
extern int acqdata_set_value(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,int *arg_n);
extern int acqdata_set_value_flag(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,char flag,int *arg_n);
extern int acqdata_set_value_orig(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,float orig_val,int *arg_n);
extern int acqdata_set_rtdata(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float value,char flag,int *arg_n);
extern int acqdata_set_stattdata(struct acquisition_data *acq_data,
	char *polcode,UNIT_DECLARATION unit,float avg,float min,float max,float cou,char flag,int *arg_n);
 

int protocol_HeBei(struct acquisition_data *acq_data,char *polcode2005HB[100],int polcode2005HB_len,
	                       char * polcode2017HB[100],int polcode2017HB_len,UNIT_DECLARATION units[100],int units_len);

int protocol_XinJiang(struct acquisition_data *acq_data,char *polcode05[],char *polcode17[],UNIT_DECLARATION *units,int polcode_num);

int protocol_FLUX_jiubo_LS(struct acquisition_data *acq);
int protocol_FLUX_jiubo_M3H(struct acquisition_data *acq);
int protocol_FLUX_QDHuanKe(struct acquisition_data *acq_data);
int protocol_FLUX_WXqiuxin(struct acquisition_data *acq);
int protocol_FLUX_yiwen_LS(struct acquisition_data *acq_data);
int protocol_FLUX_KeSheng(struct acquisition_data *acq_data);
int protocol_FLUX_DLXiGeMa(struct acquisition_data *acq_data);
int protocol_FLUX_CQHuaZhengShuiWen(struct acquisition_data *acq_data);
int protocol_FLUX_GZDongWen(struct acquisition_data *acq_data);
int protocol_FLUX_GZHuiGu(struct acquisition_data *acq_data);
int protocol_FLUX_BJhuanke(struct acquisition_data *acq_data);
int protocol_FLUX_XiAnSanQuan(struct acquisition_data *acq_data);
int protocol_FLUX_DLBoKeSi(struct acquisition_data *acq_data);
int protocol_FLUX_DLfulang(struct acquisition_data *acq_data);
int protocol_FLUX_Weike(struct acquisition_data *acq_data);
int protocol_FLUX_DLfulang(struct acquisition_data *acq_data);
int protocol_FLUX_HZNanKong(struct acquisition_data *acq_data);
int protocol_FLUX_Lmag(struct acquisition_data * acq_data);
int protocol_FLUX_WHZhongHe(struct acquisition_data *acq_data);
int protocol_FLUX_SHYingShenMingQu(struct acquisition_data *acq_data);
int protocol_FLUX_SHYingShen(struct acquisition_data *acq_data);
int protocol_FLUX_JinYuan(struct acquisition_data *acq_data);
int protocol_FLUX_HeBeiErDai(struct acquisition_data *acq_data);
int protocol_FLUX_KeLongIFC050(struct acquisition_data *acq_data);
int protocol_FLUX_KeLongIFC050_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_FLUX_JMHuangYu(struct acquisition_data *acq_data);
int protocol_FLUX_WeiChuang(struct acquisition_data *acq_data);
int protocol_FLUX_XinJiang(struct acquisition_data *acq_data);
int protocol_FLUX_GuoBiao212(struct acquisition_data *acq_data);
int protocol_FLUX_DLFuLangX2(struct acquisition_data *acq_data);
int protocol_FLUX_AnJun(struct acquisition_data *acq_data);
int protocol_FLUX_DongNan(struct acquisition_data *acq_data);
int protocol_FLUX_GuangHua(struct acquisition_data *acq_data);
int protocol_FLUX_JinXi(struct acquisition_data *acq_data);
int protocol_FLUX_KenTe(struct acquisition_data *acq_data);
int protocol_FLUX_VOTEC_VT2000(struct acquisition_data *acq_data);
int protocol_FLUX_HBKeruida(struct acquisition_data *acq_data);
int protocol_FLUX_DuoPuLe(struct acquisition_data *acq_data);
int protocol_FLUX_NeiMengGu(struct acquisition_data * acq_data);
int protocol_FLUX_HuBei(struct acquisition_data *acq_data);
int protocol_FLUX_MiKe(struct acquisition_data * acq_data);
int protocol_FLUX_MiKe2(struct acquisition_data * acq_data);
int protocol_FLUX_JiuLian_in(struct acquisition_data * acq_data);
int protocol_FLUX_JiuLian_out(struct acquisition_data * acq_data);
int protocol_FLUX_JiuLian_hui(struct acquisition_data * acq_data);
int protocol_FLUX_JSJinBiao_in(struct acquisition_data * acq_data);
int protocol_FLUX_JSJinBiao_out(struct acquisition_data * acq_data);
int protocol_FLUX_JSJinBiao_hui(struct acquisition_data * acq_data);
int protocol_FLUX_JSJinBiao_shenghuo(struct acquisition_data * acq_data);
int protocol_FLUX_Lmag_in(struct acquisition_data * acq_data);
int protocol_FLUX_Lmag_out(struct acquisition_data * acq_data);
int protocol_FLUX_Lmag_handle(struct acquisition_data * acq_data);
int protocol_FLUX_Lmag_huiChanSheng(struct acquisition_data * acq_data);
int protocol_FLUX_Lmag_huiShiYong(struct acquisition_data * acq_data);
int protocol_FLUX_CN311(struct acquisition_data * acq_data);
int protocol_FLUX_CN311_old(struct acquisition_data * acq_data);
int protocol_FLUX_dongguan(struct acquisition_data * acq_data);
int protocol_FLUX_ZhuoZheng(struct acquisition_data * acq_data);
int protocol_FLUX_KFJinXiu(struct acquisition_data * acq_data);
int protocol_FLUX_ZhuoZheng2(struct acquisition_data * acq_data);
int protocol_FLUX_WoLun(struct acquisition_data *acq_data);
int protocol_FLUX_NJShuiMengDianZi_SM10(struct acquisition_data *acq_data);
int protocol_FLUX_HZMeiKong(struct acquisition_data *acq_data);
int protocol_FLUX_HuaXun(struct acquisition_data *acq_data);
int protocol_FLUX_SHFanYang(struct acquisition_data *acq_data);
int protocol_FLUX_ZhuoZheng3(struct acquisition_data *acq_data);
int protocol_FLUX_ZhuoZheng4(struct acquisition_data *acq_data);
int protocol_FLUX_TaiChuang_CT5201(struct acquisition_data *acq_data);
int protocol_FLUX_dongguan2(struct acquisition_data *acq_data);
int protocol_FLUX_PD_880B(struct acquisition_data *acq_data);
int protocol_FLUX_LiDe(struct acquisition_data *acq_data);
int protocol_FLUX_BJRuiPuSanYuan_ULFM_A(struct acquisition_data *acq_data);
int protocol_FLUX_HBHuaHou(struct acquisition_data *acq_data);
int protocol_FLUX_TUF2000(struct acquisition_data *acq_data);
int protocol_FLUX_BuLuKeSi(struct acquisition_data *acq_data);
int protocol_FLUX_CangZhouTianYi(struct acquisition_data *acq_data);
int protocol_FLUX_DLBoKeSi_104M(struct acquisition_data *acq_data);
int protocol_FLUX_TDS100(struct acquisition_data *acq_data);
int protocol_FLUX_SHGuangHua(struct acquisition_data *acq_data);
int protocol_FLUX_MBmag(struct acquisition_data *acq_data);
int protocol_FLUX_HBTianYun(struct acquisition_data *acq_data);

int protocol_COD_LianHua5B5(struct acquisition_data *acq_data);
int protocol_COD_SHAiHua(struct acquisition_data *acq_data);
int protocol_COD_HNQianZhengJHC(struct acquisition_data *acq_data);
int protocol_COD_WXChuangChen(struct acquisition_data *acq_data);
int protocol_COD_NJGangNeng(struct acquisition_data *acq_data);
int protocol_COD_BJYongXing(struct acquisition_data *acq_data);
int protocol_COD_SZZhongXing(struct acquisition_data *acq_data);
int protocol_COD_SongXiaYi(struct acquisition_data *acq_data);
int protocol_COD_SongXiaEr(struct acquisition_data *acq_data);
int protocol_COD_SongXiaSan(struct acquisition_data *acq_data);
int protocol_COD_SongXiaSi(struct acquisition_data *acq_data);
int protocol_COD_XiSiDi(struct acquisition_data *acq_data);
int protocol_COD_JSTianRui2(struct acquisition_data *acq_data);
int protocol_COD_JSTianRui1(struct acquisition_data *acq_data);
int protocol_COD_GermanWTW3520(struct acquisition_data *acq_data);
int protocol_COD_SZLangShi(struct acquisition_data *acq_data);
int protocol_COD_ZJHZWeiLan(struct acquisition_data *acq_data);
int protocol_COD_HBWHtkc(struct acquisition_data *acq_data);
int protocol_COD_ZJHZWeiLanD01(struct acquisition_data *acq_data);
int protocol_COD_GXGLYunJin(struct acquisition_data *acq_data);
int protocol_COD_NeiMengGu(struct acquisition_data * acq_data);			/*yuiki 20191228*/
int protocol_COD_CZShengLiHong(struct acquisition_data * acq_data);
int protocol_COD_NJGangNeng_HeBei(struct acquisition_data *acq_data);
int protocol_COD_GBhjt2005(struct acquisition_data * acq_data);
int protocol_COD_GBhjt2017(struct acquisition_data * acq_data);
int protocol_COD_HNHuaShiJie(struct acquisition_data *acq_data);
int protocol_COD_HNHuaShiJieModbus(struct acquisition_data *acq_data);
int protocol_COD_SZYuXing1(struct acquisition_data *acq_data);
int protocol_COD_JuGuangKeJi(struct acquisition_data *acq_data);
int protocol_COD_USAhaxi(struct acquisition_data *acq_data);
int protocol_COD_GZyiwen(struct acquisition_data * acq_data);
int protocol_COD_GZyiwenDynamic(struct acquisition_data *acq_data);
int protocol_COD_BJhuanke(struct acquisition_data * acq_data);
int protocol_COD_NJhongkai(struct acquisition_data *acq_data);
int protocol_COD_NJhongkaiModbus(struct acquisition_data *acq_data);
int protocol_COD_NJruiquan(struct acquisition_data *acq_data);
int protocol_COD_NJRuiQuanModbus(struct acquisition_data *acq_data);
int protocol_COD_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_COD_NJdelin(struct acquisition_data *acq_data);
int protocol_COD_WXdianchuang(struct acquisition_data *acq_data);
int protocol_COD_SZkete(struct acquisition_data *acq_data);
int protocol_COD_BJhoutian(struct acquisition_data *acq_data);
int protocol_COD_HZfuming(struct acquisition_data *acq_data);
int protocol_COD_GZyingsi(struct acquisition_data *acq_data);
int protocol_COD_Certificate(struct acquisition_data *acq_data);
int protocol_COD_ShiHuiLin(struct acquisition_data *acq_data);
int protocol_COD_ZhuoZheng2(struct acquisition_data *acq_data);
int protocol_COD_HeBei(struct acquisition_data *acq_data);
int protocol_COD_DaoJin4100CN(struct acquisition_data *acq_data);
int protocol_COD_DaoJin4200CN(struct acquisition_data *acq_data);
int protocol_COD_JSDeLin(struct acquisition_data * acq_data);
int protocol_COD_HBKeruida(struct acquisition_data *acq_data);
int protocol_COD_WHSiFuMing(struct acquisition_data * acq_data);	/*yuiki 20191206*/
int protocol_COD_ChuangZao(struct acquisition_data * acq_data);		/*yuiki 20191220*/
int protocol_COD_USAhaxiDynamic(struct acquisition_data * acq_data);
int protocol_COD_LvYe(struct acquisition_data * acq_data);				/*yuiki 20191220*/
int protocol_COD_CE(struct acquisition_data * acq_data);
int protocol_COD_HuiHuang(struct acquisition_data * acq_data);
int protocol_COD_SZZhongXingModbus(struct acquisition_data * acq_data);
int protocol_COD_WHJunDe(struct acquisition_data * acq_data);
int protocol_COD_AmtaxInter2C(struct acquisition_data * acq_data);
int protocol_COD_GZYiWenModbus(struct acquisition_data *acq_data);
int protocol_COD_NJHongGuang(struct acquisition_data *acq_data);
int protocol_COD_HBHuaHouTianCheng(struct acquisition_data *acq_data);
int protocol_COD_ZeTian(struct acquisition_data *acq_data);
int protocol_COD_JuGuang_HeBei(struct acquisition_data *acq_data);
int protocol_COD_HZLuHeng_HeBei(struct acquisition_data *acq_data);
int protocol_COD_Thermo_orion3106(struct acquisition_data *acq_data);
int protocol_COD_GLYunJing_HeBei(struct acquisition_data *acq_data);
int protocol_COD_DaoJin_HeBei(struct acquisition_data *acq_data);
int protocol_COD_GDWeiChuang_HeBei(struct acquisition_data *acq_data);
int protocol_COD_JSLingHeng(struct acquisition_data *acq_data);
int protocol_COD_NJJuGe(struct acquisition_data *acq_data);
int protocol_COD_USAHaXi_maxII(struct acquisition_data *acq_data);
int protocol_COD_GZYiWen_HeBei(struct acquisition_data *acq_data);
int protocol_COD_BoKeSi(struct acquisition_data *acq_data);
int protocol_COD_AHWanYi(struct acquisition_data *acq_data);
int protocol_COD_BJHuanKe_HeBei(struct acquisition_data *acq_data);
int protocol_COD_HBHuaHouTianCheng_new(struct acquisition_data *acq_data);
int protocol_COD_ZhongLv(struct acquisition_data * acq_data);
int protocol_COD_LiDa(struct acquisition_data * acq_data);
int protocol_COD_YuXing_HeBei(struct acquisition_data *acq_data);
int protocol_COD_NJXinRuiPeng(struct acquisition_data *acq_data);
int protocol_COD_QDJiaMing(struct acquisition_data *acq_data);
int protocol_COD_JSZhuoZheng_HeBei(struct acquisition_data *acq_data);
int protocol_COD_USAHaXi_maxII_New(struct acquisition_data *acq_data);
int protocol_COD_ZeTian_GBhjt2005(struct acquisition_data *acq_data);
int protocol_COD_SZLangShi_PhotoTek6000(struct acquisition_data *acq_data);
int protocol_COD_GJSurfaceWater(struct acquisition_data *acq_data);
int protocol_COD_ZhengQi_Modbus(struct acquisition_data *acq_data);
int protocol_COD_ZhongJieNengTianRong(struct acquisition_data *acq_data);
int protocol_COD_JSTianZe(struct acquisition_data *acq_data);
int protocol_COD_NJHongGuang2(struct acquisition_data *acq_data);
int protocol_COD_JSRuiQuanRenQ_IV(struct acquisition_data *acq_data);
int protocol_COD_ZJHZLiQi_V2 (struct acquisition_data *acq_data);
int protocol_COD_HuiHuan_New(struct acquisition_data * acq_data);
int protocol_COD_NJJuGe_New(struct acquisition_data *acq_data);
int protocol_COD_EndressHauser(struct acquisition_data *acq_data);
int protocol_COD_EndressHauser_CA80(struct acquisition_data *acq_data);
int protocol_COD_HeBeiHuaHouTianCheng_E(struct acquisition_data *acq_data);
int protocol_COD_BJLiDa(struct acquisition_data *acq_data);
int protocol_COD_GLYunJing_HeBei_Stay(struct acquisition_data *acq_data);
int protocol_COD_NJXinRuiPeng_New (struct acquisition_data *acq_data);
	
int protocol_ANDAN_USAhaxi(struct acquisition_data *acq_data);
int protocol_ANDAN_GZyiwen(struct acquisition_data *acq_data);
int protocol_ANDAN_HZfuming(struct acquisition_data *acq_data);
int protocol_ANDAN_GZyingsi(struct acquisition_data *acq_data);
int protocol_ANDAN_JuGuangKeJi(struct acquisition_data *acq_data);
int protocol_ANDAN_NJRuiQuan(struct acquisition_data *acq_data);
int protocol_ANDAN_HLTianCheng(struct acquisition_data *acq_data);
int protocol_ANDAN_SZZhongXing(struct acquisition_data *acq_data);
int protocol_ANDAN_ZJHZWeiLan(struct acquisition_data *acq_data);
int protocol_ANDAN_GXGLYunJin(struct acquisition_data *acq_data);
int protocol_ANDAN_NJRuiQuanModbus(struct acquisition_data *acq_data);
int protocol_ANDAN_NeiMengGu(struct acquisition_data * acq_data);	/*yuiki 20191228*/
int protocol_ANDAN_NJhongkai(struct acquisition_data *acq_data);/*yuiki 20191220*/
int protocol_ANDAN_JSDeLin(struct acquisition_data * acq_data);
int protocol_ANDAN_SongXiaYi(struct acquisition_data *acq_data);
int protocol_ANDAN_SongXiaEr(struct acquisition_data *acq_data);
int protocol_ANDAN_SongXiaSan(struct acquisition_data *acq_data);
int protocol_ANDAN_SongXiaSi(struct acquisition_data *acq_data);
int protocol_ANDAN_XiSiDi(struct acquisition_data *acq_data);
int protocol_ANDAN_JSTianRui2(struct acquisition_data *acq_data);
int protocol_ANDAN_JSTianRui1(struct acquisition_data *acq_data);
int protocol_ANDAN_GermanWTW(struct acquisition_data *acq_data);
int protocol_ANDAN_USAhaxi3001(struct acquisition_data *acq_data);
int protocol_ANDAN_USAhaxi3001(struct acquisition_data *acq_data);
int protocol_ANDAN_HNHuaShiJie(struct acquisition_data *acq_data);
int protocol_ANDAN_HNHuaShiJieModbus(struct acquisition_data *acq_data);
int protocol_ANDAN_CZShengLiHong(struct acquisition_data *acq_data);
int protocol_ANDAN_USAhaxiDynamic(struct acquisition_data * acq_data);
int protocol_ANDAN_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_ANDAN_ZhuoZheng2(struct acquisition_data *acq_data);
int protocol_ANDAN_LZLianHua(struct acquisition_data *acq_data);
int protocol_ANDAN_NJGangNeng(struct acquisition_data *acq_data);
int protocol_ANDAN_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_WHSiFuMing(struct acquisition_data *acq_data);
int protocol_ANDAN_ChuangZao(struct acquisition_data * acq_data);		/*yuiki 20191220*/
int protocol_ANDAN_LvYe(struct acquisition_data * acq_data);				/*yuiki 20200306*/
int protocol_ANDAN_GBhjt2005(struct acquisition_data * acq_data);
int protocol_ANDAN_GBhjt2017(struct acquisition_data * acq_data);
int protocol_ANDAN_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_ANDAN_CE(struct acquisition_data * acq_data);
int protocol_ANDAN_HuiHuang(struct acquisition_data * acq_data);
int protocol_ANDAN_SZZhongXingModbus(struct acquisition_data * acq_data);
int protocol_ANDAN_WHJunDe(struct acquisition_data * acq_data);
int protocol_ANDAN_SZPengYue(struct acquisition_data * acq_data);
int protocol_ANDAN_AmtaxInter2C(struct acquisition_data * acq_data);
int protocol_ANDAN_GZYiWenModbus(struct acquisition_data *acq_data);
int protocol_ANDAN_NA8000(struct acquisition_data *acq_data);
int protocol_ANDAN_USAhaxiNA8000(struct acquisition_data *acq_data);
int protocol_ANDAN_NJHongGuang(struct acquisition_data * acq_data);
int protocol_ANDAN_HBHuaHouTianCheng(struct acquisition_data *acq_data);
int protocol_ANDAN_ZJHZWeiLanD01(struct acquisition_data *acq_data);
int protocol_ANDAN_ZeTian(struct acquisition_data *acq_data);
int protocol_ANDAN_JuGuang_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_HZLuHeng_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_Thermo_orion2240(struct acquisition_data *acq_data);
int protocol_ANDAN_GLYunJing_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_DaoJin_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_GDWeiChuang_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_JSLingHeng(struct acquisition_data *acq_data);
int protocol_ANDAN_NJJuGe(struct acquisition_data *acq_data);
int protocol_ANDAN_GZYiWen_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_BoKeSi(struct acquisition_data *acq_data);
int protocol_ANDAN_AHWanYi(struct acquisition_data *acq_data);
int protocol_ANDAN_NJGangNeng_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_BJHuanKe_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_DaoJin_4210(struct acquisition_data *acq_data);
int protocol_ANDAN_HBHuaHouTianCheng_new(struct acquisition_data *acq_data);
int protocol_ANDAN_ZhongLv(struct acquisition_data * acq_data);
int protocol_ANDAN_LiDa(struct acquisition_data * acq_data);
int protocol_ANDAN_HBKeruida(struct acquisition_data *acq_data);
int protocol_ANDAN_YuXing_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_NJXinRuiPeng(struct acquisition_data *acq_data);
int protocol_ANDAN_BJSaiLaiMo(struct acquisition_data * acq_data);
int protocol_ANDAN_QDJiaMing(struct acquisition_data *acq_data);
int protocol_ANDAN_JSZhuoZheng_HeBei(struct acquisition_data *acq_data);
int protocol_ANDAN_USAHaXi_maxII_New(struct acquisition_data *acq_data);
int protocol_ANDAN_ZeTian_GBhjt2005(struct acquisition_data *acq_data);
int protocol_ANDAN_SZLangShi_PhotoTek6000(struct acquisition_data *acq_data);
int protocol_ANDAN_GJSurfaceWater(struct acquisition_data *acq_data);
int protocol_ANDAN_ZhengQi_Modbus(struct acquisition_data *acq_data);
int protocol_ANDAN_ZhongJieNengTianRong(struct acquisition_data *acq_data);
int protocol_ANDAN_JSTianZe(struct acquisition_data *acq_data);
int protocol_ANDAN_NJHongGuang2(struct acquisition_data * acq_data);
int protocol_ANDAN_JSRuiQuanRenQ_IV(struct acquisition_data *acq_data);
int protocol_ANDAN_ZJHZLiQi_V2 (struct acquisition_data *acq_data);
int protocol_ANDAN_NJJuGe_New(struct acquisition_data *acq_data);
int protocol_ANDAN_EndressHauser(struct acquisition_data *acq_data);
int protocol_ANDAN_EndressHauser_CA80(struct acquisition_data *acq_data);
int protocol_ANDAN_HeBeiHuaHouTianCheng_E(struct acquisition_data *acq_data);
int protocol_ANDAN_BJLiDa(struct acquisition_data *acq_data);
int protocol_ANDAN_GLYunJing_HeBei_Stay(struct acquisition_data *acq_data);
int protocol_ANDAN_NJXinRuiPeng_New (struct acquisition_data *acq_data);

int protocol_Lin_GZYiWenEST2003(struct acquisition_data *acq_data);
int protocol_Lin_HZFuMing(struct acquisition_data *acq_data);
int protocol_Lin_SXZhongLv(struct acquisition_data *acq_data);
int protocol_Lin_JSTianRui1(struct acquisition_data *acq_data);
int protocol_Lin_JSTianRui2(struct acquisition_data *acq_data);
int protocol_Lin_USAHaXi(struct acquisition_data *acq_data);
int protocol_Lin_N_SHJingFang(struct acquisition_data *acq_data);
int protocol_Lin_SongXiaYi(struct acquisition_data *acq_data);
int protocol_Lin_SongXiaEr(struct acquisition_data *acq_data);
int protocol_Lin_SongXiaSan(struct acquisition_data *acq_data);
int protocol_Lin_SongXiaSi(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZhuoZheng2(struct acquisition_data *acq_data);
int protocol_ZONGLIN_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_GXGLYunJin(struct acquisition_data *acq_data);
int protocol_ZONGLIN_NJRuiQuanModbus(struct acquisition_data *acq_data);
int protocol_ZONGLIN_HNHuaShiJieModbus(struct acquisition_data * acq_data);
int protocol_ZONGLIN_NeiMengGu(struct acquisition_data * acq_data);		/*yuiki 20191228*/
int protocol_ZONGLIN_QianDa(struct acquisition_data *acq_data);
int protocol_ZONGLIN_LvYe(struct acquisition_data * acq_data);				/*yuiki 20200306*/
int protocol_ZONGLIN_ChuangZao(struct acquisition_data * acq_data);
int protocol_ZONGLIN_GBhjt2005(struct acquisition_data * acq_data);
int protocol_ZONGLIN_GBhjt2017(struct acquisition_data * acq_data);
int protocol_ZONGLIN_NJhongkai(struct acquisition_data * acq_data);
int protocol_ZONGLIN_CZShengLiHong(struct acquisition_data * acq_data);
int protocol_ZONGLIN_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_ZONGLIN_CE(struct acquisition_data * acq_data);
int protocol_ZONGLIN_HuiHuang(struct acquisition_data * acq_data);
int protocol_ZONGLIN_SZZhongXingModbus(struct acquisition_data * acq_data);
int protocol_ZONGLIN_AmtaxInter2C(struct acquisition_data * acq_data);
int protocol_ZONGLIN_USAhaxiDynamic(struct acquisition_data * acq_data);
int protocol_ZONGLIN_GZYiWenModbus(struct acquisition_data *acq_data);
int protocol_ZONGLIN_NJHongGuang(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZJHZWeiLanD01(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZeTian(struct acquisition_data *acq_data);
int protocol_ZONGLIN_JuGuang_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_HZLuHeng_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_GLYunJing_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_HBHuaHouTianCheng(struct acquisition_data *acq_data);
int protocol_ZONGLIN_GDWeiChuang_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_JSLingHeng(struct acquisition_data *acq_data);
int protocol_ZONGLIN_NJJuGe(struct acquisition_data *acq_data);
int protocol_ZONGLIN_GZYiWen_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_BoKeSi(struct acquisition_data *acq_data);
int protocol_ZONGLIN_AHWanYi(struct acquisition_data *acq_data);
int protocol_ZONGLIN_NJGangNeng_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_DaoJin_4200(struct acquisition_data *acq_data);
int protocol_ZONGLIN_BJHuanKe_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_HBHuaHouTianCheng_new(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZhongLv(struct acquisition_data * acq_data);
int protocol_ZONGLIN_LiDa(struct acquisition_data * acq_data);
int protocol_ZONGLIN_HBKeruida(struct acquisition_data *acq_data);
int protocol_others_TPTN_UASHaXi_NPW160(struct acquisition_data *acq_data);
int protocol_ZONGLIN_YuXing_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_NJXinRuiPeng(struct acquisition_data *acq_data);
int protocol_ZONGLIN_QDJiaMing(struct acquisition_data *acq_data);
int protocol_ZONGLIN_JSZhuoZheng_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGLIN_USAHaXi_maxII_New(struct acquisition_data *acq_data);
int protocol_TNTP_Thermo(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZeTian_GBhjt2005(struct acquisition_data *acq_data);
int protocol_ZONGLIN_SZLangShi_PhotoTek6000(struct acquisition_data *acq_data);
int protocol_ZONGLIN_GJSurfaceWater(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZhengQi_Modbus(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZhongJieNengTianRong(struct acquisition_data *acq_data);
int protocol_ZONGLIN_JSTianZe(struct acquisition_data *acq_data);
int protocol_ZONGLIN_NJHongGuang2(struct acquisition_data *acq_data);
int protocol_ZONGLIN_JSRuiQuanRenQ_IV(struct acquisition_data *acq_data);
int protocol_ZONGLIN_ZJHZLiQi_V2 (struct acquisition_data *acq_data);
int protocol_ZONGLIN_NJJuGe_New(struct acquisition_data *acq_data);
int protocol_ZONGLIN_HeBeiHuaHouTianCheng_E(struct acquisition_data *acq_data);
int protocol_ZONGLIN_BJLiDa(struct acquisition_data *acq_data);
int protocol_ZONGLIN_GLYunJing_HeBei_Stay(struct acquisition_data *acq_data);
int protocol_ZONGLIN_NJXinRuiPeng_New (struct acquisition_data *acq_data);

int protocol_ZONGDAN_ChuangZao(struct acquisition_data * acq_data);
int protocol_ZONGDAN_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_ZONGDAN_ZhuoZheng2(struct acquisition_data *acq_data);
int protocol_ZONGDAN_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_JSTianRui2(struct acquisition_data *acq_data);
int protocol_ZONGDAN_AHYingKai(struct acquisition_data *acq_data);
int protocol_ZONGDAN_NJRuiQuanModbus(struct acquisition_data * acq_data);		/*yuiki 20191228*/
int protocol_ZONGDAN_HNHuaShiJieModbus(struct acquisition_data * acq_data);
int protocol_ZONGDAN_NeiMengGu(struct acquisition_data * acq_data);				/*yuiki 20191228*/
int protocol_ZONGDAN_LvYe(struct acquisition_data * acq_data);						/*yuiki 20200306*/
int protocol_ZONGDAN_CZShengLiHong(struct acquisition_data * acq_data);
int protocol_ZONGDAN_GBhjt2005(struct acquisition_data * acq_data);
int protocol_ZONGDAN_GBhjt2017(struct acquisition_data * acq_data);
int protocol_ZONGDAN_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_ZONGDAN_HuiHuang(struct acquisition_data * acq_data);
int protocol_ZONGDAN_SZZhongXingModbus(struct acquisition_data * acq_data);
int protocol_ZONGDAN_AmtaxInter2C(struct acquisition_data * acq_data);
int protocol_ZONGDAN_USAhaxiDynamic(struct acquisition_data * acq_data);
int protocol_ZONGDAN_GZYiWenModbus(struct acquisition_data * acq_data);
int protocol_ZONGDAN_NJHongGuang(struct acquisition_data *acq_data);
int protocol_ZONGDAN_ZJHZWeiLanD01(struct acquisition_data *acq_data);
int protocol_ZONGDAN_ZeTian(struct acquisition_data *acq_data);
int protocol_ZONGDAN_JuGuang_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_HZLuHeng_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_GLYunJing_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_HBHuaHouTianCheng(struct acquisition_data *acq_data);
int protocol_ZONGDAN_GDWeiChuang_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_JSLingHeng(struct acquisition_data *acq_data);
int protocol_ZONGDAN_NJJuGe(struct acquisition_data *acq_data);
int protocol_others_TPTN_DaoJin_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_GZYiWen_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_BoKeSi(struct acquisition_data *acq_data);
int protocol_ZONGDAN_AHWanYi(struct acquisition_data *acq_data);
int protocol_ZONGDAN_DaoJin_4200(struct acquisition_data *acq_data);
int protocol_ZONGDAN_BJHuanKe_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_HBHuaHouTianCheng_new(struct acquisition_data *acq_data);
int protocol_ZONGDAN_ZhongLv(struct acquisition_data * acq_data);
int protocol_ZONGDAN_LiDa(struct acquisition_data * acq_data);
int protocol_ZONGDAN_HBKeruida(struct acquisition_data *acq_data);
int protocol_ZONGDAN_YuXing_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_NJXinRuiPeng(struct acquisition_data *acq_data);
int protocol_ZONGDAN_QDJiaMing(struct acquisition_data *acq_data);
int protocol_ZONGDAN_JSZhuoZheng_HeBei(struct acquisition_data *acq_data);
int protocol_ZONGDAN_USAHaXi_maxII_New(struct acquisition_data *acq_data);
int protocol_ZONGDAN_ZeTian_GBhjt2005(struct acquisition_data *acq_data);
int protocol_ZONGDAN_SZLangShi_PhotoTek6000(struct acquisition_data *acq_data);
int protocol_ZONGDAN_ZhengQi_Modbus(struct acquisition_data *acq_data);
int protocol_ZONGDAN_NJHongGuang2(struct acquisition_data *acq_data);
int protocol_ZONGDAN_ZhongJieNengTianRong(struct acquisition_data *acq_data);
int protocol_ZONGDAN_JSTianZe(struct acquisition_data *acq_data);
int protocol_ZONGDAN_JSRuiQuanRenQ_IV(struct acquisition_data *acq_data);
int protocol_ZONGDAN_ZJHZLiQi_V2 (struct acquisition_data *acq_data);
int protocol_ZONGDAN_NJJuGe_New(struct acquisition_data *acq_data);
int protocol_ZONGDAN_HeBeiHuaHouTianCheng_E(struct acquisition_data *acq_data);
int protocol_ZONGDAN_BJLiDa(struct acquisition_data *acq_data);
int protocol_ZONGDAN_GLYunJing_HeBei_Stay(struct acquisition_data *acq_data);
int protocol_ZONGDAN_NJXinRuiPeng_New (struct acquisition_data *acq_data);

int protocol_water_YuXing_Modbus(struct acquisition_data *acq_data);
int protocol_water_GLYunJing_HeBei(struct acquisition_data *acq_data);
int protocol_water_NJXinRuiPeng(struct acquisition_data *acq_data);

int protocol_ZONGNIE_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_ZONGNIE_ZhuoZheng2(struct acquisition_data *acq_data);
int protocol_ZONGNIE_HeBei(struct acquisition_data *acq_data);


int protocol_CHROME_GZyingsi(struct acquisition_data *acq_data);
int protocol_NICKEL_GZyingsi(struct acquisition_data *acq_data);
int protocol_TONG_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_TONG_ZhuoZheng2(struct acquisition_data *acq_data);
int protocol_ZONGGE_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_ZONGGE_ZhuoZheng2(struct acquisition_data *acq_data);
int protocol_ZONGQUAN_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_ZONGQUAN_ZhuoZheng_UGL(struct acquisition_data *acq_data);

int protocol_ZONGQUAN_ZhuoZheng2(struct acquisition_data *acq_data);
int protocol_LIUJIAGE_HeBei(struct acquisition_data *acq_data);
int protocol_LIUJIAGE_HNHuaShiJie(struct acquisition_data *acq_data);

int protocol_NI_NeiMengGu(struct acquisition_data * acq_data);			/*yuiki 20191228*/
int protocol_LIUJIACR_NeiMengGu(struct acquisition_data * acq_data);	/*yuiki 20191228*/

int protocol_CR_GZYiWen(struct acquisition_data *acq_data);
int protocol_Ni_HNHuaShiJie(struct acquisition_data *acq_data);
int protocol_NI_GZYiWen(struct acquisition_data *acq_data);
int protocol_CU_GZYiWen(struct acquisition_data *acq_data);
int protocol_CU_JuGuangKeJi(struct acquisition_data *acq_data);
int protocol_ZN_JuGuangKeJi(struct acquisition_data *acq_data);
int protocol_CD_JSTianRui2(struct acquisition_data *acq_data);
int protocol_PB_JSTianRui2(struct acquisition_data *acq_data);
int protocol_NI_JSTianRui2(struct acquisition_data *acq_data);
int protocol_CU_JSTianRui2(struct acquisition_data *acq_data);
int protocol_ZN_JSTianRui2(struct acquisition_data *acq_data);
int protocol_FE_JSTianRui2(struct acquisition_data *acq_data);

int protocol_Pb_SZLangShi_PhotoTek6000(struct acquisition_data *acq_data);
int protocol_Zn_SZLangShi_PhotoTek6000(struct acquisition_data *acq_data);

int protocol_Cu_NJGangNeng(struct acquisition_data * acq_data);
int protocol_Ni_NJGangNeng(struct acquisition_data * acq_data);

int protocol_AS_SongXiaSyi(struct acquisition_data *acq_data);
int protocol_As_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_As_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_As_SongXiaSSi(struct acquisition_data *acq_data);
int protocol_CD_SongXiaSYi(struct acquisition_data *acq_data);
int protocol_CD_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_CD_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_CD_SongXiaSSi(struct acquisition_data *acq_data);
int protocol_CR_SongXiaSYi(struct acquisition_data *acq_data);
int protocol_CR_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_CR_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_CR_SongXiaSSi(struct acquisition_data *acq_data);
int protocol_LiuJiaCR_SongXiaSYi(struct acquisition_data *acq_data);
int protocol_LiuJiaCR_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_LiuJiaCR_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_LiuJiaCR_SongXiaSSi(struct acquisition_data *acq_data);
int protocol_CU_SongXiaSYi(struct acquisition_data *acq_data);
int protocol_CU_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_CU_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_CU_SongXiaSSi(struct acquisition_data *acq_data);
int protocol_PB_SongXiaSYi(struct acquisition_data *acq_data);
int protocol_PB_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_PB_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_PB_SongXiaSSi(struct acquisition_data *acq_data);
int protocol_MN_SongXiaSYi(struct acquisition_data *acq_data);
int protocol_MN_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_MN_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_MN_SongXiaSSi(struct acquisition_data *acq_data);
int protocol_NI_SongXiaSYi(struct acquisition_data *acq_data);
int protocol_NI_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_NI_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_NI_SongXiaSSi(struct acquisition_data *acq_data);
int protocol_ZN_SongXiaSYi(struct acquisition_data *acq_data);
int protocol_ZN_SongXiaSEr(struct acquisition_data *acq_data);
int protocol_ZN_SongXiaSSan(struct acquisition_data *acq_data);
int protocol_ZN_SongXiaSSi(struct acquisition_data *acq_data);

int protocol_LiuJiaCr_NJJuGe(struct acquisition_data *acq_data);
int protocol_Pb_QDJiaMing(struct acquisition_data *acq_data);

int protocol_Cu_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_LIUJIAGE_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_Cr_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_ZONGNIE_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_Pb_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_Cd_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_Fe_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_Zn_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_As_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_Mn_JMWeiChuang(struct acquisition_data * acq_data);

//yuiki 20191228
int protocol_AS_NJRuiQuanModbus(struct acquisition_data * acq_data);
int protocol_CD_NJRuiQuanModbus(struct acquisition_data * acq_data);
int protocol_CR_NJRuiQuanModbus(struct acquisition_data * acq_data);
int protocol_CU_NJRuiQuanModbus(struct acquisition_data * acq_data);
int protocol_MN_NJRuiQuanModbus(struct acquisition_data * acq_data);
int protocol_NI_NJRuiQuanModbus(struct acquisition_data * acq_data);
int protocol_PB_NJRuiQuanModbus(struct acquisition_data * acq_data);
int protocol_ZN_NJRuiQuanModbus(struct acquisition_data * acq_data);
int protocol_LIUJIACR_NJRuiQuanModbus(struct acquisition_data * acq_data);

int protocol_CD_SZZhongXing(struct acquisition_data *acq_data);
int protocol_PB_SZZhongXing(struct acquisition_data *acq_data);
int protocol_NI_SZZhongXing(struct acquisition_data *acq_data);
int protocol_CU_SZZhongXing(struct acquisition_data *acq_data);
int protocol_ZN_SZZhongXing(struct acquisition_data *acq_data);
int protocol_MN_SZZhongXing(struct acquisition_data *acq_data);

int protocol_ai(struct acquisition_data *acq);
int protocol_yiwen_heavy_metal(struct acquisition_data *acq_data);
int protocol_keze_nickel(struct acquisition_data *acq_data);
int protocol_keze_copper(struct acquisition_data *acq_data);
int protocol_xianhe_heavy_metal(struct acquisition_data *acq_data);
int protocol_huashijie_heavy_metal(struct acquisition_data *acq_data);
int protocol_lihe_heavy_metal(struct acquisition_data *acq_data);

int protocol_modtcp_DCS_GongKuang(struct acquisition_data *acq_data);
int protocol_modtcp_DCS_GongKuang2(struct acquisition_data *acq_data);
int protocol_modtcp_DCS_GongKuang3(struct acquisition_data *acq_data);
int protocol_modtcp_DCS_GongKuang4(struct acquisition_data *acq_data);
int protocol_modtcp_DCS_GongKuang_modbus(struct acquisition_data *acq_data);


int protocol_kunlun_test(struct acquisition_data *acq_data);
int protocol_GPS_test(struct acquisition_data *acq_data);
int protocol_xianhe_test1(struct acquisition_data *acq_data);
int protocol_xianhe_test2(struct acquisition_data *acq_data);
int protocol_xianhe_test3(struct acquisition_data *acq_data);

int protocol_CEMS_BJxuedilong(struct acquisition_data *acq_data);
int protocol_CEMS_BJxuedilong_fujian(struct acquisition_data *acq_data);

//int protocol_CEMS_GBhjt212(struct acquisition_data *acq_data);
int protocol_CEMS_GBhjt2005(struct acquisition_data *acq_data);
int protocol_CEMS_GBhjt2005_PaMh(struct acquisition_data *acq_data);
int protocol_CEMS_GBhjt2005_PaMs(struct acquisition_data *acq_data);

int protocol_CEMS_GBhjt2017(struct acquisition_data *acq_data);
int protocol_CEMS_GBhjt2017_PaMh(struct acquisition_data *acq_data);
int protocol_CEMS_GBhjt2017_PaMs(struct acquisition_data *acq_data);
int protocol_CEMS_GBhjt2017_ChenduLePan(struct acquisition_data *acq_data);
int protocol_CEMS_GBhjt2017_ChenduLePanXJ(struct acquisition_data *acq_data);
int protocol_TSP_ZhongDianXingYe(struct acquisition_data *acq_data);

int protocol_CEMS_GBhjt2005_XinJiang(struct acquisition_data *acq_data);
int protocol_CEMS_GBhjt2005_ChuangChen(struct acquisition_data *acq_data);
int protocol_CEMS_WeiChuang(struct acquisition_data * acq_data);
int protocol_CEMS_furnace_temperature(struct acquisition_data *acq_data);
int protocol_CEMS_HBManDeKe3_DCEM3000(struct acquisition_data *acq_data);;

int protocol_CEMS_HBManDeKe_NOx(struct acquisition_data *acq_data);
int protocol_TSP_HBManDeKe_PLC_S7_200(struct acquisition_data *acq_data);
int protocol_TSP_HBManDeKe_PLC_Smart200(struct acquisition_data *acq_data);

int protocol_CEMS_SHbsjHuiGu(struct acquisition_data *acq_data);
//yuiki 20190920
int protocol_CEMS_HeBei_M3H(struct acquisition_data *acq_data);
int protocol_CEMS_HeBei_KPA(struct acquisition_data *acq_data);
int protocol_CEMS_HeBei(struct acquisition_data *acq_data);
int protocol_CEMS_XinJiang(struct acquisition_data *acq_data);
int protocol_CEMS_NeiMengGu(struct acquisition_data * acq_data);
int protocol_CEMS_TLD_PLC(struct acquisition_data * acq_data);
int protocol_CEMS_ZeTian_HeBeiModbus(struct acquisition_data *acq_data);
int protocol_TSP_ZeTian_circle(struct acquisition_data *acq_data);
int protocol_TSP_ZeTian_rectangle(struct acquisition_data *acq_data);
/*yuiki 20191015*/
int protocol_CEMS_garbage(struct acquisition_data *acq_data);		
int protocol_CEMS_garbage_modbus(struct acquisition_data *acq_data);
int protocol_CEMS_garbage_modbus_s27I(struct acquisition_data *acq_data);
int protocol_CEMS_garbage_modbus_s3bII(struct acquisition_data *acq_data);
int protocol_CEMS_BJzhongdian(struct acquisition_data *acq_data);
int protocol_CEMS_Furnace_modbus(struct acquisition_data *acq_data);
int protocol_CEMS_AHwanyi(struct acquisition_data *acq_data);
int protocol_CEMS_DAMASCII(struct acquisition_data *acq_data);
int protocol_CEMS_LaJi_pack_HeBeiXT(struct acquisition_data *acq_data);
int protocol_CEMS_LaJi_pack_HeBeiGRST(struct acquisition_data *acq_data);
int protocol_CEMS_LaJi_pack_HeBeiGRST_GongKuang(struct acquisition_data *acq_data);
int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000(struct acquisition_data *acq_data);
int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_PLC(struct acquisition_data *acq_data);
int protocol_CEMS_CQChuanYiModBus_PS7400_F(struct acquisition_data *acq_data);
int protocol_PLC_CQChuanYiModBus_PS7400_F(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_TXO_1000(struct acquisition_data *acq_data);
int protocol_PLC_SaiMoFei(struct acquisition_data *acq_data);
int protocol_TSP_SaiMoFei_PM(struct acquisition_data *acq_data);
int protocol_CEMS_SZCuiYunGu_TLGAS280(struct acquisition_data *acq_data);
int protocol_CEMS_SZCuiYunGu_TL_PMM180_4(struct acquisition_data *acq_data);
int protocol_TSP_WXChuangChen_DA600(struct acquisition_data *acq_data);
int protocol_CEMS_ZeTian_HeBeiModbus2(struct acquisition_data *acq_data);
int protocol_CEMS_ZhongKe_TR216I(struct acquisition_data *acq_data);
int protocol_CEMS_ChuoMei_hjt2005_HeBei(struct acquisition_data *acq_data);
int protocol_CEMS_HBKaiErUniMAT120(struct acquisition_data * acq_data);
int protocol_PLC_HBKaiErUniMAT120(struct acquisition_data *acq_data);
int protocol_PLC_SaiMoFei_HBDongTai(struct acquisition_data *acq_data);

int protocol_CEMS_WHTianHong(struct acquisition_data * acq_data);
int protocol_CEMS_GermanySick_pack1(struct acquisition_data *acq_data);
int protocol_CEMS_GermanySick_pack2(struct acquisition_data *acq_data);
int protocol_CEMS_GermanySick_pack3(struct acquisition_data *acq_data);
int protocol_CEMS_BJxuedilongModbus(struct acquisition_data *acq_data);
int protocol_CEMS_ShengKaiAn(struct acquisition_data *acq_data);
int protocol_CEMS_SZCuiYunGu_TL_PMM180(struct acquisition_data *acq_data);
int protocol_CEMS_SZCuiYunGu_TL_PMM180_2(struct acquisition_data *acq_data);
int protocol_CEMS_SZCuiYunGu_TL_PMM180_3(struct acquisition_data *acq_data);
int protocol_CEMS_JuGuang_HeBei_JinDing(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_42i_HI(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_42i_LO(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_43i_HI(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_43i_LO(struct acquisition_data *acq_data);
int protocol_CEMS_HBManDeKe(struct acquisition_data *acq_data);
int protocol_TSP_HBManDeKe(struct acquisition_data *acq_data);
int protocol_TSP_HBManDeKe2(struct acquisition_data *acq_data);
int protocol_CEMS_JuGuang_HeBei(struct acquisition_data *acq_data);
int protocol_TSP_JuGuang_HeBei(struct acquisition_data *acq_data);
int protocol_TSP_AnRomhXin(struct acquisition_data *acq_data);
int protocol_CEMS_QDJiaMing(struct acquisition_data *acq_data);
int protocol_cems_JuGuang_ai(struct acquisition_data *acq_data);
int protocol_CEMS_AHWanYi_HeBei(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_scs900uv(struct acquisition_data *acq_data);
int protocol_PLC_BJXueDiLong_scs900uv(struct acquisition_data *acq_data);
int protocol_PLC_BJXueDiLong_scs900c(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_U23(struct acquisition_data *acq_data);
int protocol_PLC_BJXueDiLong_200(struct acquisition_data *acq_data);
int protocol_cems_JuGuang_ai_JinDing(struct acquisition_data *acq_data);
int protocol_CEMS_BJHangTianYiLai(struct acquisition_data *acq_data);
int protocol_PLC_BJHangTianYiLai(struct acquisition_data *acq_data);
int protocol_CEMS_SDdongTai_G3000_UV(struct acquisition_data *acq_data);
int protocol_CEMS_KaiLan(struct acquisition_data *acq_data);
int protocol_PLC_ChuoMei(struct acquisition_data *acq_data);
int protocol_TSP_ZhongLv_SGEP300PM(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_MODEL1080(struct acquisition_data *acq_data);
int protocol_CEMS_DaoJin_URA208A(struct acquisition_data *acq_data);
int protocol_CEMS_DaoJin_URA209(struct acquisition_data *acq_data);
int protocol_PLC_DaoJin(struct acquisition_data *acq_data);
int protocol_O2_BJXueDiLong_MODEL1080(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_MODEL4000(struct acquisition_data *acq_data);
int protocol_PLC_BJXueDiLong_SCS900FT(struct acquisition_data *acq_data);
int protocol_TSP_BeiFeng_SBF800_900_2021A(struct acquisition_data *acq_data);
int protocol_TSP_BeiFeng_SBF800_900_2021B(struct acquisition_data *acq_data);
int protocol_PLC_BJXueDiLong_S7_200(struct acquisition_data *acq_data);
int protocol_PLC_BJXueDiLong_Smart(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_SCS900PM(struct acquisition_data *acq_data);
int protocol_O2_ZeTian_HeBei(struct acquisition_data *acq_data);
int protocol_CEMS_SHHeRu_APxx370(struct acquisition_data *acq_data);
int protocol_O2_KuChang(struct acquisition_data *acq_data);
int protocol_CEMS_KuChang(struct acquisition_data *acq_data);
int protocol_PLC_KuChang(struct acquisition_data *acq_data);
int protocol_PLC_SaiMoFei_not_flag(struct acquisition_data *acq_data);
int protocol_PLC_ZhongDianXingYe(struct acquisition_data *acq_data);
int protocol_TSP_JuGuang_SynspecPM(struct acquisition_data *acq_data);
int protocol_CEMS_HBHeFeng(struct acquisition_data *acq_data);
int protocol_CEMS_HBHeFengCEMS(struct acquisition_data *acq_data);
int protocol_CEMS_ZeTian_HeBeiModbus_old(struct acquisition_data *acq_data);
int protocol_ai_8017rc(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_MODEL1080_v3_7(struct acquisition_data *acq_data);
int protocol_CEMS_ZhongKe(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_SCS900CPM(struct acquisition_data *acq_data);
int protocol_PLC_SHBeiFen_SBF1500(struct acquisition_data *acq_data);
int protocol_PLC_SHBeiFen_SBF1500_STATUS(struct acquisition_data *acq_data);
int protocol_PLC_CEMS_ZhuoMeiKeJi(struct acquisition_data *acq_data);
int protocol_CEMS_JuGuang_HeBei_2000D_SO2(struct acquisition_data *acq_data);
int protocol_CEMS_JuGuang_HeBei_2000D_NOx(struct acquisition_data *acq_data);
int protocol_cems_JuGuang_HeBei_ai_2000D(struct acquisition_data *acq_data);
int protocol_cems_JuGuang_HeBei_PLC_2000D(struct acquisition_data * acq_data);
int protocol_CEMS_BJXueDiLong_1080uv(struct acquisition_data *acq_data);
int protocol_CEMS_LaJi_pack_HBZhongJieNengDCS(struct acquisition_data *acq_data);
int protocol_CEMS_SZCuiYunGu_TL_PMM180_Low(struct acquisition_data *acq_data);
int protocol_PLC_HBManDeKe_MDK116_B(struct acquisition_data *acq_data);
int protocol_CEMS_HBManDeKe1_DCEM3000(struct acquisition_data *acq_data);
int protocol_CEMS_HBManDeKe2_DCEM3000(struct acquisition_data *acq_data);
int protocol_ai_XingZe_CJ01(struct acquisition_data *acq_data);
int protocol_CEMS_AnRongXin_AGA1000(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei1_42i_HI(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei1_42i_LO(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei1_43i_HI(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei1_43i_LO(struct acquisition_data *acq_data);
int protocol_PLC_SaiMoFei1(struct acquisition_data *acq_data);
int protocol_PLC_SaiMoFei1_not_flag(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei1_TXO_1000(struct acquisition_data *acq_data);
int protocol_TSP_BJXueDiLong_Model2030(struct acquisition_data *acq_data);
int protocol_TSP_NJBoRui_MD6000_B(struct acquisition_data *acq_data);
int protocol_CEMS_SHHeRu_APxx370_v1_6(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_MODEL1080_v4(struct acquisition_data *acq_data);
int protocol_CEMS_LaJi_pack_HBManDeKeDCS(struct acquisition_data *acq_data);
int protocol_TSP_QDJiaMing_LaserDustMP(struct acquisition_data *acq_data);
int protocol_CEMS_XiKeMaiHaKe_MCS100FT(struct acquisition_data *acq_data);
int protocol_cems_XiKeMaiHaKe_PLC_MCS100FT(struct acquisition_data *acq_data);
int protocol_cems_XiKeMaiHaKe_PLC_MCS100FT_New(struct acquisition_data *acq_data);
int protocol_TSP_NJBoRui_MD6000(struct acquisition_data *acq_data);
int protocol_CEMS_SDXinZe_ShiYang(struct acquisition_data *acq_data);
int protocol_CEMS_BeiFangBoLi(struct acquisition_data *acq_data);
int protocol_CEMS_YaoJiBoLi(struct acquisition_data *acq_data);
int protocol_CEMS_BJHangTianYiLai_HeiBei(struct acquisition_data *acq_data);
int protocol_PLC_BJHangTianYiLai_HeiBei(struct acquisition_data *acq_data);
int protocol_CEMS_HBManDeKe_NO_V4(struct acquisition_data *acq_data);

int protocol_CEMS_SaiMoFei_42i_HI_100(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_42i_LO_100(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_43i_HI_100(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_43i_LO_100(struct acquisition_data *acq_data);
int protocol_CEMS_SaiMoFei_TXO_1000_100(struct acquisition_data *acq_data);
int protocol_PLC_SDXinZe_v1_7(struct acquisition_data *acq_data);
int protocol_CEMS_FSLaJiDianChang_TaiLiDa(struct acquisition_data *acq_data);
int protocol_CEMS_FSLuTangWenDu_TaiLiDa(struct acquisition_data *acq_data);
int protocol_CEMS_GuangHuanTouDCS_TaiLiDa(struct acquisition_data *acq_data);
int protocol_CEMS_AHWanYi_SG1000_HeBei(struct acquisition_data *acq_data);
int protocol_CEMS_WuXiChenChuang_PLC(struct acquisition_data *acq_data);
int protocol_CEMS_WuXiChenChuang(struct acquisition_data *acq_data);
int protocol_CEMS_HBManDeKe_Modbus(struct acquisition_data *acq_data);
int protocol_CEMS_QDZhongPing(struct acquisition_data *acq_data);
int protocol_TSP_QDZhongPing_LRCD2000WV_info(struct acquisition_data *acq_data);
int protocol_PLC_QDZhongPing_7017Aplus(struct acquisition_data *acq_data);
int protocol_PLC_QDZhongPing_7017Aplus_PLC(struct acquisition_data *acq_data);
int protocol_NH3_LG1100(struct acquisition_data *acq_data);
int protocol_TSP_SDXinZe_SDUST100(struct acquisition_data *acq_data);
int protocol_CEMS_SDXinZe_V2(struct acquisition_data *acq_data);
int protocol_PLC_HongRuiDe(struct acquisition_data *acq_data);
int protocol_CEMS_RuiAnRA8000(struct acquisition_data *acq_data);
int protocol_TSP_AnRomhXin_LSS2004_LSS2014(struct acquisition_data *acq_data);
int protocol_O2_AnRongXin_ARX5100(struct acquisition_data *acq_data);
int protocol_PLC_AnRomhXin_APT2000(struct acquisition_data *acq_data);
int protocol_CEMS_BJXueDiLong_MODEL4000_1(struct acquisition_data *acq_data);
int protocol_VOCs_TianJin712_DHT508(struct acquisition_data *acq_data);
int protocol_ai_TianJin712_DHT508(struct acquisition_data *acq_data);
int protocol_CEMS_AHLvShi(struct acquisition_data *acq_data);
int protocol_TSP_ZhongXing_DM601_Low(struct acquisition_data *acq_data);

int protocol_VOCs_ZhuozhengGB2017(struct acquisition_data *acq_data);
int protocol_VOCs_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_VOCs_JSTianRui(struct acquisition_data *acq_data);
int protocol_VOCs_JSChangZhouPanNuo(struct acquisition_data *acq_data);
int protocol_VOCs_SDHaiHui(struct acquisition_data * acq_data);
int protocol_VOCs_GZLiuEn(struct acquisition_data *acq_data);
int protocol_VOCs_GZLiuEn_DG(struct acquisition_data * acq_data);
int protocol_VOCs_GZLiuEn_DG2(struct acquisition_data * acq_data);
int protocol_VOCs_GZLiuEn_DG3(struct acquisition_data * acq_data);
int protocol_VOCs_GZLiuEn_DG4(struct acquisition_data * acq_data);
int protocol_VOCs_GBhjt2005(struct acquisition_data * acq_data);
int protocol_VOCs_GBhjt2017(struct acquisition_data * acq_data);
int protocol_VOCs_GBhjt2005_kpamh(struct acquisition_data * acq_data);
int protocol_VOCs_GBhjt2017_kpamh(struct acquisition_data * acq_data);
int protocol_VOCs_GBhjt2005_pamh(struct acquisition_data * acq_data);
int protocol_VOCs_GBhjt2017_pamh(struct acquisition_data * acq_data);
int protocol_VOCs_GBhjt2005_pams(struct acquisition_data * acq_data);
int protocol_VOCs_GBhjt2017_pams(struct acquisition_data * acq_data);
int protocol_VOCs_DGChuangTuo(struct acquisition_data * acq_data);
int protocol_VOCs_DGZhongGeng(struct acquisition_data * acq_data);
int protocol_VOCs_SHLanBao(struct acquisition_data * acq_data);
int protocol_VOCs_ZeTian(struct acquisition_data * acq_data);
int protocol_VOCs_SHBaoYing(struct acquisition_data * acq_data);
int protocol_VOCs_SHBaoYing2(struct acquisition_data * acq_data);
int protocol_VOCs_ChuangTuo(struct acquisition_data *acq_data);
int protocol_VOCs_SZHengFuShang_SZ(struct acquisition_data *acq_data);
int protocol_VOCs_SZHengFuShang_JX(struct acquisition_data *acq_data);

int protocol_VOCs_TianZhiLan3(struct acquisition_data * acq_data);
int protocol_VOCs_TianZhiLan2(struct acquisition_data * acq_data);
int protocol_VOCs_TianZhiLan(struct acquisition_data * acq_data);
int protocol_VOCs_SZHengFuSheng(struct acquisition_data * acq_data);
int protocol_VOCs_SZHengFuSheng2(struct acquisition_data * acq_data);
int protocol_VOCs_SZHengFuSheng3(struct acquisition_data * acq_data);
int protocol_VOCs_SuChen(struct acquisition_data * acq_data);
int protocol_VOCs_SuChen2(struct acquisition_data * acq_data);
int protocol_VOCs_FanYi(struct acquisition_data * acq_data);
int protocol_VOCs_DGYongBang(struct acquisition_data * acq_data);
int protocol_VOCs_DGYongBang2(struct acquisition_data * acq_data);
int protocol_VOCs_YingDeWanJu(struct acquisition_data *acq_data);
int protocol_VOC_WanAnDi(struct acquisition_data *acq_data);
int protocol_VOC_TianDeYi(struct acquisition_data *acq_data);
int protocol_VOC_TianZhiLan_HBCangZhou(struct acquisition_data *acq_data);

int protocol_gas_thermo450i(struct acquisition_data *acq_data);
int protocol_bocon_test1(struct acquisition_data *acq_data);

int protocol_hjt212_test(struct acquisition_data *acq_data);
int protocol_temperature(struct acquisition_data *acq_data);
int protocol_RONGYUAN_RYIS01501(struct acquisition_data *acq_data);

int protocol_DODI_TEST(struct acquisition_data *acq_data);

int protocol_SHjinji_water(struct acquisition_data *acq_data);
int protocol_SHjinji_water2(struct acquisition_data *acq_data);
int protocol_SHjinji_water3(struct acquisition_data *acq_data);
int protocol_SHjinji_water4(struct acquisition_data *acq_data);
int protocol_SHjinji_water5(struct acquisition_data *acq_data);

int protocol_TincKay_PH(struct acquisition_data *acq_data);
int protocol_TincKay_COND(struct acquisition_data *acq_data);


int protocol_WATER_TEST(struct acquisition_data *acq_data);

int protocol_modtcp_NET_4013M1(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4013M2(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4013M3(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4013M4(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4013M5(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4013M6(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4013M7(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4013M8(struct acquisition_data *acq_data);

int protocol_modtcp_NET_4011M1(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4011M2(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4011M3(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4011M4(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4011M5(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4011M6(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4011M7(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4011M8(struct acquisition_data *acq_data);


int protocol_door_HaiKang_DSK1T641M(struct acquisition_data *acq_data);

int protocol_PH_KeRuiDa_ph8500A(struct acquisition_data *acq_data);
int protocol_PH_BoKeSi_301M(struct acquisition_data * acq_data);
int protocol_ph_SZMinBo(struct acquisition_data *acq_data);
int protocol_RAINTECH_RY_FSX485(struct acquisition_data *acq_data);
int protocol_BATTERY_VOLTAGE(struct acquisition_data *acq_data);

int protocol_AGRICLTURE_TEST(struct acquisition_data *acq_data);
int protocol_DODI_ALL_TEST(struct acquisition_data *acq_data);
int protocol_watermeter_SM_10H_in(struct acquisition_data *acq_data);
int protocol_watermeter_SM_10H_out(struct acquisition_data *acq_data);
int protocol_watermeter_SM_10H_in_6integers(struct acquisition_data *acq_data);
int protocol_watermeter_SM_10H_out_6integers(struct acquisition_data *acq_data);

int protocol_modtcp_NET_4021M1(struct acquisition_data *acq_data);
int protocol_modtcp_NET_4021M2(struct acquisition_data *acq_data);

int protocol_modtcp_C2000_SDD8020_1(struct acquisition_data *acq_data);
int protocol_modtcp_C2000_SDD8020_2(struct acquisition_data *acq_data);

int protocol_modtcp_C2000_SAX0800_1(struct acquisition_data *acq_data);
int protocol_modtcp_C2000_SAX0800_2(struct acquisition_data *acq_data);

int protocol_modtcp_SIS_ModbusTCP(struct acquisition_data *acq_data);
int protocol_ZONGLIANGYI_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_CN_JMWeiChuang(struct acquisition_data * acq_data);
int protocol_ZONGLIN_ZONGDAN_USAhaxi(struct acquisition_data * acq_data);
int protocol_others_Chlorophyll_PengYue(struct acquisition_data *acq_data);
int protocol_others_Cyanin_PengYue(struct acquisition_data *acq_data);
int protocol_others_Diandaolv_PengYue(struct acquisition_data *acq_data);
int protocol_others_DO_PengYue(struct acquisition_data *acq_data);
int protocol_others_ph_PengYue(struct acquisition_data *acq_data);
int protocol_others_Zhuodu_PengYue(struct acquisition_data *acq_data);
int protocol_others_YULV_PengYue(struct acquisition_data * acq_data);

/*yuiki 20191219*/
int protocol_others_fuhuawu(struct acquisition_data * acq_data);
int protocol_others_HuiguMP3100(struct acquisition_data * acq_data);
int protocol_liquid_level_2088(struct acquisition_data * acq_data);
int protocol_ph_kezo_pc1000(struct acquisition_data * acq_data);
int protocol_ORP_Yosemitech(struct acquisition_data *acq_data);
int protocol_ph_Yosemitech(struct acquisition_data *acq_data);
int protocol_others_Chlorophyll_Yosemitech(struct acquisition_data *acq_data);
int protocol_others_DO_Yosemitech(struct acquisition_data *acq_data);
int protocol_xF_kezo_pc1000(struct acquisition_data * acq_data);
int protocol_DO_WHSiFuMing(struct acquisition_data * acq_data);
int protocol_ZHUODU_WHSiFuMing(struct acquisition_data * acq_data);
int protocol_CL_JSZhuoZheng(struct acquisition_data * acq_data);
int protocol_ZHUODU_JMHuangYu(struct acquisition_data * acq_data); /*yuiki 20191206*/
int protocol_diaphaneity_DongGuang(struct acquisition_data * acq_data);
int protocol_WATER_GBhjt2005(struct acquisition_data * acq_data);	/*yuiki 20191230*/
int protocol_WATER_GBhjt2017(struct acquisition_data * acq_data);	/*yuiki 20191230*/
int protocol_WATER_HeBei(struct acquisition_data * acq_data);			/*yuiki 20191231*/
int protocol_others_ups_dongguang(struct acquisition_data * acq_data);
int protocol_ph_LianCe(struct acquisition_data * acq_data);
int protocol_PH_SZQingYan(struct acquisition_data * acq_data);		/*yuiki 20200103*/
int protocol_PH_GDWeiChuang(struct acquisition_data *acq_data);/*miles zhang 20200628*/
int protocol_PH_02(struct acquisition_data * acq_data);
int protocol_PH_SHLuoBo(struct acquisition_data *acq_data);
int protocol_ai_7017rc(struct acquisition_data *acq_data);/*miles zhang 202000709*/
int protocol_ai_7017rc_vol(struct acquisition_data *acq_data);
int protocol_ai_7017rc_vol1(struct acquisition_data *acq_data);
int protocol_ai_7017rc_vol_Di(struct acquisition_data *acq_data);
int protocol_CEMS_7017rc_RO(struct acquisition_data *acq_data);
int protocol_ai_4017rc(struct acquisition_data *acq_data);
int protocol_modbus_ai_4017rc(struct acquisition_data *acq_data);
int protocol_modbus_ai_4017rc_XinTianCompany(struct acquisition_data *acq_data);


int protocol_modbus_gas_ai(struct acquisition_data *acq_data);
int protocol_modbus_gas(struct acquisition_data *acq_data);
int protocol_modbus_water_ai(struct acquisition_data *acq_data);
int protocol_modbus_water(struct acquisition_data *acq_data);
int protocol_direct_XJWater(struct acquisition_data *acq_data);
int protocol_direct_XJGas(struct acquisition_data *acq_data);
int protocol_direct_NMGas(struct acquisition_data *acq_data);
int protocol_COND_SDKaiMiSi(struct acquisition_data *acq_data);
int protocol_PH_SDKaiMiSi(struct acquisition_data *acq_data);
int protocol_PH_8001(struct acquisition_data *acq_data);
int protocol_COND_LanChang(struct acquisition_data *acq_data);
int protocol_PH_LanChang(struct acquisition_data *acq_data);
int protocol_PH_ZhuoZheng(struct acquisition_data *acq_data);
int protocol_PH_SDDongRun(struct acquisition_data *acq_data);
int protocol_PH_BJHuanKe(struct acquisition_data *acq_data);
int protocol_others_YULV_LiDe(struct acquisition_data *acq_data);
int protocol_PH_221B(struct acquisition_data *acq_data);
int protocol_ph_LianCeV8(struct acquisition_data * acq_data);
int protocol_ph_PHORP_G8306(struct acquisition_data *acq_data);

int protocol_xCN_HBHuaHouTianCheng(struct acquisition_data *acq_data);
int protocol_HUIFAFEN_HBHuaHouTianCheng(struct acquisition_data *acq_data);

/*yuiki 20191218*/
int protocol_others_tengentTx(struct acquisition_data * acq_data);
int protocol_others_tengentRx(struct acquisition_data * acq_data);
int protocol_others_TPTN_USAhaxi(struct acquisition_data * acq_data);
int protocol_others_ph_JSTaiChuan_TC5100(struct acquisition_data *acq_data);

int protocol_others_vocs_pid(struct acquisition_data *acq_data);
int protocol_others_vocs_pid_div10(struct acquisition_data *acq_data);
int protocol_others_vocs_pid_div1000(struct acquisition_data *acq_data);
int protocol_others_vocs_pid_mgm3(struct acquisition_data *acq_data);

int protocol_electric_meter_TEST(struct acquisition_data *acq_data);
int protocol_DLT645_1997(struct acquisition_data *acq_data);
int protocol_DLT645_2007(struct acquisition_data *acq_data);
int protocol_dianbiao_ZJZhengTai_DTSU666_modbus(struct acquisition_data * acq_data);
int protocol_dianbiao_DeLiXi_DTSU6606_modbus(struct acquisition_data * acq_data);
int protocol_dianbiao_SuChen(struct acquisition_data * acq_data);
int protocol_dianbiao_SuChen2(struct acquisition_data * acq_data);
int protocol_dianbiao_CL7339N(struct acquisition_data * acq_data);
int protocol_dianbiao_DGYongBang(struct acquisition_data * acq_data);
int protocol_dianbiao_DGYongBang2(struct acquisition_data * acq_data);
int protocol_dianbiao_DDSU666_modbus(struct acquisition_data *acq_data);
int protocol_dianbiao_DongGuang(struct acquisition_data *acq_data);
int protocol_dianbiao_YingDeWanJu(struct acquisition_data *acq_data);
int protocol_dianbiao_KeLu_YiTong(struct acquisition_data *acq_data);
int protocol_dianbiao_GZBoKong_K35A(struct acquisition_data *acq_data);

int protocol_modbus_voc(struct acquisition_data *acq_data);


int protocol_DLT645_GDDG1(struct acquisition_data * acq_data);
int protocol_DLT645_GDDG2(struct acquisition_data * acq_data);

int protocol_DIANBIAO_DianBen_zhiwu(struct acquisition_data * acq_data);
int protocol_DIANBIAO_DianBen_shengchan(struct acquisition_data * acq_data);
int protocol_DIANBIAO_AnKeRui(struct acquisition_data * acq_data);

int protocol_others_process(struct acquisition_data *acq_data);
int protocol_others_processModbus(struct acquisition_data * acq_data);

int protocol_test_yuiki(struct acquisition_data * acq_data);

int protocol_PH_SinPh80(struct acquisition_data * acq_data);
int protocol_KMnO4_ZhuoZheng(struct acquisition_data *acq_data);

int protocol_level_LianCe(struct acquisition_data * acq_data);
int protocol_ShuXianYi_LianCe(struct acquisition_data * acq_data);
int protocol_level_MiKe(struct acquisition_data * acq_data);
int protocol_level_BS(struct acquisition_data * acq_data);
int protocol_level_SZHuaTian(struct acquisition_data *acq_data);
int protocol_level_MiKe_P260(struct acquisition_data *acq_data);

int protocol_sampler_BJGeRuiSiPu_FC9624YLAB(struct acquisition_data *acq_data);
int protocol_sampler_HZDaHu(struct acquisition_data *acq_data);
int protocol_sampler_HuaHouTianCheng_WS_A(struct acquisition_data *acq_data);
int protocol_sampler_HBShiJiaZhuang(struct acquisition_data *acq_data);
int protocol_sampler_WangWei_WQS2000(struct acquisition_data *acq_data);
int protocol_sampler_HBDeRun_DR803K(struct acquisition_data *acq_data);
int protocol_sampler_KeSheng(struct acquisition_data *acq_data);
int protocol_sampler_KeSheng_old(struct acquisition_data *acq_data);
int protocol_sampler_AHWanYi(struct acquisition_data *acq_data);
int protocol_sampler_HZJiYi_WQSEC(struct acquisition_data *acq_data);
int protocol_sampler_HuiHuan_DEK1302(struct acquisition_data *acq_data);

int protocol_MARK_8DI(struct acquisition_data *acq_data);
int protocol_MARK_JingNengReDian(struct acquisition_data *acq_data);
int protocol_MARK_JingNengReDian2(struct acquisition_data *acq_data);
int protocol_MARK_zz_3DI(struct acquisition_data *acq_data);
int protocol_MARK_GongKuangArg_modbus(struct acquisition_data *acq_data);
int protocol_MARK_GDWeiChuang(struct acquisition_data *acq_data);
int protocol_MARK_HeBeiZhongDian(struct acquisition_data *acq_data);
int protocol_MARK_JiuTian(struct acquisition_data *acq_data);
int protocol_MARK_SIS_ModbusTCP(struct acquisition_data *acq_data);
int protocol_MARK_ChengXinJiuTian(struct acquisition_data *acq_data);

int protocol_mark_GongKuang_Steel_staddr(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_FirePower_staddr(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_JiaoHua_coking_staddr(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_ShuiNi_cement_staddr(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_LaJi_fire_staddr(struct acquisition_data *acq_data);

int protocol_mark_GongKuang_Steel(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_FirePower(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_JiaoHua_coking(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_ShuiNi_cement(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_LaJi_fire(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_LaJi_fire_HBManDeKe(struct acquisition_data *acq_data);

int protocol_GongKuang_ChangRun(struct acquisition_data *acq_data);
int protocol_GongKuang_ShouQinLongHui(struct acquisition_data *acq_data);

int protocol_hjt2017_GongKuang_Steel(struct acquisition_data *acq_data);
int protocol_hjt2017_GongKuang_FirePower(struct acquisition_data *acq_data);
int protocol_hjt2017_GongKuang_JiaoHua_coking(struct acquisition_data *acq_data);
int protocol_hjt2017_GongKuang_ShuiNi_cement(struct acquisition_data *acq_data);
int protocol_hjt2017_GongKuang_LaJi_fire(struct acquisition_data *acq_data);

int protocol_mark_GongKuang_Steel_DI(struct acquisition_data *acq_data);
int protocol_mark_GongKuang_FirePower_DI(struct acquisition_data *acq_data);

int protocol_GongKuang_HBManDeKe_ChengFengReDian1(struct acquisition_data *acq_data);
int protocol_GongKuang_HBManDeKe_ChengFengReDian2(struct acquisition_data *acq_data);
int protocol_GongKuang_HBManDeKe_ChengFengReDian3(struct acquisition_data *acq_data);

int protocol_RFID_R200_16bytes(struct acquisition_data *acq_data);
int protocol_RFID_R200_8bytes(struct acquisition_data *acq_data);

int protocol_rdtu_t36z(struct acquisition_data *acq_data);


static struct k37_dev_instrument dev_instrument[DEV_MAX_NUM]={
};

static struct k37_instrument_protocols instrument_protocols[]={
	{1001, protocol_FLUX_jiubo_LS},
	{1002, protocol_FLUX_jiubo_M3H},
	{1003,protocol_FLUX_QDHuanKe},
	{1004, protocol_FLUX_WXqiuxin},
	{1005,protocol_FLUX_yiwen_LS},
	{1006,protocol_FLUX_JinYuan},
	{1008,protocol_FLUX_DLXiGeMa},
	{1009,protocol_FLUX_CQHuaZhengShuiWen},
	{1010,protocol_FLUX_GZDongWen},
	{1011,protocol_FLUX_GZHuiGu},
	{1013, protocol_FLUX_BJhuanke},
	{1015,protocol_FLUX_XiAnSanQuan},
	{1016,protocol_FLUX_DLBoKeSi},
	{1018, protocol_FLUX_Weike},
	{1019,protocol_FLUX_DLfulang},
	{1020,protocol_FLUX_HZNanKong},
	{1021,protocol_FLUX_WeiChuang},
	{1022,protocol_FLUX_CN311},
	{1023,protocol_FLUX_dongguan},
	{1024,protocol_FLUX_KeSheng},
	{1025,protocol_FLUX_MiKe},
	{1026,protocol_FLUX_Lmag},
	{1027,protocol_FLUX_ZhuoZheng},
	{1028,protocol_FLUX_MiKe2},
	{1029,protocol_FLUX_KFJinXiu},
	{1030,protocol_FLUX_WHZhongHe},
	{1031,protocol_FLUX_SHYingShenMingQu},
	{1032,protocol_FLUX_SHYingShen},
	{1033,protocol_FLUX_ZhuoZheng2},
	{1034,protocol_FLUX_WoLun},
	{1035,protocol_FLUX_NJShuiMengDianZi_SM10},
	{1036,protocol_FLUX_HZMeiKong},
	{1037,protocol_FLUX_HuaXun},
	{1038,protocol_FLUX_SHFanYang},
	{1039,protocol_FLUX_CN311_old},
	{1040,protocol_FLUX_ZhuoZheng3},
	{1041, protocol_FLUX_KeLongIFC050},
	{1042, protocol_FLUX_KeLongIFC050_ZhuoZheng},
	{1043,protocol_FLUX_ZhuoZheng4},
	{1044,protocol_FLUX_TaiChuang_CT5201},
	{1045,protocol_FLUX_dongguan2},
	{1046,protocol_FLUX_PD_880B},
	
	{1056, protocol_FLUX_HeBeiErDai},
	{1057, protocol_FLUX_NeiMengGu},					/*yuiki 20191228*/
	{1058, protocol_FLUX_TUF2000},
	{1059, protocol_FLUX_DLBoKeSi_104M},
	
	{1070,protocol_FLUX_JMHuangYu},
	{1071,protocol_FLUX_Lmag_in},
	{1072,protocol_FLUX_Lmag_out},
	{1073,protocol_FLUX_Lmag_handle},
	{1074,protocol_FLUX_Lmag_huiChanSheng},
	{1075,protocol_FLUX_Lmag_huiShiYong},

	{1080,protocol_FLUX_LiDe},
	{1081,protocol_FLUX_SHGuangHua},
	{1082,protocol_FLUX_MBmag},
	{1083,protocol_FLUX_HBTianYun},
	{1084,protocol_FLUX_TDS100},
	
	{1100,protocol_FLUX_XinJiang},
	{1101,protocol_FLUX_HuBei},
	{1102,protocol_FLUX_GuoBiao212},
	{1201,protocol_FLUX_DLFuLangX2},
	{1301,protocol_FLUX_AnJun},
	{1302,protocol_FLUX_DongNan},
	{1303,protocol_FLUX_GuangHua},
	{1304,protocol_FLUX_JinXi},
	{1305,protocol_FLUX_KenTe},
	{1306,protocol_FLUX_VOTEC_VT2000},
	{1350,protocol_FLUX_DuoPuLe},

	{1355,protocol_FLUX_JSJinBiao_in},					/*yuiki 20200118 JSjinbiao*/
	{1356,protocol_FLUX_JSJinBiao_out},					/*yuiki 20200118 JSjinbiao*/
	{1357,protocol_FLUX_JSJinBiao_hui},					/*yuiki 20200118 JSjinbiao*/
	{1358,protocol_FLUX_JSJinBiao_shenghuo},			/*yuiki 20200430 JSjinbiao*/
	{1359,protocol_FLUX_BJRuiPuSanYuan_ULFM_A},
	{1360,protocol_FLUX_JiuLian_in},						/*yuiki 20200114*/
	{1361,protocol_FLUX_JiuLian_out},					/*yuiki 20200114*/
	{1362,protocol_FLUX_JiuLian_hui},						/*yuiki 20200114*/
	{1363,protocol_FLUX_BuLuKeSi},
	{1364,protocol_FLUX_CangZhouTianYi},
	
	{1406,protocol_FLUX_HBKeruida},
	{1407,protocol_FLUX_HBHuaHou},


	{2001,protocol_COD_LianHua5B5},
	{2002,protocol_COD_SHAiHua},
	{2003,protocol_COD_HNQianZhengJHC},
	{2004,protocol_COD_WXChuangChen},
	{2005,protocol_COD_NJGangNeng},
	{2006,protocol_COD_BJYongXing},
	{2007,protocol_COD_SZZhongXing},
	{2008,protocol_COD_SongXiaYi},
	{2009,protocol_COD_SongXiaEr},
	{2010,protocol_COD_SongXiaSan},
	{2011,protocol_COD_SongXiaSi},
	{2012,protocol_COD_XiSiDi},
	{2013,protocol_COD_JSTianRui2},
	{2014,protocol_COD_JSTianRui1},
	{2015,protocol_COD_GermanWTW3520},
	{2016,protocol_COD_SZLangShi},
	{2017,protocol_COD_ZJHZWeiLan},
	{2018,protocol_COD_HBWHtkc},
	{2019,protocol_COD_ZJHZWeiLanD01},					/*yuiki 20190925*/
	{2020,protocol_COD_CZShengLiHong},
	{2021,protocol_COD_JSDeLin},
	{2022,protocol_COD_GXGLYunJin},						/*yuiki 20190925 pengyue*/
	{2023,protocol_COD_LvYe},							/*yuiki 20191220*/
	{2024,protocol_COD_USAhaxi},						/*yuiki 20191224 zhengqi*/
	{2025,protocol_COD_NJGangNeng_HeBei},				/*yuiki 20190925*/
	{2026,protocol_COD_GXGLYunJin},						/*yuiki 20191226*/
	{2027,protocol_COD_USAhaxiDynamic},
	{2028,protocol_COD_GBhjt2005},
	{2029,protocol_COD_GBhjt2017},
	{2030,protocol_COD_WHSiFuMing},					/*yuiki 20191206*/
	{2031,protocol_COD_ChuangZao},						/*yuiki 20191220*/
	{2032,protocol_COD_NeiMengGu},						/*yuiki 20191228*/
	{2033,protocol_COD_JMWeiChuang},
	{2034,protocol_COD_CE},
	{2035,protocol_COD_HuiHuang},
	{2036,protocol_COD_SZZhongXingModbus},
	{2037,protocol_COD_WHJunDe},
	{2038,protocol_COD_AmtaxInter2C},
	{2039,protocol_COD_GZYiWenModbus},
	{2040,protocol_COD_NJHongGuang},
	{2041,protocol_COD_HBHuaHouTianCheng},
	{2042,protocol_COD_ZeTian},
	{2043,protocol_COD_JuGuang_HeBei},
	{2044,protocol_COD_HZLuHeng_HeBei},
	{2045,protocol_COD_Thermo_orion3106},
	{2046,protocol_water_GLYunJing_HeBei},
	{2047,protocol_COD_DaoJin_HeBei},
	{2048,protocol_COD_GDWeiChuang_HeBei},
	{2049,protocol_COD_JSLingHeng},
	{2050,protocol_COD_NJJuGe},
	{2051,protocol_COD_USAHaXi_maxII},
	{2052,protocol_COD_GZYiWen_HeBei},
	{2053,protocol_COD_BoKeSi},
	{2054,protocol_COD_AHWanYi},
	{2055,protocol_COD_BJHuanKe_HeBei},
	{2056,protocol_COD_HBHuaHouTianCheng_new},
	{2057,protocol_COD_ZhongLv},
	{2058,protocol_COD_LiDa},
	{2059,protocol_COD_ZhongJieNengTianRong},
	{2060,protocol_COD_NJHongGuang2},
	{2061,protocol_COD_JSZhuoZheng_HeBei},
	{2062,protocol_COD_USAHaXi_maxII_New},
	{2063,protocol_COD_GJSurfaceWater},
	{2064,protocol_COD_SZLangShi_PhotoTek6000},
	{2065,protocol_COD_ZhengQi_Modbus},
	{2066,protocol_COD_JSTianZe},
	{2067,protocol_COD_JSRuiQuanRenQ_IV},
	{2068,protocol_COD_NJJuGe_New},
	{2069,protocol_COD_HeBeiHuaHouTianCheng_E},
	{2070,protocol_COD_BJLiDa},
	{2071,protocol_COD_GLYunJing_HeBei_Stay},

	{2099,protocol_COD_HuiHuan_New},
	{2100,protocol_COD_GZyiwenDynamic},				/*yuiki 20190925*/
	{2101,protocol_COD_USAhaxi},
	{2102,protocol_COD_GZyiwen},
	{2103,protocol_COD_HNHuaShiJie},
	{2104,protocol_COD_SZYuXing1},
	{2108,protocol_COD_JuGuangKeJi},
	{2109,protocol_COD_BJhuanke},
	{2110,protocol_COD_NJhongkai},
	{2111,protocol_COD_NJhongkaiModbus}, 				/*yuiki 20190925*/
	{2112,protocol_COD_HNHuaShiJieModbus},				/*yuiki 20191228*/
	{2113,protocol_COD_NJruiquan},
	{2114,protocol_COD_NJRuiQuanModbus},				/*yuiki 20190925*/
	{2122,protocol_COD_NJdelin},	
	{2123,protocol_COD_YuXing_HeBei},
	{2124,protocol_water_NJXinRuiPeng},
	{2125,protocol_COD_WXdianchuang},
	{2133,protocol_COD_SZkete},
	{2152,protocol_COD_BJhoutian},
	{2153,protocol_COD_QDJiaMing},
	{2154,protocol_COD_HZfuming},
	{2155,protocol_COD_DaoJin4100CN},
	{2156,protocol_COD_DaoJin4200CN},		        
	{2161,protocol_COD_ShiHuiLin},
	{2162,protocol_COD_HeBei},
	{2163,protocol_COD_ZhuoZheng2},
	{2164,protocol_COD_EndressHauser},
	{2165,protocol_COD_EndressHauser_CA80},

	{2200,protocol_COD_ZeTian_GBhjt2005},
	{2201,protocol_COD_ZJHZLiQi_V2},
	{2202,protocol_COD_NJXinRuiPeng_New},

	{2266,protocol_COD_HBKeruida},

	{2956,protocol_COD_Certificate},


	{3001,protocol_ANDAN_USAhaxi},
	{3002,protocol_ANDAN_GZyiwen},
	{3003,protocol_ANDAN_ZhuoZheng},
	{3004,protocol_ANDAN_ZhuoZheng2},
	{3005,protocol_ANDAN_LZLianHua},
	{3006,protocol_ANDAN_NJGangNeng},
	{3007,protocol_ANDAN_HZfuming},
	{3008,protocol_ANDAN_GZyingsi},
	{3009,protocol_ANDAN_JuGuangKeJi},
	{3010,protocol_ANDAN_NJRuiQuan},
	{3011,protocol_ANDAN_HLTianCheng},
	{3012,protocol_ANDAN_SZZhongXing},
	{3013,protocol_ANDAN_ZJHZWeiLan},
	{3014,protocol_ANDAN_GXGLYunJin},					/*yuiki 20190925*/
	{3015,protocol_ANDAN_NJRuiQuanModbus},				/*yuiki 20190925*/
	{3016,protocol_ANDAN_ChuangZao},					/*yuiki 20191220*/
	{3017,protocol_ANDAN_USAhaxi},						/*yuiki 20191224 zhengqi*/
	{3018,protocol_ANDAN_ZhuoZheng},					/*yuiki 20191216 shihuilin*/
	{3019,protocol_ANDAN_GXGLYunJin},					/*yuiki 20191226*/
	{3020,protocol_ANDAN_NJhongkai},					/*yuiki 20191226*/
	{3021,protocol_ANDAN_SongXiaYi},
	{3022,protocol_ANDAN_SongXiaEr},
	{3023,protocol_ANDAN_SongXiaSan},
	{3024,protocol_ANDAN_SongXiaSi},
	{3025,protocol_ANDAN_LvYe},							/*yuiki 20200306*/
	{3026,protocol_ANDAN_JSDeLin},
	{3027,protocol_ANDAN_CZShengLiHong},
	{3028,protocol_ANDAN_USAhaxiDynamic},
	{3029,protocol_ANDAN_GBhjt2005},
	{3030,protocol_ANDAN_GBhjt2017},
	{3031,protocol_ANDAN_JMWeiChuang},
	{3032,protocol_ANDAN_CE},
	{3033,protocol_ANDAN_HuiHuang},
	{3034,protocol_ANDAN_SZZhongXingModbus},
	{3035,protocol_ANDAN_WHJunDe},
	{3036,protocol_ANDAN_SZPengYue},
	{3037,protocol_ANDAN_AmtaxInter2C},
	{3038,protocol_ANDAN_GZYiWenModbus},
	{3039,protocol_ANDAN_NA8000},
	{3040,protocol_ANDAN_NJHongGuang},
	{3041,protocol_ANDAN_HBHuaHouTianCheng},
	{3042,protocol_ANDAN_ZJHZWeiLanD01},
	{3043,protocol_ANDAN_ZeTian},
	{3044,protocol_ANDAN_JuGuang_HeBei},
	{3045,protocol_ANDAN_HZLuHeng_HeBei},
	{3046,protocol_ANDAN_Thermo_orion2240},
	{3047,protocol_water_GLYunJing_HeBei},
	{3048,protocol_ANDAN_DaoJin_HeBei},
	{3049,protocol_ANDAN_GDWeiChuang_HeBei},
	{3050,protocol_ANDAN_JSLingHeng},
	{3051,protocol_ANDAN_NJJuGe},
	{3052,protocol_ANDAN_GZYiWen_HeBei},
	{3053,protocol_ANDAN_BoKeSi},
	{3054,protocol_ANDAN_AHWanYi},
	{3055,protocol_ANDAN_NJGangNeng_HeBei},
	{3056,protocol_ANDAN_BJHuanKe_HeBei},
	{3057,protocol_ANDAN_DaoJin_4210},
	{3058,protocol_ANDAN_HBHuaHouTianCheng_new},
	{3059,protocol_ANDAN_ZhongLv},
	{3060,protocol_ANDAN_LiDa},
	{3061,protocol_ANDAN_ZhongJieNengTianRong},
	{3062,protocol_ANDAN_NJHongGuang2},
	{3063,protocol_ANDAN_EndressHauser},
	{3064,protocol_ANDAN_JSZhuoZheng_HeBei},
	{3065,protocol_ANDAN_XiSiDi},
	{3066,protocol_ANDAN_JSTianRui2},
	{3067,protocol_ANDAN_JSTianRui1},
	{3068,protocol_ANDAN_USAhaxiNA8000},
	{3069,protocol_ANDAN_GermanWTW},
	{3070,protocol_ANDAN_QDJiaMing},
	{3071,protocol_ANDAN_USAhaxi3001},
	{3072,protocol_ANDAN_USAhaxi3001},
	{3073,protocol_ANDAN_GJSurfaceWater},
	{3074,protocol_ANDAN_SZLangShi_PhotoTek6000},
	{3075,protocol_ANDAN_ZhengQi_Modbus},
	{3076,protocol_ANDAN_JSTianZe},
	{3077,protocol_ANDAN_JSRuiQuanRenQ_IV},
	{3078,protocol_ANDAN_EndressHauser_CA80},
	
	{3080,protocol_ANDAN_HNHuaShiJie},
	{3081,protocol_ANDAN_HNHuaShiJieModbus},			/*yuiki 20191228*/
	{3082,protocol_ANDAN_USAHaXi_maxII_New},

	{3084,protocol_ANDAN_NJJuGe_New},
	{3085,protocol_ANDAN_HeBeiHuaHouTianCheng_E},
	{3086,protocol_ANDAN_BJLiDa},
	{3087,protocol_ANDAN_GLYunJing_HeBei_Stay},
	
	{3100,protocol_ANDAN_HeBei},
	{3101,protocol_ANDAN_NeiMengGu},					/*yuiki 20191228*/
	{3110,protocol_ANDAN_WHSiFuMing},					/*yuiki 20191126*/
	{3111,protocol_ANDAN_ZJHZLiQi_V2},
	{3112,protocol_ANDAN_NJXinRuiPeng_New},

	{3120,protocol_ANDAN_HBKeruida},
	{3121,protocol_ANDAN_YuXing_HeBei},
	{3122,protocol_water_NJXinRuiPeng},
	{3123,protocol_ANDAN_BJSaiLaiMo},

	{3200,protocol_ANDAN_ZeTian_GBhjt2005},

	{4001,protocol_Lin_GZYiWenEST2003},
	{4002,protocol_Lin_HZFuMing},
	{4003,protocol_Lin_SXZhongLv},
	{4004,protocol_Lin_JSTianRui1},
	{4005,protocol_ZONGLIN_GBhjt2005},
	{4006,protocol_ZONGLIN_GBhjt2017},
	{4007,protocol_ZONGLIN_LvYe},						/*yuiki 20200306*/				
	{4008,protocol_ZONGLIN_NJhongkai},
	{4009,protocol_Lin_USAHaXi},
	{4010,protocol_Lin_N_SHJingFang},
	{4011,protocol_Lin_SongXiaYi},
	{4012,protocol_Lin_SongXiaEr},
	{4013,protocol_Lin_SongXiaSan},
	{4014,protocol_Lin_SongXiaSi},
	{4015,protocol_Lin_USAHaXi},							/*yuiki 20191224 zhengqi*/
	{4016,protocol_Lin_JSTianRui2},
	{4017,protocol_ZONGLIN_CZShengLiHong},
	{4018,protocol_ZONGLIN_ChuangZao},
	{4019,protocol_ZONGLIN_JMWeiChuang},
	{4020,protocol_ZONGLIN_CE},
	{4021,protocol_ZONGLIN_HuiHuang},
	{4022,protocol_ZONGLIN_SZZhongXingModbus},
	{4023,protocol_ZONGLIN_AmtaxInter2C},
	{4024,protocol_ZONGLIN_USAhaxiDynamic},
	{4025,protocol_ZONGLIN_GZYiWenModbus},
	{4026,protocol_ZONGLIN_NJHongGuang},
	{4027,protocol_ZONGLIN_ZJHZWeiLanD01},
	{4028,protocol_ZONGLIN_ZeTian},
	{4029,protocol_ZONGLIN_JuGuang_HeBei},
	{4030,protocol_ZONGLIN_HZLuHeng_HeBei},
	{4031,protocol_water_GLYunJing_HeBei},
	{4032,protocol_ZONGLIN_HBHuaHouTianCheng},
	{4033,protocol_ZONGLIN_GDWeiChuang_HeBei},
	{4034,protocol_ZONGLIN_JSLingHeng},
	{4035,protocol_ZONGLIN_NJJuGe},
	{4036,protocol_ZONGLIN_GZYiWen_HeBei},
	{4037,protocol_ZONGLIN_BoKeSi},
	{4038,protocol_ZONGLIN_AHWanYi},
	{4039,protocol_ZONGLIN_NJGangNeng_HeBei},
	{4040,protocol_ZONGLIN_DaoJin_4200},
	{4041,protocol_ZONGLIN_BJHuanKe_HeBei},
	{4042,protocol_ZONGLIN_HBHuaHouTianCheng_new},
	{4043,protocol_ZONGLIN_ZhongLv},
	{4044,protocol_ZONGLIN_LiDa},
	{4045,protocol_others_TPTN_UASHaXi_NPW160},
	{4046,protocol_TNTP_Thermo},
	{4047,protocol_ZONGLIN_JSZhuoZheng_HeBei},
	{4048,protocol_ZONGLIN_USAHaXi_maxII_New},
	{4049,protocol_ZONGLIN_ZhongJieNengTianRong},
	{4050,protocol_ZONGLIN_HBKeruida},
	{4051,protocol_ZONGLIN_YuXing_HeBei},
	{4052,protocol_water_NJXinRuiPeng},
	{4053,protocol_ZONGLIN_QDJiaMing},
	{4054,protocol_ZONGLIN_SZLangShi_PhotoTek6000},
	{4055,protocol_ZONGLIN_GJSurfaceWater},
	{4056,protocol_ZONGLIN_ZhengQi_Modbus},
	{4057,protocol_ZONGLIN_JSTianZe},
	{4058,protocol_ZONGLIN_JSRuiQuanRenQ_IV},
	{4059,protocol_ZONGLIN_NJJuGe_New},
	{4060,protocol_ZONGLIN_HeBeiHuaHouTianCheng_E},
	{4061,protocol_ZONGLIN_BJLiDa},
	{4062,protocol_ZONGLIN_GLYunJing_HeBei_Stay},

	{4100,protocol_ZONGLIN_NJHongGuang2},
	{4101,protocol_ZONGLIN_ZJHZLiQi_V2},
	{4102,protocol_ZONGLIN_NJXinRuiPeng_New},
	
	{4200,protocol_ZONGLIN_ZeTian_GBhjt2005},
	
	{4700,protocol_ZONGLIN_ZhuoZheng},
	{4701,protocol_ZONGLIN_HeBei},
	{4702,protocol_ZONGLIN_ZhuoZheng2},
	{4703,protocol_ZONGLIN_GXGLYunJin},					/*yuiki 20190925*/
	{4704,protocol_ZONGLIN_NJRuiQuanModbus},			/*yuiki 20190925*/
	{4705,protocol_ZONGLIN_ZhuoZheng},					/*yuiki 20191226 shihuilin*/
	{4706,protocol_ZONGLIN_GXGLYunJin},					/*yuiki 20191227*/
	{4707,protocol_ZONGLIN_NeiMengGu},					/*yuiki 20191228*/
	{4708,protocol_ZONGLIN_QianDa},						/*yuiki 20200624*/
	
	{4710,protocol_ZONGLIN_HNHuaShiJieModbus},			/*yuiki 20191228*/


	{5001,protocol_ZONGDAN_HNHuaShiJieModbus},		/*yuiki 20191228*/
	{5002,protocol_ZONGDAN_LvYe},						/*yuiki 20200306*/
	{5003,protocol_ZONGDAN_CZShengLiHong},
	{5004,protocol_ZONGDAN_ChuangZao},
	{5005,protocol_ZONGDAN_GBhjt2005},
	{5006,protocol_ZONGDAN_GBhjt2017},
	{5007,protocol_ZONGDAN_JMWeiChuang},
	{5008,protocol_ZONGDAN_HuiHuang},
	{5009,protocol_ZONGDAN_SZZhongXingModbus},
	{5010,protocol_ZONGDAN_AmtaxInter2C},
	{5011,protocol_ZONGDAN_USAhaxiDynamic},
	{5012,protocol_ZONGDAN_GZYiWenModbus},
	{5013,protocol_ZONGDAN_NJHongGuang},
	{5014,protocol_ZONGDAN_ZJHZWeiLanD01},
	{5015,protocol_ZONGDAN_ZeTian},
	{5016,protocol_ZONGDAN_JuGuang_HeBei},
	{5017,protocol_ZONGDAN_HZLuHeng_HeBei},
	{5018,protocol_water_GLYunJing_HeBei},
	{5019,protocol_ZONGDAN_HBHuaHouTianCheng},
	{5020,protocol_ZONGDAN_GDWeiChuang_HeBei},
	{5021,protocol_ZONGDAN_JSLingHeng},
	{5022,protocol_ZONGDAN_NJJuGe},
	{5023,protocol_others_TPTN_DaoJin_HeBei},
	{5024,protocol_ZONGDAN_GZYiWen_HeBei},
	{5025,protocol_ZONGDAN_BoKeSi},
	{5026,protocol_ZONGDAN_AHWanYi},
	{5027,protocol_ZONGDAN_DaoJin_4200},
	{5028,protocol_ZONGDAN_BJHuanKe_HeBei},
	{5029,protocol_ZONGDAN_HBHuaHouTianCheng_new},
	{5030,protocol_ZONGDAN_ZhongLv},
	{5031,protocol_ZONGDAN_LiDa},
	{5032,protocol_ZONGDAN_ZhongJieNengTianRong},
	{5033,protocol_ZONGDAN_JSRuiQuanRenQ_IV},
	{5034,protocol_ZONGDAN_JSZhuoZheng_HeBei},
	{5035,protocol_ZONGDAN_USAHaXi_maxII_New},

	{5037,protocol_ZONGDAN_NJHongGuang2},
	{5038,protocol_ZONGDAN_ZJHZLiQi_V2},
	{5039,protocol_ZONGDAN_NJJuGe_New},
	{5040,protocol_ZONGDAN_HeBeiHuaHouTianCheng_E},
	{5041,protocol_ZONGDAN_BJLiDa},
	{5042,protocol_ZONGDAN_GLYunJing_HeBei_Stay},
	{5043,protocol_ZONGDAN_NJXinRuiPeng_New},

	{5050,protocol_ZONGDAN_HBKeruida},
	{5051,protocol_ZONGDAN_YuXing_HeBei},
	{5052,protocol_water_NJXinRuiPeng},
	{5053,protocol_ZONGDAN_QDJiaMing},
	{5054,protocol_ZONGDAN_SZLangShi_PhotoTek6000},
	{5055,protocol_ZONGDAN_ZhengQi_Modbus},
	{5056,protocol_ZONGDAN_JSTianZe},

	{5200,protocol_ZONGDAN_ZeTian_GBhjt2005},
	{5201,protocol_water_YuXing_Modbus},
	
	{5500,protocol_ZONGDAN_ZhuoZheng},
	{5501,protocol_ZONGDAN_HeBei},
	{5502,protocol_ZONGDAN_AHYingKai},
	{5503,protocol_ZONGDAN_ZhuoZheng2},
	{5504,protocol_ZONGDAN_ZhuoZheng},					/*yuiki 20191226 shihuilin*/
	{5505,protocol_ZONGDAN_NJRuiQuanModbus},			/*yuiki 20191228*/
	{5506,protocol_ZONGDAN_NeiMengGu},					/*yuiki 20191228*/
	{5601,protocol_ZONGDAN_JSTianRui2},


	{6000,protocol_ZONGNIE_ZhuoZheng},					/*yuiki 20191226 shihuilin*/
	{6001,protocol_ZONGNIE_ZhuoZheng},
	{6002,protocol_ZONGNIE_HeBei},
	{6003,protocol_ZONGNIE_ZhuoZheng2},
	{6004,protocol_NI_NeiMengGu},						/*yuiki 20191228*/
	{6005,protocol_Cu_NJGangNeng},
	{6006,protocol_Ni_NJGangNeng},
	
	{6013,protocol_CHROME_GZyingsi},
	
	{6018,protocol_NICKEL_GZyingsi},
	{6019,protocol_Cu_JMWeiChuang},
	{6020,protocol_LIUJIAGE_JMWeiChuang},
	{6021,protocol_Cr_JMWeiChuang},
	{6022,protocol_ZONGNIE_JMWeiChuang},
	{6023,protocol_Cd_JMWeiChuang},
	{6024,protocol_Pb_JMWeiChuang},
	{6025,protocol_Fe_JMWeiChuang},
	{6026,protocol_Zn_JMWeiChuang},
	{6027,protocol_As_JMWeiChuang},
	{6028,protocol_Mn_JMWeiChuang},

	{6050,protocol_LiuJiaCr_NJJuGe},
	
	{6100,protocol_TONG_ZhuoZheng},
	{6101,protocol_TONG_ZhuoZheng2},
	{6102,protocol_TONG_ZhuoZheng},					/*yuiki 20191226 shihuilin*/
	{6200,protocol_ZONGGE_ZhuoZheng},
	{6201,protocol_ZONGGE_ZhuoZheng2},
	{6203,protocol_ZONGGE_ZhuoZheng},					/*yuiki 20191226 shihuilin*/
	{6300,protocol_ZONGQUAN_ZhuoZheng},
	{6301,protocol_ZONGQUAN_ZhuoZheng2},
	{6302,protocol_ZONGQUAN_ZhuoZheng_UGL},				/*yuiki 20191226 shihuilin*/
	{6400,protocol_LIUJIAGE_HeBei},
	{6401,protocol_LIUJIAGE_HNHuaShiJie},
	{6402,protocol_LIUJIACR_NeiMengGu},					/*yuiki 2019128*/

	{6503,protocol_CR_GZYiWen},
	{6507,protocol_Ni_HNHuaShiJie},
	{6508,protocol_NI_GZYiWen},
	{6509,protocol_CU_GZYiWen},
	{6510,protocol_AS_NJRuiQuanModbus},
	{6511,protocol_CD_NJRuiQuanModbus},
	{6512,protocol_CR_NJRuiQuanModbus},
	{6513,protocol_CU_NJRuiQuanModbus},
	{6514,protocol_MN_NJRuiQuanModbus},
	{6515,protocol_NI_NJRuiQuanModbus},
	{6516,protocol_PB_NJRuiQuanModbus},
	{6517,protocol_ZN_NJRuiQuanModbus},
	{6518,protocol_LIUJIACR_NJRuiQuanModbus},
	{6529,protocol_CU_JuGuangKeJi},
	{6530,protocol_ZN_JuGuangKeJi},
	{6531,protocol_CD_JSTianRui2},
	{6532,protocol_PB_JSTianRui2},
	{6533,protocol_NI_JSTianRui2},
	{6534,protocol_CU_JSTianRui2},
	{6535,protocol_ZN_JSTianRui2},
	{6536,protocol_FE_JSTianRui2},


	{6541,protocol_AS_SongXiaSyi},
	{6542,protocol_As_SongXiaSEr},
	{6543,protocol_As_SongXiaSSan},
	{6544,protocol_As_SongXiaSSi},
	{6546,protocol_CD_SongXiaSYi},
	{6547,protocol_CD_SongXiaSEr},
	{6548,protocol_CD_SongXiaSSan},
	{6549,protocol_CD_SongXiaSSi},
	{6551,protocol_CR_SongXiaSYi},
	{6552,protocol_CR_SongXiaSEr},
	{6553,protocol_CR_SongXiaSSan},
	{6554,protocol_CR_SongXiaSSi},
	{6556,protocol_LiuJiaCR_SongXiaSYi},
	{6557,protocol_LiuJiaCR_SongXiaSEr},
	{6558,protocol_LiuJiaCR_SongXiaSSan},
	{6559,protocol_LiuJiaCR_SongXiaSSi},
	{6561,protocol_CU_SongXiaSYi},
	{6562,protocol_CU_SongXiaSEr},
	{6563,protocol_CU_SongXiaSSan},
	{6564,protocol_CU_SongXiaSSi},
	{6566,protocol_PB_SongXiaSYi},
	{6567,protocol_PB_SongXiaSEr},
	{6568,protocol_PB_SongXiaSSan},
	{6569,protocol_PB_SongXiaSSi},
	{6571,protocol_MN_SongXiaSYi},
	{6572,protocol_MN_SongXiaSEr},
	{6573,protocol_MN_SongXiaSSan},
	{6574,protocol_MN_SongXiaSSi},
	{6576,protocol_NI_SongXiaSYi},
	{6577,protocol_NI_SongXiaSEr},
	{6578,protocol_NI_SongXiaSSan},
	{6579,protocol_NI_SongXiaSSi},
	{6581,protocol_ZN_SongXiaSYi},
	{6582,protocol_ZN_SongXiaSEr},
	{6583,protocol_ZN_SongXiaSSan},
	{6584,protocol_ZN_SongXiaSSi},        

	{6591,protocol_CD_SZZhongXing},
	{6592,protocol_PB_SZZhongXing},
	{6593,protocol_NI_SZZhongXing},
	{6594,protocol_CU_SZZhongXing},
	{6595,protocol_ZN_SZZhongXing},
	{6596,protocol_MN_SZZhongXing},

	{6600,protocol_Pb_QDJiaMing},
	{6601,protocol_Pb_SZLangShi_PhotoTek6000},
	{6602,protocol_Zn_SZLangShi_PhotoTek6000},
	
	{7001,protocol_modtcp_NET_4013M1},
	{7002,protocol_modtcp_NET_4013M2},
	{7003,protocol_others_process},

	{7015,protocol_modtcp_DCS_GongKuang},
	{7016,protocol_modtcp_DCS_GongKuang2},
	{7017,protocol_modtcp_DCS_GongKuang3},
	{7018,protocol_modtcp_DCS_GongKuang4},
	{7019,protocol_modtcp_SIS_ModbusTCP},
	{7020,protocol_GongKuang_HBManDeKe_ChengFengReDian1},
	{7021,protocol_GongKuang_HBManDeKe_ChengFengReDian2},
	{7022,protocol_GongKuang_HBManDeKe_ChengFengReDian3},
	
	{7350,protocol_modtcp_DCS_GongKuang_modbus},
	

	{7400,protocol_door_HaiKang_DSK1T641M},

	{7502,protocol_sampler_BJGeRuiSiPu_FC9624YLAB},
	{7503,protocol_sampler_HZDaHu},
	{7504,protocol_sampler_HuaHouTianCheng_WS_A},
	{7505,protocol_sampler_HBShiJiaZhuang},
	{7506,protocol_sampler_WangWei_WQS2000},
	{7507,protocol_sampler_HBDeRun_DR803K},
	{7508,protocol_sampler_KeSheng},
	{7509,protocol_sampler_KeSheng_old},
	{7510,protocol_sampler_AHWanYi},
	{7511,protocol_sampler_HZJiYi_WQSEC},
	{7512,protocol_sampler_HuiHuan_DEK1302},

	{8001,protocol_CEMS_BJxuedilong},
	{8002,protocol_CEMS_SHbsjHuiGu},
	{8003,protocol_CEMS_WHTianHong},
	{8004,protocol_CEMS_BJxuedilong_fujian},
	{8005,protocol_CEMS_TLD_PLC},
	{8006,protocol_CEMS_WeiChuang},
	{8007,protocol_CEMS_BJxuedilongModbus},
	{8008,protocol_CEMS_ZeTian_HeBeiModbus},
	{8009,protocol_CEMS_SZCuiYunGu_TL_PMM180},
	{8010,protocol_CEMS_HBManDeKe},
	{8011,protocol_TSP_HBManDeKe},
	{8012,protocol_TSP_HBManDeKe2},
	{8013,protocol_TSP_AnRomhXin},
	{8014,protocol_CEMS_GBhjt2017},
	{8015,protocol_CEMS_GBhjt2005},
	{8016,protocol_CEMS_GBhjt2017_ChenduLePan},
	{8017,protocol_CEMS_GBhjt2017_ChenduLePanXJ},
	{8018,protocol_CEMS_JuGuang_HeBei},
	{8019,protocol_TSP_JuGuang_HeBei},
	{8020,protocol_CEMS_QDJiaMing},
	{8021,protocol_cems_JuGuang_ai},
	{8022,protocol_CEMS_AHWanYi_HeBei},
	{8023,protocol_CEMS_BJXueDiLong_scs900uv},
    {8024,protocol_PLC_BJXueDiLong_scs900uv},
	{8025,protocol_CEMS_SDdongTai_G3000_UV},
	{8026,protocol_PLC_BJXueDiLong_scs900c},
	{8027,protocol_CEMS_SaiMoFei_42i_HI},
	{8028,protocol_CEMS_SaiMoFei_42i_LO},
	{8029,protocol_CEMS_BJXueDiLong_MODEL1080},
	{8030,protocol_CEMS_SZCuiYunGu_TL_PMM180_2},
	{8031,protocol_CEMS_JuGuang_HeBei_JinDing},
	{8032,protocol_cems_JuGuang_ai_JinDing},
	{8033,protocol_CEMS_BJXueDiLong_U23},
	{8034,protocol_CEMS_SaiMoFei_43i_HI},
	{8035,protocol_CEMS_SaiMoFei_43i_LO},
	{8036,protocol_CEMS_SZCuiYunGu_TL_PMM180_3},
	{8037,protocol_PLC_BJXueDiLong_200},
	{8038,protocol_CEMS_BJHangTianYiLai},
	{8039,protocol_PLC_BJHangTianYiLai},
	{8040,protocol_CEMS_CQChuanYiModBus_PS7400_F},
	{8041,protocol_PLC_CQChuanYiModBus_PS7400_F},
	{8042,protocol_CEMS_SaiMoFei_TXO_1000},
	{8043,protocol_PLC_SaiMoFei},
	{8044,protocol_TSP_SaiMoFei_PM},
	{8045,protocol_PLC_SaiMoFei_not_flag},
	{8046,protocol_CEMS_JuGuang_HeBei_2000D_SO2},
	{8047,protocol_CEMS_JuGuang_HeBei_2000D_NOx},
	{8048,protocol_TSP_BJXueDiLong_Model2030},
	{8049,protocol_cems_JuGuang_HeBei_PLC_2000D},
	{8050,protocol_CEMS_HBKaiErUniMAT120},
	{8051,protocol_PLC_HBKaiErUniMAT120},
	{8052,protocol_CEMS_SZCuiYunGu_TL_PMM180_Low},
	{8053,protocol_CEMS_AnRongXin_AGA1000},
    {8054,protocol_CEMS_HeBei_M3H},
	{8055,protocol_CEMS_HeBei_KPA},
	{8056,protocol_CEMS_HeBei},
	{8057,protocol_CEMS_NeiMengGu},					/*yuiki 20191228*/
	{8058,protocol_CEMS_HBHeFeng},
	{8059,protocol_CEMS_HBHeFengCEMS},
	{8060,protocol_CEMS_garbage},						/*yuiki 20191015*/
	{8061,protocol_CEMS_garbage_modbus},				/*yuiki 20191015*/
	{8062,protocol_CEMS_garbage_modbus_s27I},			/*yuiki 20191015*/
	{8063,protocol_CEMS_garbage_modbus_s3bII},			/*yuiki 20191015*/
	{8065,protocol_CEMS_BJzhongdian},					/*yuiki 20191015*/                
	{8066,protocol_CEMS_XinJiang},
	{8067,protocol_CEMS_Furnace_modbus},				/*yuiki 20191015*/
	{8069,protocol_CEMS_AHwanyi},						/*yuiki 20191015*/
	{8070,protocol_TSP_ZhongDianXingYe},
	{8071,protocol_CEMS_DAMASCII},						/*yuiki 20191015*/  
	{8072,protocol_CEMS_LaJi_pack_HeBeiXT},
	{8073,protocol_CEMS_LaJi_pack_HeBeiGRST},
	{8074,protocol_CEMS_LaJi_pack_HeBeiGRST_GongKuang},
	{8075,protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000},
	{8076,protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_PLC},
	{8077,protocol_O2_ZeTian_HeBei},
	{8078,protocol_PLC_ZhongDianXingYe},
	{8079,protocol_TSP_JuGuang_SynspecPM},
	{8080,protocol_CEMS_GermanySick_pack1},
	{8081,protocol_CEMS_GermanySick_pack2},
	{8082,protocol_CEMS_GermanySick_pack3},
	{8083,protocol_CEMS_ZeTian_HeBeiModbus_old},
	{8084,protocol_CEMS_LaJi_pack_HBZhongJieNengDCS},
	{8085,protocol_PLC_CEMS_ZhuoMeiKeJi},
	{8086,protocol_CEMS_SaiMoFei1_42i_HI},
	{8087,protocol_CEMS_SaiMoFei1_42i_LO},
	{8088,protocol_CEMS_SaiMoFei1_43i_HI},
	{8089,protocol_CEMS_SaiMoFei1_43i_LO},
	{8090,protocol_PLC_SaiMoFei1},
	{8091,protocol_PLC_SaiMoFei1_not_flag},
	{8092,protocol_CEMS_SaiMoFei1_TXO_1000},
	{8093,protocol_TSP_NJBoRui_MD6000_B},
	{8094,protocol_CEMS_LaJi_pack_HBManDeKeDCS},
	{8095,protocol_TSP_NJBoRui_MD6000},
	{8096,protocol_CEMS_SDXinZe_ShiYang},
	{8097,protocol_PLC_SDXinZe_v1_7},
	{8098,protocol_PLC_SaiMoFei_HBDongTai},
	{8099,protocol_CEMS_HBManDeKe_Modbus},
	{8100,protocol_CEMS_KaiLan},
	{8101,protocol_PLC_ChuoMei},
	{8102,protocol_TSP_ZhongLv_SGEP300PM},
	{8103,protocol_TSP_ZeTian_circle},
	{8104,protocol_TSP_ZeTian_rectangle},
	{8105,protocol_CEMS_SZCuiYunGu_TLGAS280},
	{8106,protocol_CEMS_SZCuiYunGu_TL_PMM180_4},
	{8107,protocol_TSP_WXChuangChen_DA600},
	{8108,protocol_CEMS_GBhjt2017_PaMs},
	{8109,protocol_CEMS_GBhjt2005_PaMh},
	{8110,protocol_CEMS_GBhjt2017_PaMh},
	{8111,protocol_CEMS_GBhjt2005_PaMs},
	{8112,protocol_O2_BJXueDiLong_MODEL1080},
	{8113,protocol_CEMS_BJXueDiLong_MODEL4000},
	{8114,protocol_PLC_BJXueDiLong_SCS900FT},
	{8115,protocol_TSP_BeiFeng_SBF800_900_2021A},
	{8116,protocol_TSP_BeiFeng_SBF800_900_2021B},
	{8117,protocol_CEMS_GBhjt2005_XinJiang},
	{8118,protocol_CEMS_GBhjt2005_ChuangChen},
	{8119,protocol_PLC_BJXueDiLong_S7_200},
	{8120,protocol_PLC_BJXueDiLong_Smart},
	{8121,protocol_CEMS_BJXueDiLong_SCS900PM},
	{8122,protocol_CEMS_SHHeRu_APxx370},
	{8123,protocol_O2_KuChang},
	{8124,protocol_CEMS_KuChang},
	{8125,protocol_PLC_KuChang},
	{8126,protocol_CEMS_ZhongKe},
	{8127,protocol_CEMS_BJXueDiLong_SCS900CPM},
	{8128,protocol_CEMS_ZhongKe_TR216I},
	{8129,protocol_CEMS_SHHeRu_APxx370_v1_6},
	{8130,protocol_CEMS_AHWanYi_SG1000_HeBei},
	{8131,protocol_CEMS_HBManDeKe3_DCEM3000},
	{8132,protocol_TSP_SDXinZe_SDUST100},
	{8133,protocol_CEMS_SDXinZe_V2},
	{8134,protocol_CEMS_RuiAnRA8000},
	{8135,protocol_CEMS_BJXueDiLong_MODEL4000_1},
	{8136,protocol_CEMS_HBManDeKe_NO_V4},

	{8157,protocol_TSP_AnRomhXin_LSS2004_LSS2014},
	{8158,protocol_O2_AnRongXin_ARX5100},
	{8159,protocol_PLC_AnRomhXin_APT2000},

    {8160,protocol_CEMS_HBManDeKe_NOx},
	{8161,protocol_TSP_HBManDeKe_PLC_S7_200},
	{8162,protocol_TSP_HBManDeKe_PLC_Smart200},
	{8163,protocol_CEMS_BJXueDiLong_MODEL1080_v3_7},
	{8164,protocol_PLC_HBManDeKe_MDK116_B},
	{8165,protocol_TSP_QDJiaMing_LaserDustMP},
	{8166,protocol_CEMS_XiKeMaiHaKe_MCS100FT},
	{8167,protocol_cems_XiKeMaiHaKe_PLC_MCS100FT},
	{8168,protocol_cems_XiKeMaiHaKe_PLC_MCS100FT_New},
	{8169,protocol_CEMS_WuXiChenChuang},
	{8170,protocol_CEMS_WuXiChenChuang_PLC},
	{8171,protocol_CEMS_QDZhongPing},
	{8172,protocol_TSP_QDZhongPing_LRCD2000WV_info},
	{8173,protocol_PLC_QDZhongPing_7017Aplus},
	{8174,protocol_PLC_QDZhongPing_7017Aplus_PLC},
	{8175,protocol_PLC_HongRuiDe},
	{8176,protocol_CEMS_BJHangTianYiLai_HeiBei},
	{8177,protocol_PLC_BJHangTianYiLai_HeiBei},
	{8178,protocol_CEMS_AHLvShi},
	{8179,protocol_TSP_ZhongXing_DM601_Low},

	{8200,protocol_PLC_SHBeiFen_SBF1500},
	{8201,protocol_PLC_SHBeiFen_SBF1500_STATUS},
	{8202,protocol_CEMS_ChuoMei_hjt2005_HeBei},
	{8203,protocol_CEMS_BJXueDiLong_1080uv},
	{8204,protocol_CEMS_HBManDeKe1_DCEM3000},
	{8205,protocol_CEMS_HBManDeKe2_DCEM3000},
	{8206,protocol_CEMS_BJXueDiLong_MODEL1080_v4},
	{8207,protocol_CEMS_SaiMoFei_42i_HI_100},
	{8208,protocol_CEMS_SaiMoFei_42i_LO_100},
	{8209,protocol_CEMS_SaiMoFei_43i_HI_100},
	{8210,protocol_CEMS_SaiMoFei_43i_LO_100},
	{8211,protocol_CEMS_SaiMoFei_TXO_1000_100},
	{8212,protocol_CEMS_FSLaJiDianChang_TaiLiDa},
	{8213,protocol_CEMS_BeiFangBoLi},
	{8214,protocol_CEMS_YaoJiBoLi},
	{8215,protocol_CEMS_FSLuTangWenDu_TaiLiDa},
	{8216,protocol_CEMS_GuangHuanTouDCS_TaiLiDa},
	{8217,protocol_NH3_LG1100},

	{8251,protocol_CEMS_furnace_temperature},
	//yuiki test
	{8407,protocol_test_yuiki},

	{8499,protocol_VOCs_ZhuozhengGB2017},
	{8500,protocol_VOCs_ZhuoZheng},
	{8501,protocol_gas_thermo450i},
	{8502,protocol_VOCs_JSTianRui},
	{8503,protocol_VOCs_JSChangZhouPanNuo},
	{8504,protocol_VOCs_ZhuozhengGB2017},				/*SDYouTe*/
	{8505,protocol_VOCs_GBhjt2017},
	{8506,protocol_VOCs_GBhjt2005},
	{8507,protocol_VOCs_DGChuangTuo},
	{8508,protocol_VOCs_DGZhongGeng},
	{8509,protocol_VOCs_SHLanBao},
	{8510,protocol_VOCs_SDHaiHui},						/*yuiki 20191210*/
	{8511,protocol_VOCs_GZLiuEn},
	{8512,protocol_VOCs_ZeTian},
	{8513,protocol_VOCs_GZLiuEn_DG},
	{8514,protocol_VOCs_GZLiuEn_DG2},
	{8515,protocol_VOCs_SHBaoYing},
	{8516,protocol_VOCs_GZLiuEn_DG3},
	{8517,protocol_VOCs_GZLiuEn_DG4},
	{8518,protocol_VOCs_SHBaoYing2},
	{8519,protocol_VOCs_SZHengFuShang_SZ},
	{8520,protocol_VOCs_SZHengFuShang_JX},
	{8521,protocol_VOC_TianDeYi},
	{8522,protocol_VOCs_GBhjt2017_kpamh},
	{8523,protocol_VOCs_GBhjt2005_kpamh},
	{8524,protocol_VOCs_GBhjt2017_pams},
	{8525,protocol_VOCs_GBhjt2005_pams},
	{8526,protocol_VOCs_GBhjt2017_pamh},
	{8527,protocol_VOCs_GBhjt2005_pamh},
	{8528,protocol_VOC_TianZhiLan_HBCangZhou},
	
	{8700,protocol_CEMS_DaoJin_URA208A},
	{8701,protocol_PLC_DaoJin},
	{8702,protocol_CEMS_DaoJin_URA209},
	{8703,protocol_ai_8017rc},
	{8704,protocol_CEMS_ZeTian_HeBeiModbus2},
	{8705,protocol_cems_JuGuang_HeBei_ai_2000D},
	{8706,protocol_CEMS_7017rc_RO},
	{8707,protocol_ai_XingZe_CJ01},
	{8708,protocol_modbus_voc},
	{8709,protocol_ai_7017rc_vol1},
	{8710,protocol_VOCs_TianJin712_DHT508},
	{8711,protocol_ai_TianJin712_DHT508},
	{8712,protocol_ai_7017rc_vol_Di},

	{8800,protocol_VOCs_TianZhiLan},
	{8801,protocol_VOCs_SZHengFuSheng},
	{8802,protocol_VOCs_SZHengFuSheng2},
	{8803,protocol_VOCs_SuChen},
	{8804,protocol_VOCs_SZHengFuSheng3},
	{8805,protocol_VOCs_FanYi},
	{8806,protocol_VOCs_TianZhiLan2},
	{8807,protocol_VOCs_TianZhiLan3},
	{8808,protocol_VOCs_DGYongBang},
	{8809,protocol_VOCs_DGYongBang2},
	{8810,protocol_VOCs_SuChen2},
	{8811,protocol_VOCs_YingDeWanJu},
	{8812,protocol_VOCs_ChuangTuo},
	{8813,protocol_CEMS_ShengKaiAn},
	{8814,protocol_VOC_WanAnDi},

	{9001,protocol_ph_SZMinBo},
	{9002,protocol_PH_SZQingYan},						/*yuiki 20200103*/
	{9003,protocol_PH_SZQingYan},						/*yuiki 20200103 shangtai*/
	{9004,protocol_others_HuiguMP3100},					/*yuiki 20200305*/
	{9005,protocol_PH_GDWeiChuang},      					/*yuiki 20200628*/
	{9006,protocol_ZONGLIN_ZONGDAN_USAhaxi},
	{9007,protocol_ph_LianCe},
	{9008,protocol_CN_JMWeiChuang},
	{9009,protocol_liquid_level_2088},
	{9010,protocol_others_DO_PengYue},					/*yuiki 20191014*/
	{9011,protocol_others_Diandaolv_PengYue},			/*yuiki 20191014*/
	{9012,protocol_others_Chlorophyll_PengYue},			/*yuiki 20191014*/
	{9013,protocol_others_Cyanin_PengYue},				/*yuiki 20191014*/
	{9014,protocol_others_Zhuodu_PengYue},				/*yuiki 20191014*/
	{9015,protocol_others_ph_PengYue},					/*yuiki 20191014*/
	{9016,protocol_others_YULV_PengYue},
	{9017,protocol_PH_02},
	{9018,protocol_CL_JSZhuoZheng},
	{9019,protocol_diaphaneity_DongGuang},
	{9020,protocol_others_DO_Yosemitech},				/*yuiki 20191014*/
	{9021,protocol_others_Chlorophyll_Yosemitech},		/*yuiki 20191014*/
	{9022,protocol_ph_Yosemitech},						/*yuiki 20191014*/
	{9023,protocol_ORP_Yosemitech},						/*yuiki 20191014*/
	{9024,protocol_others_ups_dongguang},
	{9025,protocol_ph_kezo_pc1000},
	{9026,protocol_PH_SinPh80},
	{9027,protocol_xF_kezo_pc1000},
	{9028,protocol_PH_BoKeSi_301M},
	{9029,protocol_ShuXianYi_LianCe},
	{9030,protocol_DO_WHSiFuMing},						/*yuiki 20191206*/
	{9031,protocol_ZHUODU_WHSiFuMing},					/*yuiki 20191206*/
	{9032,protocol_others_TPTN_USAhaxi},
	{9033,protocol_COND_SDKaiMiSi},
	{9034,protocol_PH_SDKaiMiSi},
	{9035,protocol_PH_8001},
	{9036,protocol_COND_LanChang},
	{9037,protocol_PH_LanChang},
	{9038,protocol_PH_ZhuoZheng},
	{9039,protocol_others_ph_JSTaiChuan_TC5100},
	{9040,protocol_ZHUODU_JMHuangYu},
	{9041,protocol_others_fuhuawu},						/*yuiki 20191219*/
	{9042,protocol_KMnO4_ZhuoZheng},
	{9043,protocol_xCN_HBHuaHouTianCheng},
	{9044,protocol_HUIFAFEN_HBHuaHouTianCheng},
	{9045,protocol_PH_SDDongRun},
	{9046,protocol_PH_BJHuanKe},
	{9047,protocol_others_YULV_LiDe},
	{9048,protocol_PH_221B},
	{9049,protocol_PH_SHLuoBo},
	{9050,protocol_ph_LianCeV8},
	{9051,protocol_PH_KeRuiDa_ph8500A},

	{9055,protocol_ph_PHORP_G8306},

	{9100,protocol_WATER_GBhjt2005},					/*yuiki 20191230*/
	{9101,protocol_WATER_GBhjt2017},					/*yuiki 20191230*/
	{9102,protocol_WATER_HeBei},						/*yuiki 20191231*/
	{9110,protocol_DLT645_GDDG1},						/*yuiki 20200114*/
	{9111,protocol_DLT645_GDDG2},						/*yuiki 20200114*/
	{9112,protocol_DIANBIAO_DianBen_zhiwu},
	{9113,protocol_DIANBIAO_DianBen_shengchan},
	{9114,protocol_DIANBIAO_AnKeRui},

	{9134,protocol_others_processModbus},

	{9200,protocol_ZONGLIANGYI_ZhuoZheng},
	{9201,protocol_others_vocs_pid},
	{9202,protocol_others_vocs_pid_div10},
	{9203,protocol_others_vocs_pid_div1000},
	{9204,protocol_others_vocs_pid_mgm3},
	{9205,protocol_GongKuang_ShouQinLongHui},

	{9302,protocol_others_tengentTx},					/*yuiki 20191218*/
	{9303,protocol_others_tengentRx},					/*yuiki 20191218*/
	//{9890,protocol_electric_meter_TEST},					//DTL645_2007
	//{9891,protocol_electric_meter_TEST},					//DTL645_1997

	{9400,protocol_level_MiKe},
	{9401,protocol_level_BS},
	{9402,protocol_level_LianCe},
	{9403,protocol_level_SZHuaTian},
	{9404,protocol_level_MiKe_P260},

	{9500,protocol_MARK_JingNengReDian},
	{9501,protocol_MARK_JingNengReDian2},
	{9502,protocol_MARK_8DI},
	{9503,protocol_MARK_zz_3DI},
	{9504,protocol_MARK_JiuTian},
	{9505,protocol_MARK_HeBeiZhongDian},
	{9506,protocol_MARK_SIS_ModbusTCP},
	{9507,protocol_MARK_ChengXinJiuTian},
	{9508,protocol_GongKuang_ChangRun},

	{9515,protocol_MARK_GongKuangArg_modbus},

	{9522,protocol_MARK_GDWeiChuang},

	{9525,protocol_mark_GongKuang_Steel_staddr},
	{9526,protocol_mark_GongKuang_FirePower_staddr},
	{9527,protocol_mark_GongKuang_JiaoHua_coking_staddr},
	{9528,protocol_mark_GongKuang_ShuiNi_cement_staddr},
	{9529,protocol_mark_GongKuang_LaJi_fire_staddr},
	
	{9530,protocol_mark_GongKuang_Steel},
	{9531,protocol_mark_GongKuang_FirePower},
	{9532,protocol_mark_GongKuang_JiaoHua_coking},
	{9533,protocol_mark_GongKuang_ShuiNi_cement},
	{9534,protocol_mark_GongKuang_LaJi_fire},
	
	{9535,protocol_hjt2017_GongKuang_Steel},
	{9536,protocol_hjt2017_GongKuang_FirePower},
	{9537,protocol_hjt2017_GongKuang_JiaoHua_coking},
	{9538,protocol_hjt2017_GongKuang_ShuiNi_cement},
	{9539,protocol_hjt2017_GongKuang_LaJi_fire},	

        {9540,protocol_mark_GongKuang_Steel_DI},	
        {9541,protocol_mark_GongKuang_FirePower_DI},	
	{9550,protocol_mark_GongKuang_LaJi_fire_HBManDeKe},

	{9600,protocol_dianbiao_ZJZhengTai_DTSU666_modbus},
	{9601,protocol_dianbiao_DeLiXi_DTSU6606_modbus},
	{9602,protocol_dianbiao_SuChen},
	{9603,protocol_dianbiao_SuChen2},
	{9604,protocol_dianbiao_CL7339N},
	{9605,protocol_dianbiao_DGYongBang},
	{9606,protocol_dianbiao_DGYongBang2},
	{9607,protocol_dianbiao_DDSU666_modbus},
	{9608,protocol_dianbiao_DongGuang},
	{9609,protocol_dianbiao_YingDeWanJu},
	{9610,protocol_dianbiao_KeLu_YiTong},
	{9611,protocol_dianbiao_GZBoKong_K35A},	

	{9850,protocol_RFID_R200_16bytes},
	{9851,protocol_RFID_R200_8bytes},

	{9879,protocol_modbus_voc},

	{9889,protocol_rdtu_t36z},

	//{9890,protocol_DLT645_2007},						//DTL645_2007
	//{9891,protocol_DLT645_1997},						//DTL645_1997     
	
	//8 ai 7017rc modul
	{9891,protocol_ai_7017rc_vol},               
	{9892,protocol_modbus_ai_4017rc_XinTianCompany},
	{9893,protocol_modbus_ai_4017rc},
	{9894,protocol_ai_4017rc},
	{9895,protocol_ai_7017rc},               
	{9896,protocol_modbus_gas_ai}, 
	{9897,protocol_modbus_water_ai},
	//must be 9899, cannot be changed
	{9898,protocol_modbus_gas}, 						// all polcodes are not startup,must be 9898, cannot be changed				
	{9899,protocol_modbus_water},						// all polcodes are not startup,must be 9899, cannot be changed

	{9901,protocol_direct_XJWater},
	{9902,protocol_direct_XJGas},
	{9904,protocol_direct_NMGas},


	{87654321,protocol_ai},
};

extern int protocol_COD_ZJHZWeiLanD01_info(struct acquisition_data *acq_data);
extern int protocol_COD_NJGangNeng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_NJHongGuang_info(struct acquisition_data *acq_data);
extern int protocol_COD_USAhaxi_info(struct acquisition_data *acq_data);
extern int protocol_COD_HBHuaHouTianCheng_info(struct acquisition_data *acq_data);
extern int protocol_COD_ZeTian_info(struct acquisition_data *acq_data);
extern int protocol_COD_JuGuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_HZLuHeng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_NJRuiQuanModbus_info(struct acquisition_data *acq_data);
extern int protocol_COD_GLYunJing_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_DaoJin_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_GDWeiChuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_JSLingHeng_info(struct acquisition_data *acq_data);
extern int protocol_COD_NJJuGe_info(struct acquisition_data *acq_data);
extern int protocol_COD_USAHaXi_maxII_info(struct acquisition_data *acq_data);
extern int protocol_COD_AmtaxInter2C_info(struct acquisition_data *acq_data);
extern int protocol_COD_GZYiWen_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_BoKeSi_info(struct acquisition_data *acq_data);
extern int protocol_COD_AHWanYi_info(struct acquisition_data *acq_data);
extern int protocol_COD_BJHuanKe_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_HBHuaHouTianCheng_new_info(struct acquisition_data *acq_data);
extern int protocol_COD_ZhongLv_info(struct acquisition_data *acq_data);
extern int protocol_COD_LiDa_info(struct acquisition_data *acq_data);
extern int protocol_COD_SZLangShi_PhotoTek6000_info(struct acquisition_data *acq_data);
extern int protocol_COD_HBKeruida_info(struct acquisition_data *acq_data);
extern int protocol_COD_YuXing_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_NJXinRuiPeng_info(struct acquisition_data *acq_data);
extern int protocol_COD_QDJiaMing_info(struct acquisition_data *acq_data);
extern int protocol_COD_JSZhuoZheng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_COD_USAHaXi_maxII_New_info(struct acquisition_data *acq_data);
extern int protocol_COD_HuiHuang_info(struct acquisition_data *acq_data);
extern int protocol_COD_ZeTian_GBhjt2005_status(struct acquisition_data *acq_data);
extern int protocol_COD_ZeTian_GBhjt2005_info1(struct acquisition_data *acq_data);
extern int protocol_COD_ZeTian_GBhjt2005_info2(struct acquisition_data *acq_data);
extern int protocol_COD_ZeTian_GBhjt2005_info3(struct acquisition_data *acq_data);
extern int protocol_COD_ZeTian_GBhjt2005_info4(struct acquisition_data *acq_data);
extern int protocol_COD_ZeTian_GBhjt2005_info5(struct acquisition_data *acq_data);
extern int protocol_COD_GJSurfaceWater_info(struct acquisition_data *acq_data);
extern int protocol_COD_ZhengQi_Modbus_info(struct acquisition_data *acq_data);
extern int protocol_COD_ZhongJieNengTianRong_info(struct acquisition_data *acq_data);
extern int protocol_COD_JSTianZe_info(struct acquisition_data *acq_data);
extern int protocol_COD_NJHongGuang2_info(struct acquisition_data *acq_data);
extern int protocol_COD_Thermo_orion3106_info(struct acquisition_data *acq_data);
extern int protocol_COD_JSRuiQuanRenQ_IV_info(struct acquisition_data *acq_data);
extern int protocol_COD_HuiHuan_New_info(struct acquisition_data *acq_data);
extern int protocol_COD_NJJuGe_New_info(struct acquisition_data *acq_data);
extern int protocol_COD_ZJHZLiQi_V2_info(struct acquisition_data *acq_data);
extern int protocol_COD_EndressHauser_CA80_info(struct acquisition_data *acq_data);
extern int protocol_COD_HeBeiHuaHouTianCheng_E_info(struct acquisition_data *acq_data);
extern int protocol_COD_BJLiDa_info(struct acquisition_data *acq_data);
extern int protocol_COD_NJXinRuiPeng_New_info(struct acquisition_data *acq_data);

extern int protocol_ANDAN_NJRuiQuanModbus_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_NJHongGuang_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_HBHuaHouTianCheng_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZJHZWeiLanD01_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZeTian_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_JuGuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_HZLuHeng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_GLYunJing_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_DaoJin_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_GDWeiChuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_JSLingHeng_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_NJJuGe_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_AmtaxInter2C_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_GZYiWen_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_BoKeSi_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_AHWanYi_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_NJGangNeng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_BJHuanKe_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_DaoJin_4210_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_HBHuaHouTianCheng_new_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZhongLv_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_LiDa_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_HBKeruida_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_YuXing_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_USAhaxiNA8000_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_YuXing_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_NJXinRuiPeng_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_QDJiaMing_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_JSZhuoZheng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_USAHaXi_maxII_New_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_HuiHuang_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZeTian_GBhjt2005_status(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZeTian_GBhjt2005_info1(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZeTian_GBhjt2005_info2(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZeTian_GBhjt2005_info3(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZeTian_GBhjt2005_info4(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZeTian_GBhjt2005_info5(struct acquisition_data *acq_data);
extern int protocol_ANDAN_SZLangShi_PhotoTek6000_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_GJSurfaceWater_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZhengQi_Modbus_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_BJSaiLaiMo_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZhongJieNengTianRong_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_JSTianZe_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_NJHongGuang2_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_Thermo_orion2240_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_JSRuiQuanRenQ_IV_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_NJJuGe_New_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_ZJHZLiQi_V2_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_EndressHauser_CA80_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_HeBeiHuaHouTianCheng_E_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_BJLiDa_info(struct acquisition_data *acq_data);
extern int protocol_ANDAN_NJXinRuiPeng_New_info(struct acquisition_data *acq_data);

extern int protocol_ZONGLIN_NJHongGuang_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZJHZWeiLanD01_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZeTian_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_JuGuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_HZLuHeng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_NJRuiQuanModbus_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_GLYunJing_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_HBHuaHouTianCheng_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_GDWeiChuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_JSLingHeng_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_NJJuGe_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_AmtaxInter2C_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_GZYiWen_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_BoKeSi_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_AHWanYi_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_NJGangNeng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_DaoJin_4200_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_BJHuanKe_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_HBHuaHouTianCheng_new_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZhongLv_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_LiDa_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_HBKeruida_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_YuXing_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_NJXinRuiPeng_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_QDJiaMing_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_JSZhuoZheng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_USAHaXi_maxII_New_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_HuiHuang_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZeTian_GBhjt2005_status(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZeTian_GBhjt2005_info1(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZeTian_GBhjt2005_info2(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZeTian_GBhjt2005_info3(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZeTian_GBhjt2005_info4(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZeTian_GBhjt2005_info5(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_SZLangShi_PhotoTek6000_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_GJSurfaceWater_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZhengQi_Modbus_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZhongJieNengTianRong_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_JSTianZe_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_NJHongGuang2_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_JSRuiQuanRenQ_IV_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_NJJuGe_New_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_ZJHZLiQi_V2_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_HeBeiHuaHouTianCheng_E_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_BJLiDa_info(struct acquisition_data *acq_data);
extern int protocol_ZONGLIN_NJXinRuiPeng_New_info(struct acquisition_data *acq_data);

extern int protocol_others_TPTN_UASHaXi_NPW160_info_TN(struct acquisition_data *acq_data);
extern int protocol_others_TPTN_UASHaXi_NPW160_info_TP(struct acquisition_data *acq_data);
extern int protocol_TP_Thermo_3150_info(struct acquisition_data *acq_data);
extern int protocol_TN_Thermo_3150_info(struct acquisition_data *acq_data);

extern int protocol_ZONGDAN_NJHongGuang_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZJHZWeiLanD01_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZeTian_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_JuGuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_HZLuHeng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_NJRuiQuanModbus_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_GLYunJing_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_HBHuaHouTianCheng_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_GDWeiChuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_JSLingHeng_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_NJJuGe_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_AmtaxInter2C_info(struct acquisition_data *acq_data);
extern int protocol_others_TPTN_DaoJin_HeBei_info_TN(struct acquisition_data *acq_data);
extern int protocol_others_TPTN_DaoJin_HeBei_info_TP(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_GZYiWen_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_BoKeSi_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_AHWanYi_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_DaoJin_4200_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_BJHuanKe_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_HBHuaHouTianCheng_new_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZhongLv_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_LiDa_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_HBKeruida_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_YuXing_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_NJXinRuiPeng_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_QDJiaMing_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_JSZhuoZheng_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_USAHaXi_maxII_New_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_HuiHuang_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZeTian_GBhjt2005_status(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZeTian_GBhjt2005_info1(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZeTian_GBhjt2005_info2(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZeTian_GBhjt2005_info3(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZeTian_GBhjt2005_info4(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZeTian_GBhjt2005_info5(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_SZLangShi_PhotoTek6000_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZhengQi_Modbus_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_NJHongGuang_info2(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZhongJieNengTianRong_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_JSTianZe_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_JSRuiQuanRenQ_IV_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_NJJuGe_New_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_ZJHZLiQi_V2_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_HeBeiHuaHouTianCheng_E_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_BJLiDa_info(struct acquisition_data *acq_data);
extern int protocol_ZONGDAN_NJXinRuiPeng_New_info(struct acquisition_data *acq_data);

extern int protocol_water_YuXing_Modbus_info(struct acquisition_data *acq_data);
extern int protocol_water_SZZhongXingModbus_info1(struct acquisition_data *acq_data);
extern int protocol_water_SZZhongXingModbus_info2(struct acquisition_data *acq_data);
extern int protocol_water_SZZhongXingModbus_info3(struct acquisition_data *acq_data);
extern int protocol_water_NJHongGuang_info1(struct acquisition_data *acq_data);
extern int protocol_water_NJHongGuang_info2(struct acquisition_data *acq_data);
extern int protocol_water_NJHongGuang_info3(struct acquisition_data *acq_data);
extern int protocol_water_NJHongGuang_info4(struct acquisition_data *acq_data);
extern int protocol_water_NJXinRuiPeng_info(struct acquisition_data *acq_data);

extern int protocol_Pb_QDJiaMing_info(struct acquisition_data * acq_data);

extern int protocol_Pb_SZLangShi_PhotoTek6000_info(struct acquisition_data *acq_data);
extern int protocol_Zn_SZLangShi_PhotoTek6000_info(struct acquisition_data *acq_data);

extern int protocol_LiuJiaCr_NJJuGe_info(struct acquisition_data * acq_data);

extern int protocol_door_HaiKang_DSK1T641M_info(struct acquisition_data *acq_data);

extern int protocol_sampler_BJGeRuiSiPu_FC9624YLAB_info(struct acquisition_data *acq_data);
extern int protocol_sampler_HuaHouTianCheng_WS_A_info(struct acquisition_data *acq_data);
extern int protocol_sampler_HBShiJiaZhuang_info(struct acquisition_data *acq_data);
extern int protocol_sampler_WangWei_WQS2000_info(struct acquisition_data *acq_data);
extern int protocol_sampler_HBDeRun_DR803K_info(struct acquisition_data *acq_data);
extern int protocol_sampler_KeSheng_info(struct acquisition_data *acq_data);
extern int protocol_sampler_KeSheng_old_info(struct acquisition_data *acq_data);
extern int protocol_sampler_AHWanYi_info(struct acquisition_data *acq_data);
extern int protocol_sampler_HZDaHu_info(struct acquisition_data *acq_data);
extern int protocol_sampler_HuiHuan_DEK1302_info(struct acquisition_data *acq_data);

extern int protocol_CEMS_ZeTian_HeBeiModbus_STATUS(struct acquisition_data *acq_data);
extern int protocol_CEMS_ZeTian_HeBeiModbus_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_ZeTian_HeBeiModbus_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_ZeTian_HeBeiModbus_NO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_ZeTian_HeBeiModbus_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_ZeTian_HeBeiModbus_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_ZeTian_HeBeiModbus_CO2_info(struct acquisition_data *acq_data);
extern int protocol_TSP_ZeTian_circle_info1(struct acquisition_data *acq_data);
extern int protocol_TSP_ZeTian_circle_info2(struct acquisition_data *acq_data);
extern int protocol_TSP_ZeTian_circle_info3(struct acquisition_data *acq_data);
extern int protocol_TSP_ZeTian_rectangle_info1(struct acquisition_data *acq_data);
extern int protocol_TSP_ZeTian_rectangle_info2(struct acquisition_data *acq_data);
extern int protocol_TSP_ZeTian_rectangle_info3(struct acquisition_data *acq_data);
extern int protocol_TSP_SZCuiYunGu_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_O2_info(struct acquisition_data *acq_data);
extern int protocol_TSP_HBManDeKe_info(struct acquisition_data *acq_data);
extern int protocol_TSP_ZhongLv_SGEP300PM_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_JuGuang_HeBei_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_JuGuang_HeBei_NOx_info(struct acquisition_data *acq_data);
extern int protocol_TSP_JuGuang_HeBei_info(struct acquisition_data *acq_data);
extern int protocol_TSP_AnRomhXin_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_QDJiaMing_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_QDJiaMing_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_QDJiaMing_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHWanYi_HeBei_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHWanYi_HeBei_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHWanYi_HeBei_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHWanYi_HeBei_TSP_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_scs900uv_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_scs900uv_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_scs900uv_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_42i_HI_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_42i_LO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_43i_HI_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_43i_LO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SDdongTai_G3000_UV_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SDdongTai_G3000_UV_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SDdongTai_G3000_UV_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_NOx_info2(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_SO2_info2(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_O2_info2(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe3_DCEM3000_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_ZhongKe_TR216I_info(struct acquisition_data *acq_data);
extern int protocol_TSP_SZCuiYunGu_TL_PMM180_Low_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe1_DCEM3000_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe2_DCEM3000_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_ChuoMei_hjt2005_HeBei_SO2(struct acquisition_data *acq_data);
extern int protocol_CEMS_ChuoMei_hjt2005_HeBei_NOx(struct acquisition_data *acq_data);
extern int protocol_CEMS_ChuoMei_hjt2005_HeBei_O2(struct acquisition_data *acq_data);
extern int protocol_TSP_QDJiaMing_LaserDustMP_info(struct acquisition_data *acq_data);

extern int protocol_PLC_SHBeiFen_SBF1500_SO2_INFO(struct acquisition_data *acq_data);
extern int protocol_PLC_SHBeiFen_SBF1500_NO_INFO(struct acquisition_data *acq_data);
extern int protocol_PLC_SHBeiFen_SBF1500_NO2_INFO(struct acquisition_data *acq_data);
extern int protocol_PLC_SHBeiFen_SBF1500_HCL_INFO(struct acquisition_data *acq_data);
extern int protocol_PLC_SHBeiFen_SBF1500_O2_INFO(struct acquisition_data *acq_data);

extern int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_HCL_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SZCuiYunGu_TL_PMM180_3_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJHangTianYiLai_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJHangTianYiLai_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJHangTianYiLai_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_CQChuanYiModBus_PS7400_F_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_CQChuanYiModBus_PS7400_F_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_CQChuanYiModBus_PS7400_F_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_CQChuanYiModBus_PS7400_F_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_CQChuanYiModBus_PS7400_F_NO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_CQChuanYiModBus_PS7400_F_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_CQChuanYiModBus_PS7400_F_HCL_info(struct acquisition_data *acq_data);
extern int protocol_PLC_CQChuanYiModBus_PS7400_F_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_JuGuang_HeBei_JinDing_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_JuGuang_HeBei_NOx_JinDing_iinfo(struct acquisition_data *acq_data);
extern int protocol_CEMS_JuGuang_HeBei_O2_JinDing_iinfo(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_U23_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_U23_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_U23_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_TXO_1000_info1(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_TXO_1000_info2(struct acquisition_data *acq_data);
extern int protocol_TSP_SaiMoFei_PM_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_KaiLan_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_KaiLan_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_KaiLan_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SZCuiYunGu_TLGAS280_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SZCuiYunGu_TLGAS280_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SZCuiYunGu_TLGAS280_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_DaoJin_URA208A_data(struct acquisition_data *acq_data);
extern int protocol_CEMS_DaoJin_URA208A_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_DaoJin_URA208A_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_DaoJin_URA208A_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_DaoJin_URA208A_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_DaoJin_URA209_NO_info(struct acquisition_data *acq_data);
extern int protocol_O2_BJXueDiLong_MODEL1080_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_STATUS_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_NO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_CO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_HCL_info(struct acquisition_data *acq_data);
extern int protocol_TSP_BeiFeng_SBF800_900_2021A_info(struct acquisition_data *acq_data);
extern int protocol_TSP_BeiFeng_SBF800_900_2021B_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_SCS900PM_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_SCS900PM_info1(struct acquisition_data *acq_data);
extern int protocol_TSP_WXChuangChen_DA600_info(struct acquisition_data *acq_data);
extern int protocol_O2_ZeTian_HeBei_info1(struct acquisition_data *acq_data);
extern int protocol_O2_ZeTian_HeBei_info2(struct acquisition_data *acq_data);
extern int protocol_O2_ZeTian_HeBei_info3(struct acquisition_data *acq_data);
extern int protocol_CEMS_SHHeRu_APxx370_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SHHeRu_APxx370_NOx_info(struct acquisition_data *acq_data);
extern int protocol_O2_KuChang_status(struct acquisition_data *acq_data);
extern int protocol_O2_KuChang_info1(struct acquisition_data *acq_data);
extern int protocol_O2_KuChang_info2(struct acquisition_data *acq_data);
extern int protocol_O2_KuChang_info3(struct acquisition_data *acq_data);
extern int protocol_O2_KuChang_info4(struct acquisition_data *acq_data);
extern int protocol_O2_KuChang_info5(struct acquisition_data *acq_data);
extern int protocol_CEMS_KuChang_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_KuChang_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_KuChang_CO_info(struct acquisition_data *acq_data);
extern int protocol_PLC_KuChang_info(struct acquisition_data *acq_data);
extern int protocol_TSP_ZhongDianXingYe_info(struct acquisition_data *acq_data);
extern int protocol_TSP_JuGuang_SynspecPM_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBHeFeng_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBHeFeng_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBHeFeng_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_ZhongKe_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_SCS900CPM_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_JuGuang_HeBei_2000D_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_JuGuang_HeBei_2000D_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_1080uv_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_1080uv_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_1080uv_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_info(struct acquisition_data *acq_data);

extern int protocol_CEMS_AnRongXin_AGA1000_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AnRongXin_AGA1000_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AnRongXin_AGA1000_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei1_42i_HI_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei1_42i_LO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei1_43i_HI_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei1_43i_LO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei1_TXO_1000_info1(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei1_TXO_1000_info2(struct acquisition_data *acq_data);
extern int protocol_TSP_BJXueDiLong_Model2030_info(struct acquisition_data *acq_data);
extern int protocol_TSP_NJBoRui_MD6000_B_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SHHeRu_APxx370_v1_6_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SHHeRu_APxx370_v1_6_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v4_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v4_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v4_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v4_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v4_CO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL1080_v4_CH4_info(struct acquisition_data *acq_data);

extern int protocol_CEMS_XiKeMaiHaKe_MCS100FT_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_XiKeMaiHaKe_MCS100FT_HCL_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_XiKeMaiHaKe_MCS100FT_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_XiKeMaiHaKe_MCS100FT_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_XiKeMaiHaKe_MCS100FT_O2_info(struct acquisition_data *acq_data);
extern int protocol_TSP_NJBoRui_MD6000_info(struct acquisition_data *acq_data);

extern int protocol_CEMS_AHWanYi_SG1000_HeBei_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHWanYi_SG1000_HeBei_NOx_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHWanYi_SG1000_HeBei_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHWanYi_SG1000_HeBei_TSP_info(struct acquisition_data *acq_data);

extern int protocol_CEMS_SaiMoFei_42i_HI_100_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_42i_LO_100_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_43i_HI_100_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_43i_LO_100_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_TXO_1000_100_info1(struct acquisition_data *acq_data);
extern int protocol_CEMS_SaiMoFei_TXO_1000_100_info2(struct acquisition_data *acq_data);

extern int protocol_CEMS_TianZhiLan_HBCangZhou_STATUS(struct acquisition_data *acq_data);
extern int protocol_CEMS_WuXiChenChuang_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_WuXiChenChuang_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_WuXiChenChuang_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_WuXiChenChuang_STATUS_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_QDZhongPing_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_QDZhongPing_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_QDZhongPing_O2_info(struct acquisition_data *acq_data);
extern int protocol_TSP_QDZhongPing_LRCD2000WV_info(struct acquisition_data *acq_data);

extern int protocol_TSP_SDXinZe_SDUST100_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SDXinZe_V2_STATUS_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SDXinZe_V2_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SDXinZe_V2_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_SDXinZe_V2_O2_info(struct acquisition_data *acq_data);

extern int protocol_CEMS_RuiAnRA8000_info(struct acquisition_data *acq_data);

extern int protocol_CEMS_BJHangTianYiLai_HeiBei_STATUS_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJHangTianYiLai_HeiBei_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJHangTianYiLai_HeiBei_NO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJHangTianYiLai_HeiBei_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJHangTianYiLai_HeiBei_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJHangTianYiLai_HeiBei_HCL_info(struct acquisition_data *acq_data);
extern int protocol_TSP_AnRomhXin_LSS2004_LSS2014_info(struct acquisition_data *acq_data);
extern int protocol_O2_AnRongXin_ARX5100_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_1_STATUS_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_1_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_1_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_1_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_1_NO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_1_CO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_BJXueDiLong_MODEL4000_1_HCL_info(struct acquisition_data *acq_data);
extern int protocol_VOCs_TianJin712_DHT508_STATUS_info(struct acquisition_data *acq_data);
extern int protocol_VOCs_TianJin712_DHT508_THC_info(struct acquisition_data *acq_data);
extern int protocol_VOCs_TianJin712_DHT508_NMHC_info(struct acquisition_data *acq_data);
extern int protocol_VOCs_TianJin712_DHT508_NMHC_CH4_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_V4_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_V4_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_HBManDeKe_V4_O2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHLvShi_STATUS_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHLvShi_SO2_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHLvShi_NO_info(struct acquisition_data *acq_data);
extern int protocol_CEMS_AHLvShi_O2_info(struct acquisition_data *acq_data);
extern int protocol_TSP_ZhongXing_DM601_Low_info(struct acquisition_data *acq_data);

extern int protocol_test_yuiki_info(struct acquisition_data *acq_data);
extern int protocol_test_yuiki_info1(struct acquisition_data *acq_data);
extern int protocol_test_yuiki_info2(struct acquisition_data *acq_data);
extern int protocol_rdtu_t36z_info(struct acquisition_data *acq_data);

struct rtu_instrinfo_protocols{
	const int const instrument_code;
	const int index; // from 0,1,2,3,4...9
	const PROTOCOL_T protocol;
}; 

static struct rtu_instrinfo_protocols instrinfo_protocols[]={
	{102019,0,protocol_COD_ZJHZWeiLanD01_info},
	{102025,0,protocol_COD_NJGangNeng_HeBei_info},
	{102035,0,protocol_COD_HuiHuang_info},
	{102038,0,protocol_COD_AmtaxInter2C_info},
	{102040,0,protocol_water_NJHongGuang_info1},
	{102040,1,protocol_water_NJHongGuang_info2},
	{102040,2,protocol_water_NJHongGuang_info3},
	{102040,3,protocol_water_NJHongGuang_info4},
	{102041,0,protocol_COD_HBHuaHouTianCheng_info},
	{102042,0,protocol_COD_ZeTian_info},
	{102043,0,protocol_COD_JuGuang_HeBei_info},
	{102044,0,protocol_COD_HZLuHeng_HeBei_info},
	{102045,0,protocol_COD_Thermo_orion3106_info},
	{102046,0,protocol_COD_GLYunJing_HeBei_info},
	{102047,0,protocol_COD_DaoJin_HeBei_info},
	{102048,0,protocol_COD_GDWeiChuang_HeBei_info},
	{102049,0,protocol_COD_JSLingHeng_info},
	{102050,0,protocol_COD_NJJuGe_info},
	{102051,0,protocol_COD_USAHaXi_maxII_info},
	{102052,0,protocol_COD_GZYiWen_HeBei_info},
	{102053,0,protocol_COD_BoKeSi_info},
	{102054,0,protocol_COD_AHWanYi_info},
	{102055,0,protocol_COD_BJHuanKe_HeBei_info},
	{102056,0,protocol_COD_HBHuaHouTianCheng_new_info},
	{102057,0,protocol_COD_ZhongLv_info},
	{102058,0,protocol_COD_LiDa_info},
	{102059,0,protocol_COD_ZhongJieNengTianRong_info},
	{102060,0,protocol_COD_NJHongGuang2_info},
	{102061,0,protocol_COD_JSZhuoZheng_HeBei_info},
	{102062,0,protocol_COD_USAHaXi_maxII_New_info},
	{102063,0,protocol_COD_GJSurfaceWater_info},
	{102064,0,protocol_COD_SZLangShi_PhotoTek6000_info},
	{102065,0,protocol_COD_ZhengQi_Modbus_info},
	{102066,0,protocol_COD_JSTianZe_info},
	{102067,0,protocol_COD_JSRuiQuanRenQ_IV_info},
	{102068,0,protocol_COD_NJJuGe_New_info},
	{102069,0,protocol_COD_HeBeiHuaHouTianCheng_E_info},
	{102070,0,protocol_COD_BJLiDa_info},
	{102071,0,protocol_COD_GLYunJing_HeBei_info},
	{102099,0,protocol_COD_HuiHuan_New_info},
	
	{102101,0,protocol_COD_USAhaxi_info},
	{102114,0,protocol_COD_NJRuiQuanModbus_info},
	{102123,0,protocol_COD_YuXing_HeBei_info},
	{102124,0,protocol_water_NJXinRuiPeng_info},
	{102153,0,protocol_COD_QDJiaMing_info},
	{102165,0,protocol_COD_EndressHauser_CA80_info},
	
	{102200,0,protocol_COD_ZeTian_GBhjt2005_status},
	{102200,1,protocol_COD_ZeTian_GBhjt2005_info1},
	{102200,2,protocol_COD_ZeTian_GBhjt2005_info2},
	{102200,3,protocol_COD_ZeTian_GBhjt2005_info3},
	{102200,4,protocol_COD_ZeTian_GBhjt2005_info4},
	{102200,5,protocol_COD_ZeTian_GBhjt2005_info5},
	{102201,0,protocol_COD_ZJHZLiQi_V2_info},
	{102202,0,protocol_COD_NJXinRuiPeng_New_info},
	{102266,0,protocol_COD_HBKeruida_info},


	{103015,0,protocol_ANDAN_NJRuiQuanModbus_info},
	{103033,0,protocol_ANDAN_HuiHuang_info},
	{103037,0,protocol_ANDAN_AmtaxInter2C_info},
	{103040,0,protocol_water_NJHongGuang_info1},
	{103040,1,protocol_water_NJHongGuang_info2},
	{103040,2,protocol_water_NJHongGuang_info3},
	{103040,3,protocol_water_NJHongGuang_info4},
	{103041,0,protocol_ANDAN_HBHuaHouTianCheng_info},
	{103042,0,protocol_ANDAN_ZJHZWeiLanD01_info},
	{103043,0,protocol_ANDAN_ZeTian_info},
	{103044,0,protocol_ANDAN_JuGuang_HeBei_info},
	{103045,0,protocol_ANDAN_HZLuHeng_HeBei_info},
	{103046,0,protocol_ANDAN_Thermo_orion2240_info},
	{103047,0,protocol_ANDAN_GLYunJing_HeBei_info},
	{103048,0,protocol_ANDAN_DaoJin_HeBei_info},
	{103049,0,protocol_ANDAN_GDWeiChuang_HeBei_info},
	{103050,0,protocol_ANDAN_JSLingHeng_info},
	{103051,0,protocol_ANDAN_NJJuGe_info},
	{103052,0,protocol_ANDAN_GZYiWen_HeBei_info},
	{103053,0,protocol_ANDAN_BoKeSi_info},
	{103054,0,protocol_ANDAN_AHWanYi_info},
	{103055,0,protocol_ANDAN_NJGangNeng_HeBei_info},
	{103056,0,protocol_ANDAN_BJHuanKe_HeBei_info},
	{103057,0,protocol_ANDAN_DaoJin_4210_info},
	{103058,0,protocol_ANDAN_HBHuaHouTianCheng_new_info},
	{103059,0,protocol_ANDAN_ZhongLv_info},
	{103060,0,protocol_ANDAN_LiDa_info},
	{103061,0,protocol_ANDAN_ZhongJieNengTianRong_info},
	{103062,0,protocol_ANDAN_NJHongGuang2_info},
	{103064,0,protocol_ANDAN_JSZhuoZheng_HeBei_info},
	{103068,0,protocol_ANDAN_USAhaxiNA8000_info},
	{103070,0,protocol_ANDAN_QDJiaMing_info},
	{103073,0,protocol_ANDAN_GJSurfaceWater_info},
	{103074,0,protocol_ANDAN_SZLangShi_PhotoTek6000_info},
	{103075,0,protocol_ANDAN_ZhengQi_Modbus_info},
	{103076,0,protocol_ANDAN_JSTianZe_info},
	{103077,0,protocol_ANDAN_JSRuiQuanRenQ_IV_info},
	{103078,0,protocol_ANDAN_EndressHauser_CA80_info},
	
	{103082,0,protocol_ANDAN_USAHaXi_maxII_New_info},
	{103084,0,protocol_ANDAN_NJJuGe_New_info},
	{103085,0,protocol_ANDAN_HeBeiHuaHouTianCheng_E_info},
	{103086,0,protocol_ANDAN_BJLiDa_info},
	{103087,0,protocol_ANDAN_GLYunJing_HeBei_info},
	
	{103111,0,protocol_ANDAN_ZJHZLiQi_V2_info},
	{103112,0,protocol_ANDAN_NJXinRuiPeng_New_info},
	{103120,0,protocol_ANDAN_HBKeruida_info},
	{103121,0,protocol_ANDAN_YuXing_HeBei_info},
	{103122,0,protocol_water_NJXinRuiPeng_info},
	{103123,0,protocol_ANDAN_BJSaiLaiMo_info},
	
	{103200,0,protocol_ANDAN_ZeTian_GBhjt2005_status},
	{103200,1,protocol_ANDAN_ZeTian_GBhjt2005_info1},
	{103200,2,protocol_ANDAN_ZeTian_GBhjt2005_info2},
	{103200,3,protocol_ANDAN_ZeTian_GBhjt2005_info3},
	{103200,4,protocol_ANDAN_ZeTian_GBhjt2005_info4},
	{103200,5,protocol_ANDAN_ZeTian_GBhjt2005_info5},


	{104021,0,protocol_ZONGLIN_HuiHuang_info},
	{104022,0,protocol_water_SZZhongXingModbus_info1},
	{104022,1,protocol_water_SZZhongXingModbus_info2},
	{104022,2,protocol_water_SZZhongXingModbus_info3},
	{104023,0,protocol_ZONGLIN_AmtaxInter2C_info},
	{104026,0,protocol_water_NJHongGuang_info1},
	{104026,1,protocol_water_NJHongGuang_info2},
	{104026,2,protocol_water_NJHongGuang_info3},
	{104026,3,protocol_water_NJHongGuang_info4},
	{104027,0,protocol_ZONGLIN_ZJHZWeiLanD01_info},
	{104028,0,protocol_ZONGLIN_ZeTian_info},
	{104029,0,protocol_ZONGLIN_JuGuang_HeBei_info},
	{104030,0,protocol_ZONGLIN_HZLuHeng_HeBei_info},
	{104031,0,protocol_ZONGLIN_GLYunJing_HeBei_info},
	{104032,0,protocol_ZONGLIN_HBHuaHouTianCheng_info},
	{104033,0,protocol_ZONGLIN_GDWeiChuang_HeBei_info},
	{104034,0,protocol_ZONGLIN_JSLingHeng_info},
	{104035,0,protocol_ZONGLIN_NJJuGe_info},
	{104036,0,protocol_ZONGLIN_GZYiWen_HeBei_info},
	{104037,0,protocol_ZONGLIN_BoKeSi_info},
	{104038,0,protocol_ZONGLIN_AHWanYi_info},
	{104039,0,protocol_ZONGLIN_NJGangNeng_HeBei_info},
	{104040,0,protocol_ZONGLIN_DaoJin_4200_info},
	{104041,0,protocol_ZONGLIN_BJHuanKe_HeBei_info},
	{104042,0,protocol_ZONGLIN_HBHuaHouTianCheng_new_info},
	{104043,0,protocol_ZONGLIN_ZhongLv_info},
	{104044,0,protocol_ZONGLIN_LiDa_info},
	{104045,0,protocol_others_TPTN_UASHaXi_NPW160_info_TN},
	{104045,1,protocol_others_TPTN_UASHaXi_NPW160_info_TP},
	{104046,0,protocol_TP_Thermo_3150_info},
	{104046,1,protocol_TN_Thermo_3150_info},
	{104047,0,protocol_ZONGLIN_JSZhuoZheng_HeBei_info},
	{104048,0,protocol_ZONGLIN_USAHaXi_maxII_New_info},
	{104049,0,protocol_ZONGLIN_ZhongJieNengTianRong_info},
	{104050,0,protocol_ZONGLIN_HBKeruida_info},
	{104051,0,protocol_ZONGLIN_YuXing_HeBei_info},
	{104052,0,protocol_water_NJXinRuiPeng_info},
	{104053,0,protocol_ZONGLIN_QDJiaMing_info},
	{104054,0,protocol_ZONGLIN_SZLangShi_PhotoTek6000_info},
	{104055,0,protocol_ZONGLIN_GJSurfaceWater_info},
	{104056,0,protocol_ZONGLIN_ZhengQi_Modbus_info},
	{104057,0,protocol_ZONGLIN_JSTianZe_info},
	{104058,0,protocol_ZONGLIN_JSRuiQuanRenQ_IV_info},
	{104059,0,protocol_ZONGLIN_NJJuGe_New_info},
	{104060,0,protocol_ZONGLIN_HeBeiHuaHouTianCheng_E_info},
	{104061,0,protocol_ZONGLIN_BJLiDa_info},
	{104062,0,protocol_ZONGLIN_GLYunJing_HeBei_info},

	{104100,0,protocol_ZONGLIN_NJHongGuang2_info},
	{104101,0,protocol_ZONGLIN_ZJHZLiQi_V2_info},
	{104102,0,protocol_ZONGLIN_NJXinRuiPeng_New_info},
	
	{104200,0,protocol_ZONGLIN_ZeTian_GBhjt2005_status},
	{104200,1,protocol_ZONGLIN_ZeTian_GBhjt2005_info1},
	{104200,2,protocol_ZONGLIN_ZeTian_GBhjt2005_info2},
	{104200,3,protocol_ZONGLIN_ZeTian_GBhjt2005_info3},
	{104200,4,protocol_ZONGLIN_ZeTian_GBhjt2005_info4},
	{104200,5,protocol_ZONGLIN_ZeTian_GBhjt2005_info5},
	
	{104704,0,protocol_ZONGLIN_NJRuiQuanModbus_info},


	{105008,0,protocol_ZONGDAN_HuiHuang_info},
	{105010,0,protocol_ZONGDAN_AmtaxInter2C_info},
	{105013,0,protocol_water_NJHongGuang_info1},
	{105013,1,protocol_water_NJHongGuang_info2},
	{105013,2,protocol_water_NJHongGuang_info3},
	{105013,3,protocol_water_NJHongGuang_info4},
	{105014,0,protocol_ZONGDAN_ZJHZWeiLanD01_info},
	{105015,0,protocol_ZONGDAN_ZeTian_info},
	{105016,0,protocol_ZONGDAN_JuGuang_HeBei_info},
	{105017,0,protocol_ZONGDAN_HZLuHeng_HeBei_info},
	{105018,0,protocol_ZONGDAN_GLYunJing_HeBei_info},
	{105019,0,protocol_ZONGDAN_HBHuaHouTianCheng_info},
	{105020,0,protocol_ZONGDAN_GDWeiChuang_HeBei_info},
	{105021,0,protocol_ZONGDAN_JSLingHeng_info},
	{105022,0,protocol_ZONGDAN_NJJuGe_info},
	{105023,0,protocol_others_TPTN_DaoJin_HeBei_info_TN},
	{105023,1,protocol_others_TPTN_DaoJin_HeBei_info_TP},
	{105024,0,protocol_ZONGDAN_GZYiWen_HeBei_info},
	{105025,0,protocol_ZONGDAN_BoKeSi_info},
	{105026,0,protocol_ZONGDAN_AHWanYi_info},
	{105027,0,protocol_ZONGDAN_DaoJin_4200_info},
	{105027,1,protocol_ZONGLIN_DaoJin_4200_info},
	{105028,0,protocol_ZONGDAN_BJHuanKe_HeBei_info},
	{105029,0,protocol_ZONGDAN_HBHuaHouTianCheng_new_info},
	{105030,0,protocol_ZONGDAN_ZhongLv_info},
	{105031,0,protocol_ZONGDAN_LiDa_info},
	{105032,0,protocol_ZONGDAN_ZhongJieNengTianRong_info},
	{105033,0,protocol_ZONGDAN_JSRuiQuanRenQ_IV_info},
	{105034,0,protocol_ZONGDAN_JSZhuoZheng_HeBei_info},
	{105035,0,protocol_ZONGDAN_USAHaXi_maxII_New_info},
	{105037,0,protocol_ZONGDAN_NJHongGuang_info2},
	{105038,0,protocol_ZONGDAN_ZJHZLiQi_V2_info},
	{105039,0,protocol_ZONGDAN_NJJuGe_New_info},
	{105040,0,protocol_ZONGDAN_HeBeiHuaHouTianCheng_E_info},
	{105041,0,protocol_ZONGDAN_BJLiDa_info},
	{105042,0,protocol_ZONGDAN_GLYunJing_HeBei_info},
	{105043,0,protocol_ZONGDAN_NJXinRuiPeng_New_info},
	{105050,0,protocol_ZONGDAN_HBKeruida_info},
	{105051,0,protocol_ZONGDAN_YuXing_HeBei_info},
	{105052,0,protocol_water_NJXinRuiPeng_info},
	{105053,0,protocol_ZONGDAN_QDJiaMing_info},
	{105054,0,protocol_ZONGDAN_SZLangShi_PhotoTek6000_info},
	{105055,0,protocol_ZONGDAN_ZhengQi_Modbus_info},
	{105056,0,protocol_ZONGDAN_JSTianZe_info},
	
	{105200,0,protocol_ZONGDAN_ZeTian_GBhjt2005_status},
	{105200,1,protocol_ZONGDAN_ZeTian_GBhjt2005_info1},
	{105200,2,protocol_ZONGDAN_ZeTian_GBhjt2005_info2},
	{105200,3,protocol_ZONGDAN_ZeTian_GBhjt2005_info3},
	{105200,4,protocol_ZONGDAN_ZeTian_GBhjt2005_info4},
	{105200,5,protocol_ZONGDAN_ZeTian_GBhjt2005_info5},
	{105201,0,protocol_water_YuXing_Modbus_info},
	{105505,0,protocol_ZONGDAN_NJRuiQuanModbus_info},

	{106050,0,protocol_LiuJiaCr_NJJuGe_info},
	{106600,0,protocol_Pb_QDJiaMing_info},
	{106601,0,protocol_Pb_SZLangShi_PhotoTek6000_info},
	{106602,0,protocol_Zn_SZLangShi_PhotoTek6000_info},
	
	{107400,0,protocol_door_HaiKang_DSK1T641M_info},

	{107502,0,protocol_sampler_BJGeRuiSiPu_FC9624YLAB_info},
	{107503,0,protocol_sampler_HZDaHu_info},
	{107504,0,protocol_sampler_HuaHouTianCheng_WS_A_info},
	{107505,0,protocol_sampler_HBShiJiaZhuang_info},
	{107506,0,protocol_sampler_WangWei_WQS2000_info},
	{107507,0,protocol_sampler_HBDeRun_DR803K_info},
	{107508,0,protocol_sampler_KeSheng_info},
	{107509,0,protocol_sampler_KeSheng_old_info},
	{107510,0,protocol_sampler_AHWanYi_info},
	{107512,0,protocol_sampler_HuiHuan_DEK1302_info},

	{108008,0,protocol_CEMS_ZeTian_HeBeiModbus_STATUS},
	{108008,1,protocol_CEMS_ZeTian_HeBeiModbus_SO2_info},
	{108008,2,protocol_CEMS_ZeTian_HeBeiModbus_NOx_info},
	{108008,3,protocol_CEMS_ZeTian_HeBeiModbus_NO2_info},
	{108008,4,protocol_CEMS_ZeTian_HeBeiModbus_O2_info},
	{108008,5,protocol_CEMS_ZeTian_HeBeiModbus_CO_info}, //del by miles zhang 20210519
	{108008,6,protocol_CEMS_ZeTian_HeBeiModbus_CO2_info}, //del by miles zhang 20210519

	
	{108009,0,protocol_TSP_SZCuiYunGu_info}, //del by miles zhang for ZeSuan 0 20210519
	{108010,0,protocol_CEMS_HBManDeKe_SO2_info},
	{108010,1,protocol_CEMS_HBManDeKe_NOx_info},
	{108010,2,protocol_CEMS_HBManDeKe_O2_info},
	{108011,0,protocol_TSP_HBManDeKe_info},
	{108013,0,protocol_TSP_AnRomhXin_info},
	{108018,0,protocol_CEMS_JuGuang_HeBei_SO2_info},
	{108018,1,protocol_CEMS_JuGuang_HeBei_NOx_info},
	{108019,0,protocol_TSP_JuGuang_HeBei_info},
	{108020,0,protocol_CEMS_QDJiaMing_SO2_info},
	{108020,1,protocol_CEMS_QDJiaMing_NOx_info},
	{108020,2,protocol_CEMS_QDJiaMing_O2_info},
	{108022,0,protocol_CEMS_AHWanYi_HeBei_SO2_info},
    {108022,1,protocol_CEMS_AHWanYi_HeBei_NOx_info},
	{108022,2,protocol_CEMS_AHWanYi_HeBei_O2_info},
	{108022,3,protocol_CEMS_AHWanYi_HeBei_TSP_info},
	{108023,0,protocol_CEMS_BJXueDiLong_scs900uv_SO2_info},
	{108023,1,protocol_CEMS_BJXueDiLong_scs900uv_NOx_info},
	{108023,2,protocol_CEMS_BJXueDiLong_scs900uv_O2_info},
	{108025,0,protocol_CEMS_SDdongTai_G3000_UV_SO2_info},
	{108025,1,protocol_CEMS_SDdongTai_G3000_UV_NO_info},
	{108025,2,protocol_CEMS_SDdongTai_G3000_UV_O2_info},
	{108027,0,protocol_CEMS_SaiMoFei_42i_HI_info},
	{108028,0,protocol_CEMS_SaiMoFei_42i_LO_info},
	{108029,0,protocol_CEMS_BJXueDiLong_MODEL1080_info},
	{108031,0,protocol_CEMS_JuGuang_HeBei_JinDing_SO2_info},
	{108031,1,protocol_CEMS_JuGuang_HeBei_NOx_JinDing_iinfo},
	{108031,2,protocol_CEMS_JuGuang_HeBei_O2_JinDing_iinfo},
	{108033,0,protocol_CEMS_BJXueDiLong_U23_SO2_info},
	{108033,1,protocol_CEMS_BJXueDiLong_U23_NOx_info},
	{108033,2,protocol_CEMS_BJXueDiLong_U23_O2_info},
	{108034,0,protocol_CEMS_SaiMoFei_43i_HI_info},
	{108035,0,protocol_CEMS_SaiMoFei_43i_LO_info},
	{108036,0,protocol_CEMS_SZCuiYunGu_TL_PMM180_3_info},
	{108038,0,protocol_CEMS_BJHangTianYiLai_SO2_info},
	{108038,1,protocol_CEMS_BJHangTianYiLai_NO_info},
	{108038,2,protocol_CEMS_BJHangTianYiLai_O2_info},
	{108040,0,protocol_CEMS_CQChuanYiModBus_PS7400_F_SO2_info},
	{108040,1,protocol_CEMS_CQChuanYiModBus_PS7400_F_NOx_info},
	{108040,2,protocol_CEMS_CQChuanYiModBus_PS7400_F_CO_info},
	{108040,3,protocol_CEMS_CQChuanYiModBus_PS7400_F_HCL_info},
	//{108040,4,protocol_CEMS_CQChuanYiModBus_PS7400_F_NO2_info},
	//{108040,5,protocol_CEMS_CQChuanYiModBus_PS7400_F_O2_info},
	//{108040,6,protocol_CEMS_CQChuanYiModBus_PS7400_F_NO_info},
	
	{108041,0,protocol_PLC_CQChuanYiModBus_PS7400_F_info},
	{108042,0,protocol_CEMS_SaiMoFei_TXO_1000_info1},
	{108042,1,protocol_CEMS_SaiMoFei_TXO_1000_info2},
	{108044,0,protocol_TSP_SaiMoFei_PM_info},
	{108046,0,protocol_CEMS_JuGuang_HeBei_2000D_SO2_info},
	{108047,0,protocol_CEMS_JuGuang_HeBei_2000D_NOx_info},
	{108048,0,protocol_TSP_BJXueDiLong_Model2030_info},
	{108052,0,protocol_TSP_SZCuiYunGu_TL_PMM180_Low_info},
	{108053,0,protocol_CEMS_AnRongXin_AGA1000_NOx_info},
	{108053,1,protocol_CEMS_AnRongXin_AGA1000_SO2_info},
	{108053,2,protocol_CEMS_AnRongXin_AGA1000_O2_info},
	{108058,0,protocol_CEMS_HBHeFeng_SO2_info},
	{108058,1,protocol_CEMS_HBHeFeng_NO_info},
	{108058,2,protocol_CEMS_HBHeFeng_O2_info},
	{108070,0,protocol_TSP_ZhongDianXingYe_info},
	{108075,0,protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_SO2_info},
	{108075,1,protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_NOx_info},
	{108075,2,protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_CO_info},
	{108075,3,protocol_CEMS_LaJi_pack_HeBeiGRST_CX4000_HCL_info},
	{108077,0,protocol_O2_ZeTian_HeBei_info1},
	{108077,1,protocol_O2_ZeTian_HeBei_info2},
	{108077,2,protocol_O2_ZeTian_HeBei_info3},
	{108079,0,protocol_TSP_JuGuang_SynspecPM_info},

	{108083,0,protocol_CEMS_ZeTian_HeBeiModbus_STATUS},
	{108083,1,protocol_CEMS_ZeTian_HeBeiModbus_SO2_info},
	{108083,2,protocol_CEMS_ZeTian_HeBeiModbus_NOx_info},
	{108083,3,protocol_CEMS_ZeTian_HeBeiModbus_NO2_info},
	{108083,4,protocol_CEMS_ZeTian_HeBeiModbus_O2_info},
	{108083,5,protocol_CEMS_ZeTian_HeBeiModbus_CO_info}, //del by miles zhang 20210519
	{108083,6,protocol_CEMS_ZeTian_HeBeiModbus_CO2_info}, //del by miles zhang 20210519

	{108086,0,protocol_CEMS_SaiMoFei1_42i_HI_info},
	{108087,0,protocol_CEMS_SaiMoFei1_42i_LO_info},
	{108088,0,protocol_CEMS_SaiMoFei1_43i_HI_info},
	{108089,0,protocol_CEMS_SaiMoFei1_43i_LO_info},
	{108092,0,protocol_CEMS_SaiMoFei1_TXO_1000_info1},
	{108092,1,protocol_CEMS_SaiMoFei1_TXO_1000_info2},
	{108093,0,protocol_TSP_NJBoRui_MD6000_B_info},
	{108095,0,protocol_TSP_NJBoRui_MD6000_info},
	
	{108100,0,protocol_CEMS_KaiLan_SO2_info},
	{108100,1,protocol_CEMS_KaiLan_NOx_info},
	{108100,2,protocol_CEMS_KaiLan_O2_info},
	{108102,0,protocol_TSP_ZhongLv_SGEP300PM_info},
	{108103,0,protocol_TSP_ZeTian_circle_info1},
	{108103,1,protocol_TSP_ZeTian_circle_info2},
	{108103,2,protocol_TSP_ZeTian_circle_info3},
	{108104,0,protocol_TSP_ZeTian_rectangle_info1},
	{108104,1,protocol_TSP_ZeTian_rectangle_info2},
	{108104,2,protocol_TSP_ZeTian_rectangle_info3},
	{108105,0,protocol_CEMS_SZCuiYunGu_TLGAS280_SO2_info},
	{108105,1,protocol_CEMS_SZCuiYunGu_TLGAS280_NOx_info},
	{108105,2,protocol_CEMS_SZCuiYunGu_TLGAS280_O2_info},
	{108107,0,protocol_TSP_WXChuangChen_DA600_info},
	{108112,0,protocol_O2_BJXueDiLong_MODEL1080_info},
	{108113,0,protocol_CEMS_BJXueDiLong_MODEL4000_STATUS_info},
	{108113,1,protocol_CEMS_BJXueDiLong_MODEL4000_SO2_info},
	{108113,2,protocol_CEMS_BJXueDiLong_MODEL4000_NO_info},
	{108113,3,protocol_CEMS_BJXueDiLong_MODEL4000_NO2_info},
	{108113,4,protocol_CEMS_BJXueDiLong_MODEL4000_CO_info},
	{108113,5,protocol_CEMS_BJXueDiLong_MODEL4000_CO2_info},
	{108113,6,protocol_CEMS_BJXueDiLong_MODEL4000_HCL_info},
	{108115,0,protocol_TSP_BeiFeng_SBF800_900_2021A_info},
	{108116,0,protocol_TSP_BeiFeng_SBF800_900_2021B_info},
	{108121,0,protocol_CEMS_BJXueDiLong_SCS900PM_info},
	{108121,1,protocol_CEMS_BJXueDiLong_SCS900PM_info1},
	{108122,0,protocol_CEMS_SHHeRu_APxx370_SO2_info},
	{108122,1,protocol_CEMS_SHHeRu_APxx370_NOx_info},
	{108123,0,protocol_O2_KuChang_status},
	{108123,1,protocol_O2_KuChang_info1},
	{108123,2,protocol_O2_KuChang_info2},
	{108123,3,protocol_O2_KuChang_info3},
	{108123,4,protocol_O2_KuChang_info4},
	{108123,5,protocol_O2_KuChang_info5},
	{108124,0,protocol_CEMS_KuChang_SO2_info},
	{108124,1,protocol_CEMS_KuChang_NOx_info},
	{108124,2,protocol_CEMS_KuChang_CO_info},
	{108125,0,protocol_PLC_KuChang_info},
	{108126,0,protocol_CEMS_ZhongKe_info},
	{108127,0,protocol_CEMS_BJXueDiLong_SCS900CPM_info},
	{108128,0,protocol_CEMS_ZhongKe_TR216I_info},
	{108129,0,protocol_CEMS_SHHeRu_APxx370_v1_6_SO2_info},
	{108129,1,protocol_CEMS_SHHeRu_APxx370_v1_6_NOx_info},
	{108130,0,protocol_CEMS_AHWanYi_SG1000_HeBei_SO2_info},
	{108130,1,protocol_CEMS_AHWanYi_SG1000_HeBei_NOx_info},
	{108130,2,protocol_CEMS_AHWanYi_SG1000_HeBei_O2_info},
	{108130,3,protocol_CEMS_AHWanYi_SG1000_HeBei_TSP_info},
	{108131,0,protocol_CEMS_HBManDeKe3_DCEM3000_info},
	{108132,0,protocol_TSP_SDXinZe_SDUST100_info},
	{108133,0,protocol_CEMS_SDXinZe_V2_STATUS_info},
	{108133,1,protocol_CEMS_SDXinZe_V2_SO2_info},
	{108133,2,protocol_CEMS_SDXinZe_V2_NO_info},
	{108133,3,protocol_CEMS_SDXinZe_V2_O2_info},
	{108134,0,protocol_CEMS_RuiAnRA8000_info},
	{108135,0,protocol_CEMS_BJXueDiLong_MODEL4000_1_STATUS_info},
	{108135,1,protocol_CEMS_BJXueDiLong_MODEL4000_1_info},
	{108135,2,protocol_CEMS_BJXueDiLong_MODEL4000_1_SO2_info},
	{108135,3,protocol_CEMS_BJXueDiLong_MODEL4000_1_NO_info},
	{108135,4,protocol_CEMS_BJXueDiLong_MODEL4000_1_NO2_info},
	{108135,5,protocol_CEMS_BJXueDiLong_MODEL4000_1_CO_info},
	{108135,6,protocol_CEMS_BJXueDiLong_MODEL4000_1_HCL_info},
	{108136,0,protocol_CEMS_HBManDeKe_V4_SO2_info},
	{108136,1,protocol_CEMS_HBManDeKe_V4_NO_info},
	{108136,2,protocol_CEMS_HBManDeKe_V4_O2_info},
	{108157,0,protocol_TSP_AnRomhXin_LSS2004_LSS2014_info},
	{108158,0,protocol_O2_AnRongXin_ARX5100_info},
	{108160,0,protocol_CEMS_HBManDeKe_NOx_info2},
	{108160,1,protocol_CEMS_HBManDeKe_SO2_info2},
	{108160,2,protocol_CEMS_HBManDeKe_O2_info2},
	{108163,0,protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_SO2_info},
	{108163,1,protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_NO_info},
	{108163,2,protocol_CEMS_BJXueDiLong_MODEL1080_v3_7_O2_info},
	{108165,0,protocol_TSP_QDJiaMing_LaserDustMP_info},
	{108166,0,protocol_CEMS_XiKeMaiHaKe_MCS100FT_SO2_info},
	{108166,1,protocol_CEMS_XiKeMaiHaKe_MCS100FT_HCL_info},
	{108166,2,protocol_CEMS_XiKeMaiHaKe_MCS100FT_CO_info},
	{108166,3,protocol_CEMS_XiKeMaiHaKe_MCS100FT_NOx_info},
	{108166,4,protocol_CEMS_XiKeMaiHaKe_MCS100FT_O2_info},
	{108169,0,protocol_CEMS_WuXiChenChuang_STATUS_info},
	{108169,1,protocol_CEMS_WuXiChenChuang_SO2_info},
	{108169,2,protocol_CEMS_WuXiChenChuang_NO_info},
	{108169,3,protocol_CEMS_WuXiChenChuang_O2_info},
	{108171,0,protocol_CEMS_QDZhongPing_SO2_info},
	{108171,1,protocol_CEMS_QDZhongPing_NO_info},
	{108171,2,protocol_CEMS_QDZhongPing_O2_info},
	{108172,0,protocol_TSP_QDZhongPing_LRCD2000WV_info},

	{108176,0,protocol_CEMS_BJHangTianYiLai_HeiBei_STATUS_info},
	{108176,1,protocol_CEMS_BJHangTianYiLai_HeiBei_SO2_info},
	{108176,2,protocol_CEMS_BJHangTianYiLai_HeiBei_NO2_info},
	{108176,3,protocol_CEMS_BJHangTianYiLai_HeiBei_NO_info},
	{108176,4,protocol_CEMS_BJHangTianYiLai_HeiBei_CO_info},
	{108176,5,protocol_CEMS_BJHangTianYiLai_HeiBei_HCL_info},
	{108178,0,protocol_CEMS_AHLvShi_STATUS_info},
	{108178,1,protocol_CEMS_AHLvShi_SO2_info},
	{108178,2,protocol_CEMS_AHLvShi_NO_info},
	{108178,3,protocol_CEMS_AHLvShi_O2_info},
	{108179,0,protocol_TSP_ZhongXing_DM601_Low_info},

	{108201,0,protocol_PLC_SHBeiFen_SBF1500_SO2_INFO},
	{108201,1,protocol_PLC_SHBeiFen_SBF1500_NO_INFO},
	{108201,2,protocol_PLC_SHBeiFen_SBF1500_NO2_INFO},
	{108201,3,protocol_PLC_SHBeiFen_SBF1500_HCL_INFO},
	{108201,4,protocol_PLC_SHBeiFen_SBF1500_O2_INFO},
	{108202,0,protocol_CEMS_ChuoMei_hjt2005_HeBei_SO2},
	{108202,1,protocol_CEMS_ChuoMei_hjt2005_HeBei_NOx},
	{108202,2,protocol_CEMS_ChuoMei_hjt2005_HeBei_O2},
	{108203,0,protocol_CEMS_BJXueDiLong_1080uv_SO2_info},
	{108203,1,protocol_CEMS_BJXueDiLong_1080uv_NOx_info},
	{108203,2,protocol_CEMS_BJXueDiLong_1080uv_O2_info},
	{108204,0,protocol_CEMS_HBManDeKe1_DCEM3000_info},
	{108205,0,protocol_CEMS_HBManDeKe2_DCEM3000_info},
	{108206,0,protocol_CEMS_BJXueDiLong_MODEL1080_v4_SO2_info},
	{108206,1,protocol_CEMS_BJXueDiLong_MODEL1080_v4_NO_info},
	{108206,2,protocol_CEMS_BJXueDiLong_MODEL1080_v4_O2_info},
	{108206,3,protocol_CEMS_BJXueDiLong_MODEL1080_v4_CO_info},
	{108206,4,protocol_CEMS_BJXueDiLong_MODEL1080_v4_CO2_info},
	{108206,5,protocol_CEMS_BJXueDiLong_MODEL1080_v4_CH4_info},
	{108207,0,protocol_CEMS_SaiMoFei_42i_HI_100_info},
	{108208,0,protocol_CEMS_SaiMoFei_42i_LO_100_info},
	{108209,0,protocol_CEMS_SaiMoFei_43i_HI_100_info},
	{108210,0,protocol_CEMS_SaiMoFei_43i_LO_100_info},
	{108211,0,protocol_CEMS_SaiMoFei_TXO_1000_100_info1},
	{108211,1,protocol_CEMS_SaiMoFei_TXO_1000_100_info2},
	
	//{108407,0,protocol_test_yuiki_info},
	//{108407,1,protocol_test_yuiki_info1},
	//{108407,2,protocol_test_yuiki_info2},

	{108528,0,protocol_CEMS_TianZhiLan_HBCangZhou_STATUS},

	{108700,0,protocol_CEMS_DaoJin_URA208A_data},
	{108700,1,protocol_CEMS_DaoJin_URA208A_SO2_info},
	{108700,2,protocol_CEMS_DaoJin_URA208A_NOx_info},
	{108700,3,protocol_CEMS_DaoJin_URA208A_O2_info},
	{108700,4,protocol_CEMS_DaoJin_URA208A_CO_info},
	{108702,0,protocol_CEMS_DaoJin_URA208A_data},
	{108702,1,protocol_CEMS_DaoJin_URA208A_SO2_info},
	{108702,2,protocol_CEMS_DaoJin_URA209_NO_info},
	{108702,3,protocol_CEMS_DaoJin_URA208A_O2_info},
	{108702,4,protocol_CEMS_DaoJin_URA208A_CO_info},
	{108704,0,protocol_CEMS_ZeTian_HeBeiModbus_STATUS},
	{108704,1,protocol_CEMS_ZeTian_HeBeiModbus_SO2_info},
	{108704,2,protocol_CEMS_ZeTian_HeBeiModbus_NOx_info},
	{108704,3,protocol_CEMS_ZeTian_HeBeiModbus_NO2_info},
	{108704,4,protocol_CEMS_ZeTian_HeBeiModbus_O2_info},
	{108704,5,protocol_CEMS_ZeTian_HeBeiModbus_CO_info}, //del by miles zhang 20210519
	{108704,6,protocol_CEMS_ZeTian_HeBeiModbus_CO2_info}, //del by miles zhang 20210519
	{108710,0,protocol_VOCs_TianJin712_DHT508_STATUS_info},
	{108710,1,protocol_VOCs_TianJin712_DHT508_THC_info},
	{108710,2,protocol_VOCs_TianJin712_DHT508_NMHC_info},
	{108710,3,protocol_VOCs_TianJin712_DHT508_NMHC_CH4_info},

	{109889,0,protocol_rdtu_t36z_info},
};


#endif /*__COMMON_PRO__*/

