#ifndef _NF_H
#define _NF_H

#include <stdbool.h>
#include <stdlib.h>
#include <cm.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef char   NFbyte;
typedef short  NFshort;
typedef float  NFfloat;
typedef double NFdouble;

typedef enum { NFvarTypeVoid   = 0,
	           NFvarTypeByte   = sizeof (NFbyte),
	           NFvarTypeShort  = sizeof (NFshort),
	           NFvarTypeFloat  = sizeof (NFfloat),
	           NFvarTypeDouble = sizeof (NFdouble) } NFvarType;

const char* NFvarTypeString (NFvarType);

typedef enum { NFintervalBegin, NFintervalCenter, NFintervalEnd } NFinterval;

typedef struct NFparameter_s {
	const char *Name;

	double      Value;
	double      Minimum;
	double      Maximum;
	size_t      ElementNum;
	size_t      ElementSize;
	size_t      Address;
} NFparameter_t, *NFparameter_p;

typedef enum { NFinput, NFoutput, NFmodified } NFrole;

typedef struct NFvariable_s
{
	char      *Name;
	NFrole     Role;
	char      *UnitString;
	size_t     ElementNum;
	NFvarType  VarType;
	size_t     Address;
	double     Default;
} NFvariable_t, *NFvariable_p;

typedef struct NFcontext_s {
	const char    *TimeStepUnit;
	size_t         MinTimeStep;
	size_t         MaxTimeStep;
	size_t         ParameterNum;
	NFparameter_t *Parameters;
	size_t         VariableNum;
	NFvariable_t  *Variables;
	size_t         UserDataSize;
	void          *UserData;
} NFcontext_t, *NFcontext_p;

NFcontext_p NFcontextCreate ();
void          NFcontextFree   (NFcontext_p);

#define NFscalar(dataType,element) 1,sizeof (((dataType *) NULL)->element),((char *) &(((dataType *) NULL)->element) - (char *) ((dataType *) NULL))
#define NFvector(dataType,element) sizeof(((dataType *) NULL)->element)/sizeof(((dataType *) NULL)->element[0]),sizeof (((dataType *) NULL)->element[0]),((char *) (((dataType *) NULL)->element) - (char *) ((dataType *) NULL))
#define NFparameterNum(parameters) sizeof(parameters)/sizeof(parameters[0])
#define NFvariableNum(variables)   sizeof(variables)/sizeof(variables[0])
#define NFmissingVal -9999.0

typedef void (*NFinitializeFunc) (NFcontext_p);
typedef void (*NFexecuteFunc)    (void *);
typedef void (*NFfinalizeFunc)   (void *);

#if defined(__cplusplus)
}
#endif

#endif
