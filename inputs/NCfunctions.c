#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include <cm.h>
#include "NCfunctions.h"

NCurlType NCurlGetType (const char *url) {
	int ret;
	NCurlType  codes [] = { NCurlFile, NCurlHttp, NCurlFtp, NCurlInvalid };
	const char *opts [] = { "file",  "http",  "ftp", NULL };

	if ((ret = CMoptLookup (opts, url, false)) == CMfailed) {
		CMmsgPrint (CMmsgUsrError,"Invalid url: \"%s\"!\n", url);
		return (NCurlInvalid);
	}
	return (codes [ret]);
}

const char *NCurlGetAddress (const char *url) {
	size_t i, len, tokenLen;
	char *token = "://";

	if (url == (const char *) NULL) return ((const char *) NULL);

	tokenLen = strlen (token);
	len = strlen (url) - tokenLen;
	for (i = 0;i < len;++i)
		if (strncmp (url + i, token, tokenLen) == 0)
			return (url + i + tokenLen);
	return ((const char *) NULL);
}

static size_t _NCbundleMatchString (const char *url, NFtimeUnit bundleTimeUnit) {
	size_t i, len;
	const char *yearStr   = "${YEAR}";
	const char *monthStr  = "${YEAR-MO}";
	const char *dailyStr  = "${YEAR-MO-DY}";
	const char *hourlyStr = "${YEAR-MO-DY hr}";
	const char *minuteStr = "${YEAR-MO-DY hr:mn}";

	len = strlen (url);
	switch (bundleTimeUnit) {
	case NFtimeUnitYear:
		for (i = 0;i < len; ++i) if (strncmp (url + i, yearStr, NFtimeUnitYear)     == 0) return (i);
		break;
	case NFtimeUnitMonth:
		for (i = 0;i < len; ++i) if (strncmp (url + i, monthStr, NFtimeUnitMonth)   == 0) return (i);
		break;
	case NFtimeUnitDay:
		for (i = 0;i < len; ++i) if (strncmp (url + i, dailyStr, NFtimeUnitDay)     == 0) return (i);
		break;
	case NFtimeUnitHour:
		for (i = 0;i < len; ++i) if (strncmp (url + i, hourlyStr, NFtimeUnitHour)   == 0) return (i);
		break;
	case NFtimeUnitMinute:
		for (i = 0;i < len; ++i) if (strncmp (url + i, minuteStr, NFtimeUnitMinute) == 0) return (i);
		break;
	default: break;
	}
	return (len);
}

NFtimeUnit NCurlGetBundleTimeUnit (const char *url) {
	size_t len;

	len = strlen (url);
	if      (_NCbundleMatchString (url, NFtimeUnitYear)   < len) return (NFtimeUnitYear);
	else if (_NCbundleMatchString (url, NFtimeUnitMonth)  < len) return (NFtimeUnitMonth);
	else if (_NCbundleMatchString (url, NFtimeUnitDay)    < len) return (NFtimeUnitDay);
	else if (_NCbundleMatchString (url, NFtimeUnitHour)   < len) return (NFtimeUnitHour);
	else if (_NCbundleMatchString (url, NFtimeUnitMinute) < len) return (NFtimeUnitMinute);

	return (NFtimeUnitUnset);
}

char *NCcompleteURLaddress (const char *url, char *fileName, NFtimeUnit timeUnit, NFtime_p time) {
	size_t i;

	i = _NCbundleMatchString (url, timeUnit);
	strncpy (fileName, url, i);

	switch (timeUnit) {
	case NFtimeUnitYear:   sprintf (fileName + i,"%04d%s",                     time->Year,                                                  url + i + timeUnit); break;
	case NFtimeUnitMonth:  sprintf (fileName + i,"%04d-%02d%s",                time->Year, time->Month,                                     url + i + timeUnit); break;
	case NFtimeUnitDay:    sprintf (fileName + i,"%04d-%02d-%02d%s",           time->Year, time->Month, time->Day,                          url + i + timeUnit); break;
	case NFtimeUnitHour:   sprintf (fileName + i,"%04d-%02d-%02d %02d%s",      time->Year, time->Month, time->Day, time->Hour,              url + i + timeUnit); break;
	case NFtimeUnitMinute: sprintf (fileName + i,"%04d-%02d-%02d %02d:%02d%s", time->Year, time->Month, time->Day, time->Hour,time->Minute, url + i + timeUnit); break;
	default: strcpy (fileName, url); break;
	}
	return (fileName);
}

NCaxis_t *NCaxisCreate () {
	NCaxis_p axis;

	if ((axis = (NCaxis_p) malloc (sizeof (NCaxis_t))) == (NCaxis_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Axis memory llocation error in: %s:%d\n",__FILE__,__LINE__);
		return ((NCaxis_t *) NULL);
	}
	axis->Axis         = (char *)    NULL;
	axis->LongName     = (char *)    NULL;
	axis->StandardName = (char *)    NULL;
	axis->Unit         = (ut_unit *) NULL;
	axis->Data         = (double *)  NULL;
	axis->Bounds       = (double *)  NULL;
	return (axis);
}

void NCaxisFree (NCaxis_p axis) {
	if (axis == (NCaxis_p) NULL) return;

	if (axis->Axis         != (char *)   NULL) free (axis->Axis);
	if (axis->LongName     != (char *)   NULL) free (axis->LongName);
	if (axis->StandardName != (char *)   NULL) free (axis->StandardName);
	if (axis->Data         != (double *) NULL) free (axis->Data);
	if (axis->Bounds       != (double *) NULL) free (axis->Bounds);
	if (axis->Unit != (ut_unit *) NULL) ut_free (axis->Unit);
}

static const char *_NCaxisGetTypeString (NCaxisType axisType) {
	switch (axisType) {
	case NCaxisX:    return ("X");
	case NCaxisY:    return ("Y");
	case NCaxisZ:    return ("Z");
	case NCaxisTime: return ("Time");
	}
	return ((char *) NULL);
}

int NCfindVariableByAttribute (int ncid, int ndims, const char *attribute, const char *content) {
	int status;
	int varid;
	int nvars;
	int n;
	size_t attlen;
	char text [NC_MAX_NAME + 1];

	if ((status = nc_inq_nvars (ncid, &nvars)) != NC_NOERR) {
		CMmsgPrint (CMmsgAppError,"NetCDF file inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
		return (CMfailed);
	}
	for (varid = 0;varid < nvars; ++varid) {
		if (ndims > 0) {
			if ((status = nc_inq_varndims (ncid, varid, &n)) != NC_NOERR) {
				CMmsgPrint (CMmsgAppError,"NetCDF variable dimensions inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
				return (CMfailed);			
			}
			if (n != ndims) continue;
		}
		if ((status = nc_inq_attlen (ncid, varid, attribute, &attlen)) != NC_NOERR) continue;
		if ((attlen > strlen (content)) || (attlen > NC_MAX_NAME)) continue;
		if ((status = nc_get_att_text (ncid,varid,attribute,text)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError,"NetCDF attribute inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
			return (CMfailed);			
		}
		else text [attlen] = '\0';
		if (strcmp (text,content) == 0) break;
 	}
	return (varid < nvars ? varid : CMfailed);
}

bool NCaxisInitialize (int ncid, NCaxis_p axis, NCaxisType axisType, ut_system *utSystem) {
	int status;
	int varid;
	int ndims;
	size_t attlen, i;
	char text [NC_MAX_NAME + 1];
	double interval;
	bool doUnits = true;

	switch (axis->AxisType = axisType) {
	case NCaxisX:
		if (((varid = NCfindVariableByAttribute (ncid,1,NCaxisStr,"x")) == CMfailed) &&
		    ((varid = NCfindVariableByAttribute (ncid,1,NCaxisStr,"X")) == CMfailed)) {
			CMmsgPrint (CMmsgAppError, "Missing x axis variable in NetCDF file!\n");
			return (false);
		}
		break;
	case NCaxisY:
		if (((varid = NCfindVariableByAttribute (ncid,1,NCaxisStr,"y")) == CMfailed) &&
		    ((varid = NCfindVariableByAttribute (ncid,1,NCaxisStr,"Y")) == CMfailed)) {
			CMmsgPrint (CMmsgAppError, "Missing y axis variable in NetCDF file!\n");
			return (false);
		}
		break;
	case NCaxisZ:
		if (((varid = NCfindVariableByAttribute (ncid,1,NCaxisStr,"z")) == CMfailed) &&
		    ((varid = NCfindVariableByAttribute (ncid,1,NCaxisStr,"Z")) == CMfailed)) {
			axis->Dimid = 0;
			if (((axis->Data   = (double *) malloc (sizeof (double)))     == (double *) NULL) ||
				((axis->Bounds = (double *) malloc (sizeof (double) * 2)) == (double *) NULL)) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
				return (false);
			}
			axis->N     = 1;
			axis->Dimid = CMfailed;
			axis->Data [0] = axis->Bounds [0] = axis->Bounds [1] = 0.0;
			return (true);
		}
		break;
	case NCaxisTime:
		if (((varid = NCfindVariableByAttribute (ncid,1,NCaxisStr,"t"))    == CMfailed) &&
		    ((varid = NCfindVariableByAttribute (ncid,1,NCaxisStr,"time")) == CMfailed)) {
			CMmsgPrint (CMmsgAppError, "Missing time axis variable in NetCDF file!\n");
			return (false);
		}
		break;
	}
	if (((status = nc_inq_varndims (ncid,varid,&ndims))           != NC_NOERR) || (ndims != 1) ||
		((status = nc_inq_vardimid (ncid,varid,&(axis->Dimid)))   != NC_NOERR) ||
	    ((status = nc_inq_dimlen   (ncid,axis->Dimid,&(axis->N))) != NC_NOERR)) {
		CMmsgPrint (CMmsgAppError,"NetCDF variable dimension inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
		return (false);
	}
	if ((status = nc_inq_attlen   (ncid,varid,NCaxisStr,&attlen)) == NC_NOERR) {
		if ((axis->Axis = (char *) malloc (attlen + 1)) == (char *) NULL) {
			CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d!\n",__FILE__,__LINE__);
			return (false);
		}
		if ((status = nc_get_att_text (ncid,varid,NCaxisStr,axis->Axis)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError,"NetCDF attribute inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
			return (false);
		}
		else axis->Axis [attlen] = '\0';
	}
	if ((status = nc_inq_attlen   (ncid,varid,NClongNameStr,&attlen)) == NC_NOERR) {
		if ((axis->LongName = (char *) malloc (attlen + 1)) == (char *) NULL) {
			CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d!\n",__FILE__,__LINE__);
			return (false);
		}
		if ((status = nc_get_att_text (ncid,varid,NClongNameStr,axis->LongName)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError,"NetCDF attribute inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
			return (false);
		}
		else axis->LongName [attlen] = '\0';
	}
	if ((status = nc_inq_attlen   (ncid,varid,NCstandardNameStr,&attlen)) == NC_NOERR) {
		if ((axis->StandardName = (char *) malloc (attlen + 1)) == (char *) NULL) {
			CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d!\n",__FILE__,__LINE__);
			return (false);
		}
		if ((status = nc_get_att_text (ncid,varid,NCstandardNameStr,axis->StandardName)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError,"NetCDF attribute inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
			return (false);
		}
		else axis->StandardName [attlen] = '\0';
	}
	if (doUnits) {
		if ((status = nc_inq_attlen   (ncid,varid,NCunitsStr,&attlen)) == NC_NOERR) {
			if (attlen > 0) {
				if ((status = nc_get_att_text (ncid,varid,NCunitsStr,text)) != NC_NOERR) {
					CMmsgPrint (CMmsgAppError,"NetCDF attribute inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
					return (false);
				}
				else text [attlen] = '\0';
				if ((axis->Unit = ut_parse (utSystem, ut_trim (text, UT_ASCII), UT_ASCII)) == (ut_unit *) NULL) {
					switch (ut_get_status ()) {
					case UT_BAD_ARG: CMmsgPrint (CMmsgAppError, "System or string is NULL!\n");               break;
					case UT_SYNTAX:  CMmsgPrint (CMmsgAppError, "String contained a syntax error!n");         break;
					case UT_UNKNOWN: CMmsgPrint (CMmsgAppError, "String contained an unknown identifier!\n"); break;
					default:         CMmsgPrint (CMmsgSysError, "System error in %s:%d!n",__FILE__,__LINE__);
					}
				}
			}
		}
	}
	if (((status = nc_inq_attlen   (ncid,varid,NCpositiveStr,&attlen)) == NC_NOERR) && (attlen < NC_MAX_NAME)) {
		if ((status = nc_get_att_text (ncid,varid,NCpositiveStr,text)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError,"NetCDF attribute inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
			return (false);
		}
		else text [attlen] = '\0';
		axis->Direction = strcmp (text,NCpositiveUpStr) == 0 ? NCdirUp : NCdirDown;
	}
	else axis->Direction = NCdirUp;
	if (((axis->Data   = (double *) malloc (axis->N * sizeof (double)))     == (double *) NULL) ||
	    ((axis->Bounds = (double *) malloc (axis->N * sizeof (double) * 2)) == (double *) NULL)) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d!\n",__FILE__,__LINE__);
		return (false);
	}
	if ((status = nc_get_var_double (ncid,varid,axis->Data)) != NC_NOERR) {
		CMmsgPrint (CMmsgAppError,"NetCDF %s axis data loading error \"%s\" in %s:%d!\n",_NCaxisGetTypeString (axis->AxisType),nc_strerror (status),__FILE__,__LINE__);
		return (false);
	}
	if (((status = nc_inq_attlen (ncid,varid,NCboundsStr, &attlen)) == NC_NOERR) && (attlen < NC_MAX_NAME)) {
		if ((status = nc_get_att_text (ncid,varid,NCboundsStr,text)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError,"NetCDF bounds attribute inquiring error \"%s\" in %s:%d!\n",nc_strerror (status),__FILE__,__LINE__);
			return (false);
		}
		else text [attlen] = '\0';
		if ((status = nc_inq_varid (ncid,text,&varid)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError,"NetCDF bounds variable inquery error \"%s\" in %s:%d!\n",   nc_strerror (status),__FILE__,__LINE__);
			return (false);
		}
		if ((status = nc_get_var_double (ncid,varid,axis->Bounds)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError,"NetCDF %s axis bounds loading error \"%s\" in %s:%d!\n",_NCaxisGetTypeString (axis->AxisType),nc_strerror (status),__FILE__,__LINE__);
			return (false);
		}
		
	}
	else {
		for (i = 1;i < axis->N - 1; ++i) {
			axis->Bounds [i * 2]     = (axis->Data [i]     + axis->Data [i - 1]) / 2.0;
			axis->Bounds [i * 2 + 1] = (axis->Data [i + 1] + axis->Data [i])     / 2.0;
		}
		axis->Bounds [1]         = axis->Bounds [2];
		axis->Bounds [0]         = axis->Data [0] - (axis->Bounds [1] - axis->Data [0]);
		axis->Bounds [i * 2]     = axis->Bounds [(i - 1) * 2 + 1];
		axis->Bounds [i * 2 + 1] = axis->Data [i] + (axis->Data [i]   - axis->Bounds [i * 2]);
	}
	axis->IntervalMin = axis->Bounds [i * 2 + 1] - axis->Bounds [0];
	axis->IntervalMax = 0.0;
	for (i = 0;i < axis->N; ++i) {
		interval = axis->Bounds [i * 2 + 1] - axis->Bounds [i * 2];
		axis->IntervalMin = CMmathMinimum (axis->IntervalMin,interval);
		axis->IntervalMax = CMmathMaximum (axis->IntervalMax,interval);
	}
	axis->Regular = axis->N > 1 ? CMmathEqualValues (axis->IntervalMin,axis->IntervalMax) : true;
	return (true);
}

NFtimeLine_p NCtimeLineExpand (NCaxis_p timeAxis, NFtime_p beginTime, NFtime_p endTime, cv_converter *cvConverter, NFtimeLine_p timeLine) {
	size_t timeStep = 0;
	NFtime_p timePtr = (NFtime_p) NULL;
	double variable;

	if ((timePtr = (NFtimeCreate ())) == (NFtime_p) NULL) {
		CMmsgPrint (CMmsgAppError,"Time object creation error in %s:%d!\n",__FILE__,__LINE__);
		return ((NFtimeLine_p) NULL);
	}
	if (timeLine == (NFtimeLine_p) NULL) timeLine = NFtimeLineCreate ();
	
	for (timeStep = 0;timeStep < timeAxis->N; ++timeStep) {
		variable = cv_convert_double (cvConverter, timeAxis->Bounds [timeStep * 2]);
		if (NFtimeSetFromVariable (timePtr, variable) == false) {
			CMmsgPrint (CMmsgAppError, "Time reading error in %s:%d!\n",__FILE__,__LINE__);
			NFtimeLineFree (timeLine);
			return ((NFtimeLine_p) NULL);
		}
		if (NFtimeCompare (beginTime, timePtr) <= 0) break;
	}
	if (timeStep < timeAxis->N) {
		for ( ;timeStep < timeAxis->N; ++timeStep) {
			variable = cv_convert_double (cvConverter, timeAxis->Bounds [timeStep * 2]);
			if (NFtimeSetFromVariable (timePtr, variable) == false) {
				CMmsgPrint (CMmsgAppError, "Time reading error in %s:%d!\n",__FILE__,__LINE__);
				NFtimeLineFree (timeLine);
				return ((NFtimeLine_p) NULL);
			}
			if (NFtimeLineAddStep (timeLine,timePtr) == false) {
				CMmsgPrint (CMmsgAppError, "Time step insertion error in %s:%d!\n",__FILE__,__LINE__);
				return ((NFtimeLine_p) NULL);
			}
			variable = cv_convert_double (cvConverter, timeAxis->Bounds [timeStep * 2 + 1]);
			if (NFtimeSetFromVariable (timePtr, variable) == false) {
				CMmsgPrint (CMmsgAppError, "Time reading error in %s:%d!\n",__FILE__,__LINE__);
				NFtimeLineFree (timeLine);
				return ((NFtimeLine_p) NULL);
			}
			if (NFtimeCompare (endTime, timePtr) < 0) break;			
		}
		if (NFtimeSetFromVariable (timePtr, variable) == false) {
			CMmsgPrint (CMmsgAppError, "Time reading error in %s:%d!\n",__FILE__,__LINE__);
			NFtimeLineFree (timeLine);
			return ((NFtimeLine_p) NULL);
		}
		NFtimeCopy (timePtr,timeLine->TimeLine [timeLine->TimeStepNum]);
	}
	else {
		CMmsgPrint (CMmsgAppError, "Time axis outside of time period in %s:%d!\n",__FILE__,__LINE__);
		NFtimeLineFree (timeLine);
		return ((NFtimeLine_p) NULL);
	}
	if (timePtr != (NFtime_p) NULL) NFtimeFree (timePtr);
	return (timeLine);
}
