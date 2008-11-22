#include <NFobjects.h>

#include <string.h>

/****************************************************************************************************************
 * List create
*****************************************************************************************************************/
NFlist_p NFlistCreate () {
	NFlist_p list;
	
	if ((list = (NFlist_p) malloc (sizeof (NFlist_t))) == (NFlist_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((NFlist_p) NULL);
	}
	list->Num    = 0;
	list->List   = (void **) NULL;
	return (list);
}

/****************************************************************************************************************
 * List free
*****************************************************************************************************************/
void NFlistFree (NFlist_p list) {
	if (list->List != (void **) NULL) free (list->List);
	free (list);
	return;
}

/****************************************************************************************************************
 * Object List create
*****************************************************************************************************************/
NFobjList_p NFobjListCreate (NFobject_p parent) {
	NFobjList_p list;
	
	if ((list = (NFobjList_p) malloc (sizeof (NFobjList_t))) == (NFobjList_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((NFobjList_p) NULL);
	}
	list->Parent = parent;
	list->Num    = 0;
	list->List   = (NFobject_p *) NULL;
	return (list);
}

/****************************************************************************************************************
 * List Add item
*****************************************************************************************************************/
CMreturn NFlistAddItem (NFlist_p list, void *item) {
	if ((list->List = (void **) realloc (list->List, (list->Num + 1) * sizeof (void *))) == (void **) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		list->Num = 0;
		return (CMfailed);
	}
	list->List [list->Num] = item;
	list->Num++;
	return (CMsucceeded);
}

/****************************************************************************************************************
 * Object List free
*****************************************************************************************************************/
void NFobjListFree (NFobjList_p list) {
	size_t i;

	for (i = 0;i < list->Num;++i)
		if (list->Parent == (list->List [i])->Parent) NFobjectFree (list->List [i]);
	if (list->List != (NFobject_p *) NULL) free (list->List);
	free (list);
	return;
}
/****************************************************************************************************************
 * Objcet list Add item
*****************************************************************************************************************/
CMreturn NFobjListAddItem (NFobjList_p list, NFobject_p object) {
	if ((list->List = realloc (list->List, (list->Num + 1) * sizeof (NFobject_p))) == (NFobject_p *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		list->Num = 0;
		return (CMfailed);
	}
	list->List [list->Num] = object;
	list->Num++;
	return (CMsucceeded);
}

NFobject_p NFobjListFindItemByID   (NFobjList_p list, size_t id) {
	if (id >= list->Num) return ((NFobject_p) NULL);
	return (list->List [id]);
}

NFobject_p NFobjListFindItemByName (NFobjList_p list, const char *name) {
	size_t i;
	NFobject_p object;

	for (i = 0;i < list->Num; ++i) {
		object = list->List [i];
		if (strcmp (object->Name,name) == 0) break;
	}
	if (i == list->Num) return ((NFobject_p) NULL);
	return (object);
}
