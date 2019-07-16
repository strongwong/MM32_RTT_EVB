////////////////////////////////////////////////////////////////////////////////
#define _DATETIME_C_
////////////////////////////////////////////////////////////////////////////////

#include "string.h"
#include "HAL_device.h"
#include "main.h"
#include "datetime.h"
#include "rtc.h"
#include "led_key.h"

////////////////////////////////////////////////////////////////////////////////
u32 DateTimeToSeconds(dateTimeDef *tp)
{
	u32 LeapY = (tp->year == 2000) ? 0 : (( tp->year - 2000 - 1) / 4 + 1);
	u32 ComY  = (tp->year - 2000) - LeapY;
	
	u32 Days =  ( tp->year % 4) ? 
		(LeapY * 366 + ComY * 365 + Month_Days_Accu_C[tp->month - 1] + (tp->day - 1)) : 
		(LeapY * 366 + ComY * 365 + Month_Days_Accu_L[tp->month - 1] + (tp->day - 1)); 
		
		
	return Days * SecsPerDay + tp->hours * 3600 + tp->minute * 60 + tp->second;
}

////////////////////////////////////////////////////////////////////////////////
u8 GetWeek(u32* sec)
{
	return  ((*sec / SecsPerDay) + 6) % 7;										//2000/1/1 week Saturday
}

////////////////////////////////////////////////////////////////////////////////
u16 GetYear(u32* sec)
{
	u8 i;
	u32 OffSec = *sec % SecsPerFourYear;
	for (i = 0; i < 4; i++){
		if (OffSec < Year_Secs_Accu[i + 1])
			break;
	}
	u16 year = 2000 + (*sec / SecsPerFourYear * 4) + i;
	*sec =  OffSec - Year_Secs_Accu[i];
	return year;
}

////////////////////////////////////////////////////////////////////////////////
u16 GetMouthItem(u32 *sec,  u32 *item)
{
	u8 i = 0;
	for (i = 0; i < 12; i++){
		if (*sec < *(item + i + 1)) {
			*sec = *sec - *(item + i);
			break;
		}
	}
	return i + 1;
}

////////////////////////////////////////////////////////////////////////////////
u16 GetMouth(u16 year, u32* sec)
{
	return (year % 4) ? GetMouthItem(sec,  &Month_Secs_Accu_C[0]) : 
		                GetMouthItem(sec,  &Month_Secs_Accu_L[0]);
}

////////////////////////////////////////////////////////////////////////////////
u16 GetDayItem(u16 day,  u16 *item)
{
	for (u8 i = 0; i < 12; i++){
		if (day < *(item + i + 1)) {
			day = day - *(item + i);
			break;
		}
	}
	return day + 1;
}

////////////////////////////////////////////////////////////////////////////////
u16 GetDay(u16 year, u32 *sec)
{
	u16 day = *sec / SecsPerDay;
	*sec = *sec % SecsPerDay;
	return (year % 4) ? GetDayItem(day,  &Month_Days_Accu_C[0]) : 
		                GetDayItem(day,  &Month_Days_Accu_L[0]);
}

////////////////////////////////////////////////////////////////////////////////
void RTC_GetTime(dateTimeDef *tp)
{ 
	u32 sec = RTC_GetCounter();
	
	tp->week = GetWeek(&sec);
	tp->year = GetYear(&sec);
	tp->month = GetMouth(tp->year ,&sec);
	tp->day = GetDay(tp->year ,&sec);
	
	tp->hours 	=  sec / 3600;
	tp->minute 	= (sec % 3600) / 60;
	tp->second 	= (sec % 3600) % 60;
}

////////////////////////////////////////////////////////////////////////////////
void RTC_SetTime(dateTimeDef *tp)
{
	RTC_SetCounter(DateTimeToSeconds(tp));
}

void adjYear()
{
	if (Key3CntF)	{if (--gtp.year < 2016)	gtp.year = 2025;}
	else 			{if (++gtp.year > 2025)	gtp.year = 2016;}
	u8 day = Month_Days[gtp.month - 1];
	if (gtp.year % 4)	day--; 
	if (gtp.day > day) gtp.day = day;
}
void adjMonth()
{
	if (Key3CntF)	{if (--gtp.month < 1) 	gtp.month = 12;}
	else 			{if (++gtp.month > 12) 	gtp.month = 1;}
	u8 day = Month_Days[gtp.month - 1];
	if (gtp.year % 4)	day--; 
	if (gtp.day > day) gtp.day = day;
}

void adjDay()
{
	u8 day = Month_Days[gtp.month - 1];
	if (gtp.year % 4)	day--; 
	if (Key3CntF)	{if (--gtp.day < 1)	 	gtp.day = day;}
	else 			{if (++gtp.day > day) 	gtp.day = 1;}
}

void adjHours()
{
	if (Key3CntF)	{if (--gtp.hours == -1) gtp.hours = 23;}
	else 			{if (++gtp.hours > 23) 	gtp.hours = 0;}
}		

void adjMinute()
{
	if (Key3CntF)	{if (--gtp.minute == -1) gtp.minute = 59;}
	else 			{if (++gtp.minute > 59) 	gtp.minute = 0;}
}

void adjSecond()
{
	if (Key3CntF)	{if (--gtp.second == -1) gtp.second = 59;}
	else 			{if (++gtp.second > 59) 	gtp.second = 0;}
}
////////////////////////////////////////////////////////////////////////////////
void modifyTime(u8 sel)
{
	switch (sel) {
	case 0:		adjYear();		break;
	case 1:		adjMonth();		break;
	case 2:		adjDay();		break;
	case 3:		adjHours();		break;
	case 4:		adjMinute();	break;
	case 5:		adjSecond();	break;
	default:	RTC_SetTime(&gtp);		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
