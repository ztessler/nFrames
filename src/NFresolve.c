#include <cm.h>
#include <NFobjects.h>
#include <NFio.h>

static CMreturn _NFresolveVariable (NFobject_p object, bool report) {
	NFobject_p      parent  = object->Parent;
	NFnumVariable_p variable = (NFnumVariable_p) object;

	if (parent->Type == NFcompInput) {
		if ((variable->VarHandle = NFioVariableHandle (((NFcompInput_p) parent)->Domain->IO, variable->StandardName)) == (NFvarHandle) NULL) {
			CMmsgPrint (CMmsgUsrError, "Missing input variable in line %d!\n", variable->XMLline);
			goto Abort;
		}
		if ((variable->VarType   = NFioVariableType   (((NFcompInput_p) parent)->Domain->IO, variable->VarHandle))    == NFvarTypeVoid) {
			CMmsgPrint (CMmsgAppError, "Variable type error in %s:%d!\n",__FILE__,__LINE__);
			goto Abort;
		}
	}
	else variable->VarType = NFvarTypeFloat;

	if (report) CMmsgPrint (CMmsgInfo, "Variable[%s] type %s\n",variable->StandardName,NFvarTypeString (variable->VarType));
	if ((variable->Data.Void = malloc (((NFcompInput_p) parent)->Domain->IO->ItemNum * variable->VarType)) == (void *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	return (CMfailed);
}

static CMreturn _NFresolveInput (NFobject_p object, bool report) {
	size_t i;
	NFcompInput_p   component = (NFcompInput_p) object;
	NFcompModel_p   model;
	NFioOpenFunc    ioOpen;
	NFnumVariable_p variable;

	model = (NFcompModel_p) NFobjectRoot (object);
	if ((ioOpen   = (NFioOpenFunc) NFpluginFunction (component->IOplugin, NFfuncOpenStr,component->IOmethod)) == (NFioOpenFunc)   NULL) {
		CMmsgPrint (CMmsgUsrError, "Error loading plugin [%s] function [%s] in line %d!\n",component->IOplugin->Name, component->IOmethod,component->XMLline);
		goto Abort;
	}

	if ((component->Domain->IO = ioOpen (component->URL,model->Begin,model->End, model->UtSystem)) == (NFio_p) NULL) {
		CMmsgPrint (CMmsgUsrError, "Input pluging opening error in line %d!\n",component->XMLline);
		goto Abort;
	}
	if (component->TimeStep == (NFtimeStep_p) NULL) {
		if (component->Domain->IO->TimeLine->Regular == false) {
			CMmsgPrint (CMmsgUsrError, "Irrigular input time step cannot be inherited in line %d!\n",component->XMLline);
			goto Abort;
		}
		else component->TimeStep = component->Domain->IO->TimeLine->TimeStep;
	}
	for (i = 0;i < component->Variables->Num; ++i) {
		variable = (NFnumVariable_p) component->Variables->List [i];
		if ((variable->VarHandle = NFioVariableHandle (component->Domain->IO,variable->StandardName)) == (NFvarHandle) NULL) {
			CMmsgPrint (CMmsgAppError, "Variable [%s] loading error in line %d!\n", variable->StandardName, component->XMLline);
			goto Abort;
		}
	}
	return (CMsucceeded);
Abort:
	return (CMfailed);
}

static CMreturn _NFresolveAggregate (NFobject_p object, bool report) {
	NFcompAggregate_t *component = (NFcompAggregate_t *) object;

	component = component; // TODO dummy placeholder
	return (CMsucceeded);
//Abort:
//	return (CMfailed);
}

static CMreturn _NFresolveRegion    (NFobject_p object, bool repor) {

	return (CMsucceeded);
}

static CMreturn _NFresolveContainer (NFobject_p object, bool report) {
	size_t i;
	NFcompContainer_p container = (NFcompContainer_p) object;
	NFobject_p variable;

	for (i = 0;i < container->Variables->Num; ++i) {
		if ((variable = container->Variables->List [i]) == (NFobject_p) NULL) {
			CMmsgPrint (CMmsgAppError, "Corrupt container in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
		}
		if ((variable->Parent == object) && (_NFresolveVariable (variable, report) == CMfailed))
			goto Abort;
	}
	return (CMsucceeded);
Abort:
	return (CMfailed);
}

CMreturn NFmodelResolve (NFcompModel_p model, bool report) {
	size_t i;
	CMreturn ret;
	NFobject_p component;

	for (i = 0;i < model->Components->Num; ++i) {
		if ((component = model->Components->List [i]) == (NFobject_p) NULL) {
			CMmsgPrint (CMmsgAppError, "Corrupt component in %s:%d!\n",__FILE__,__LINE__);
			goto Abort;
		}
		switch (component->Type) {
		case NFcompAggregate: ret = _NFresolveAggregate (component, report); break;
		case NFcompContainer: ret = _NFresolveContainer (component, report); break;
		case NFcompInput:     ret = _NFresolveInput     (component, report); break;
		case NFcompRegion:    ret = _NFresolveRegion    (component, report); break;
		default:
			CMmsgPrint (CMmsgAppError, "Invalid component in %s:%d!\n,"__FILE__,__LINE__);
			ret = CMfailed;
			break;
		}
		if (ret == CMfailed) goto Abort;
	}
	for (i = 0;i < model->TimeSteps->Num; ++i) {
		printf ("%d %s\n",(int) ((NFtimeStep_p) model->TimeSteps->List [i])->Length,
		        NFtimeUnitString (((NFtimeStep_p) model->TimeSteps->List [i])->Unit));
	}
	return (CMsucceeded);
Abort:
	return (CMfailed);
}
