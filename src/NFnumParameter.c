#include <NFobjects.h>

NFobject_p NFparseNumParameterCreate (XML_Parser parser, NFobject_p parent,const char *name, const char **attr) {
	NFnumParameter_p parameter = (NFnumParameter_p) NULL;
	const char *attrib;

	if ((parent->Type != NFcompModel) && (parent->Type != NFcompContainer) && (parent->Type != NFmodEquation) && (parent->Type != NFmodProcess)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((parameter = (NFnumParameter_p) NFobjectCreate (name,NFnumParameter)) == (NFnumParameter_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Parameter creation error in %s:%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFattrValueStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing model parameter [%s] value.\n",name);
		goto Abort;
	}
	if (sscanf (attrib,"%lf",&(parameter->Value)) != 1) {
		CMmsgPrint (CMmsgUsrError, "Parameter [%s] parsing error.\n",name);
		goto Abort;
	}

	parameter->Parent = parent;
	return ((NFobject_p) parameter);
Abort:
	if (parameter != (NFnumParameter_p) NULL) NFobjectFree ((NFobject_p) parameter);
	return ((NFobject_p) NULL);
}

CMreturn NFparseNumParameterFinalize (NFobject_p parent, NFobject_p object) {
	NFobjList_p list;

	if ( object->Type != NFnumParameter) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	switch (parent->Type) {
	case NFcompContainer: list = ((NFcompContainer_p) parent)->Parameters; break;
	case NFcompModel:     list = ((NFcompModel_p)     parent)->Parameters; break;
	case NFmodEquation:   list = ((NFmodEquation_p)   parent)->Parameters; break;
	case NFmodProcess:    list = ((NFmodProcess_p)    parent)->Parameters; break;
	default: CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d!\n",__FILE__,__LINE__); return (CMfailed);
	}
	if (NFobjListAddItem (list, object) == CMfailed) {
		CMmsgPrint (CMmsgAppError, "Error adding compnent to model/container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
