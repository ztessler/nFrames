#include <stdlib.h>
#include <NFobjects.h>

static CMreturn _NFexecuteInput (NFcomponent_p component, bool report) {
	NFcompInput_t *input = (NFcompInput_t *) component;

	input = input;
	return (CMsucceeded);
//Abort:
//	return (CMfailed);
}

static CMreturn _NFexecuteAggregate (NFcomponent_p component, bool report) {
	NFcompAggregate_t *aggregate = (NFcompAggregate_t *) component;

	aggregate = aggregate;
	return (CMsucceeded);
//Abort:
//	return (CMfailed);
}

static CMreturn _NFexecuteRegion    (NFcomponent_p component, bool repor) {
	NFcompRegion_t *region = (NFcompRegion_t *) component;

	region = region;
	return (CMsucceeded);
//Abort:
//	return (CMfailed);
}

static CMreturn _NFexecuteContainer (NFcomponent_p component, bool report) {
	NFcompContainer_t *container = (NFcompContainer_t *) component;

	container = container;
	return (CMsucceeded);
//Abort:
//	return (CMfailed);
}

static void _NFmodelTimeArrayFree (NFtime_p *timePtr, size_t num) {
	size_t i;

	for (i = 0;i < num; ++i) if (timePtr [i] != (NFtime_p) NULL) NFtimeFree (timePtr [i]);
	free (timePtr);
}

static NFtime_p *_NFmodelTimeArrayAllocate (size_t num) {
	size_t i;
	NFtime_p *timePtr;

	if ((timePtr = (NFtime_p *) calloc (num, sizeof (NFtime_p))) == (NFtime_p *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		return ((NFtime_p *) NULL);
	}
	for (i = 0;i < num; ++i) {
		if ((timePtr [i] = NFtimeCreate ()) == (NFtime_p) NULL) {
			_NFmodelTimeArrayFree (timePtr,i);
			return ((NFtime_p *) NULL);
		}
	}
	return (timePtr);
}

static CMreturn _NFmodelExecute (NFcompModel_p model, bool report, size_t timeBeat, NFtime_p *timeBegin, NFtime_p *timeEnd, NFtime_p *beatBegin, NFtime_p *beatEnd) {
	char timeStr [NFtimeUnitMinute + 1], *separator;
	size_t i;
	CMreturn ret;
	NFcomponent_p component;

	for (NFtimeSetComplete (timeBegin [timeBeat], beatBegin [timeBeat], (NFtimeStep_p) (model->TimeSteps->List [timeBeat]), NFintervalBegin);
	     NFtimeCompare     (beatBegin [timeBeat], timeEnd   [timeBeat]) < 0;
	     NFtimeAdvance     (beatBegin [timeBeat], (NFtimeStep_p) (model->TimeSteps->List [timeBeat]))) {

		for (i = 0;i < timeBeat; ++i) printf ("  ");
		printf ("Begin: %s\n", NFtimePrint (beatBegin [timeBeat],timeStr));
		NFtimeCopy        (beatBegin [timeBeat], beatEnd [timeBeat]);
		NFtimeAdvance     (beatEnd   [timeBeat], (NFtimeStep_p) (model->TimeSteps->List [timeBeat]));

		if (timeBeat < model->TimeSteps->Num - 1) {
			NFtimeCopy    (beatBegin [timeBeat], timeBegin [timeBeat + 1]);
			NFtimeCopy    (beatBegin [timeBeat], timeEnd   [timeBeat + 1]);
			NFtimeAdvance (timeEnd [timeBeat + 1], (NFtimeStep_p) (model->TimeSteps->List [timeBeat]));
			if (_NFmodelExecute (model, report, timeBeat + 1, timeBegin, timeEnd, beatBegin, beatEnd) == CMfailed) {
				CMmsgPrint (CMmsgAppError,"Model execution error in %s:%d!\n",__FILE__,__LINE__);
				return (CMfailed);
			}
		}
		for (i = 0;i <= timeBeat; ++i) printf ("  ");
		printf ("Execute:");
		separator = " ";
		for (i = 0;i < model->Components->Num; ++i) {
			if ((component = (NFcomponent_p) (model->Components->List [i])) == (NFcomponent_p) NULL) {
				CMmsgPrint (CMmsgAppError, "Corrupt component in %s:%d!\n",__FILE__,__LINE__);
				return (CMfailed);
			}
			if (component->TimeStep == (NFtimeStep_p) (model->TimeSteps->List [timeBeat])) {
				printf ("%s%s",separator,component->Name);
				separator = ", ";
				switch (component->Type) {
				case NFcompAggregate: ret = _NFexecuteAggregate (component, report); break;
				case NFcompContainer: ret = _NFexecuteContainer (component, report); break;
				case NFcompInput:     ret = _NFexecuteInput     (component, report); break;
				case NFcompRegion:    ret = _NFexecuteRegion    (component, report); break;
				default:
					CMmsgPrint (CMmsgAppError, "Invalid component [%s] in %s:%d!\n",component->Name,__FILE__,__LINE__);
					ret = CMfailed;
					break;
				}
				if (ret == CMfailed) return (CMfailed);
			}
		}
		printf ("\n");
		for (i = 0;i < timeBeat; ++i) printf ("  ");
		printf ("End:   %s\n", NFtimePrint (beatEnd [timeBeat],timeStr));
	}
	return (CMsucceeded);
}

CMreturn NFmodelExecute (NFcompModel_p model, bool report) {
	NFtime_p     *begin     = (NFtime_p *) NULL;
	NFtime_p     *end       = (NFtime_p *) NULL;
	NFtime_p     *timeBegin = (NFtime_p *) NULL;
	NFtime_p     *timeEnd   = (NFtime_p *) NULL;

	if ((begin    = _NFmodelTimeArrayAllocate (model->TimeSteps->Num)) == (NFtime_p *) NULL) {
		CMmsgPrint (CMmsgAppError,"Time array allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((end      = _NFmodelTimeArrayAllocate (model->TimeSteps->Num)) == (NFtime_p *) NULL) {
		CMmsgPrint (CMmsgAppError,"Time array allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((timeBegin = _NFmodelTimeArrayAllocate (model->TimeSteps->Num)) == (NFtime_p *) NULL) {
		CMmsgPrint (CMmsgAppError,"Time array allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((timeEnd   = _NFmodelTimeArrayAllocate (model->TimeSteps->Num)) == (NFtime_p *) NULL) {
		CMmsgPrint (CMmsgAppError,"Time array allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}

	NFtimeCopy (model->Begin,begin [0]);
	NFtimeCopy (model->End,  end   [0]);
	NFtimeAdvance (end [0], (NFtimeStep_p) (model->TimeSteps->List [0]));
	if (_NFmodelExecute (model, report, 0,begin, end, timeBegin, timeEnd) == CMfailed) {
		CMmsgPrint (CMmsgAppError,"Model execution error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}

	_NFmodelTimeArrayFree (begin,     model->TimeSteps->Num);
	_NFmodelTimeArrayFree (end,       model->TimeSteps->Num);
	_NFmodelTimeArrayFree (timeBegin, model->TimeSteps->Num);
	_NFmodelTimeArrayFree (timeEnd,   model->TimeSteps->Num);
	return (CMsucceeded);
Abort:
	if (begin     != (NFtime_p *) NULL) _NFmodelTimeArrayFree (begin,     model->TimeSteps->Num);
	if (end       != (NFtime_p *) NULL) _NFmodelTimeArrayFree (end,       model->TimeSteps->Num);
	if (timeBegin != (NFtime_p *) NULL) _NFmodelTimeArrayFree (timeBegin, model->TimeSteps->Num);
	if (timeEnd   != (NFtime_p *) NULL) _NFmodelTimeArrayFree (timeEnd,   model->TimeSteps->Num);
	return (CMfailed);
}
