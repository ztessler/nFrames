#include <string.h>
#include <expat.h>
#include <NFobjects.h>

/****************************************************************************************************************
 * Data structure
*****************************************************************************************************************/
typedef struct NFdata_s {
	char           *SkipName;
	bool            Skip, Abort;
	size_t          Stack, SkipLevel;
	NFcompModel_p Model;
	size_t          StackSize;
	NFobject_p  *Objects;
	size_t          BufferLen;
	char           *Buffer;
} NFdata_t;

/****************************************************************************************************************
 * Get Attribute
*****************************************************************************************************************/
const char *NFparseGetAttribute (const char **attr, const char *name, const char *defaultString) {
	int i;

	for (i = 0; attr[i] != (char *) NULL; i += 2) if (strcmp (attr[i],name) == 0) return (attr[i + 1]);

	return (defaultString);
}

/****************************************************************************************************************
 * Entry start
*****************************************************************************************************************/
static void XMLCALL _NFentryStart (void *argPtr, const char *entry, const char **attr) {
	const char *name;
	const char *modelChildren     [] = { NFcompAggregateStr, NFcompContainerStr, NFcompInputStr, NFcompRegionStr, NFnumParameterStr, (char *) NULL };
	const char *containerChildren [] = { NFcompAggregateStr,
	                                     NFcompContainerStr,
			                             NFcompRegionStr,
			                             NFnumParameterStr,
			                             NFnumInitialStr,
			                             NFnumVariableStr,
			                             NFmodDerivativeStr, 
			                             NFmodEquationStr,
			                             NFmodInterfaceStr,
			                             NFmodProcessStr,
			                             NFmodRouteStr,
			                             NFobjOutputStr,
			                             (char *) NULL };
	const char *ioChildren        [] = { NFnumVariableStr, (char *) NULL };
	const char *regionChildren    [] = { NFobjAliasStr,    (char *) NULL };
	const char *aggregateChildren [] = { NFobjCategoryStr, (char *) NULL };
	const char *moduleChildren    [] = { NFobjAliasStr, NFnumParameterStr, (char *) NULL };
	NFdata_t   *data;
	NFobject_p object, parent;
	XML_Parser parser = (XML_Parser) argPtr;

	data = (NFdata_t *) XML_GetUserData (parser);

	if (data->Skip) {
		if (strcmp (data->SkipName,entry) == 0) data->SkipLevel++;
		return;
	}
	if (data->Stack + 1 >= data->StackSize) {
		data->Objects = (NFobject_p *) realloc (data->Objects, (data->StackSize + 1) * sizeof (NFobject_p));
		if (data->Objects == (NFobject_p *) NULL) {
			CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
			goto Abort;
		}
		data->Objects [data->StackSize] = (NFobject_p) NULL;
		data->StackSize++;
	}

	if ((name = NFparseGetAttribute (attr, NFattrNameStr, (const char *) NULL)) == (char *) NULL) {
		CMmsgPrint (CMmsgWarning,"Skipping unnamed %s section in line: %d\n",entry, XML_GetCurrentLineNumber (parser));
		goto Skip;
	}
	if (data->Stack == 0) { // Document root level
		if (strcmp (entry,NFcompModelStr) == 0) {
			if (data->Objects [data->Stack] != (NFobject_p) NULL) {
				CMmsgPrint (CMmsgWarning, "Skipping second model [%s] section [%s] in line: %d\n",entry, name, XML_GetCurrentLineNumber (parser));
				goto Skip;
			}
			if ((data->Objects [data->Stack] = NFparseCompModelCreate (parser,name,attr)) == (NFobject_p) NULL) {
				CMmsgPrint (CMmsgAppError, "Model creation error in %s:%d\n",__FILE__,__LINE__);
				goto Abort;
			}
			data->Model = (NFcompModel_p) data->Objects [data->Stack];
		}
		else {
			CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in line: %d\n",entry, name, XML_GetCurrentLineNumber (parser));
			goto Skip;
		}
	}
	else {
		parent = data->Objects [data->Stack - 1];
		switch (parent->Type) {
		case NFcompAggregate:
			if (CMoptLookup (aggregateChildren, entry, true) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name);
				goto Skip;
			}
			break;
		case NFcompContainer:
			if (CMoptLookup (containerChildren, entry, true) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name);
				goto Skip;
			}
			break;
		case NFcompInput:
			if (CMoptLookup (ioChildren, entry, true) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name);
				goto Skip;
			}
			break;
		case NFcompModel:
			if (CMoptLookup (modelChildren, entry, true) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name);
				goto Skip;
			}
			break;
		case NFcompRegion: 
			if (CMoptLookup (regionChildren, entry, true) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name);
				goto Skip;
			}
			break;
		case NFnumInitial:    CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		case NFnumParameter:  CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		case NFnumVariable:   CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		case NFmodDerivative: CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		case NFmodEquation:
			if (CMoptLookup (moduleChildren, entry, true) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name);
				goto Skip;
			}
			break;
		case NFmodInterface:  CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		case NFmodProcess:
			if (CMoptLookup (moduleChildren, entry, true) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name);
				goto Skip;
			}
			break;
		case NFmodRoute:      CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		case NFobjAlias:      CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		case NFobjCategory:   CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		case NFobjOutput:
			if (CMoptLookup (ioChildren, entry, true) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name);
				goto Skip;
			}
			break;
		case NFobjPlugin:   CMmsgPrint (CMmsgWarning,"Skipping [%s] section [%s] in %s [%s]\n",entry, name, NFobjTypeName (parent->Type), parent->Name); goto Skip;
		}
		if      (strcmp (entry, NFdimensionStr) == 0) {
			
		}
		else if (strcmp (entry, NFextentStr)    == 0) {
			
		}
		else {
			switch (NFobjTypeCode (entry)) {
				case NFcompAggregate: object = NFparseCompAggregateCreate (parser,parent,name,attr); break;
				case NFcompContainer: object = NFparseCompContainerCreate (parser,parent,name,attr); break;
				case NFcompInput:     object = NFparseCompInputCreate     (parser,parent,name,attr); break;
				case NFcompModel:     CMmsgPrint (CMmsgAppError, "This should never happen! %s:%d\n",__FILE__,__LINE__); goto Skip;
				case NFcompRegion:    object = NFparseCompRegionCreate    (parser,parent,name,attr); break;
				case NFnumInitial:    object = NFparseNumVariableCreate   (parser,parent,name,attr); break;
				case NFnumParameter:  object = NFparseNumParameterCreate  (parser,parent,name,attr); break;
				case NFnumVariable:   object = NFparseNumVariableCreate   (parser,parent,name,attr); break;
				case NFmodDerivative: object = NFparseModDerivativeCreate (parser,parent,name,attr); break;
				case NFmodEquation:   object = NFparseModEquationCreate   (parser,parent,name,attr); break;
				case NFmodInterface:  object = NFparseModInterfaceCreate  (parser,parent,name,attr); break;
				case NFmodProcess:    object = NFparseModProcessCreate    (parser,parent,name,attr); break;
				case NFmodRoute:      object = NFparseModRouteCreate      (parser,parent,name,attr); break;
				case NFobjAlias:      object = NFparseObjAliasCreate      (parser,parent,name,attr); break;
				case NFobjCategory:   object = NFparseObjCategoryCreate   (parser,parent,name,attr); break;
				case NFobjOutput:     object = NFparseObjOutputCreate     (parser,parent,name,attr); break;
				case NFobjPlugin:     CMmsgPrint (CMmsgAppError, "This should never happen! %s:%d\n",__FILE__,__LINE__); goto Skip;
			}
		if (object == (NFobject_p) NULL) goto Abort;
		data->Objects [data->Stack] = object;
		}
	}
	data->Stack++;
	return;

Abort:
	data->Abort     = true;
	
Skip:
	data->SkipName  = CMstrDuplicate (entry);
	data->Skip      = true;
	data->SkipLevel = 1;
	return;
}

/****************************************************************************************************************
 * Entry end
*****************************************************************************************************************/
static void XMLCALL _NFentryEnd (void *argPtr, const char *entry) {
	CMreturn ret;
	NFdata_t   *data;
	NFobject_p object, parent;
	XML_Parser parser = (XML_Parser) argPtr;

	data = (NFdata_t *) XML_GetUserData (parser);

	if (data->Skip) {
		if (strcmp (data->SkipName,entry) == 0) {
			data->SkipLevel--;
			if (data->SkipLevel == 0) {
				data->Skip = false;
				free (data->SkipName);
			}
		}
		return;
	}
	data->Stack--;
	object = data->Objects [data->Stack];
	parent = data->Stack > 0 ? data->Objects [data->Stack - 1] : (NFobject_p) NULL;
	switch (object->Type) {
	case NFcompAggregate: ret = NFparseCompAggregateFinalize (parent, object); break;
	case NFcompContainer: ret = NFparseCompContainerFinalize (parent, object); break;
	case NFcompInput:     ret = NFparseCompInputFinalize     (parent, object); break;
	case NFcompModel:     ret = NFparseCompModelFinalize     (object);         break;
	case NFcompRegion:    ret = NFparseCompRegionFinalize    (parent, object); break;
	case NFnumInitial:    ret = NFparseNumVariableFinalize   (parent, object); break;
	case NFnumParameter:  ret = NFparseNumParameterFinalize  (parent, object); break;
	case NFnumVariable:   ret = NFparseNumVariableFinalize   (parent, object); break;
	case NFmodDerivative: ret = NFparseModDerivativeFinalize (parent, object); break;
	case NFmodInterface:  ret = NFparseModInterfaceFinalize  (parent, object); break;
	case NFmodEquation:   ret = NFparseModEquationFinalize   (parent, object); break;
	case NFmodProcess:    ret = NFparseModProcessFinalize    (parent, object); break;
	case NFmodRoute:      ret = NFparseModRouteFinalize      (parent, object); break;
	case NFobjAlias:      ret = NFparseObjAliasFinalize      (parent, object); break;
	case NFobjCategory:   ret = NFparseObjCategoryFinalize   (parent, object); break;
	case NFobjOutput:     ret = NFparseObjOutputFinalize     (parent, object); break;
	case NFobjPlugin:     CMmsgPrint (CMmsgAppError, "This should never happen! %s:%d\n",__FILE__,__LINE__); break;
	}
	if (ret == CMfailed) goto Abort;
	data->Objects [data->Stack] = (NFobject_p) NULL;
	return;

Abort:
	data->Abort = true;
	return;
}

/****************************************************************************************************************
 * Entry text
*****************************************************************************************************************/
static void XMLCALL _NFentryText (void *argPtr, const char *str, int len) {
	char *whiteChars = " \t\n";
	size_t i, start, end;
	bool white;
	NFdata_t *data;
	NFobject_p object;
	XML_Parser parser = (XML_Parser) argPtr;

	data = (NFdata_t *) XML_GetUserData (parser);

	if (data->Skip) return;
	
	for (start = 0;start < len; start++) {
		for (i = 0;i < strlen (whiteChars); ++i)
			if (str [start] == whiteChars [i]) { white = true; break; }
		if (white == false) break;
		white = false;
	}
	if (start == len) return;

	for (end = len;end > 0; end--) {
		for (i = 0;i < strlen (whiteChars); ++i)
			if (str [end - 1] == whiteChars [i]) { white = true; break; }
		if (white == false) break;
		white = false;
	}
	if (end - start + 1 > data->BufferLen) {
		if ((data->Buffer = (char *) realloc (data->Buffer,end - start + 1)) == (char *) NULL) {
			CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n\n",__FILE__,__LINE__);
			return;
		}
		data->BufferLen = end - start + 1;
	}

	strncpy (data->Buffer, str + start, end - start);
	data->Buffer [end - start] = '\0';
//	TODO Initialize
//	object->Notes = CMstrAppend (object->Notes, data->Buffer, "\n");
}

/****************************************************************************************************************
 * Read document
*****************************************************************************************************************/
#define CHUNKSIZE 256

static char *_NFreadDoc (FILE *inFile) {
	int   len = 0, ret;
	char *doc = (char *) NULL;

	do
	{
		if ((doc = (char *) realloc (doc, len + CHUNKSIZE + 1)) == (char *) NULL) {
			CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
			return ((char *) NULL);
		}
		ret = fread(doc + len, 1, CHUNKSIZE, inFile);
		if ((ret == 0) && ferror(stdin)) {
			CMmsgPrint (CMmsgSysError, "Read error in %s:%d\n",__FILE__,__LINE__);
			free (doc);
			return ((char *) NULL);
		}
		len += ret;
	} while (ret > 0);
	doc [len] = '\0';

	return (doc);
}

/****************************************************************************************************************
 * Parse configuration
*****************************************************************************************************************/
NFcompModel_p NFmodelParse (FILE *inFile, bool report)
{
	char *doc;

	NFdata_t      data;
	XML_Parser parser;

	data.SkipName   = "root";
	data.Abort      = false;
	data.Buffer     = (char *) NULL;
	data.BufferLen  = 0;
	data.Skip       = false;
	data.Stack      =
	data.StackSize  =
	data.SkipLevel  = 0;
	data.Model      = (NFcompModel_p) NULL;
	data.Objects    = (NFobject_p *)  NULL;

	if ((doc = _NFreadDoc (inFile)) == (char *) NULL) {
		CMmsgPrint (CMmsgAppError, "Document reading error in %s:%d\n",__FILE__,__LINE__);
		return ((NFcompModel_p) NULL);
	}

	if ((parser = XML_ParserCreate(NULL)) == (XML_Parser) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation in %s:%d\n",__FILE__,__LINE__);
		return ((NFcompModel_p) NULL);
	}

	XML_SetUserData             (parser, &data);
	XML_SetElementHandler       (parser, _NFentryStart, _NFentryEnd);
	XML_SetCharacterDataHandler (parser, _NFentryText);
	XML_UseParserAsHandlerArg   (parser);

	if (XML_Parse(parser, doc, strlen (doc), true) == XML_STATUS_ERROR) {
		CMmsgPrint (CMmsgAppError,"Parse error at line %d\n", XML_GetCurrentLineNumber(parser));
		return ((NFcompModel_p) NULL);
	}
	XML_ParserFree (parser);
	if (data.Objects != (NFobject_p *) NULL) free (data.Objects);
	if ((data.Abort) && (data.Model != (NFcompModel_p) NULL)) {
		NFobjectFree ((NFobject_p) data.Model);
		return ((NFcompModel_p) NULL);
	}
	if (report) NFobjectPrint ((NFobject_p) (data.Model));
	return (data.Model);
}
