#include <NFobjects.h>

NFobject_p NFparseObjCategoryCreate (XML_Parser parser, NFobject_p parent,const char *name, const char **attr) {
	NFobjCategory_p category = (NFobjCategory_p) NULL;

	if (parent->Type != NFcompAggregate) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((category = (NFobjCategory_p) NFobjectCreate (name,NFobjCategory)) == (NFobjCategory_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Category creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((category->Variable = (NFnumVariable_p) NFobjectCreate (name,NFnumVariable)) == (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Variable creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	category->Variable->Parent     = (NFobject_p) category;
	category->Variable->UnitString = CMstrDuplicate ("%");
	category->Parent = parent;
	return ((NFobject_p) category);
Abort:
	if (category != (NFobjCategory_p) NULL) NFobjectFree ((NFobject_p) category);
	return ((NFobject_p) NULL);
}

CMreturn NFparseObjCategoryFinalize (NFobject_p parent, NFobject_p object) {

	if ((object->Type != NFobjCategory) || (parent->Type != NFcompAggregate)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompAggregate_p) parent)->Categories, object)   != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding category to aggregate component in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompAggregate_p) parent)->Variables, (NFobject_p) ((NFobjCategory_p) object)->Variable) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding variable to aggregate component in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
