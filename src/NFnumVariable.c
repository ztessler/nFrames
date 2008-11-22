#include <NFobjects.h>

NFobject_p NFparseNumVariableCreate (XML_Parser parser,NFobject_p parent,const char *name, const char **attr) {
	NFnumVariable_p variable = (NFnumVariable_p) NULL;
	NFobjList_p list;

	switch (parent->Type) {
	case NFcompInput:
	case NFcompContainer: // Only initial variables are children of containers
		if ((variable = (NFnumVariable_p) NFobjectCreate (name,NFnumVariable)) == (NFnumVariable_p) NULL) {
			CMmsgPrint (CMmsgAppError, "Variable creation error in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
		}
		if (sscanf (NFparseGetAttribute (attr, NFattrInitialValStr, "0.0"),"%lf", &variable->InitialValue) != 1) {
			CMmsgPrint (CMmsgAppError, "Variable [%s] Initial value scanning error.\n",name);
			goto Abort;
		}
		variable->StandardName = CMstrDuplicate (NFparseGetAttribute (attr, NFattrStandardNameStr, name));
		variable->UnitString   = CMstrDuplicate (NFparseGetAttribute (attr, NFattrUnitStr,         NFkeyUnitlessStr));
		variable->Parent       = parent;
		variable->XMLline      = XML_GetCurrentLineNumber (parser);
		break;
	case NFobjOutput:
		if ((parent->Parent == (NFobject_p) NULL) || (parent->Parent->Type != NFcompContainer)) {
			CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
			goto Abort;		
		}
		list = ((NFcompContainer_p) parent->Parent)->Variables;
		if ((variable = (NFnumVariable_p) NFobjListFindItemByName (list, name)) == (NFnumVariable_p) NULL) {
			CMmsgPrint (CMmsgUsrError, "Missing output [%s] variable %s in line %d!\n",parent->Name, name, XML_GetCurrentLineNumber (parser));
			goto Abort;	
		}
		break;
	default:
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;		
	}

	return ((NFobject_p) variable);
Abort:
	if (variable != (NFnumVariable_p) NULL) NFobjectFree ((NFobject_p) variable);
	return ((NFobject_p) NULL);
}

CMreturn NFparseNumVariableFinalize (NFobject_p parent, NFobject_p object) {
	NFobjList_p list;

	if (object->Type != NFnumVariable) {
		CMmsgPrint (CMmsgUsrError, "This shouldn't have happend in %s:%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	switch (parent->Type) {
	case NFcompContainer: list = ((NFcompContainer_p) parent)->Variables; break;
	case NFcompInput:     list = ((NFcompInput_p)     parent)->Variables; break;
	case NFobjOutput:     list = ((NFobjOutput_p)     parent)->Variables; break;
	default:                CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d!\n",__FILE__,__LINE__); return (CMfailed);
	}
	if (NFobjListAddItem (list, object) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding variable to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
