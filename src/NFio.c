#include <stdlib.h>
#include <cm.h>
#include <NFio.h>

NFio_p NFioCreate () {
	NFio_p io;
	
	if ((io = malloc (sizeof (NFio_t))) == (NFio_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d",__FILE__,__LINE__);
		return ((NFio_p) NULL);
	}
	io->ItemNum       = 0;
	io->TimeLine      = (NFtimeLine_p)             NULL;
	io->PluginData    = (void *)                     NULL;
	io->GetMapping    = (NFioGetMappingFunc)       NULL; 
	io->Close         = (NFioCloseFunc)            NULL;
	io->VarHandleFunc = (NFioVariableHandleFunc)   NULL;
	io->VarTypeFunc   = (NFioVariableTypeFunc)     NULL;
	io->VarLoadFunc   = (NFioVariableLoadFunc)     NULL;
	return (io);
}

NFmapping_p NFioMapping (NFio_p io,size_t num,NFcoordinate_t *coordinates) {
	if (io == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
		return ((NFmapping_p) NULL);
	}
	if (io->GetMapping == (NFioGetMappingFunc) NULL) {
		CMmsgPrint (CMmsgAppError, "Requesting mapping from incomplete io in: %s:%d\n",__FILE__,__LINE__);
		return ((NFmapping_p) NULL);		
	}
	return (io->GetMapping (io, num, coordinates));
}

void NFioFree (NFio_p io) {
	if (io == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
	}
	if (io->Close != (NFioCloseFunc) NULL) io->Close (io);
	free (io);
}

NFvarHandle NFioVariableHandle (NFio_p io, const char *variable) {
	if (io == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
		return ((NFvarHandle) NULL);
	}
	return (io->VarHandleFunc != (NFioVariableHandleFunc) NULL ? io->VarHandleFunc (io,variable) : (NFvarHandle) NULL);
}

NFvarType NFioVariableType (NFio_p io, NFvarHandle varHandle) {
	if (io == (NFio_p ) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
		return (NFvarTypeVoid);
	}
	return (io->VarTypeFunc != (NFioVariableTypeFunc) NULL ? io->VarTypeFunc (io,varHandle) : NFvarTypeVoid);
}

bool NFioVariableLoad (NFio_p io, NFvarHandle varHandle, NFvarType varType, size_t step, void *array) {
	if (io == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
		return (NFvarTypeVoid);
	}
	return (io->VarLoadFunc != (NFioVariableLoadFunc) NULL ? io->VarLoadFunc (io,varHandle, varType, step, array) : false);
}
