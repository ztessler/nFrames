#include <NFobjects.h>
#include <string.h>

NFobject_p NFparseCompInputCreate (XML_Parser parser, NFobject_p parent,const char *name, const char **attr) {
	const char *attrib;
	NFcompInput_p component = (NFcompInput_p) NULL;
	NFcompModel_p  model;
	NFobjPlugin_p  plugin   = (NFobjPlugin_p) NULL;
	NFtimeStep_p   timeStep;

	if (parent->Type != NFcompModel) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((component = (NFcompInput_p) NFobjectCreate (name,NFcompInput)) == (NFcompInput_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Input component creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFattrTimeStepStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing time step in line %d!\n", XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	model = (NFcompModel_p) NFobjectRoot (parent);
	if (strcmp (attrib,NFkeyInheritStr) != 0) {
		if (NFtimeStepSetFromString (component->TimeStep, attrib) == false) {
			CMmsgPrint (CMmsgAppError,"Time step [%s] error in %d!\n",attrib, XML_GetCurrentLineNumber (parser));
			goto Abort;
		}
		if ((timeStep = NFmodelAddTimeStep (model,component->TimeStep)) != component->TimeStep) {
			NFtimeStepFree (component->TimeStep);
			component->TimeStep = timeStep;
		}
	}
	if ((attrib = NFparseGetAttribute (attr, NFattrUrlStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing input URL in line %d!\n", XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	component->URL = CMstrDuplicate (attrib);
	if ((attrib = NFparseGetAttribute (attr, NFobjPluginStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing input [%s] plugin in line %d!\n",name, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((plugin = NFpluginGet (model->IOPlugins,attrib)) == (NFobjPlugin_p) NULL) {
		CMmsgPrint (CMmsgUsrError, "Input [%s] plugin [%s] loading error in line %d!\n",name, attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	component->IOplugin = plugin;
	component->XMLline  = XML_GetCurrentLineNumber (parser);

	if ((attrib = NFparseGetAttribute (attr, NFattrMethodStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing plugin [%s] method in line %d!\n",plugin->Name, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	component->IOmethod = CMstrDuplicate (attrib);

	component->Parent = parent;
	return ((NFobject_p) component);
Abort:
	if (component != (NFcompInput_p) NULL) NFobjectFree ((NFobject_p) component);
	return ((NFobject_p) NULL);
}

CMreturn NFparseCompInputFinalize (NFobject_p parent, NFobject_p object) {

	if ((object->Type != NFcompInput) || (parent->Type != NFcompModel)) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s:%d!\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompModel_p) parent)->Components, object) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding component to model in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
