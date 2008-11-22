#include <NFobjects.h>

NFobject_p NFparseCompRegionCreate (XML_Parser parser,NFobject_p parent,const char *name, const char **attr) {
	NFcompRegion_p    component = (NFcompRegion_p) NULL;
	
	if (parent->Type != NFcompContainer) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((component = (NFcompRegion_p) NFobjectCreate (name,NFcompRegion)) == (NFcompRegion_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Region component creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (parent->Type != NFcompContainer) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	
//    variable CDATA #REQUIRED
//   condition (&lt|&le|&eq|&ne|&ge|&gt) #REQUIRED
//       value CDATA #REQUIRED>
	component->TimeStep = ((NFcomponent_p) parent)->TimeStep;
	component->Parent = parent;
	return ((NFobject_p) component);
Abort:
	if (component != (NFcompRegion_p) NULL) NFobjectFree ((NFobject_p) component);
	return ((NFobject_p) NULL);
}

CMreturn NFparseCompRegionFinalize (NFobject_p parent, NFobject_p object) {
	NFcompModel_p model;

	if ((object->Type != NFcompRegion) || (parent->Type != NFcompContainer)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	switch (parent->Type) {
	case NFcompContainer:
		if (NFobjListAddItem (((NFcompContainer_p) parent)->Components, object) != CMsucceeded) {
			CMmsgPrint (CMmsgAppError, "Error adding component to model in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
		}
	case NFcompModel:
		model = (NFcompModel_p) NFobjectRoot (parent);
		if (NFobjListAddItem (model->Components, object) != CMsucceeded) {
			CMmsgPrint (CMmsgAppError, "Error adding component to model in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
		}
		break;
	default:
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
