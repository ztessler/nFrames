#include <NFobjects.h>

NFobject_p NFparseModRouteCreate (XML_Parser parser, NFobject_p parent,const char *name, const char **attr) {
	NFmodRoute_p    module   = (NFmodRoute_p) NULL;

	if (parent->Type != NFcompContainer) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((module = (NFmodRoute_p) NFobjectCreate (name,NFmodRoute)) == (NFmodRoute_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Route module creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((module->Variable = (NFnumVariable_p) NFobjectCreate (name, NFnumVariable)) == (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Route variable creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;		
	}
	module->Variable->Parent       = (NFobject_p) module;
	module->Variable->StandardName = CMstrDuplicate (NFparseGetAttribute (attr, NFattrStandardNameStr, name));
	module->Variable->UnitString   = CMstrDuplicate (NFparseGetAttribute (attr, NFattrUnitStr,         NFkeyUnitlessStr));
	module->Variable->Route        = true;

	module->Parent = parent;
	return ((NFobject_p) module);
Abort:
	if (module->Variable != (NFnumVariable_p) NULL) NFobjectFree ((NFobject_p) module->Variable);
	if (module           != (NFmodRoute_p)    NULL) NFobjectFree ((NFobject_p) module);
	return ((NFobject_p) NULL);
}

CMreturn NFparseModRouteFinalize (NFobject_p parent, NFobject_p object) {

	if ((object->Type != NFmodRoute) || (parent->Type != NFcompContainer)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Modules, object) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding variable to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Variables, (NFobject_p) ((NFmodRoute_p) object)->Variable) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding variable to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}

	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
