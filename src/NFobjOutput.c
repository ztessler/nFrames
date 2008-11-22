#include <NFobjects.h>

NFobject_p NFparseObjOutputCreate (XML_Parser parser, NFobject_p parent,const char *name, const char **attr) {
	const char *attrib;
	NFobjOutput_p output = (NFobjOutput_p) NULL;

	if (parent->Type != NFcompContainer) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((output = (NFobjOutput_p) NFobjectCreate (name,NFobjOutput)) == (NFobjOutput_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Output creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFattrPathStr, (char *) NULL)) == (char *) NULL) {
		if ((output = (NFobjOutput_p) NFobjectCreate (name,NFobjOutput)) == (NFobjOutput_p) NULL) {
			CMmsgPrint (CMmsgUsrError, "Missing output [%s] path in line %d!\n",name, XML_GetCurrentLineNumber (parser));
			goto Abort;
		}
	}
	output->Path   = CMstrDuplicate (attrib);
	output->Parent = parent;
	return ((NFobject_p) output);
Abort:
	if (output != (NFobjOutput_p) NULL) NFobjectFree ((NFobject_p) output);
	return ((NFobject_p) NULL);
}

CMreturn NFparseObjOutputFinalize (NFobject_p parent, NFobject_p object) {

	if ((object->Type != NFobjOutput) || (parent->Type != NFcompContainer)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Outputs, object) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding variable to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
