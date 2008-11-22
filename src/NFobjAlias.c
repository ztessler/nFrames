#include <string.h>
#include <NFobjects.h>

NFobject_p NFparseObjAliasCreate (XML_Parser parser,NFobject_p parent,const char *name, const char **attr) {
	const char *attrib;
	NFobjAlias_p alias = (NFobjAlias_p) NULL;

	if ((parent->Type != NFcompRegion) && (parent->Type != NFmodEquation) && (parent->Type != NFmodProcess)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((alias = (NFobjAlias_p) NFobjectCreate (name,NFobjAlias)) == (NFobjAlias_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Alias creation error in %s:%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	attrib = NFparseGetAttribute (attr, NFattrValueStr, NFkeyVariableStr);
	if ((strcmp (attrib,NFkeyVariableStr) != 0) && (strcmp (attrib, NFkeyParameterStr) != 0)) {
		CMmsgPrint (CMmsgUsrError, "Invalid alias type in line %d!\n", XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	alias->AliasType = strcmp (attrib,NFkeyVariableStr) == 0 ? NFaliasVariable : NFaliasParameter;
	if ((attrib = NFparseGetAttribute (attr, NFobjAliasStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing alias attribute URL in line %d!\n", XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	alias->Alias = CMstrDuplicate (attrib);

	alias->Parent = parent;
	return ((NFobject_p) alias);
Abort:
	if (alias != (NFobjAlias_p) NULL) NFobjectFree ((NFobject_p) alias);
	return ((NFobject_p) NULL);
}

CMreturn NFparseObjAliasFinalize (NFobject_p parent, NFobject_p object) {
	NFobjList_p list;

	if (object->Type != NFobjAlias) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	switch (parent->Type) {
	case NFcompRegion:  list = ((NFcompRegion_p)  parent)->Aliases; break;
	case NFmodEquation: list = ((NFmodEquation_p) parent)->Aliases; break;
	case NFmodProcess:  list = ((NFmodProcess_p)  parent)->Aliases; break;
	default:
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (list, object) == CMfailed) {
		CMmsgPrint (CMmsgAppError, "Error adding alias to %s in %s:%d\n", NFobjTypeName (object->Parent->Type),__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
