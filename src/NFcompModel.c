#include <NFobjects.h>

NFobject_p NFparseCompModelCreate (XML_Parser parser, const char *name, const char **attr) {
	NFcompModel_p model = (NFcompModel_p) NULL;

	if ((model = (NFcompModel_p) NFobjectCreate (name, NFcompModel)) == (NFcompModel_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Creating model in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFtimeSetFromString (model->Begin, NFparseGetAttribute (attr, NFattrBeginStr, "0000-01-01")) == false) {
		CMmsgPrint (CMmsgAppError,"Begin time setting error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFtimeSetFromString (model->End,   NFparseGetAttribute (attr, NFattrEndStr,   "0000-12-31")) == false) {
		CMmsgPrint (CMmsgAppError,"End time setting error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;

	}
	model->Version  = CMstrDuplicate (NFparseGetAttribute (attr, NFattrVersionStr,  "noversion"));
	return ((NFobject_p) model);
Abort:
	if (model != (NFcompModel_p) NULL) NFobjectFree ((NFobject_p) model);
	return ((NFobject_p) NULL);
}

CMreturn NFparseCompModelFinalize (NFobject_p object) {

	if (object->Type != NFcompModel) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}

NFtimeStep_p NFmodelAddTimeStep (NFcompModel_p model, NFtimeStep_p timeStep) {
	size_t i, pos;

	if (model->Type != NFcompModel) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		return ((NFtimeStep_p) NULL);
	}

	for (i = 0;i < model->TimeSteps->Num; ++i) {
		if ((((NFtimeStep_p) (model->TimeSteps->List [i]))->Length == timeStep->Length) &&
		    (((NFtimeStep_p) (model->TimeSteps->List [i]))->Unit   == timeStep->Unit)) break;
	}
	if (i < model->TimeSteps->Num) return ((NFtimeStep_p) (model->TimeSteps->List [i]));
	NFlistAddItem (model->TimeSteps,(void *) timeStep);
	for (i = 0;i < model->TimeSteps->Num - 1; ++i)
		if ( (((NFtimeStep_p) (model->TimeSteps->List [i]))->Unit   > timeStep->Unit) ||
		    ((((NFtimeStep_p) (model->TimeSteps->List [i]))->Unit  == timeStep->Unit) &&
		     (((NFtimeStep_p) (model->TimeSteps->List [i]))->Length < timeStep->Length))) break;
	pos = i;
	for (     ;i < model->TimeSteps->Num - 1; ++i) {
		timeStep = model->TimeSteps->List [i];
		model->TimeSteps->List [i] = model->TimeSteps->List [model->TimeSteps->Num - 1];
		model->TimeSteps->List [model->TimeSteps->Num - 1] = timeStep;
	}
	return (model->TimeSteps->List [pos]);
}
