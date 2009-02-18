#include <string.h>
#include <NFobjects.h>

NFobject_p NFparseCompContainerCreate (XML_Parser parser, NFobject_p parent,const char *name, const char **attr) {
	const char *attrib;
	NFcompContainer_p component = (NFcompContainer_p) NULL;
	NFcompModel_p model;
	NFcomponent_p sibling;
	NFobjList_p   componentList;
	NFtimeStep_p  timeStep;

	if ((parent->Type != NFcompModel) && (parent->Type != NFcompContainer)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((component = (NFcompContainer_p) NFobjectCreate (name,NFcompContainer)) == (NFcompContainer_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Component creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	attrib = NFparseGetAttribute (attr, NFcompLayoutStr, NFkeyInheritStr);
	if (strcmp (attrib,NFkeyInheritStr) == 0) {
		if (parent->Type == NFcompContainer)
			component->Domain = ((NFcomponent_p) parent)->Domain;
		else {
			CMmsgPrint (CMmsgUsrError, "Container [%s] in model can't inherit layout in line %d!\n", name, XML_GetCurrentLineNumber (parser));
			goto Abort;
		}
	}
	else {
		switch (parent->Type) {
		case NFcompContainer: componentList = ((NFcompContainer_p) parent)->Components; break;
		case NFcompModel:     componentList = ((NFcompModel_p)     parent)->Components; break;
		default:
			CMmsgPrint (CMmsgUsrError, "This shouldn't have happened in %s,%d!\n",__FILE__,__LINE__);
			goto Abort;
		}
		if ((sibling = (NFcomponent_p) NFobjListFindItemByName (componentList, attrib)) == (NFcomponent_p) NULL) {
			CMmsgPrint (CMmsgUsrError, "Invalid container [%s] layout [%s] in line %d.\n",name, attrib, XML_GetCurrentLineNumber (parser));
			goto Abort;
		}
		component->Domain = sibling->Domain;
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
	if ((attrib = NFparseGetAttribute (attr, NFattrStatesStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing container state in line %d\n", XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	component->States = CMstrDuplicate (attrib);

	component->Parent = parent;
	return ((NFobject_p) component);
Abort:
	if (component != (NFcompContainer_p) NULL) NFobjectFree ((NFobject_p) component);
	return ((NFobject_p) NULL);
}

CMreturn NFparseCompContainerFinalize (NFobject_p parent, NFobject_p object) {;
	NFcompModel_p model;

	if (object->Type != NFcompContainer) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d!\n",__FILE__,__LINE__); return (CMfailed);
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
