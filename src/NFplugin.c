#include <string.h>
#include <dlfcn.h>
#include <NFobjects.h>

NFobjPlugin_p NFpluginGet (NFobjList_p pluginList, const char *name) {
	char *env = (char *) NULL, *path = (char *) NULL, *fileName = (char *) NULL;
	int i = 0, eLen = 0, pLen, nLen;
	NFobjPlugin_p plugin = (NFobjPlugin_p) NULL;

	if ((plugin = (NFobjPlugin_p) NFobjListFindItemByName (pluginList, name)) == (NFobjPlugin_p) NULL) {
	    if ((plugin = (NFobjPlugin_p) NFobjectCreate (name,NFobjPlugin)) == (NFobjPlugin_p) NULL) {
			CMmsgPrint (CMmsgAppError, "Plugin creation error in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
	    }
	    if ((env = getenv ("NFRAMES_PLUGIN_PATH")) != (char *) NULL) {
	    	env  = CMstrDuplicate (env);
	    	eLen = strlen (env);
	    }
	    nLen = strlen (name);
	    fileName = CMstrDuplicate (name);
	    while (((plugin->Handle = dlopen (fileName, RTLD_LAZY)) == (void *) NULL) && (i < eLen)) {
	    	path = path == (char *) NULL ? env : path + i + 1;
	    	for (  ;i < eLen; ++i) if (path [i] == ';') { path [i] = '\0'; break; }
	    	pLen = strlen (path);
			if (strlen (fileName) < (pLen + nLen + 1)) {
				if ((fileName = realloc (fileName, pLen + nLen + 2)) == (char *) NULL) {
					CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
					goto Abort;
				}
			}
			sprintf (fileName,"%s/%s",path,name);
		}
		if (plugin->Handle == (void *) NULL) {
			CMmsgPrint (CMmsgAppError, "Plugin [%s] loading error!\n%s\n", name, dlerror ());
			goto Abort;
		}

	    plugin->Parent = pluginList->Parent;
	    if ((NFobjListAddItem (pluginList, (NFobject_p) plugin)) == CMfailed) {
			CMmsgPrint (CMmsgAppError, "Error adding plugin to model in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
	    }
	}
	free (fileName);
	return (plugin);
Abort:
	if (plugin   != (NFobjPlugin_p) NULL) NFobjectFree ((NFobject_p) plugin);
	if (fileName != (char *) NULL) free (fileName);
	return ((NFobjPlugin_p) NULL);
}

void *NFpluginFunction (NFobjPlugin_p plugin, const char *prefix, const char *method) {
	char *functionStr;
	void *function;

	functionStr = CMstrDuplicate (prefix);
	functionStr = CMstrAppend    (functionStr, method, "_");

	function = dlsym (plugin->Handle, functionStr);
	free (functionStr);
	return (function);
}
