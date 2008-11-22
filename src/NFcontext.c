#include <NF.h>
#include <stdlib.h>

NFcontext_p NFcontextCreate () {
	NFcontext_p context;
	
	if ((context = (NFcontext_p ) malloc (sizeof (NFcontext_t))) == (NFcontext_p)  NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((NFcontext_p) NULL);
	}
	context->TimeStepUnit = (const char *) NULL;
	context->MinTimeStep  = 0;
	context->MaxTimeStep  = 0;
	context->ParameterNum = context->VariableNum  = 0;
	context->Parameters   = (struct NFparameter_s *) NULL;
	context->Variables    = (struct NFvariable_s  *)  NULL;
	context->UserDataSize = 0;
	context->UserData     = (void *) NULL;
	return (context);
}

void NFcontextFree (NFcontext_p context) {
	if (context != (NFcontext_p) NULL) free (context);
}

