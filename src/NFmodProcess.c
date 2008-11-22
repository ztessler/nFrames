#include <string.h>
#include <NFobjects.h>

NFobject_p NFparseModProcessCreate (XML_Parser parser,NFobject_p parent,const char *name, const char **attr) {
	const char *attrib;

	NFmodProcess_p module = (NFmodProcess_p) NULL;
	NFcompModel_p  model;
	NFobjPlugin_p  plugin = (NFobjPlugin_p)  NULL;

	if (parent->Type != NFcompContainer) {
		CMmsgPrint (CMmsgAppError, "This shouldn't have happened in %s%d.\n",__FILE__,__LINE__);
		goto Abort;
	}

	if ((module = (NFmodProcess_p) NFobjectCreate (name,NFmodProcess)) == (NFmodProcess_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Process module creation error in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFobjPluginStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing process [%s] plugi in line %d!\n",name, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	model = (NFcompModel_p) NFobjectRoot (parent);
	if ((plugin = NFpluginGet (model->ModPlugins,attrib)) == (NFobjPlugin_p) NULL) {
		CMmsgPrint (CMmsgUsrError, "Modules plugin [%s] loading error in line %d!\n",attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((attrib = NFparseGetAttribute (attr, NFattrMethodStr, (char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Missing plugin [%s] method in line %d!\n",plugin->Name, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((module->Initialize = (NFinitializeFunc) NFpluginFunction (plugin, NFfuncInitializeStr, attrib)) == (NFinitializeFunc) NULL) {
		CMmsgPrint (CMmsgUsrError, "Error loding plugin [%s] function [%s] in line %d!\n",plugin->Name, attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((module->Execute    = (NFexecuteFunc)    NFpluginFunction (plugin, NFfuncExecuteStr,   attrib)) == (NFexecuteFunc)    NULL) {
		CMmsgPrint (CMmsgUsrError, "Error loding plugin [%s] function [%s] in line %d!\n",plugin->Name, attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	if ((module->Finalize   = (NFfinalizeFunc)   NFpluginFunction (plugin, NFfuncFinalizeStr,  attrib)) == (NFfinalizeFunc)   NULL) {
		CMmsgPrint (CMmsgUsrError, "Error loading plugin [%s] function [%s] in line %d!\n",plugin->Name, attrib, XML_GetCurrentLineNumber (parser));
		goto Abort;
	}
	module->Initialize (module->Context);
	if ((module->Context->UserData = (void *) malloc (module->Context->UserDataSize)) == (void *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation errord in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	module->Parent = parent;
	CMmsgPrint (CMmsgInfo,"Parameter num: %d Variable num: %d\n", module->Context->ParameterNum, module->Context->VariableNum);
	return ((NFobject_p) module);
Abort:
	if (module != (NFmodProcess_p) NULL) NFobjectFree ((NFobject_p) module);
	return ((NFobject_p) NULL);	
}

CMreturn NFparseModProcessFinalize (NFobject_p parent, NFobject_p object) {
	size_t i, j;
	NFmodProcess_p module = (NFmodProcess_p) object;

	if ((object->Type != NFmodProcess) || (parent->Type != NFcompContainer)) {
		CMmsgPrint (CMmsgUsrError, "This shouldn't have happend in %s:%d.\n",__FILE__,__LINE__);
		goto Abort;
	}
	if (NFobjListAddItem (((NFcompContainer_p) parent)->Modules, object) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError, "Error adding process module to container in %s:%d\n",__FILE__,__LINE__);
		goto Abort;
	}
	for (i = 0;i < module->Context->ParameterNum; ++i) {
	
	}
	for (i = 0;i < module->Context->VariableNum;  ++i) {
		if ((module->Context->Variables [i].Role == NFinput) ||
			(module->Context->Variables [i].Role == NFmodified)) {
			for (j = 0; j < module->Aliases->Num; ++j) {
				if ((((NFobjAlias_p) module->Aliases->List [j])->AliasType == NFaliasVariable) &&
					(strcmp (module->Context->Variables [i].Name, (((NFobjAlias_p) module->Aliases->List [j])->Alias)) == 0))
					break;
			}
			if (j == module->Aliases->Num) {
				CMmsgPrint (CMmsgAppError,"Process [%s] variable [%s] is not found!\n", module->Name, module->Context->Variables [i].Name);
				goto Abort;
			}
		}
		else {
			
		}
	}

	module->Execute    (module->Context->UserData);
	module->Finalize   (module->Context->UserData);

	return (CMsucceeded);
Abort:
	NFobjectFree (object);
	return (CMfailed);
}
