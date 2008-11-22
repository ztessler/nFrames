#include <string.h>
#include <NFobjects.h>

NFobject_p NFparseModInterfaceCreate (XML_Parser parser,NFobject_p parent,const char *name, const char **attr) {
	const char *attrib;
	NFmodInterface_p module   = (NFmodInterface_p) NULL;
	NFnumVariable_p  inputVar;
	NFobject_p      component;
	NFobjList_p list;

	if ((parent->Type != NFcompContainer) || (parent->Parent == (NFobject_p) NULL))  {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFattrRelationStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing interface [%s] relation in %d!\n",name, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if      (strcmp (attrib,NFkeyRootStr)   == 0) {
		component = NFobjectRoot (parent);
		if (component->Type == NFcompModel) list = ((NFcompModel_p) component)->Components;
		else {
			CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
			goto Abort;
		}
	}
	else if (strcmp (attrib,NFkeyParentStr) == 0) {
		component = parent->Parent;
		switch (component->Type) {
		case NFcompModel:     list = ((NFcompModel_p)     component)->Components; break;
		case NFcompContainer: list = ((NFcompContainer_p) component)->Components; break;
		default:
			CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
			goto Abort;
		}
	}
	else if (strcmp (attrib,NFkeyOwnStr)    == 0) list = ((NFcompModel_p) parent)->Components;

	if ((attrib = NFparseGetAttribute (attr, NFcomponentStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing interface [%s] component in %d!\n",name, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((component = NFobjListFindItemByName (list,attrib)) == (NFobject_p) NULL) {
		CMmsgPrint (CMmsgUsrError, "Invalid interface [%s] component [%s] in line %d!\n",name, attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFnumVariableStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing interface [%s] variable in line %d!\n",name,XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	switch (component->Type) {
	case NFcompAggregate: inputVar = (NFnumVariable_p) NFobjListFindItemByName (((NFcompAggregate_p) component)->Variables,attrib); break;
	case NFcompInput:     inputVar = (NFnumVariable_p) NFobjListFindItemByName (((NFcompInput_p    ) component)->Variables,attrib); break;
	case NFcompContainer: inputVar = (NFnumVariable_p) NFobjListFindItemByName (((NFcompContainer_p) component)->Variables,attrib); break;
	default:
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;		
	}
	if (inputVar == (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing interface [%s] variable [%s] in line %d!\n",name, attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((module    = (NFmodInterface_p) NFobjectCreate (name,NFmodInterface)) == (NFmodInterface_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Interface module creation error in %s:%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((module->Variable = (NFnumVariable_p) NFobjectCreate (name,NFnumVariable))  == (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Derivative variable creation error in %s:%d.\n",__FILE__,__LINE__);
		goto Abort;		
	}
	module->Variable->Parent  = (NFobject_p) module;
	module->Variable->StandardName = CMstrDuplicate (inputVar->StandardName);
	module->Variable->UnitString   = CMstrDuplicate (inputVar->UnitString);

	module->InputVar   = inputVar;

	module->Parent = parent;
	return ((NFobject_p) module);
Abort:
	if (module           != (NFmodInterface_p) NULL) {
		if (module->Variable != (NFnumVariable_p) NULL)
			NFobjectFree ((NFobject_p) module->Variable);
		NFobjectFree ((NFobject_p) module);
	}
	return ((NFobject_p) NULL);
}

CMreturn NFparseModInterfaceFinalize (NFobject_p parent, NFobject_p object) {

	if ((object->Type != NFmodInterface) || (parent->Type != NFcompContainer)) {
		CMmsgPrint (CMmsgAppError, "Wrong object in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Modules, object) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding interface to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Variables, (NFobject_p) ((NFmodInterface_p) object)->Variable) != CMsucceeded) {
			CMmsgPrint (CMmsgAppError, "Error adding variable to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
