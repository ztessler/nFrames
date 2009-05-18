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
	io->PluginData    = (void *)                   NULL;
	io->Close         = (NFioCloseFunc)            NULL;
	io->GetItem       = (NFioGetItemFunc)          NULL;
	io->GetItemList   = (NFioGetItemListFunc)      NULL;
	io->GetVertexes   = (NFioGetVertexesFunc)      NULL;
	io->ProjectXY2UV  = (NFioProjectXY2UVFunc)     NULL;
	io->VarHandleFunc = (NFioVariableHandleFunc)   NULL;
	io->VarTypeFunc   = (NFioVariableTypeFunc)     NULL;
	io->VarLoadFunc   = (NFioVariableLoadFunc)     NULL;
	return (io);
}

int NFioGetItem (NFio_p io, NFcoordinate_p coordinate) {
	if (io == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (io->GetItem == (NFioGetItemFunc) NULL) {
		CMmsgPrint (CMmsgAppError, "Requesting item from incomplete io in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	return (io->GetItem (io, coordinate));
}

int NFioGetItemList (NFio_p io, NFextent_p extent, size_t *itemBuffer, size_t *itemBufferLen) {
	if (io == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (io->GetItemList == (NFioGetItemListFunc) NULL) {
		CMmsgPrint (CMmsgAppError, "Requesting item list from incomplete io in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	return (io->GetItemList (io, extent, itemBuffer, itemBufferLen));
}

int NFioGetVertexes (NFio_p io, size_t itemID, NFcoordinate_p vertexBuffer, size_t *vertexBufferLen) {
	if (io == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (io->GetVertexes == (NFioGetVertexesFunc) NULL) {
		CMmsgPrint (CMmsgAppError, "Requesting item vertexes from incomplete io in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	return (io->GetVertexes (io, itemID, vertexBuffer, vertexBufferLen));
}

int NFioProjextXY2UV (NFio_p io, NFcoordinate_p xyCoord, NFcoordinate_p uvCoord) {
	if (io == (NFio_p) NULL) {
		CMmsgPrint (CMmsgAppError, "Invalid io in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (io->ProjectXY2UV == (NFioProjectXY2UVFunc) NULL) {
		CMmsgPrint (CMmsgAppError, "Requesting project from incomplete io in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	return (io->ProjectXY2UV (io, xyCoord, uvCoord));
}

int NFioDefaultProjectXY2UV (NFio_p io, NFcoordinate_p xyCoord, NFcoordinate_p uvCoord) {
	uvCoord->X = xyCoord->X;
	uvCoord->Y = xyCoord->Y;
	if ((uvCoord->X < -180.0) || (uvCoord->X > 360.0))
		CMmsgPrint (CMmsgWarning,"Longitude is outside of valid range!\n");
	if ((uvCoord->X  <  -90.0) || (uvCoord->Y > 90.0))
		CMmsgPrint (CMmsgWarning,"Latitude is outside of valid range!\n");
	return (CMsucceeded);
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
