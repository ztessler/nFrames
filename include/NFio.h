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
} NFextent_t;

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
	NFmapping_p  (*GetMapping)    (struct NFio_s *, size_t, NFcoordinate_t *);
	void         (*Close)         (struct NFio_s *);
	NFvarHandle  (*VarHandleFunc) (struct NFio_s *, const char *);
	NFvarType    (*VarTypeFunc)   (struct NFio_s *, NFvarHandle);
	bool         (*VarLoadFunc)   (struct NFio_s *, NFvarHandle, NFvarType, size_t, void *);
} NFio_t, *NFio_p;

NFio_p      NFioCreate ();
NFmapping_p NFioMapping        (NFio_p, size_t, NFcoordinate_t *);
void        NFioFree           (NFio_p);
NFvarHandle NFioVariableHandle (NFio_p, const char *);
NFvarType   NFioVariableType   (NFio_p, NFvarHandle);
bool        NFioVariableLoad   (NFio_p, NFvarHandle, NFvarType, size_t, void *);

typedef NFio_p      (*NFioOpenFunc)           (const char *, NFtime_p, NFtime_p, ut_system *);
typedef NFmapping_p (*NFioGetMappingFunc)     (NFio_p, size_t, NFcoordinate_t *);
typedef void        (*NFioCloseFunc)          (NFio_p);
typedef NFvarHandle (*NFioVariableHandleFunc) (NFio_p, const char *);
typedef NFvarType   (*NFioVariableTypeFunc)   (NFio_p, NFvarHandle);
typedef bool        (*NFioVariableLoadFunc)   (NFio_p, NFvarHandle, NFvarType, size_t, void *);

#endif /*_NFio_H_*/
