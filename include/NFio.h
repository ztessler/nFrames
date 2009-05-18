#ifndef _NFio_H
#define _NFio_H

#include <udunits2.h>
#include <NF.h>
#include <NFtime.h>

typedef void *NFvarHandle;

/****************************************************************************************************************
 * Coordinate
*****************************************************************************************************************/
typedef struct NFcoordinate_s {
	double X;
	double Y;;
} NFcoordinate_t, *NFcoordinate_p;

/****************************************************************************************************************
 * Extent
*****************************************************************************************************************/
typedef struct NFextent_s {
	NFcoordinate_t LowerLeft;
	NFcoordinate_t UpperRight;
} NFextent_t, *NFextent_p;

/****************************************************************************************************************
 * Mapping
*****************************************************************************************************************/
typedef struct NFmapping_s {
	size_t  Num;
	size_t *List;
	double *Weights;
} NFmapping_t, *NFmapping_p;

/****************************************************************************************************************
 * Input output
*****************************************************************************************************************/
typedef struct NFio_s {
	size_t         ItemNum;
	NFcoordinate_t ItemBoxMin;
	NFcoordinate_t ItemBoxMax;
	NFextent_t     Extent;
	NFtimeLine_p   TimeLine;
	void          *PluginData;
	void         (*Close)         (struct NFio_s *);
	int          (*GetItem)       (struct NFio_s *, NFcoordinate_p);
	int          (*GetItemList)   (struct NFio_s *, NFextent_p, size_t *, size_t *);
	int          (*GetVertexes)   (struct NFio_s *, size_t, NFcoordinate_p, size_t *);
	CMreturn     (*ProjectXY2UV)  (struct NFio_s *, NFcoordinate_p, NFcoordinate_p);
	NFvarHandle  (*VarHandleFunc) (struct NFio_s *, const char *);
	NFvarType    (*VarTypeFunc)   (struct NFio_s *, NFvarHandle);
	bool         (*VarLoadFunc)   (struct NFio_s *, NFvarHandle, NFvarType, size_t, void *);
} NFio_t, *NFio_p;

typedef NFio_p      (*NFioOpenFunc)           (const char *, NFtime_p, NFtime_p, ut_system *);
typedef void        (*NFioCloseFunc)          (NFio_p);
typedef int         (*NFioGetItemFunc)        (NFio_p, NFcoordinate_p);
typedef int         (*NFioGetItemListFunc)    (NFio_p, NFextent_p, size_t *, size_t *);
typedef int         (*NFioGetVertexesFunc)    (NFio_p, size_t, NFcoordinate_p, size_t *);
typedef CMreturn    (*NFioProjectXY2UVFunc)   (NFio_p, NFcoordinate_p, NFcoordinate_p);
typedef NFvarHandle (*NFioVariableHandleFunc) (NFio_p, const char *);
typedef NFvarType   (*NFioVariableTypeFunc)   (NFio_p, NFvarHandle);
typedef bool        (*NFioVariableLoadFunc)   (NFio_p, NFvarHandle, NFvarType, size_t, void *);

int NFioDefaultProjectXY2UV (NFio_p, NFcoordinate_p, NFcoordinate_p);

NFio_p      NFioCreate ();
void        NFioFree   (NFio_p);
int         NFioGetItem        (NFio_p, NFcoordinate_p);
int         NFioGetItemList    (NFio_p, NFextent_p, size_t *, size_t *);
int         NFioGetVertexes    (NFio_p, size_t, NFcoordinate_p, size_t *);
CMreturn    NFioProjectXY2UV   (NFio_p, NFcoordinate_p, NFcoordinate_p);
NFvarHandle NFioVariableHandle (NFio_p, const char *);
NFvarType   NFioVariableType   (NFio_p, NFvarHandle);
bool        NFioVariableLoad   (NFio_p, NFvarHandle, NFvarType, size_t, void *);

#endif /*_NFio_H_*/
