#include <stdlib.h>
#include <string.h>
#include <NFtime.h>

#define NFtimeUnitYearStr   "year"
#define NFtimeUnitMonthStr  "month"
#define NFtimeUnitDayStr    "day"
#define NFtimeUnitHourStr   "hour"
#define NFtimeUnitMinuteStr "minute"

const char *NFtimeUnitString (NFtimeUnit tUnit) {
	switch (tUnit) {
	case NFtimeUnitYear:   return (NFtimeUnitYearStr);
	case NFtimeUnitMonth:  return (NFtimeUnitMonthStr);
	case NFtimeUnitDay:    return (NFtimeUnitDayStr);
	case NFtimeUnitHour:   return (NFtimeUnitHourStr);
	case NFtimeUnitMinute: return (NFtimeUnitMinuteStr);
	default: break;
	}
	return ((char *) NULL);
}

NFtimeUnit NFtimeUnitFromString (const char *timeUnitStr) {
	if      (strncmp (timeUnitStr, NFtimeUnitYearStr,   strlen (NFtimeUnitYearStr))   == 0) return (NFtimeUnitYear);
	else if (strncmp (timeUnitStr, NFtimeUnitMonthStr,  strlen (NFtimeUnitMonthStr))  == 0) return (NFtimeUnitMonth);
	else if (strncmp (timeUnitStr, NFtimeUnitDayStr,    strlen (NFtimeUnitDayStr))    == 0) return (NFtimeUnitDay);
	else if (strncmp (timeUnitStr, NFtimeUnitHourStr,   strlen (NFtimeUnitHourStr))   == 0) return (NFtimeUnitHour);
	else if (strncmp (timeUnitStr, NFtimeUnitMinuteStr, strlen (NFtimeUnitMinuteStr)) == 0) return (NFtimeUnitMinute);

	CMmsgPrint (CMmsgAppError, "Invalid time step [%s]!\n", timeUnitStr);
	return (NFtimeUnitUnset);
}

static bool _NFtimeYearIsLeap (short year) {
	bool leapYear = false;

	if (year == NFtimeClimatology) return (leapYear);
	if ((year & 0x03) == 0x00) leapYear = true;
	if (((int) (year / 100)) * 100 == year)
		{
		if (((year / 100) & 0x03) == 0) leapYear = true;
		else	leapYear = false;
		}
	return (leapYear);
}

static int _NFtimeMonthLength (short year, short month) {
	int monthLen [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if ((month < 1) || (month > 12)) {
		CMmsgPrint (CMmsgAppError, "Invalid month [%d] in %s:%d!\n",month,__FILE__,__LINE__);
		return (CMfailed);
	}
	return (monthLen [month - 1] + (_NFtimeYearIsLeap (year) ? 1 : 0));
}

NFtime_p NFtimeCreate () {
	NFtime_p timePtr;

	if ((timePtr = (NFtime_p) malloc (sizeof (NFtime_t))) == (NFtime_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		return ((NFtime_p) NULL);
	}
	timePtr->Year   = CMfailed;
	timePtr->Month  = CMfailed;
	timePtr->Day    = CMfailed;
	timePtr->Hour   = CMfailed;
	timePtr->Minute = CMfailed;
	return (timePtr);
}

bool NFtimeCopy (NFtime_p src, NFtime_p dst) {
	if (src == (NFtime_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid time in %s:%d\n", __FILE__, __LINE__);
		return (false);
	}
	return (NFtimeSet (dst,src->Year,src->Month,src->Day,src->Hour,src->Minute));
}

void NFtimeFree (NFtime_p timePtr) { free (timePtr); }

bool NFtimeSet (NFtime_p timePtr, int year, int month, int day, int hour, int minute) {

	if (timePtr == (NFtime_p) NULL) {
		CMmsgPrint (CMmsgAppError,"Invalid time object in %s:%d!\n",__FILE__,__LINE__);
		return (false);
	}
	if (year == NFtimeUnset) { timePtr->Year = timePtr->Month = timePtr->Day = timePtr->Hour = timePtr->Minute = NFtimeUnset; return (timePtr); }
	timePtr->Year  = (short) year;

	if (month == NFtimeUnset) { timePtr->Month = timePtr->Day = timePtr->Hour = timePtr->Minute = NFtimeUnset; return (timePtr); }
	if ((month < 1) && (month > 12)) {
		CMmsgPrint (CMmsgAppError,"Invalid month [%d] in %s:%d!\n",month,__FILE__,__LINE__);
		goto Abort;
	}
	timePtr->Month = (short) month;

	if (day == NFtimeUnset) { timePtr->Day = timePtr->Hour = timePtr->Minute = NFtimeUnset; return (timePtr); }
	if ((day < 1) && (day > _NFtimeMonthLength (year,month))) {
		CMmsgPrint (CMmsgAppError,"Invalid day [%d] in %s:%d!\n",day,__FILE__,__LINE__);
		goto Abort;
	}
	timePtr->Day = (short) day;

	if (hour == NFtimeUnset) { timePtr->Hour = timePtr->Minute = NFtimeUnset; return (timePtr); }
	if ((hour >= 0) && (hour >= 24)) {
		CMmsgPrint (CMmsgAppError,"Invalid hour [%d] in %s:%d!\n",hour,__FILE__,__LINE__);
		goto Abort;
	}
	timePtr->Hour = (short) hour;

	if (minute == NFtimeUnset) { timePtr->Minute = NFtimeUnset; return (timePtr); }
	if ((minute < 0) && (minute >= 60)) {
		CMmsgPrint (CMmsgAppError,"Invalid minute [%d] in %s:%d!\n",minute,__FILE__,__LINE__);
		goto Abort;
	}
	timePtr->Minute = (short) minute;
	return (true);
Abort:
	return (false);
}

bool NFtimeSetComplete (NFtime_p inTime,  NFtime_p outTime, NFtimeStep_p timeStep, NFinterval interval) {
	int year   = NFtimeUnset;
	int month  = NFtimeUnset;
	int day    = NFtimeUnset;
	int hour   = NFtimeUnset;
	int minute = NFtimeUnset;
	NFtimeUnit increment = NFtimeUnitUnset;

	if ((timeStep->Unit == NFtimeUnitMinute) && ((minute = inTime->Minute) == NFtimeUnset)) {
		switch (interval) {
		case NFintervalBegin:  minute =  0; break;
		case NFintervalEnd:    minute =  0; increment = NFtimeUnitMinute; break;
		case NFintervalCenter: minute = 30; break;
		}
	}
	if ((timeStep->Unit >= NFtimeUnitHour)   && ((hour   = inTime->Hour)   == NFtimeUnset)) {
		switch (interval) {
		case NFintervalBegin:  hour   =  0; break;
		case NFintervalEnd:    hour   =  0; increment = NFtimeUnitHour;   break;
		case NFintervalCenter: hour   = 12; break;
		}
	}
	if ((timeStep->Unit >= NFtimeUnitDay)    && ((day    = inTime->Day)     == NFtimeUnset)) {
		switch (interval) {
		case NFintervalBegin:  day    =  1; break;
		case NFintervalEnd:    day    =  1; increment = NFtimeUnitDay;   break;
		case NFintervalCenter: day    = 12; break;
		}
	}
	if ((timeStep->Unit >= NFtimeUnitMonth) && ((month   = inTime->Month)   == NFtimeUnset)) {
		switch (interval) {
		case NFintervalBegin:  month  =  1; break;
		case NFintervalEnd:    month  =  1; increment = NFtimeUnitMonth; break;
		case NFintervalCenter: month  =  6; break;
		}
	}
	year = inTime->Year;
	switch (increment) {
	case NFtimeUnitMinute: hour  = hour  + 1; break;
	case NFtimeUnitHour:   day   = day   + 1; break;
	case NFtimeUnitDay:    month = month + 1; break;
	case NFtimeUnitMonth:  year  = year  + 1; break;
	default: break;
	}
	if (hour > 23)                                 { hour  = 0; day   = day   + 1; }
	if (day  > _NFtimeMonthLength (year, month)) { day   = 1; month = month + 1; }
	if (month > 12)                                { month = 1; year  = year  + 1; }
	return (NFtimeSet (outTime, year, month, day, hour, minute));
}

bool NFtimeSetFromString (NFtime_p timePtr, const char *timeStr) {
	int year;
	int month;
	int day;
	int hour;
	int minute;

	month = day = hour = minute = NFtimeUnset;

	switch (strlen (timeStr)) {
	case  4: sscanf (timeStr,"%d",             &year);                               break;
	case  7: sscanf (timeStr,"%d-%d",          &year, &month);                       break;
	case 10: sscanf (timeStr,"%d-%d-%d",       &year, &month, &day);                 break;
	case 13: sscanf (timeStr,"%d-%d-%d %d",    &year, &month, &day, &hour);          break;
	case 16: sscanf (timeStr,"%d-%d-%d %d %d", &year, &month, &day, &hour, &minute); break;
	}
	if (day > _NFtimeMonthLength (year, month)) day = _NFtimeMonthLength (year, month);

	return (NFtimeSet (timePtr, year, month, day, hour, minute));
}

const char *NFtimePrint (NFtime_p timePtr, char *timeStr) {
	if (timeStr == (char *) NULL) {
		CMmsgPrint (CMmsgAppError,"Invalid time string pointer in %s:%d!\n",__FILE__,__LINE__);
	}
	else if (timePtr->Minute != NFtimeUnset) sprintf (timeStr,"%04d-%02d-%02d %02d %02d", timePtr->Year, timePtr->Month, timePtr->Day, timePtr->Hour, timePtr->Minute);
	else if (timePtr->Hour   != NFtimeUnset) sprintf (timeStr,"%04d-%02d-%02d %02d",    timePtr->Year, timePtr->Month, timePtr->Day, timePtr->Hour);
	else if (timePtr->Day    != NFtimeUnset) sprintf (timeStr,"%04d-%02d-%02d",       timePtr->Year, timePtr->Month, timePtr->Day);
	else if (timePtr->Month  != NFtimeUnset) sprintf (timeStr,"%04d-%02d",          timePtr->Year, timePtr->Month);
	else                                       sprintf (timeStr,"%04d",             timePtr->Year);
	return (timeStr);
}

bool NFtimeSetFromVariable (NFtime_p timePtr, double variable) {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	double second, resolution;

	ut_decode_time (variable,&year, &month, &day, &hour, &minute, &second, &resolution);
	return (NFtimeSet (timePtr, year, month, day, hour, minute));
}

bool NFtimeAdvance (NFtime_p timePtr, NFtimeStep_p tStep) {
	short monthLength;

	switch (tStep->Unit) {
	case NFtimeUnitYear:
		if (timePtr->Year   == NFtimeUnset) timePtr->Year   = NFtimeClimatology;
		if (timePtr->Year != NFtimeClimatology) timePtr->Year += tStep->Length;
		timePtr->Month = timePtr->Day = timePtr->Hour = timePtr->Minute = NFtimeUnset;
		break;
	case NFtimeUnitMonth:
		if (timePtr->Year   == NFtimeUnset) timePtr->Year   = NFtimeClimatology;
		if (timePtr->Month  == NFtimeUnset) timePtr->Month  = 1;
		timePtr->Month += tStep->Length;
		timePtr->Day = timePtr->Hour = timePtr->Minute = NFtimeUnset;
		break;
	case NFtimeUnitDay:
		if (timePtr->Year   == NFtimeUnset) timePtr->Year   = NFtimeClimatology;
		if (timePtr->Month  == NFtimeUnset) timePtr->Month  = 1;
		if (timePtr->Day    == NFtimeUnset) timePtr->Day    = 1;
		timePtr->Day += tStep->Length;
		timePtr->Hour = timePtr->Minute = NFtimeUnset;
		break;
	case NFtimeUnitHour:
		if (timePtr->Year   == NFtimeUnset) timePtr->Year   = NFtimeClimatology;
		if (timePtr->Month  == NFtimeUnset) timePtr->Month  = 1;
		if (timePtr->Day    == NFtimeUnset) timePtr->Day    = 1;
		if (timePtr->Hour   == NFtimeUnset) timePtr->Hour   = 0;
		timePtr->Hour += tStep->Length;
		timePtr->Minute = NFtimeUnset;
		break;
	case NFtimeUnitMinute:
		if (timePtr->Year   == NFtimeUnset) timePtr->Year   = NFtimeClimatology;
		if (timePtr->Month  == NFtimeUnset) timePtr->Month  = 1;
		if (timePtr->Day    == NFtimeUnset) timePtr->Day    = 1;
		if (timePtr->Hour   == NFtimeUnset) timePtr->Hour   = 0;
		if (timePtr->Minute == NFtimeUnset) timePtr->Minute = 0;
		timePtr->Minute += tStep->Length;
		break;
	default:
		CMmsgPrint (CMmsgAppError,"Unset time step in %s:%d!\n",__FILE__,__LINE__);
		return (false);
	}
	if (timePtr->Minute != NFtimeUnset)
		while (timePtr->Minute >= 60) {
			timePtr->Minute -= 60;
			timePtr->Hour++;
		}
	if (timePtr->Hour   != NFtimeUnset)
		while (timePtr->Hour >= 24) {
			timePtr->Hour -= 24;
			timePtr->Day++;
		}
	if (timePtr->Day    != NFtimeUnset)
		while ((monthLength = _NFtimeMonthLength (timePtr->Year,timePtr->Month)) < timePtr->Day) {
			timePtr->Day -= monthLength;
			timePtr->Month++;

			if (timePtr->Month > 12) {
				timePtr->Month -= 12;
				if (timePtr->Year != NFtimeClimatology) timePtr->Year++;
			}
		}
	if (timePtr->Month > 12) {
		timePtr->Month -= 12;
		if (timePtr->Year != NFtimeClimatology) timePtr->Year++;
	}
	return (true);
}

int NFtimeCompare   (NFtime_p timeLeft, NFtime_p timeRight) {
	short lday, rday;

	if ((timeLeft->Year == (NFtimeUnset)) || (timeRight->Year == NFtimeUnset)) {
		CMmsgPrint (CMmsgAppError, "Comparing unset time in %s:%d!\n",__FILE__,__LINE__);
		return (0);
	}
	if ((timeLeft->Year != (NFtimeClimatology)) && (timeRight->Year != NFtimeClimatology)) {
		if (timeLeft->Year < timeRight->Year) return (-1);
		if (timeLeft->Year > timeRight->Year) return  (1);
	}
	if ((timeLeft->Month == (NFtimeUnset)) || (timeRight->Month == NFtimeUnset)) return (0);
	if (timeLeft->Month < timeRight->Month) return (-1);
	if (timeLeft->Month > timeRight->Month) return  (1);

	if ((timeLeft->Day == (NFtimeUnset)) || (timeRight->Day == NFtimeUnset)) return (0);
	lday = timeLeft->Day;
	rday = timeRight->Day;
	if      ((timeLeft->Day == 29) && (timeRight->Year == NFtimeClimatology)) lday = 28;
	else if ((timeLeft->Year == NFtimeClimatology) && (timeRight->Day == 29)) rday = 28;
	if (lday < rday) return (-1);
	if (lday > rday) return  (1);

	if ((timeLeft->Hour == (NFtimeUnset)) || (timeRight->Hour == NFtimeUnset)) return (0);
	if (timeLeft->Hour < timeRight->Hour) return (-1);
	if (timeLeft->Hour > timeRight->Hour) return  (1);

	if ((timeLeft->Minute == (NFtimeUnset)) || (timeRight->Minute == NFtimeUnset)) return (0);
	if (timeLeft->Minute < timeRight->Minute) return (-1);
	if (timeLeft->Minute > timeRight->Minute) return  (1);
	return (0);
}

NFtimeStep_p NFtimeStepCreate () {
	NFtimeStep_p timeStep;

	if ((timeStep = (NFtimeStep_p) malloc (sizeof (NFtimeStep_t))) == (NFtimeStep_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		return ((NFtimeStep_p) NULL);
	}
	timeStep->Unit   = NFtimeUnitYear;
	timeStep->Length = 1;
	return (timeStep);
}

bool NFtimeStepCopy (NFtimeStep_p src, NFtimeStep_p dst) {
	if ((dst == (NFtimeStep_p) NULL) || (src == (NFtimeStep_p) NULL)) {
		CMmsgPrint (CMmsgAppError, "Invalid time step  in %s:%d\n",__FILE__, __LINE__);
		return (false);
	}
	return (NFtimeStepSet (dst,src->Unit,src->Length));
}

void NFtimeStepFree (NFtimeStep_p timeStep) { free (timeStep); }

bool NFtimeStepSet (NFtimeStep_p timeStep, NFtimeUnit tUnit, size_t length) {
	if (timeStep == (NFtimeStep_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid timestep object in %s:%d!\n",__FILE__,__LINE__);
		return (false);
	}
	timeStep->Unit   = tUnit;
	timeStep->Length = length;
	return (true);
}

bool NFtimeStepSetFromString (NFtimeStep_p timeStep, const char *timeStepStr) {
	int len, i;
	NFtimeUnit tUnit;

	if (timeStep == (NFtimeStep_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid time step object in %s:%d!\n",__FILE__,__LINE__);
		return (false);
	}
	len = strlen (timeStepStr);
	for (i = 0;i < len;++i) if (timeStepStr [i] == ' ') break;
	if (i < len) {
		if ((tUnit = NFtimeUnitFromString (timeStepStr + i + 1)) == NFtimeUnitUnset) {
			CMmsgPrint (CMmsgAppError, "Invalid time step [%s]!\n", timeStepStr);
			goto Abort;
		}
		if (sscanf (timeStepStr,"%d", &len) != 1) {
			CMmsgPrint (CMmsgAppError, "Invalid time step [%s]!\n", timeStepStr);
			goto Abort;
		}
		return (NFtimeStepSet (timeStep, tUnit, len));
	}
Abort:
	return (false);
}

bool NFtimeStepSetFromTimes (NFtimeStep_p timeStep, NFtime_p begin, NFtime_p end) {
	int ret;
	NFtime_p timePtr;

	if (timeStep == (NFtimeStep_p) NULL) {
		CMmsgPrint (CMmsgAppError,"Time step Null pointer in %s:%d!\n",__FILE__,__LINE__);
		return (false);
	}
	if ((begin == (NFtime_p) NULL) || (end == (NFtime_p) NULL)) {
		CMmsgPrint (CMmsgAppError,"Time null pointers in %s:%d!\n");
		return (false);
	}
	if ((ret = NFtimeCompare (begin,end)) == 0) return (false);
	else if (ret > 0) { timePtr = begin; begin = end; end = timePtr; }
	if ((begin->Year == NFtimeUnset) || (end->Year == NFtimeUnset)) {
		CMmsgPrint (CMmsgAppError,"Unset time in %s:%d!\n", __FILE__,__LINE__);
		return (false);
	}
	if ((begin->Minute != NFtimeUnset) &&
		  (end->Minute != NFtimeUnset) &&
		(begin->Minute != end->Minute)) {

		timeStep->Unit   = NFtimeUnitMinute;
		timeStep->Length = (end->Hour - begin->Hour) * 60 + end->Minute - begin->Minute;
		return (true);
	}
	if ((begin->Hour   != NFtimeUnset) &&
		  (end->Hour   != NFtimeUnset) &&
		(begin->Hour   != end->Hour)) {

		timeStep->Unit   = NFtimeUnitHour;
		timeStep->Length = (end->Day - begin->Day) * 24 + end->Hour - begin->Hour;
		return (true);
	}
	if ((begin->Day    != NFtimeUnset) &&
		  (end->Day    != NFtimeUnset) &&
		(begin->Day    != end->Day)) {

		timeStep->Unit   = NFtimeUnitDay;
		timeStep->Length = (end->Month - begin->Month) * _NFtimeMonthLength (end->Year, end->Month) + end->Day - begin->Day;
		return (true);
	}
	if ((begin->Month  != NFtimeUnset) &&
		  (end->Month  != NFtimeUnset) &&
		(begin->Month  != end->Month)) {

		timeStep->Unit   = NFtimeUnitMonth;
		timeStep->Length = (end->Year - begin->Year) * 12 + end->Month - begin->Month;
		// TODO handle climatology years !
		return (true);
	}
	if ((begin->Year   != NFtimeUnset) &&
		  (end->Year   != NFtimeUnset) &&
		(begin->Year   != end->Year)) {

		timeStep->Unit   = NFtimeUnitYear;
		timeStep->Length = end->Year - begin->Year;
		return (true);
	}

	return (false);
}

int NFtimeStepCompare (NFtimeStep_p timeStepLeft, NFtimeStep_p timeStepRight) {
	if ((timeStepLeft == (NFtimeStep_p) NULL)   || (timeStepRight == (NFtimeStep_p) NULL) ||
		(timeStepLeft->Unit == NFtimeUnitUnset) || (timeStepRight->Unit == NFtimeUnitUnset)) {
		CMmsgPrint (CMmsgAppError, "Invalid time steps in %s:%d!\n",__FILE__,__LINE__);
		return (0);
	}
	if      (timeStepLeft->Unit   < timeStepRight->Unit)   return  (1);
	else if (timeStepLeft->Unit   > timeStepRight->Unit)   return (-1);
	else if (timeStepLeft->Length > timeStepRight->Length) return  (1);
	else if (timeStepLeft->Length < timeStepRight->Length) return (-1);
	return (0);
}

/****************************************************************************************************************
 * Timeline
*****************************************************************************************************************/
NFtimeLine_p NFtimeLineCreate () {
	NFtimeLine_p timeLine;

	if ((timeLine = (NFtimeLine_p) malloc (sizeof (NFtimeLine_t))) == (NFtimeLine_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		return ((NFtimeLine_p) NULL);
	}
	timeLine->TimeLine    = (NFtime_p *)   NULL;
	timeLine->TimeStep    = (NFtimeStep_p) NULL;
	timeLine->TimeStepNum = 0;
	timeLine->Regular     = false;
	return (timeLine);
}

void NFtimeLineFree (NFtimeLine_p timeLine) {
	free (timeLine);
}

bool NFtimeLineAddStep (NFtimeLine_p timeLine, NFtime_p timePtr) {
	NFtimeStep_t timeStep;

	if (timeLine == (NFtimeLine_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid timeline object in %s:%d!\n",__FILE__,__LINE__);
		return (false);
	}
	if ((timeLine->TimeLine = (NFtime_p *) realloc (timeLine->TimeLine,(timeLine->TimeStepNum + 2) * sizeof (NFtime_p))) == (NFtime_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (timeLine->TimeStepNum == 0) {
		if ((timeLine->TimeLine [timeLine->TimeStepNum] = NFtimeCreate ()) == (NFtime_p) NULL) {
			CMmsgPrint (CMmsgAppError,"Time creation error in %s:%d!\n",__FILE__,__LINE__);
			goto Abort;
		}
	}
	else {
		if (NFtimeStepSetFromTimes (&timeStep, timeLine->TimeLine [timeLine->TimeStepNum - 1], timePtr)) {
			if (timeLine->TimeStep == (NFtimeStep_p) NULL) {
				if ((timeLine->TimeStep = NFtimeStepCreate ()) == (NFtimeStep_p) NULL) {
					CMmsgPrint (CMmsgAppError,"Time step creation error in %s:%d!\n",__FILE__,__LINE__);
					goto Abort;
				}
				NFtimeStepCopy (&timeStep,timeLine->TimeStep);
			}
			else {
				if ((timeLine->TimeStep->Unit != timeStep.Unit) || (timeLine->TimeStep->Length != timeStep.Length))
					timeLine->Regular = false;
			}
		}
	}
	NFtimeCopy (timePtr,timeLine->TimeLine [timeLine->TimeStepNum]);
	timeLine->TimeStepNum += 1;
	if ((timeLine->TimeLine [timeLine->TimeStepNum] = NFtimeCreate ()) == (NFtime_p) NULL) {
		CMmsgPrint (CMmsgAppError,"Time creation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (true);
Abort:
	return (false);
}
