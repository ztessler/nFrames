#ifndef _NCfunctions_H
#define _NCfunctions_H

#include <stdbool.h>
#include <udunits2.h>
#include <NFtime.h>

typedef enum { NCaxisX, NCaxisY, NCaxisZ, NCaxisTime } NCaxisType;
typedef enum { NCprojGeographic, NCprojRotatedPole, NCprojCartesian } NCprojType;

#define NCaxisStr         "axis"
#define NCboundsStr       "bounds"
#define NCcoordinateStr   "coordinates"
#define NCstandardNameStr "standard_name"
#define NClongNameStr     "long_name"
#define NCpositiveStr     "positive"
#define NCpositiveUpStr   "up"
#define NCpositiveDownStr "down"
#define NCunitsStr        "units"

typedef enum { NCdirUp = true, NCdirDown = false, NCdirLeft = true, NDdirRight = false } NCdirection;

typedef struct NCaxis_s {
	NCaxisType AxisType;
	int      Dimid;
	char    *Axis;
	char    *LongName;
	char    *StandardName;
	ut_unit *Unit;
	size_t   N;
	bool     Regular;
	NCdirection Direction;
	double  *Data;
	double  *Bounds;
	double   IntervalMin;
	double   IntervalMax;
} NCaxis_t, *NCaxis_p;

NCaxis_t *NCaxisCreate ();
void      NCaxisFree (NCaxis_p);
bool NCaxisInitialize (int, NCaxis_p, NCaxisType, ut_system *);
int  NCfindVariableByAttribute (int, int, const char *, const char *);

typedef enum { NCurlInvalid, NCurlFile, NCurlHttp, NCurlFtp } NCurlType;

NCurlType      NCurlGetType           (const char *);
const char    *NCurlGetAddress        (const char *);
NFtimeUnit   NCurlGetBundleTimeUnit (const char *);
char          *NCcompleteURLaddress   (const char *, char *, NFtimeUnit, NFtime_p);
NFtimeLine_p NCtimeLineExpand       (NCaxis_p, NFtime_p, NFtime_p, cv_converter *, NFtimeLine_p);

#endif /* _NCfunctions_H */
