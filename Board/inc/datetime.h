////////////////////////////////////////////////////////////////////////////////
#ifndef __DATETIME_H_
#define __DATETIME_H_
////////////////////////////////////////////////////////////////////////////////

#define SecsPerComYear  31536000  		//(365*3600*24)
#define SecsPerLeapYear 31622400		//(366*3600*24)
#define SecsPerFourYear 126230400		//((365*3600*24)*3+(366*3600*24))
#define SecsPerDay      86400			//(3600 * 24)

typedef struct 
{
	u32 week;
	u32 year;
	u32 month;
	u32 day;
	u32 hours;
	u32 minute;
	u32 second;
}dateTimeDef ;


////////////////////////////////////////////////////////////////////////////////
#ifdef _DATETIME_C_
#define GLOBAL


u32 Year_Secs_Accu[5] =
{
	0,
	31622400,
	63158400,
	94694400,
	126230400
};




u32 Month_Secs_Accu_C[13] = 
{
	0,
	2678400,
	5097600,
	7776000,
	10368000,
	13046400,
	15638400,
	18316800,
	20995200,
	23587200,
	26265600,
	28857600,
	31536000
};

u32 Month_Secs_Accu_L[13] = 
{
	0,
	2678400,
	5184000,
	7862400, 
	10454400,
	13132800,
	15724800,
	18403200,
	21081600,
	23673600,
	26352000,
	28944000,
	31622400
};

u16 Month_Days_Accu_C[13] = {0,31,59,90,120,151,181,212,243,273,304,334,365};
u16 Month_Days_Accu_L[13] = {0,31,60,91,121,152,182,213,244,274,305,335,366};
u16 Month_Days[12] = {31,29,31,30,31,30,31,31,30,31,30,31};

#else
#define GLOBAL extern
#endif


GLOBAL u32 timeSel;
GLOBAL dateTimeDef gtp;
	
GLOBAL u8 menuCnt;



#undef GLOBAL
////////////////////////////////////////////////////////////////////////////////

void RTC_GetTime(dateTimeDef *tp);	
void RTC_SetTime(dateTimeDef *tp);

u32 DateTimeToSeconds(dateTimeDef *ct);
void modifyTime(u8 timeSel);

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
