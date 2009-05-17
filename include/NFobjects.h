#ifndef _NFobject_H
#define _NFobject_H

#include <expat.h>
#include <cm.h>
#include <NF.h>
#include <NFio.h>
#include <NFtime.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define NFcompAggregateStr    "aggregate"
#define NFcompContainerStr    "container"
#define NFcomponentStr        "component"
#define NFcompInputStr        "input"
#define NFcompLayoutStr       "layout"
#define NFcompModelStr        "model"
#define NFcompRegionStr       "region"

#define NFmodDerivativeStr    "derivative"
#define NFmodEquationStr      "equation"
#define NFmodInterfaceStr     "interface"
#define NFmodProcessStr       "process"
#define NFmodRouteStr         "route"

#define NFnumInitialStr       "initial"
#define NFnumParameterStr     "parameter"
#define NFnumVariableStr      "variable"

#define NFobjAliasStr         "alias"
#define NFobjCategoryStr      "category"
#define NFobjOutputStr        "output"
#define NFobjPluginStr        "plugin"
#define NFobjPointStr         "point"

#define NFdimensionStr        "dimension"
#define NFextentStr           "extent"

#define NFattrBeginStr        "begin"
#define NFattrCouplerStr      "coupler"
#define NFattrDifferenceStr   "difference"
#define NFattrDirectionStr    "direction"
#define NFattrEndStr          "end"
#define NFattrInitialValStr   "initial_value"
#define NFattrNameStr         "name"
#define NFattrMaximumStr      "maximum"
#define NFattrMethodStr       "method"
#define NFattrMinimumStr      "minimum"
#define NFattrPathStr         "path"
#define NFattrRelationStr     "relation"
#define NFattrSizeStr         "size"
#define NFattrStandardNameStr "standard_name"
#define NFattrStatesStr       "states"
#define NFattrTimeStepStr     "time_step"
#define NFattrTypeStr         "type"
#define NFattrUnitStr         "unit"
#define NFattrUrlStr          "url"
#define NFattrValueStr        "value"
#define NFattrVersionStr      "version"
#define NFattrXCoordStr       "xcoord"
#define NFattrYCoordStr       "ycoord"
#define NFattrWeightStr       "weight"

#define NFkeyBackwardStr      "backward"
#define NFkeyCenteredStr      "centered"
#define NFkeyForwardStr       "forward"
#define NFkeyCouplerFluxStr   "flux"
#define NFkeyCouplerPointStr  "point"
#define NFkeyCouplerSurfStr   "surface"
#define NFkeyDirectionXStr    "x"
#define NFkeyDirectionYStr    "y"
#define NFkeyDirectionZStr    "z"
#define NFkeyGridStr          "grid"
#define NFkeyInheritStr       "inherit"
#define NFkeyUnitlessStr      "unitless"
#define NFkeyOwnStr           "own"
#define NFkeyParameterStr     "parameter"
#define NFkeyParentStr        "parent"
#define NFkeyRootStr          "root"
#define NFkeyPointsStr        "points"
#define NFkeyVariableStr      "variable"

#define NFfuncExecuteStr      "Execute"
#define NFfuncFinalizeStr     "Finalize"
#define NFfuncInitializeStr   "Initialize"
#define NFfuncOpenStr         "Open"

typedef enum { NFcompAggregate,
               NFcompContainer,
               NFcompInput,
               NFcompModel,
               NFcompRegion,
               NFmodDerivative,
               NFmodEquation,
               NFmodInterface,
               NFmodProcess,
               NFmodRoute,
               NFnumInitial,
               NFnumParameter,
               NFnumVariable,
               NFobjAlias,
               NFobjCategory,
               NFobjOutput,
               NFobjPlugin} NFobjType;

typedef enum { NFdirX, NFdirY, NFdirZ } NFdirection;

/****************************************************************************************************************
 * List
*****************************************************************************************************************/
typedef struct NFlist_s {
	size_t Num;
	void **List;
} NFlist_t, *NFlist_p;

NFlist_p NFlistCreate     ();
void     NFlistFree       (NFlist_p);
CMreturn NFlistAddItem    (NFlist_p, void *);
CMreturn NFlistRemoveItem (NFlist_p, void *);

/****************************************************************************************************************
 * Element
*****************************************************************************************************************/
typedef struct NFelement_s {
	size_t         VertexNum;
	NFcoordinate_p Vertexes;
	NFextent_t     Extent;
} NFelement_t, *NFelement_p;

NFelement_p NFelementCreate     ();
void        NFelementFree       (NFelement_p);

/****************************************************************************************************************
 * Domain
*****************************************************************************************************************/
typedef struct NFdomain_s {
	size_t Num;
	NFio_p IO;
	NFelement_p *Elements;
} NFdomain_t, *NFdomain_p;

NFdomain_p NFdomainCreate     ();
void       NFdomainFree       (NFdomain_p);

/****************************************************************************************************************
 * Object
*****************************************************************************************************************/
typedef struct NFobject_s {
	NFobjType   Type;
	char       *Name;
	char       *Notes;
	struct NFobject_s *Parent;
} NFobject_t, *NFobject_p;

NFobject_p  NFobjectCreate (const char *, NFobjType);
void        NFobjectFree  (NFobject_p);
void        NFobjectPrint (NFobject_p);
const char *NFobjTypeName (NFobjType);
NFobjType   NFobjTypeCode (const char *);
NFobject_p  NFobjectRoot  (NFobject_p);

#define NFlistAddObject(list,object)   NFlistAddItem (list, (NFobject_t *) object)

/****************************************************************************************************************
 * Object List
*****************************************************************************************************************/
typedef struct NFobjList_s {
	NFobject_p  Parent;
	size_t Num;
	NFobject_p *List;
} NFobjList_t, *NFobjList_p;

NFobjList_p NFobjListCreate         (NFobject_p);
void        NFobjListFree           (NFobjList_p);
CMreturn    NFobjListAddItem        (NFobjList_p, NFobject_p);
NFobject_p  NFobjListFindItemByID   (NFobjList_p, size_t);
NFobject_p  NFobjListFindItemByName (NFobjList_p, const char *);

/****************************************************************************************************************
 * Component
*****************************************************************************************************************/
typedef struct NFcomponent_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFobjList_p      Variables;
	NFtimeStep_p     TimeStep;
	NFdomain_p       Domain;
} NFcompenent_t, *NFcomponent_p;

/****************************************************************************************************************
 * Parameter
*****************************************************************************************************************/
typedef struct NFnumParameter_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	double           Value;
	double           Minimum, Maximum;
} NFnumParameter_t, *NFnumParameter_p;

/****************************************************************************************************************
 * Variable
*****************************************************************************************************************/
typedef struct NFnumVariable_s {
	NFobjType       Type;
	char           *Name;
	char           *Notes;
	NFobject_p      Parent;

	bool            Route;
	char           *UnitString;
	char           *StandardName;
	size_t          XMLline;
	double          InitialValue;
	NFvarHandle     VarHandle;
	NFvarType       VarType;
	union {
		void   *Void;
		char   *Byte;
		short  *Short;
		float  *Float;
		double *Double;
	} Data;
} NFnumVariable_t, *NFnumVariable_p;

/****************************************************************************************************************
 * Alias
*****************************************************************************************************************/
typedef enum { NFaliasVariable, NFaliasParameter } NFaliasType;
typedef struct NFobjAlias_s {
	NFobjType       Type;
	char           *Name;
	char           *Notes;
	NFobject_p      Parent;

	char           *Alias;
	NFaliasType     AliasType;
} NFobjAlias_t, *NFobjAlias_p;

/****************************************************************************************************************
 * Category
*****************************************************************************************************************/
typedef struct NFobjCategory_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	char            *Values;
	NFnumVariable_p  Variable;
} NFobjCategory_t, *NFobjCategory_p;

/****************************************************************************************************************
 * Output
*****************************************************************************************************************/
typedef struct NFobjOutput_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	char            *Path;
	NFobjList_p      Variables;
} NFobjOutput_t, *NFobjOutput_p;

/****************************************************************************************************************
 * Plugin
*****************************************************************************************************************/
typedef struct NFobjPlugin_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	void            *Handle;
} NFobjPlugin_t, *NFobjPlugin_p;

NFobjPlugin_p NFpluginGet (NFobjList_p, const char *);
void *NFpluginFunction (NFobjPlugin_p, const char *, const char *);
/****************************************************************************************************************
 * Aggregate Component
*****************************************************************************************************************/
typedef struct NFcompAggregate_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFobjList_p      Variables; // owned
	NFtimeStep_p     TimeStep;  // borrowed
	NFdomain_p       Domain;    // borrowed

	NFobject_p       Component;
	NFnumVariable_p  Variable;
	NFobjList_p      Categories;
} NFcompAggregate_t, *NFcompAggregate_p;

/****************************************************************************************************************
 * Container Component
*****************************************************************************************************************/
typedef struct NFcompContainer_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFobjList_p      Variables; // owned
	NFtimeStep_p     TimeStep;  // borrowed or owned
	NFdomain_p       Domain;    // borrowed

	NFobjList_p      Components;
	NFobjList_p      Parameters;
	NFobjList_p      Modules;
	NFobjList_p      Outputs;
	char            *States;
} NFcompContainer_t, *NFcompContainer_p;

/****************************************************************************************************************
 * Input Component
*****************************************************************************************************************/
typedef struct NFcompInput_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFobjList_p      Variables; // owned
	NFtimeStep_p     TimeStep;  // owned
	NFdomain_p       Domain;    // owned

	char            *URL;
	NFobjPlugin_p    IOplugin;
	size_t           XMLline;
	char            *IOmethod;
} NFcompInput_t, *NFcompInput_p;

/****************************************************************************************************************
 * Model
*****************************************************************************************************************/
typedef struct NFcompModel_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFobjList_p      Variables; // Unused !!
	NFtimeStep_p     TimeStep;  // owned
	NFdomain_p       Domain;    // Unused !!

	NFobjList_p      Parameters;
	NFobjList_p      Components;
	NFobjList_p      IOPlugins;
	NFobjList_p      ModPlugins;

	char            *Version;
	NFtime_p         Begin;
	NFtime_p         End;
	NFlist_p         TimeSteps;
	NFlist_p         Couplers;
	ut_system       *UtSystem;
} NFcompModel_t, *NFcompModel_p;

/****************************************************************************************************************
 * Region Component
*****************************************************************************************************************/
typedef struct NFcompRegion_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFobjList_p      Variables; // Owned
	NFtimeStep_p     TimeStep;  // borrowed
	NFdomain_p       Domain;    // borrowed

	NFobjList_p      Aliases;
} NFcompRegion_t, *NFcompRegion_p;

/****************************************************************************************************************
 * Derivative
*****************************************************************************************************************/
typedef struct NFmodDerivative_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFnumVariable_p  Variable;
	NFnumVariable_p  InputVar;
	NFdirection      Direction;
	NFinterval       Difference;
} NFmodDerivative_t, *NFmodDerivative_p;

/****************************************************************************************************************
 * Equiation Module
*****************************************************************************************************************/
typedef struct NFmodEquation_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFnumVariable_p  Variable;
	NFobjList_p      InputVars;
	NFobjList_p      Aliases;
	NFobjList_p      Parameters;
} NFmodEquation_t, *NFmodEquation_p;

/****************************************************************************************************************
 * Coupler
*****************************************************************************************************************/
typedef enum { NFcouplerFlux, NFcouplerPoint, NFcouplerSurface } NFcoupler;

typedef struct NFcoupler_s {
	NFcoupler  Type;
	NFdomain_p SrcDomain, DstDomain;
	void      *Mapping;
} NFcoupler_t, *NFcoupler_p;

NFcoupler_p NFcouplerGet   (NFcomponent_p, NFcomponent_p, NFcoupler);
void        NFcouplerFree  (NFcoupler_p);
CMreturn    NFcouplerBuild (NFcoupler_p);

/****************************************************************************************************************
 * Interface
*****************************************************************************************************************/
typedef struct NFmodInterface_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFnumVariable_p  Variable;
	NFnumVariable_p  InputVar;
	NFcomponent_p    Component;
	NFcoupler        CouplerType;
	NFcoupler_p      Coupler;
} NFmodInterface_t, *NFmodInterface_p;

/****************************************************************************************************************
 * Process Module
*****************************************************************************************************************/
typedef struct NFmodProcess_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFobjList_p      InputVars;
	NFobjList_p      OutputVars;
	NFobjList_p      Aliases;
	NFobjList_p      Parameters;
	NFcontext_p      Context;
	NFexecuteFunc    Execute;
	NFinitializeFunc Initialize;
	NFfinalizeFunc   Finalize;
} NFmodProcess_t, *NFmodProcess_p;

/****************************************************************************************************************
 * Route Module
*****************************************************************************************************************/
typedef struct NFmodRoute_s {
	NFobjType        Type;
	char            *Name;
	char            *Notes;
	NFobject_p       Parent;

	NFnumVariable_p  Variable;
} NFmodRoute_t, *NFmodRoute_p;

/****************************************************************************************************************
 * Functions
*****************************************************************************************************************/
const char *NFparseGetAttribute (const char **, const char *, const char *);
NFobject_p NFparseCompAggregateCreate (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseCompContainerCreate (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseCompInputCreate     (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseCompModelCreate     (XML_Parser parser, const char *, const char **);
NFobject_p NFparseCompRegionCreate    (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseModEquationCreate   (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseModProcessCreate    (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseNumParameterCreate  (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseNumVariableCreate   (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseObjAliasCreate      (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseObjCategoryCreate   (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseModDerivativeCreate (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseModInterfaceCreate  (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseObjOutputCreate     (XML_Parser parser, NFobject_p, const char *, const char **);
NFobject_p NFparseModRouteCreate      (XML_Parser parser, NFobject_p, const char *, const char **);

CMreturn NFparseCompAggregateFinalize (NFobject_p, NFobject_p);
CMreturn NFparseCompContainerFinalize (NFobject_p, NFobject_p);
CMreturn NFparseCompInputFinalize     (NFobject_p, NFobject_p);
CMreturn NFparseCompModelFinalize     (NFobject_p);
CMreturn NFparseCompRegionFinalize    (NFobject_p, NFobject_p);
CMreturn NFparseModDerivativeFinalize (NFobject_p, NFobject_p);
CMreturn NFparseModEquationFinalize   (NFobject_p, NFobject_p);
CMreturn NFparseModInterfaceFinalize  (NFobject_p, NFobject_p);
CMreturn NFparseModProcessFinalize    (NFobject_p, NFobject_p);
CMreturn NFparseModRouteFinalize      (NFobject_p, NFobject_p);
CMreturn NFparseObjAliasFinalize      (NFobject_p, NFobject_p);
CMreturn NFparseObjCategoryFinalize   (NFobject_p, NFobject_p);
CMreturn NFparseObjOutputFinalize     (NFobject_p, NFobject_p);
CMreturn NFparseNumParameterFinalize  (NFobject_p, NFobject_p);
CMreturn NFparseNumVariableFinalize   (NFobject_p, NFobject_p);

NFcompModel_p NFmodelParse   (FILE *, bool);
CMreturn      NFmodelResolve (NFcompModel_p, bool);
CMreturn      NFmodelExecute (NFcompModel_p, bool);
void          NFmodelReport  (NFcompModel_p);

NFtimeStep_p  NFmodelAddTimeStep (NFcompModel_p, NFtimeStep_p);

#if defined(__cplusplus)
}
#endif

#endif
