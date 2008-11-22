#include <string.h>
#include <NFobjects.h>

NFobject_p NFparseCompAggregateCreate (XML_Parser parser, NFobject_p parent,const char *name, const char **attr) {
	const char *attrib;
	NFcompAggregate_p component = (NFcompAggregate_p) NULL;
	NFcomponent_p sibling;
	NFcompModel_p model;
	NFobjList_p list;
	NFtimeStep_p timeStep;

	if (parent->Type != NFcompModel) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((component = (NFcompAggregate_p) NFobjectCreate (name,NFcompAggregate)) == (NFcompAggregate_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Component creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFcomponentStr,   (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing aggregate component in line %d\n", XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	switch (parent->Type) {
	case NFcompModel:     list = ((NFcompModel_p)     parent)->Components; break;
	case NFcompContainer: list = ((NFcompContainer_p) parent)->Components; break;
	default:
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((component->Component = NFobjListFindItemByName (list,attrib)) == (NFobject_p) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing component [%s] in line %d!\n",attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	attrib = NFparseGetAttribute (attr, NFcompLayoutStr, NFkeyInheritStr);
	if (strcmp (attrib,NFkeyInheritStr) == 0) {
		if (parent->Type == NFcompContainer)
			component->Layout = (NFcomponent_p) parent;
		else {
			CMmsgPrint (CMmsgUsrError, "Container [%s] in model can't inherit layout in line %d!\n", name, XML_GetCurrentLineNumber (parser));
			goto Abort;
		}
	}
	else {
		if ((sibling = (NFcomponent_p) NFobjListFindItemByName (list, attrib)) == (NFcomponent_p) NULL) {
			CMmsgPrint (CMmsgUsrError, "Invalid container [%s] layout [%s] in line %d.\n",name, attrib, XML_GetCurrentLineNumber (parser));
			goto Abort;
		}
		component->Layout = sibling;
	}

	if ((attrib = NFparseGetAttribute (attr, NFnumVariableStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing aggregate standard_name in line %d\n", XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	list = ((NFcomponent_p) component->Component)->Variables;
	if ((component->Variable = (NFnumVariable_p) NFobjListFindItemByName (list, attrib)) == (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgUsrError,"Missing aggregate variable in line %d!\n", XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	model = (NFcompModel_p) NFobjectRoot (parent);
	attrib = NFparseGetAttribute (attr, NFattrTimeStepStr, "inherit");
	if (strcmp (attrib,NFkeyInheritStr) == 0) {
		switch (parent->Type) {
		case NFcompContainer:
		case NFcompModel: component->TimeStep = ((NFcomponent_p) parent)->TimeStep; break;
		default:
			CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d!\n",__FILE__,__LINE__);
			goto Abort;
		}
	}
	else {
		if (NFtimeStepSetFromString (component->TimeStep, attrib) == false) {
			CMmsgPrint (CMmsgAppError,"Time step [%s] error in %d!\n",attrib, XML_GetCurrentLineNumber (parser));
			goto Abort;
		}
		if ((timeStep = NFmodelAddTimeStep (model,component->TimeStep)) != component->TimeStep) {
			NFtimeStepFree (component->TimeStep);
			component->TimeStep = timeStep;
		}
	}
	component->Parent = parent;
	return ((NFobject_p) component);
Abort:
	if (component != (NFcompAggregate_p) NULL) NFobjectFree ((NFobject_p) component);
	return ((NFobject_p) NULL);
}

CMreturn NFparseCompAggregateFinalize (NFobject_p parent, NFobject_p object) {
	NFcompModel_p model;

	if ((object->Type != NFcompAggregate) || (parent->Type != NFcompModel)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	switch (parent->Type) {
	case NFcompContainer:
		if (NFobjListAddItem (((NFcompContainer_p) parent)->Components, object) != CMsucceeded) {
			CMmsgPrint (CMmsgAppError, "Error adding component to model in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
		}
	case NFcompModel:
		model = (NFcompModel_p) NFobjectRoot (parent);
		if (NFobjListAddItem (model->Components, object) != CMsucceeded) {
			CMmsgPrint (CMmsgAppError, "Error adding component to model in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
		}
		break;
	default:
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
