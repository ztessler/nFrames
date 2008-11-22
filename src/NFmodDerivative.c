#include <string.h>
#include <NFobjects.h>

NFobject_p NFparseModDerivativeCreate (XML_Parser parser, NFobject_p parent,const char *name, const char **attr) {
	const char *attrib;
	NFmodDerivative_p module = (NFmodDerivative_p) module;
	NFnumVariable_p   inputVar;

	if (parent->Type != NFcompContainer) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFnumVariableStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing derivative [%s] variable in line %d.\n",name, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((inputVar = (NFnumVariable_p) NFobjListFindItemByName (((NFcompContainer_p) parent)->Variables,attrib)) == (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgUsrError, "Invalid derivative [%s] variable [%s] in line %d\n.",name, attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((module = (NFmodDerivative_p) NFobjectCreate (name,NFmodDerivative)) == (NFmodDerivative_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Derivative module creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((module->Variable = (NFnumVariable_p) NFobjectCreate (name, NFnumVariable)) == (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Derivative variable creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;		
	}
	
	module->Variable->Parent       = (NFobject_p) module;
	module->Variable->UnitString   = CMstrDuplicate (inputVar->UnitString);
	module->Variable->StandardName = CMstrDuplicate (NFparseGetAttribute (attr, NFattrStandardNameStr, name));
	module->InputVar  = inputVar;

	attrib = NFparseGetAttribute (attr, NFattrDirectionStr, NFkeyDirectionXStr);
	if      (strcmp (attrib,NFkeyDirectionXStr) == 0) module->Direction = NFdirX;
	else if (strcmp (attrib,NFkeyDirectionYStr) == 0) module->Direction = NFdirY;
	else if (strcmp (attrib,NFkeyDirectionZStr) == 0) module->Direction = NFdirZ;
	else {
		CMmsgPrint (CMmsgUsrError, "Invalid derivative [%s] direction [%s] in line %d!\n",name, attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	attrib = NFparseGetAttribute (attr, NFattrDifferenceStr, NFkeyCenteredStr);
	if      (strcmp (attrib,NFkeyCenteredStr) == 0) module->Difference = NFintervalCenter;
	else if (strcmp (attrib,NFkeyForwardStr)  == 0) module->Difference = NFintervalEnd;
	else if (strcmp (attrib,NFkeyBackwardStr) == 0) module->Difference = NFintervalBegin;
	else {
		CMmsgPrint (CMmsgUsrError, "Invalid derivative [%s] difference type in line %d!\n",name, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}

	module->Parent = parent;
	return ((NFobject_p) module);
Abort:
	if (module->Variable != (NFnumVariable_p)   NULL) NFobjectFree ((NFobject_p) module->Variable);
	if (module           != (NFmodDerivative_p) NULL) NFobjectFree ((NFobject_p) module);
	return ((NFobject_p) NULL);
}

CMreturn NFparseModDerivativeFinalize (NFobject_p parent, NFobject_p object) {

	if ((object->Type != NFmodDerivative) || (object->Parent->Type != NFcompContainer)) {
		CMmsgPrint (CMmsgAppError, "Wrong object in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Modules, object) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding module to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Variables, (NFobject_p) ((NFmodDerivative_p) object)->Variable) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding variable to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}

	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
