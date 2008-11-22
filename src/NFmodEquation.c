#include <NFobjects.h>

NFobject_p NFparseModEquationCreate (XML_Parser parser,NFobject_p parent,const char *name, const char **attr) {
	NFmodEquation_p module = (NFmodEquation_p) NULL;

	if (parent->Type != NFcompContainer) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}

	if ((module = (NFmodEquation_p) NFobjectCreate (name,NFmodEquation)) == (NFmodEquation_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Equation module creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((module->Variable = (NFnumVariable_p) NFobjectCreate (name, NFnumVariable)) == (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Equation variable creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;		
	}
	
	module->Variable->UnitString   = CMstrDuplicate (NFparseGetAttribute (attr, NFattrUnitStr,         NFkeyUnitlessStr));
	module->Variable->StandardName = CMstrDuplicate (NFparseGetAttribute (attr, NFattrStandardNameStr, name));

	module->Parent = parent;
	return ((NFobject_p) module);
Abort:
	if (module != (NFmodEquation_p) NULL) NFobjectFree ((NFobject_p) module);
	return ((NFobject_p) NULL);
}

CMreturn NFparseModEquationFinalize (NFobject_p parent, NFobject_p object) {

	if ((object->Type != NFmodEquation) || (parent->Type != NFcompContainer)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Modules, object) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding equation to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
