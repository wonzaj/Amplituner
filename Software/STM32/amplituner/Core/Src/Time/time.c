//--------------------------------------------------------------
/* time.c handles operation on time and alarms
*/
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include <Time/time.h>

//--------------------------------------------------------------
// Variables
//--------------------------------------------------------------
_Bool alarm_set_A_or_B  = true;
_Bool PreviewAlarm 	= true;
char  AlarmMode[25];


//--------------------------------------------------------------
// Function definitions
//--------------------------------------------------------------

//
void SetAlarm(__attribute__((__unused__))RTC_AlarmTypeDef Alarm_)
    {
	Alarm_.AlarmTime.Hours = 0;
	Alarm_.AlarmTime.Minutes = 0;
	Alarm_.AlarmTime.Seconds = 0;
	Alarm_.AlarmTime.SubSeconds = 0;
	Alarm_.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	Alarm_.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	Alarm_.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	Alarm_.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	Alarm_.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	Alarm_.AlarmDateWeekDay = 1;
    }

//
void RTC_Alarm_activated_routine()
    {
	SSD1322_Screen_State = SSD1322_SCREEN_WakeUp;
	//init preamp
	//init display
	//init radio
    }

//
void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
    {

	HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BIN);

	switch (RTC_typeOfAlarm_B)
	    {
	    case ONLY_ONE_TIME_ALARM:
		__HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRB);
		IS_ALARM_SET_B = false;
		RTC_Alarm_activated_routine();
		break;
	    case ONLY_WEEKENDS_ALARM:
		if ((sDate.WeekDay == RTC_WEEKDAY_SATURDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_SUNDAY))
		    RTC_Alarm_activated_routine();
		break;
	    case EVERYDAY_ALARM:
		//keep interupt on
		break;
	    case MON_to_FRI_ALARM: //go to alarm routine if it is not weekend
		if((sDate.WeekDay == RTC_WEEKDAY_MONDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_TUESDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_WEDNESDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_THURSDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_FRIDAY))
		    RTC_Alarm_activated_routine();
		break;
	    case EXACT_DATE_ALRAM: //trzeba dzie??
		__HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRB);
		IS_ALARM_SET_A = false;
		RTC_Alarm_activated_routine();
		break;
	    default:
		__HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRB);
		IS_ALARM_SET_A = false;
		break;
	    }

    }

//
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
    {
	HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BIN);

	switch (RTC_typeOfAlarm_A)
	    {
	    case ONLY_ONE_TIME_ALARM:
		__HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRA);
		IS_ALARM_SET_A = false;
		RTC_Alarm_activated_routine();
		break;
	    case ONLY_WEEKENDS_ALARM:
		if ((sDate.WeekDay == RTC_WEEKDAY_SATURDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_SUNDAY))
		    RTC_Alarm_activated_routine();
		break;
	    case EVERYDAY_ALARM:
		//keep interupt on
		break;
	    case MON_to_FRI_ALARM: //go to alarm routine if it is not weekend
		if((sDate.WeekDay == RTC_WEEKDAY_MONDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_TUESDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_WEDNESDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_THURSDAY)
		|| (sDate.WeekDay == RTC_WEEKDAY_FRIDAY))
		    RTC_Alarm_activated_routine();
		break;
	    case EXACT_DATE_ALRAM: //trzeba dzie??
		__HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRA);
		IS_ALARM_SET_A = false;
		RTC_Alarm_activated_routine();
		break;
	    default:
		__HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRA);
		IS_ALARM_SET_A = false;
		break;
	    }
	//ustawia jak???? flag??, kt??ra trzeba potem wy????czy?? przyciskiem
	// doda?? ifa, switcha albo funkcje kt??ra zwraca typ alarmu
	//wylaczyc alarm w zaleznosci od ustawienia
	//sprawdzi?? czy przekaznik jest wlaczony i zal??czy?? muzyk?? w zale??no??ci od ustawie??
    }

//
void switch_change_time(Clock_Data_Change_t Clock, _Bool add_subb)
    {
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	switch (Clock)
	    {
	    case HOUR:
		if(add_subb)sTime.Hours--;
		else sTime.Hours++;
		if(sTime.Hours == UINT8_MAX) sTime.Hours = 23;
		if (IS_RTC_HOUR24(sTime.Hours)!= true) sTime.Hours = 0x00;
		break;
	    case MINUTE:
		if(add_subb)sTime.Minutes--;
		else sTime.Minutes++;
		if(sTime.Minutes == UINT8_MAX) sTime.Minutes = 59;
		if (IS_RTC_MINUTES(sTime.Minutes)!= true) sTime.Minutes = 0x00;
		break;
	    case SECOND:
		if(add_subb)sTime.Seconds--;
		else sTime.Seconds++;
		if(sTime.Seconds == UINT8_MAX) sTime.Seconds = 59;
		if (IS_RTC_SECONDS(sTime.Seconds)!= true) sTime.Seconds = 0x00;
		break;
	    case DAYWEEK:
		if(add_subb)sDate.WeekDay--;
		else sDate.WeekDay++;
		if(sDate.WeekDay == UINT8_MAX) sDate.WeekDay = 7;
		if (IS_RTC_WEEKDAY(sDate.WeekDay)!= true) sDate.WeekDay = 0x00;
		break;
	    case DAY:
		if(add_subb)sDate.Date--;
		else sDate.Date++;
		if(sDate.Date == UINT8_MAX) sDate.Date = 30;
		if (IS_RTC_DATE(sDate.Date)!= true) sDate.Date = 0x00;
		break;
	    case MONTH:
		if(add_subb)sDate.Month--;
		else sDate.Month++;
		if(sDate.Month == UINT8_MAX) sDate.Month = 12;
		if (IS_RTC_MONTH(sDate.Month)!= true) sDate.Month = 0x00;
		break;
	    case YEAR:
		if(add_subb)sDate.Year--;
		else sDate.Year++;
		if(sDate.Year == UINT8_MAX) sDate.Year = 0;
		if (IS_RTC_YEAR(sDate.Year)!= true) sDate.Year = 0x00;
		break;
	    default:
		//reset all values
		break;
	    }
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    }

//
void Set_Alarm_Mode(RTC_typeOfAlarm_t typeOfAlarm)
    {
	// zrobic z napisow consty
	switch (typeOfAlarm)
	    {
	    case ONLY_ONE_TIME_ALARM:
		strcpy(AlarmMode, "Jeden raz");
		break;
	    case ONLY_WEEKENDS_ALARM:
		strcpy(AlarmMode, "Weekendy");
		break;
	    case EVERYDAY_ALARM:
		strcpy(AlarmMode, "Codziennie");
		break;
	    case MON_to_FRI_ALARM:
		strcpy(AlarmMode, "Pon-Pt");
		break;
	    case EXACT_DATE_ALRAM:
		strcpy(AlarmMode, "Konkretny Dzien");
		break;
	    default:
		break;
	    }
    }

//
void switch_change_alarm(AlarmDataChange_t AlarmDataChange, _Bool add_subb)
    {
	//doda?? wyswietlanie ustawionego alarmu, mo??e jednocze??nie albo wybieralnie
	//w wyswietlaniu u??y?? HAL_RTC_GetAlarm
	switch (AlarmDataChange)
	    {
	    case HOUR_ALARM:
		if(add_subb == true)Alarm.AlarmTime.Hours--;
		else Alarm.AlarmTime.Hours++;

		if(Alarm.AlarmTime.Hours == UINT8_MAX) Alarm.AlarmTime.Hours = 23;

		if (IS_RTC_HOUR24(Alarm.AlarmTime.Hours)!= true) Alarm.AlarmTime.Hours = 0x00;
		break;
	    case MINUTE_ALARM:
		if(add_subb == true)Alarm.AlarmTime.Minutes--;
		else Alarm.AlarmTime.Minutes++;
		if(Alarm.AlarmTime.Minutes == UINT8_MAX) Alarm.AlarmTime.Minutes = 59;
		if (IS_RTC_MINUTES(Alarm.AlarmTime.Minutes)!= true) Alarm.AlarmTime.Minutes = 0x00;
		break;
	    case DAYWEEK_ALARM:
		if (Alarm.AlarmDateWeekDaySel == RTC_ALARMDATEWEEKDAYSEL_WEEKDAY)
		    {
			//doda?? zeby wy??wietla??o tak??e dany dzie?? tygodnia w tym przypadku
			if(add_subb == true)Alarm.AlarmDateWeekDay--;
			else Alarm.AlarmDateWeekDay++;
			if(Alarm.AlarmDateWeekDay == UINT8_MAX) Alarm.AlarmDateWeekDay = 7;
			if (IS_RTC_WEEKDAY(Alarm.AlarmDateWeekDay)!= true) Alarm.AlarmDateWeekDay = 0x00;
		    }
		else if(Alarm.AlarmDateWeekDaySel == RTC_ALARMDATEWEEKDAYSEL_DATE )
		    {
			if (add_subb == true)Alarm.AlarmDateWeekDay--;
			else Alarm.AlarmDateWeekDay++;
			if (Alarm.AlarmDateWeekDay == UINT8_MAX)Alarm.AlarmDateWeekDay = 31;
			if (IS_RTC_DATE(Alarm.AlarmDateWeekDay) != true)Alarm.AlarmDateWeekDay = 0x00;
		    }
		break;
	    case MODE_ALARM:
		if(add_subb == true)RTC_typeOfAlarm++;
		else RTC_typeOfAlarm--;
		if (RTC_typeOfAlarm == UINT8_MAX)RTC_typeOfAlarm = 1; //przepe??nienie
		else if(RTC_typeOfAlarm == 6)RTC_typeOfAlarm = 1;
		if((RTC_typeOfAlarm == ONLY_WEEKENDS_ALARM) || (RTC_typeOfAlarm == MON_to_FRI_ALARM))
		    Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
		else Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
		if(RTC_typeOfAlarm == EXACT_DATE_ALRAM)Alarm.AlarmMask = RTC_ALARMMASK_NONE;
		else Alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
		break;
	    case SET_ALARM:
		if(add_subb == false)      // button down  alarm_set_A_or_B - true - alarm A, false - alarm B
		    {
			if(alarm_set_A_or_B == true)alarm_set_A_or_B = false; //da?? enum false pierwszy i da?? alarm_a
			else alarm_set_A_or_B = true;
		    }
		else if (add_subb == true) //button up
		    {
			if (alarm_set_A_or_B == true)
			    {
				if (IS_ALARM_SET_A == false) //if alarm is set FLAG then deactive by pressing this button
				    {
					Alarm_A = Alarm;
					Alarm_A.Alarm = RTC_ALARM_A;
					RTC_typeOfAlarm_A = RTC_typeOfAlarm;
					HAL_RTC_SetAlarm_IT(&hrtc, &Alarm_A, RTC_FORMAT_BIN);
					IS_ALARM_SET_A = true;
					//go to screen_2 or show set alarm
				    }
				else if (IS_ALARM_SET_A == true)
				    {
					HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
					IS_ALARM_SET_A = false;
				    }
			    }
			else if (alarm_set_A_or_B == false)
			    {
				if (IS_ALARM_SET_B == false) //if alarm is set FLAG then deactive by pressing this button
				    {
					Alarm_B = Alarm;
					Alarm_B.Alarm = RTC_ALARM_B;
					RTC_typeOfAlarm_B = RTC_typeOfAlarm;
					HAL_RTC_SetAlarm_IT(&hrtc, &Alarm_B, RTC_FORMAT_BIN);
					IS_ALARM_SET_B = true;
					//go to screen_2 or show set alarm
				    }
				else if (IS_ALARM_SET_B == true)
				    {
					HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_B);
					IS_ALARM_SET_B = false;
				    }
			    }
		    }
		break;
	    case PREVIEW_ALARM:
		//zmienianie flagi ekarnu
		if(PreviewAlarm == true)PreviewAlarm = false;
		else PreviewAlarm = true;
		break;
	    default:
		break;
	    }
    }
