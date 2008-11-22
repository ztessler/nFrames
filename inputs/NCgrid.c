#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include <cm.h>
#include <NFio.h>
#include "NCfunctions.h"

typedef struct NCgridVariable_s {
	int       *VarIDs;
	ut_unit  **Units;
} NCgridVariable_t, *NCgridVariable_p;

typedef struct NCgrid_s {
	NCprojType ProjType;
	int        NCid, Open;
	size_t     FileNum;
	char     **FileNames;
	NCaxis_p  *X;
	NCaxis_p  *Y;
	NCaxis_p  *Z;
	NCaxis_p  *Time;
	size_t     VarNum;
	size_t    *TimeBundleIDs;
	NCgridVariable_p *Variables;
} NCgrid_t, *NCgrid_p;

static NCgridVariable_p _NCgridVariableCreate (size_t fileNum) {
	size_t i;
	NCgridVariable_p gridVariable;

	if (((gridVariable = (NCgridVariable_p)   calloc (1,       sizeof (NCgridVariable_t))) == (NCgridVariable_p) NULL) ||
	    ((gridVariable->VarIDs = (int *)      calloc (fileNum, sizeof (int)))              == (int *)            NULL) ||
	    ((gridVariable->Units  = (ut_unit **) calloc (fileNum, sizeof (ut_unit *)))        == (ut_unit **)       NULL)) {
		CMmsgPrint (CMmsgSysError,"Memory alloction error in %s:%s!\n",__FILE__,__LINE__);
		return ((NCgridVariable_p) NULL);
	}
	for (i = 0;i < fileNum; ++i) {
		gridVariable->VarIDs [i] = CMfailed;
		gridVariable->Units  [i] = (ut_unit *) NULL;
	}
	return (gridVariable);
}

static void _NCgridVariableFree (NCgridVariable_p gridVariable, size_t fileNum) {
	size_t i;

	for (i = 0;i < fileNum; ++i) {
		if (gridVariable->Units [i] != (ut_unit *) NULL) ut_free (gridVariable->Units [i]);
	}
	if (gridVariable != (NCgridVariable_p) NULL) free (gridVariable);
}

static NFvarHandle _NCgridVariableHandle (NFio_p io, const char *standardName) {
	int varid, ncid, status;
	size_t i;
	NCgrid_p ncGrid = (NCgrid_p) io->PluginData;

	if ((varid = NCfindVariableByAttribute (ncGrid->NCid, 0, NCstandardNameStr, standardName)) == CMfailed) {
		return ((NFvarHandle) NULL);
	}
	for (i = 0;i < ncGrid->VarNum; ++i)
		if (ncGrid->Variables [i]->VarIDs [0] == varid) {
			CMmsgPrint (CMmsgUsrError, "Multiple request to the same variable!\n");
			return ((NFvarHandle) (ncGrid->Variables [i]));
		}
	if ((ncGrid->Variables = (NCgridVariable_p *) realloc (ncGrid->Variables,(ncGrid->VarNum + 1) * sizeof (NCgridVariable_p))) == (NCgridVariable_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d:\n",__FILE__,__LINE__);
		return ((NFvarHandle) NULL);
	}
	if ((ncGrid->Variables [ncGrid->VarNum] = _NCgridVariableCreate (ncGrid->FileNum)) == (NCgridVariable_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d:\n",__FILE__,__LINE__);
		return ((NFvarHandle) NULL);
	}
	ncGrid->Variables [ncGrid->VarNum]->VarIDs [0] = varid;
	for (i = 1;i < ncGrid->FileNum; ++i) {
		if ((status = nc_open (ncGrid->FileNames [i], NC_NOWRITE,&ncid)) != NC_NOERR) {
			CMmsgPrint (CMmsgAppError, "NetCDF (%s) Openining error \"%s\"!\n", ncGrid->FileNames [i], nc_strerror(status));
			goto Abort;
		 }
		if ((varid = NCfindVariableByAttribute (ncid, 0, NCstandardNameStr, standardName)) == CMfailed) {
			return ((NFvarHandle) NULL);
		}
		ncGrid->Variables [ncGrid->VarNum]->VarIDs [i] = varid;
		nc_close (ncid);
	}
	ncGrid->VarNum++;
	return ((NFvarHandle) (ncGrid->Variables [ncGrid->VarNum - 1]));
Abort:
	_NCgridVariableFree (ncGrid->Variables [ncGrid->VarNum],ncGrid->FileNum);
	return ((NFvarHandle) NULL);
}

static NFvarType _NCgridVariableType (NFio_p io, NFvarHandle varHandle) {
	int status;
	NCgridVariable_p gridVariable = (NCgridVariable_p) varHandle;
	nc_type vartype;

	if ((status = nc_inq_vartype (((NCgrid_p) (io->PluginData))->NCid, gridVariable->VarIDs [0], &vartype)) != NC_NOERR) {
		CMmsgPrint (CMmsgAppError, "Variable type inquery error in %s:%d!\n",__FILE__,__LINE__);
		return (NFvarTypeVoid);
	}
	switch (vartype) {
	default:        return (NFvarTypeVoid);
	case NC_CHAR:   return (NFvarTypeByte);
	case NC_SHORT:
	case NC_INT:    return (NFvarTypeShort);
	case NC_FLOAT:  return (NFvarTypeFloat);
	case NC_DOUBLE: return (NFvarTypeDouble);
	}
	return (NFvarTypeVoid);
}

static bool _NCgridVariableLoad (NFio_p io, NFvarHandle varHandle, NFvarType varType, size_t step, void *data) {
	
	return (true);
}

static void _NCgridFree (NCgrid_p ncGrid) {
	size_t i;
	for (i = 0; i < ncGrid->FileNum; ++i) {
		if (ncGrid->FileNames [i] != (char *) NULL) free (ncGrid->FileNames [i]);
		NCaxisFree (ncGrid->X [i]);
		NCaxisFree (ncGrid->Y [i]);
		NCaxisFree (ncGrid->Z [i]);
		NCaxisFree (ncGrid->Time [i]);
	}
	free (ncGrid->FileNames);
	free (ncGrid->X);
	free (ncGrid->Y);
	free (ncGrid->Z);
	free (ncGrid->Time);

	if (ncGrid->Variables != (NCgridVariable_p *) NULL) {
		for (i = 0;i < ncGrid->VarNum; ++i) _NCgridVariableFree (ncGrid->Variables [i],ncGrid->FileNum);
		free (ncGrid->Variables);
	}
	free (ncGrid);
}

static NCgrid_p _NCgridRealloc (NCgrid_p ncGrid) {
	
	if ((ncGrid->FileNames = (char **) realloc (ncGrid->FileNames, (ncGrid->FileNum + 1) * sizeof (char *))) == (char **) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	else ncGrid->FileNames [ncGrid->FileNum] = (char *) NULL;
	if ((ncGrid->X    = (NCaxis_p *) realloc (ncGrid->X,           (ncGrid->FileNum + 1) * sizeof (NCaxis_p))) == (NCaxis_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	else ncGrid->X [ncGrid->FileNum]    = (NCaxis_t *) NULL;
	if ((ncGrid->Y    = (NCaxis_p *) realloc (ncGrid->Y,           (ncGrid->FileNum + 1) * sizeof (NCaxis_p))) == (NCaxis_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	else ncGrid->Y [ncGrid->FileNum]    = (NCaxis_t *) NULL;
	if ((ncGrid->Z    = (NCaxis_p *) realloc (ncGrid->Z,           (ncGrid->FileNum + 1) * sizeof (NCaxis_p))) == (NCaxis_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	else ncGrid->Z [ncGrid->FileNum]    = (NCaxis_t *) NULL;
	if ((ncGrid->Time = (NCaxis_p *) realloc (ncGrid->Time,        (ncGrid->FileNum + 1) * sizeof (NCaxis_p))) == (NCaxis_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	else ncGrid->Time [ncGrid->FileNum] = (NCaxis_t *) NULL;
	ncGrid->FileNum += 1;

	ncGrid->Variables = (NCgridVariable_p *) NULL;
	ncGrid->VarNum    = 0;
	if ((ncGrid->X    [ncGrid->FileNum - 1] = NCaxisCreate ()) == (NCaxis_t *) NULL) {
		CMmsgPrint (CMmsgAppError, "X axis creation error in: %s%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((ncGrid->Y    [ncGrid->FileNum - 1] = NCaxisCreate ()) == (NCaxis_t *) NULL) {
		CMmsgPrint (CMmsgAppError, "Y axis creation error in: %s%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((ncGrid->Z    [ncGrid->FileNum - 1] = NCaxisCreate ()) == (NCaxis_t *) NULL) {
		CMmsgPrint (CMmsgAppError, "Y axis creation error in: %s%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((ncGrid->Time [ncGrid->FileNum - 1] = NCaxisCreate ()) == (NCaxis_t *) NULL) {
		CMmsgPrint (CMmsgAppError, "Time axis creation error in: %s%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (ncGrid);
Abort:
	_NCgridFree (ncGrid);
	return ((NCgrid_p) NULL);
}

static NCgrid_p _NCgridCreate () {
	NCgrid_p ncGrid;

	if ((ncGrid = (NCgrid_p) malloc (sizeof (NCgrid_t))) == (NCgrid_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Plugin data allocation error in: %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	ncGrid->Open      = CMfailed;
	ncGrid->FileNum   = 0;
	ncGrid->FileNames = (char **) NULL;
	ncGrid->X = ncGrid->Y = ncGrid->Z = ncGrid->Time = (NCaxis_p *) NULL;
	ncGrid->TimeBundleIDs = (size_t *) NULL;
	ncGrid->Variables = (NCgridVariable_p *) NULL;
	return (_NCgridRealloc (ncGrid));
Abort:
	if (ncGrid->FileNames != (char **) NULL) free (ncGrid->FileNames);

	if (ncGrid->X    != (NCaxis_p *) NULL) { NCaxisFree (ncGrid->X [0]);    free (ncGrid->X [0]); }
 	if (ncGrid->Y    != (NCaxis_p *) NULL) { NCaxisFree (ncGrid->Y [0]);    free (ncGrid->Y [0]); }
	if (ncGrid->Z    != (NCaxis_p *) NULL) { NCaxisFree (ncGrid->Z [0]);    free (ncGrid->Z [0]); }
	if (ncGrid->Time != (NCaxis_p *) NULL) { NCaxisFree (ncGrid->Time [0]); free (ncGrid->Time [0]); }
	return ((NCgrid_p) NULL);
}

void _NCgridClose (NFio_p io) {
	_NCgridFree ((NCgrid_p) io->PluginData);
}

NFio_p Open_NCgrid (const char *url, NFtime_p beginTime, NFtime_p endTime, ut_system *utSystem) {
	int status, ncid;
	size_t i;
	const char *urlAddress;
	NCurlType urlType;
	NFio_p        io             = (NFio_p)       NULL;
	NFtime_p      timePtr        = (NFtime_p)     NULL;
	NFtime_p      bundleTime     = (NFtime_p)     NULL;
	NFtimeUnit    bundleTimeUnit;
	NFtimeStep_p  bundleTimeStep = (NFtimeStep_p) NULL;
	NCgrid_p        ncGrid         = (NCgrid_p)       NULL;
	ut_unit        *baseTimeUnit   = (ut_unit *)      NULL;
	cv_converter   *cvConverter    = (cv_converter *) NULL;

	if ((baseTimeUnit = ut_parse (utSystem, "seconds since 2001-01-01 00:00:00", UT_ASCII)) == (ut_unit *) NULL) {
		CMmsgPrint (CMmsgAppError, "Total metal gebasz in %s:%d!\n",__FILE__,__LINE__);
		switch (ut_get_status ()) {
		case UT_BAD_ARG: CMmsgPrint (CMmsgAppError, "System or string is NULL!\n");               break;
		case UT_SYNTAX:  CMmsgPrint (CMmsgAppError, "String contained a syntax error!n");         break;
		case UT_UNKNOWN: CMmsgPrint (CMmsgAppError, "String contained an unknown identifier!\n"); break;
		default:         CMmsgPrint (CMmsgSysError, "System error in %s:%d!n",__FILE__,__LINE__);
		}
		goto Abort;
	}
	if ((bundleTime = NFtimeCreate ()) == (NFtime_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Time object creation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((bundleTimeStep = NFtimeStepCreate ()) == (NFtimeStep_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Timestep object creation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
		
	}
	bundleTimeUnit = NCurlGetBundleTimeUnit (url);
	if (((urlType = NCurlGetType (url)) == NCurlInvalid) || ((urlAddress = NCurlGetAddress (url)) == (const char *) NULL)) { 
		CMmsgPrint (CMmsgAppError, "Ivalid URL: %s!\n", url);
		goto Abort;
	}
	if ((io = NFioCreate ()) == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Layout creation error in: %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((ncGrid = _NCgridCreate ()) == (NCgrid_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Plugin data allocation error in: %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	ncGrid->FileNames [0] = CMstrDuplicate (urlAddress);
	if (bundleTimeUnit != NFtimeUnitUnset) {
		ncGrid->FileNames [0] = NCcompleteURLaddress (urlAddress, ncGrid->FileNames [0], bundleTimeUnit, beginTime);
		NFtimeStepSet (bundleTimeStep,bundleTimeUnit, 1); // TODO handle return value
	}

	if ((status = nc_open (ncGrid->FileNames [0], NC_NOWRITE,&(ncGrid->NCid))) != NC_NOERR) {
		CMmsgPrint (CMmsgAppError, "NetCDF (%s) Openining error \"%s\"!\n", ncGrid->FileNames [0], nc_strerror(status));
		goto Abort;
	 }
	ncGrid->Open = 0;
	if (!NCaxisInitialize (ncGrid->NCid, ncGrid->X [0],    NCaxisX,    utSystem)) goto Abort;
	if (!NCaxisInitialize (ncGrid->NCid, ncGrid->Y [0],    NCaxisY,    utSystem)) goto Abort;
	if (!NCaxisInitialize (ncGrid->NCid, ncGrid->Z [0],    NCaxisZ,    utSystem)) goto Abort;
	if (!NCaxisInitialize (ncGrid->NCid, ncGrid->Time [0], NCaxisTime, utSystem)) goto Abort;

	io->ItemNum      = ncGrid->X [0]->N * ncGrid->Y [0]->N * ncGrid->Z [0]->N;
	io->ItemBoxMin.X = ncGrid->X [0]->IntervalMin;
	io->ItemBoxMin.Y = ncGrid->Y [0]->IntervalMin;
	io->ItemBoxMax.X = ncGrid->X [0]->IntervalMax;
	io->ItemBoxMax.Y = ncGrid->Y [0]->IntervalMax;
	io->Extent.LowerLeft.X = ncGrid->X [0]->Bounds [ncGrid->X [0]->N * 2 - 1] - ncGrid->X [0]->Bounds [0];
	io->Extent.LowerLeft.Y = ncGrid->Y [0]->Bounds [ncGrid->Y [0]->N * 2 - 1] - ncGrid->Y [0]->Bounds [0];

	if ((cvConverter = ut_get_converter (ncGrid->Time [0]->Unit, baseTimeUnit)) == (cv_converter *) NULL) {
		CMmsgPrint (CMmsgAppError, "Time converter error!n");
		switch (ut_get_status ()) {
		case UT_BAD_ARG:         CMmsgPrint (CMmsgAppError, "unit1 or unit2 is NULL.\n");                         break; 
		case UT_NOT_SAME_SYSTEM: CMmsgPrint (CMmsgAppError, "unit1 and unit2 belong to different unit-systems."); break;
		default:                 CMmsgPrint (CMmsgAppError, "Conversion between the units is not possible.");     break;
		}
	}
	io->TimeLine = NCtimeLineExpand (ncGrid->Time [0], beginTime, endTime, cvConverter, io->TimeLine);
	if (io->TimeLine == (NFtimeLine_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Timeline expansion error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	cv_free (cvConverter);
	if ((ncGrid->TimeBundleIDs = (size_t *) realloc (ncGrid->TimeBundleIDs, io->TimeLine->TimeStepNum * sizeof (size_t))) == (size_t *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	for (i = 0;i < io->TimeLine->TimeStepNum; ++i) ncGrid->TimeBundleIDs [i] = 0;
	if (bundleTimeUnit != NFtimeUnitUnset) {
		NFtimeCopy (beginTime, bundleTime); // TODO handle return value
		if ((ncGrid = _NCgridRealloc (ncGrid)) == (NCgrid_p) NULL) goto Abort;

		for (NFtimeAdvance (bundleTime,bundleTimeStep);NFtimeCompare (bundleTime,endTime) < 0;NFtimeAdvance (bundleTime,bundleTimeStep)) {

			ncGrid->FileNames [ncGrid->FileNum - 1] = NCcompleteURLaddress (urlAddress, CMstrDuplicate (urlAddress), bundleTimeUnit, bundleTime);
			if ((status = nc_open (ncGrid->FileNames [ncGrid->FileNum - 1], NC_NOWRITE,&ncid)) != NC_NOERR) {
				CMmsgPrint (CMmsgAppError, "NetCDF (%s) Openining error \"%s\"!\n", ncGrid->FileNames [ncGrid->FileNum - 1], nc_strerror(status));
				goto Abort;
			 }
			if (!NCaxisInitialize (ncid, ncGrid->X    [ncGrid->FileNum - 1], NCaxisX,    utSystem)) goto Abort;
			if (!NCaxisInitialize (ncid, ncGrid->Y    [ncGrid->FileNum - 1], NCaxisY,    utSystem)) goto Abort;
			if (!NCaxisInitialize (ncid, ncGrid->Z    [ncGrid->FileNum - 1], NCaxisZ,    utSystem)) goto Abort;
			if (!NCaxisInitialize (ncid, ncGrid->Time [ncGrid->FileNum - 1], NCaxisTime, utSystem)) goto Abort;
			nc_close (ncid);
			if ((ncGrid->X    [0]->N != ncGrid->X    [ncGrid->FileNum - 1]->N) ||
			    (ncGrid->Y    [0]->N != ncGrid->Y    [ncGrid->FileNum - 1]->N) ||
			    (ncGrid->Z    [0]->N != ncGrid->Z    [ncGrid->FileNum - 1]->N) ||
			    (ncGrid->Time [0]->N != ncGrid->Time [ncGrid->FileNum - 1]->N)) {
				CMmsgPrint (CMmsgUsrError, "Inconsisten NetCDF [%s] bundle!\n",urlAddress);
				goto Abort;
			}
			if ((cvConverter = ut_get_converter (ncGrid->Time [ncGrid->FileNum - 1]->Unit, baseTimeUnit)) == (cv_converter *) NULL) {
				CMmsgPrint (CMmsgAppError, "Time converter error!n");
				switch (ut_get_status ()) {
				case UT_BAD_ARG:         CMmsgPrint (CMmsgAppError, "unit1 or unit2 is NULL.\n");                         break; 
				case UT_NOT_SAME_SYSTEM: CMmsgPrint (CMmsgAppError, "unit1 and unit2 belong to different unit-systems."); break;
				default:                 CMmsgPrint (CMmsgAppError, "Conversion between the units is not possible.");     break;
				}
			}
			io->TimeLine = NCtimeLineExpand (ncGrid->Time [ncGrid->FileNum - 1], beginTime, endTime, cvConverter, io->TimeLine);
			if (io->TimeLine == (NFtimeLine_p) NULL) {
				CMmsgPrint (CMmsgAppError, "Timeline expansion error in %s:%d!\n",__FILE__,__LINE__);
				goto Abort;
			}
			cv_free (cvConverter);
			if ((ncGrid->TimeBundleIDs = (size_t *) realloc (ncGrid->TimeBundleIDs, io->TimeLine->TimeStepNum * sizeof (size_t))) == (size_t *) NULL) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
				goto Abort;
			}
			for ( ;i < io->TimeLine->TimeStepNum; ++i) ncGrid->TimeBundleIDs [i] = ncGrid->FileNum - 1;
		}
	}
	io->PluginData    = (void *) ncGrid;
	io->Close         = _NCgridClose;
	io->VarHandleFunc = _NCgridVariableHandle;
	io->VarTypeFunc   = _NCgridVariableType;
	io->VarLoadFunc   = _NCgridVariableLoad;
	NFtimeFree     (timePtr);
	NFtimeFree     (bundleTime);
	NFtimeStepFree (bundleTimeStep);
	ut_free (baseTimeUnit);
	return (io);
Abort:
	nc_close (ncGrid->NCid);
	if (ncGrid != (NCgrid_p) NULL) _NCgridFree (ncGrid);
	if (io             != (NFio_p)       NULL) NFioFree       (io);
	if (timePtr        != (NFtime_p)     NULL) NFtimeFree     (timePtr);
	if (bundleTime     != (NFtime_p)     NULL) NFtimeFree     (bundleTime);
	if (bundleTimeStep != (NFtimeStep_p) NULL) NFtimeStepFree (bundleTimeStep);
	if (baseTimeUnit   != (ut_unit *)      NULL) ut_free          (baseTimeUnit);
	return ((NFio_p) NULL);
}
