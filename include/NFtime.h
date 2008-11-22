#ifndef _NFtime_H
#define _NFtime_H

#include <udunits2.h>
#include <NF.h>

typedef enum { NFtimeUnitUnset  =  0,
               NFtimeUnitYear   =  7,
               NFtimeUnitMonth  = 10,
               NFtimeUnitDay    = 13,
               NFtimeUnitHour   = 16,
               NFtimeUnitMinute = 19 } NFtimeUnit;

const char *NFtimeUnitString (NFtimeUnit);

enum { NFtimeClimatology = 0, NFtimeUnset = CMfailed };

typedef struct NFtime_s {
	short  Year;
	short  Month;
	short  Day;
	short  Hour;
	short  Minute;
} NFtime_t, *NFtime_p;

typedef struct NFtimeStep_s {
	size_t       Length;
	NFtimeUnit Unit;
} NFtimeStep_t, *NFtimeStep_p;

typedef struct NFtimeLine_s {
	size_t             TimeStepNum;
	NFtime_p        *TimeLine;
	NFtimeStep_p     TimeStep;
	bool               Regular;
} NFtimeLine_t, *NFtimeLine_p;

NFtime_p     NFtimeCreate            ();
bool           NFtimeCopy              (NFtime_p, NFtime_p);
void           NFtimeFree              (NFtime_p);
bool           NFtimeSet               (NFtime_p, int, int, int, int, int);
bool           NFtimeSetComplete       (NFtime_p, NFtime_p, NFtimeStep_p, NFinterval);
bool           NFtimeSetFromString     (NFtime_p, const char *);
bool           NFtimeSetFromVariable   (NFtime_p, double);
const char    *NFtimePrint             (NFtime_p, char *);
NFtimeLine_p NFtimeLineCreate        ();
bool           NFtimeLineInitialize    (NFtimeLine_p, NFtime_p, NFtime_p, NFtimeStep_p);
bool           NFtimeLineAddStep       (NFtimeLine_p, NFtime_p);
bool           NFtimeLineSetTimeStep   (NFtimeLine_p);
void           NFtimeLineFree          (NFtimeLine_p);
NFtimeStep_p NFtimeStepCreate        ();
bool           NFtimeStepCopy          (NFtimeStep_p, NFtimeStep_p);
bool           NFtimeStepSet           (NFtimeStep_p, NFtimeUnit, size_t);
bool           NFtimeStepSetFromString (NFtimeStep_p, const char *);
int            NFtimeStepCompare       (NFtimeStep_p, NFtimeStep_p);
void           NFtimeStepFree          (NFtimeStep_p);
bool           NFtimeAdvance           (NFtime_p, NFtimeStep_p);
int            NFtimeCompare           (NFtime_p, NFtime_p);
NFtimeUnit   NFtimeUnitFromString    (const char *);

#endif /* _NFtime_H*/
