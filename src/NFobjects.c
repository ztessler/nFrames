#include <dlfcn.h>
#include <math.h>
#include <string.h>
#include <NFobjects.h>

/****************************************************************************************************************
 * Object type name
*****************************************************************************************************************/
const char *NFobjTypeName (NFobjType objType) {

	switch (objType) {
	case NFcompAggregate: return (NFcompAggregateStr);
	case NFcompContainer: return (NFcompContainerStr);
	case NFcompInput:     return (NFcompInputStr);
	case NFcompModel:     return (NFcompModelStr);
	case NFcompRegion:    return (NFcompRegionStr);
	case NFmodDerivative: return (NFmodDerivativeStr);
	case NFmodEquation:   return (NFmodEquationStr);
	case NFmodInterface:  return (NFmodInterfaceStr);
	case NFmodProcess:    return (NFmodProcessStr);
	case NFmodRoute:      return (NFmodRouteStr);
	case NFnumInitial:    return (NFnumInitialStr);
	case NFnumParameter:  return (NFnumParameterStr);
	case NFnumVariable:   return (NFnumVariableStr);
	case NFobjAlias:      return (NFobjAliasStr);
	case NFobjCategory:   return (NFobjCategoryStr);
	case NFobjOutput:     return (NFobjOutputStr);
	case NFobjPlugin:     return (NFobjPluginStr);
	}
	return ((char *) NULL);
}

/****************************************************************************************************************
 * Object type code
*****************************************************************************************************************/
NFobjType NFobjTypeCode (const char *typeName) {

	if      (strcmp (typeName,NFcompAggregateStr) == 0) return (NFcompAggregate);
	else if (strcmp (typeName,NFcompContainerStr) == 0) return (NFcompContainer);
	else if (strcmp (typeName,NFcompInputStr)     == 0) return (NFcompInput);
	else if (strcmp (typeName,NFcompModelStr)     == 0) return (NFcompModel);
	else if (strcmp (typeName,NFcompRegionStr)    == 0) return (NFcompRegion);
	else if (strcmp (typeName,NFmodDerivativeStr) == 0) return (NFmodDerivative);
	else if (strcmp (typeName,NFmodEquationStr)   == 0) return (NFmodEquation);
	else if (strcmp (typeName,NFmodInterfaceStr)  == 0) return (NFmodInterface);
	else if (strcmp (typeName,NFmodProcessStr)    == 0) return (NFmodProcess);
	else if (strcmp (typeName,NFmodRouteStr)      == 0) return (NFmodRoute);
	else if (strcmp (typeName,NFnumInitialStr)    == 0) return (NFnumInitial);
	else if (strcmp (typeName,NFnumParameterStr)  == 0) return (NFnumParameter);
	else if (strcmp (typeName,NFnumVariableStr)   == 0) return (NFnumVariable);
	else if (strcmp (typeName,NFobjAliasStr)      == 0) return (NFobjAlias);
	else if (strcmp (typeName,NFobjCategoryStr)   == 0) return (NFobjCategory);
	else if (strcmp (typeName,NFobjOutputStr)     == 0) return (NFobjOutput);
	else if (strcmp (typeName,NFobjPluginStr)     == 0) return (NFobjPlugin);
	return (NFnumVariable);
}
/****************************************************************************************************************
 * Object size
*****************************************************************************************************************/
static int _NFobjectSize (NFobjType objType) {

	switch (objType) {
	case NFcompAggregate: return (sizeof (NFcompAggregate_t));
	case NFcompContainer: return (sizeof (NFcompContainer_t));
	case NFcompInput:     return (sizeof (NFcompInput_t));
	case NFcompModel:     return (sizeof (NFcompModel_t));
	case NFcompRegion:    return (sizeof (NFcompRegion_t));
	case NFmodDerivative: return (sizeof (NFmodDerivative_t));
	case NFmodEquation:   return (sizeof (NFmodEquation_t));
	case NFmodInterface:  return (sizeof (NFmodInterface_t));
	case NFmodProcess:    return (sizeof (NFmodProcess_t));
	case NFmodRoute:      return (sizeof (NFmodRoute_t));
	case NFnumInitial:    return (sizeof (NFnumVariable_t));
	case NFnumParameter:  return (sizeof (NFnumParameter_t));
	case NFnumVariable:   return (sizeof (NFnumVariable_t));
	case NFobjAlias:      return (sizeof (NFobjAlias_t));
	case NFobjCategory:   return (sizeof (NFobjCategory_t));
	case NFobjOutput:     return (sizeof (NFobjOutput_t));
	case NFobjPlugin:     return (sizeof (NFobjPlugin_t));
	}
	return (CMfailed);
}

/****************************************************************************************************************
 * Object root
*****************************************************************************************************************/
NFobject_p NFobjectRoot (NFobject_p object) {
	while (object->Parent != (NFobject_p) NULL) object = object->Parent;
	return (object);
}

/****************************************************************************************************************
 * Print definations
*****************************************************************************************************************/
static void _NFcompAggregatePrint (NFobject_p);
static void _NFcompContainerPrint (NFobject_p);
static void _NFcompInputPrint     (NFobject_p);
static void _NFcompModelPrint     (NFobject_p);
static void _NFcompRegionPrint    (NFobject_p);
static void _NFmodDerivativePrint (NFobject_p);
static void _NFmodEquationPrint   (NFobject_p);
static void _NFmodInterfacePrint  (NFobject_p);
static void _NFmodProcessPrint    (NFobject_p);
static void _NFmodRoutePrint      (NFobject_p);
static void _NFnumVariablePrint   (NFobject_p);
static void _NFnumParameterPrint  (NFobject_p);
static void _NFnumVariablePrint   (NFobject_p);
static void _NFobjAliasPrint      (NFobject_p);
static void _NFobjCategoryPrint   (NFobject_p);
static void _NFobjOutputPrint     (NFobject_p);

/****************************************************************************************************************
 * Aggregate component
*****************************************************************************************************************/
static CMreturn _NFcompAggregateInit (NFobject_p object) {
	NFcompAggregate_p component = (NFcompAggregate_p) object;

	component->Variables  = NFobjListCreate (object);
	component->TimeStep   = (NFtimeStep_p)    NULL;
	component->Domain     = (NFdomain_p)      NULL;

	component->Component  = (NFobject_p)      NULL;
	component->Variable   = (NFnumVariable_p) NULL;
	component->Categories = NFobjListCreate (object);
 	return (CMsucceeded);
}
/****************************************************************************************************************/
static void _NFcompAggregateFree (NFobject_p object) {
	NFcompAggregate_p component = (NFcompAggregate_p) object;

	NFobjListFree (component->Categories);
	NFobjListFree (component->Variables);
}
/****************************************************************************************************************/
static void _NFcompAggregatePrint (NFobject_p object) {
	size_t i;
	NFcompAggregate_p component = (NFcompAggregate_p) object;

	if (component->TimeStep != (NFtimeStep_p) NULL) {
		if (component->TimeStep->Length > 1)
			CMmsgPrint (CMmsgInfo, "Time Step: %d %ss\n", component->TimeStep->Length,NFtimeUnitString (component->TimeStep->Unit));
		else
			CMmsgPrint (CMmsgInfo, "Time Step: %d %s\n",  component->TimeStep->Length,NFtimeUnitString (component->TimeStep->Unit));
	}
	if (component->Component    != (NFobject_p)      NULL) CMmsgPrint (CMmsgInfo,"Component: %s\n", component->Component->Name);
	if (component->Variable     != (NFnumVariable_p) NULL) CMmsgPrint (CMmsgInfo,"Variable: %s\n",  component->Variable->Name);
	CMmsgPrint (CMmsgInfo, "Categories Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0;i < component->Categories->Num; ++i) NFobjectPrint (component->Categories->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Categories End\n");
	CMmsgPrint (CMmsgInfo, "Variables Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0;i < component->Variables->Num;  ++i) NFobjectPrint (component->Variables->List  [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Variables End\n");
}

/****************************************************************************************************************
 * Container component
*****************************************************************************************************************/
static CMreturn _NFcompContainerInit (NFobject_p object) {
	NFcompContainer_p component = (NFcompContainer_p) object;

	component->Variables  = NFobjListCreate (object);
	component->TimeStep   = (NFtimeStep_p) NULL;
	component->Domain     = (NFdomain_p)   NULL;

	component->Parameters = NFobjListCreate (object);
	component->Components = NFobjListCreate (object);
	component->Modules    = NFobjListCreate (object);
	component->Outputs    = NFobjListCreate (object);
	component->TimeStep   = NFtimeStepCreate ();
	component->States     = (char *) NULL;
	return (CMsucceeded);
}
/****************************************************************************************************************/
static void _NFcompContainerFree (NFobject_p object) {
	NFcompContainer_p component = (NFcompContainer_p) object;

	NFobjListFree (component->Variables);
	NFobjListFree (component->Components);
	NFobjListFree (component->Modules);
	NFobjListFree (component->Outputs);
	NFobjListFree (component->Parameters);
	if (component->States   != (char *) NULL) free (component->States);
}

/****************************************************************************************************************/
static void _NFcompContainerPrint (NFobject_p object) {
	size_t i;
	NFcompContainer_p component = (NFcompContainer_p) object;

	if (component->TimeStep != (NFtimeStep_p) NULL) {
		if (component->TimeStep->Length > 1)
			CMmsgPrint (CMmsgInfo, "Time Step: %d %ss\n", component->TimeStep->Length,NFtimeUnitString (component->TimeStep->Unit));
		else
			CMmsgPrint (CMmsgInfo, "Time Step: %d %s\n", component->TimeStep->Length,NFtimeUnitString (component->TimeStep->Unit));
	}
	if (component->States   != (char *) NULL) CMmsgPrint (CMmsgInfo, "States: %s\n",    component->States);

	CMmsgPrint (CMmsgInfo, "Parameters Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < component->Parameters->Num; ++i) NFobjectPrint (component->Parameters->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Parameters End\n");
	CMmsgPrint (CMmsgInfo, "Variables Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < component->Variables->Num;  ++i) NFobjectPrint (component->Variables->List  [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Variables End\n");
	CMmsgPrint (CMmsgInfo, "Components Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < component->Components->Num; ++i) NFobjectPrint (component->Components->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Components End\n");
	CMmsgPrint (CMmsgInfo, "Modules Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < component->Modules->Num;    ++i) NFobjectPrint (component->Modules->List    [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Modules End\n");
	CMmsgPrint (CMmsgInfo, "Outputs Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < component->Outputs->Num;    ++i) NFobjectPrint (component->Outputs->List    [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Outputs End\n");
}

/****************************************************************************************************************
 * Input component
*****************************************************************************************************************/
static CMreturn _NFcompInputInit (NFobject_p object) {
	NFcompInput_p component = (NFcompInput_p) object;

	component->Variables = NFobjListCreate (object);
	component->TimeStep  = NFtimeStepCreate ();
	component->Domain    = NFdomainCreate  ();

	component->URL       = (char *) NULL;
	component->IOplugin  = (NFobjPlugin_p) NULL;
	component->XMLline   = 0;
	component->IOmethod  = (char *) NULL;
	return (CMsucceeded);
}
/****************************************************************************************************************/
static void _NFcompInputFree (NFobject_p object) {
	NFcompInput_p component = (NFcompInput_p) object;

	if (component->Domain   != (NFdomain_p)       NULL) NFdomainFree   (component->Domain);
	NFobjListFree (component->Variables);
	if (component->URL      != (char *) NULL) free (component->URL);
	if (component->IOmethod != (char *) NULL) free (component->IOmethod);
}

/****************************************************************************************************************/
static void _NFcompInputPrint (NFobject_p object) {
	size_t i;
	NFcompInput_p component = (NFcompInput_p) object;

	if (component->TimeStep != (NFtimeStep_p) NULL) {
		if (component->TimeStep->Length > 1)
			CMmsgPrint (CMmsgInfo, "Time Step: %d %ss\n", component->TimeStep->Length,NFtimeUnitString (component->TimeStep->Unit));
		else
			CMmsgPrint (CMmsgInfo, "Time Step: %d %s\n", component->TimeStep->Length,NFtimeUnitString (component->TimeStep->Unit));
	}
	if (component->URL != (char *) NULL) CMmsgPrint (CMmsgInfo, "URL: %s\n", component->URL);
	CMmsgPrint (CMmsgInfo, "Variables Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < component->Variables->Num;  ++i) NFobjectPrint (component->Variables->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Variables End\n");

}

/****************************************************************************************************************
 * Model
*****************************************************************************************************************/
static CMreturn _NFcompModelInit (NFobject_p object) {
	NFcompModel_p model = (NFcompModel_p) object;

	model->Version    = (char *) NULL;
	model->Begin      = NFtimeCreate ();
	model->End        = NFtimeCreate ();
	model->TimeStep   = NFtimeStepCreate ();
	model->Domain     = (NFdomain_p) NULL;

	model->Parameters = NFobjListCreate (object);
	model->Components = NFobjListCreate (object);
	model->IOPlugins  = NFobjListCreate (object);
	model->ModPlugins = NFobjListCreate (object);
	model->UtSystem   = ut_read_xml ((char *) NULL);
	model->TimeSteps  = NFlistCreate ();
	model->Couplers   = NFlistCreate ();

	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFcompModelFree (NFobject_p object) {
	size_t i;
	NFcompModel_p model = (NFcompModel_p) object;

	if (model->Version  != (char *)         NULL) free (model->Version);
	if (model->Begin    != (NFtime_p)     NULL) NFtimeFree     (model->Begin);
	if (model->End      != (NFtime_p)     NULL) NFtimeFree     (model->End);
	if (model->TimeStep != (NFtimeStep_p) NULL) NFtimeStepFree (model->TimeStep);
	NFobjListFree (model->Parameters);
	NFobjListFree (model->Components);
	NFobjListFree (model->IOPlugins);
	NFobjListFree (model->ModPlugins);
//	if (model->UtSystem != (ut_system *) NULL) ut_free_system (model->UtSystem);
	for (i = 0;i < model->TimeSteps->Num;++i) NFtimeStepFree ((NFtimeStep_p) (model->TimeSteps->List [i]));
	NFlistFree (model->TimeSteps);
	for (i = 0;i < model->Couplers->Num; ++i) NFcouplerFree  ((NFcoupler_p)  (model->Couplers->List [i]));
	NFlistFree (model->Couplers);
}

/****************************************************************************************************************/
static void _NFcompModelPrint (NFobject_p object) {
	size_t i;
	NFcompModel_p model = (NFcompModel_p) object;

	if (model->Version  != (char *) NULL) CMmsgPrint (CMmsgInfo, "Version: %s\n",  model->Version);
	if (model->Begin    != (NFtime_p) NULL)
		CMmsgPrint (CMmsgInfo, "Begin: %04d-%02d-%02d %02d:%02d\n", model->Begin->Year,
			                                                        model->Begin->Month,
			                                                        model->Begin->Day,
			                                                        model->Begin->Hour,
			                                                        model->Begin->Minute);
	if (model->End      != (NFtime_p) NULL)
		CMmsgPrint (CMmsgInfo, "End: %04d-%02d-%02d %02d:%02d\n",   model->End->Year,
		                                                            model->End->Month,
		                                                            model->End->Day,
		                                                            model->End->Hour,
		                                                            model->End->Minute);
	if (model->TimeStep != (NFtimeStep_p) NULL)
		CMmsgPrint (CMmsgInfo, "TimeStep: %d %s\n", model->TimeStep->Length,
		                                            NFtimeUnitString (model->TimeStep->Unit));
	CMmsgPrint (CMmsgInfo, "Parameters Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0;i < model->Parameters->Num; ++i) NFobjectPrint (model->Parameters->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Parameters End\n");	CMmsgPrint (CMmsgInfo, "Components Begin\n");

	CMmsgPrint (CMmsgInfo, "Components Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0;i < model->Components->Num; ++i) NFobjectPrint (model->Components->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Components End\n");

	CMmsgPrint (CMmsgInfo, "Layout Plugins Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0;i < model->IOPlugins->Num; ++i) NFobjectPrint (model->IOPlugins->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Layout Plugins End\n");

	CMmsgPrint (CMmsgInfo, "Module Plugins Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0;i < model->ModPlugins->Num; ++i) NFobjectPrint (model->ModPlugins->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Module Plugins End\n");
}

/****************************************************************************************************************
 * Region component
*****************************************************************************************************************/
static CMreturn _NFcompRegionInit (NFobject_p object) {
	NFcompRegion_p component = (NFcompRegion_p) object;

	component->Variables = NFobjListCreate (object);
	component->TimeStep  = (NFtimeStep_p) NULL;
	component->Domain    = (NFdomain_p)   NULL;
	component->Aliases   = NFobjListCreate (object);
	return (CMsucceeded);
}
/****************************************************************************************************************/
static void _NFcompRegionFree (NFobject_p object) {
	NFcompRegion_p component = (NFcompRegion_p) object;

	NFobjListFree (component->Variables);
	NFobjListFree (component->Aliases);
}

/****************************************************************************************************************/
static void _NFcompRegionPrint (NFobject_p object) {
	size_t i;
	NFcompRegion_p component = (NFcompRegion_p) object;

	if (component->TimeStep != (NFtimeStep_p) NULL)
		CMmsgPrint (CMmsgInfo, "TimeStep: %d %s\n", component->TimeStep->Length,
		                                            NFtimeUnitString (component->TimeStep->Unit));
	CMmsgPrint (CMmsgInfo, "Variables Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < component->Variables->Num;  ++i) NFobjectPrint (component->Variables->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Variables End\n");  CMmsgPrint (CMmsgInfo, "Components Begin\n");

	CMmsgPrint (CMmsgInfo, "Aliases Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < component->Aliases->Num;    ++i) NFobjectPrint (component->Aliases->List   [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Aliases End\n");    CMmsgPrint (CMmsgInfo, "Components Begin\n");
}

/****************************************************************************************************************
 * Derivative module
*****************************************************************************************************************/
static CMreturn _NFmodDerivativeInit (NFobject_p object) {
	NFmodDerivative_p module = (NFmodDerivative_p) object;

	module->Variable = (NFnumVariable_p) NULL;
	module->InputVar = (NFnumVariable_p) NULL;
	return (CMsucceeded);
}
/****************************************************************************************************************/
static void _NFmodDerivativeFree (NFobject_p object) {
	NFmodDerivative_p module = (NFmodDerivative_p) object;

	if (module->Variable != (NFnumVariable_p) NULL) NFobjectFree ((NFobject_p) module->Variable);
}

/****************************************************************************************************************/
static void _NFmodDerivativePrint (NFobject_p object) {
	NFmodDerivative_p module = (NFmodDerivative_p) object;

	if (module->Variable != (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgInfo, "Derivative variable Begin\n");
		CMmsgIndent (CMmsgInfo, true);
		NFobjectPrint ((NFobject_p) module->Variable);
		CMmsgIndent (CMmsgInfo, false);
		CMmsgPrint (CMmsgInfo, "Derivative variable End\n");
	}
	if (module->InputVar != (NFnumVariable_p) NULL) {
		CMmsgPrint  (CMmsgInfo, "Input variable Begin\n");
		CMmsgIndent (CMmsgInfo, true);
		NFobjectPrint ((NFobject_p) module->InputVar);
		CMmsgIndent (CMmsgInfo, false);
		CMmsgPrint  (CMmsgInfo, "Input variable End\n");
	}

	switch (module->Direction) {
	case NFdirX: CMmsgPrint (CMmsgInfo, "Direction: X\n"); break;
	case NFdirY: CMmsgPrint (CMmsgInfo, "Direction: Y\n"); break;
	case NFdirZ: CMmsgPrint (CMmsgInfo, "Direction: Z\n"); break;
	}
	switch (module->Difference) {
	case NFintervalCenter: CMmsgPrint (CMmsgInfo, "Difference: Centered\n"); break;
	case NFintervalEnd:    CMmsgPrint (CMmsgInfo, "Difference: Forward\n");  break;
	case NFintervalBegin:  CMmsgPrint (CMmsgInfo, "Difference: Backward\n"); break;
	}
}

/****************************************************************************************************************
 * Equation module
*****************************************************************************************************************/
static CMreturn _NFmodEquationInit (NFobject_p object)  {
	NFmodEquation_p module = (NFmodEquation_p) object;

	module->Variable   = (NFnumVariable_p) NULL;
	module->InputVars  = NFobjListCreate (object);
	module->Aliases    = NFobjListCreate (object);
	module->Parameters = NFobjListCreate (object);
	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFmodEquationFree (NFobject_p object) {
	NFmodEquation_p module = (NFmodEquation_p) object;

	if (module->Variable != (NFnumVariable_p) NULL) NFobjectFree ((NFobject_p) module->Variable);
	NFobjListFree (module->InputVars);
	NFobjListFree (module->Aliases);
	NFobjListFree (module->Parameters);
}

/****************************************************************************************************************/
static void _NFmodEquationPrint (NFobject_p object) {
	size_t i;
	NFmodEquation_p module = (NFmodEquation_p) object;

	if (module->Variable != (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgInfo, "Equation variable Begin\n");
		CMmsgIndent (CMmsgInfo, true);
		NFobjectPrint ((NFobject_p) module->Variable);
		CMmsgIndent (CMmsgInfo, false);
		CMmsgPrint (CMmsgInfo, "Equation variable End\n");
	}
	for (i = 0; i < module->Aliases->Num;    ++i) NFobjectPrint (module->Aliases->List    [i]);
	for (i = 0; i < module->Parameters->Num; ++i) NFobjectPrint (module->Parameters->List [i]);
}

/****************************************************************************************************************
 * Interface module
*****************************************************************************************************************/
static CMreturn _NFmodInterfaceInit (NFobject_p object) {
	NFmodInterface_p module = (NFmodInterface_p) object;

	module->Variable  = (NFnumVariable_p) NULL;
	module->InputVar  = (NFnumVariable_p) NULL;
	module->Component = (NFcomponent_p)   NULL;
	module->Coupler   = (NFcoupler_p)     NULL;
	return (CMsucceeded);
}
/****************************************************************************************************************/
static void _NFmodInterfaceFree (NFobject_p object) {
	NFmodInterface_p module = (NFmodInterface_p) object;

	if (module->Variable != (NFnumVariable_p) NULL) NFobjectFree ((NFobject_p) module->Variable);
}

/****************************************************************************************************************/
static void _NFmodInterfacePrint (NFobject_p object) {
	NFmodInterface_p module = (NFmodInterface_p) object;

	if (module->Variable != (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgInfo, "Output variable Begin\n");
		CMmsgIndent (CMmsgInfo, true);
		NFobjectPrint ((NFobject_p) module->Variable);
		CMmsgIndent (CMmsgInfo, false);
		CMmsgPrint (CMmsgInfo, "Output variable End\n");
	}
	if (module->InputVar != (NFnumVariable_p) NULL) {
		CMmsgPrint  (CMmsgInfo, "Input variable Begin\n");
		CMmsgIndent (CMmsgInfo, true);
		NFobjectPrint ((NFobject_p) module->InputVar);
		CMmsgIndent (CMmsgInfo, false);
		CMmsgPrint  (CMmsgInfo, "Input variable End\n");
	}
}

/****************************************************************************************************************
 * Process module
*****************************************************************************************************************/
static CMreturn _NFmodProcessInit (NFobject_p object)  {
	NFmodProcess_p module = (NFmodProcess_p) object;

	module->InputVars    = NFobjListCreate (object);
	module->OutputVars   = NFobjListCreate (object);
	module->Aliases      = NFobjListCreate (object);
	module->Parameters   = NFobjListCreate (object);
	module->Context      = NFcontextCreate ();
	module->Execute      = (NFexecuteFunc)    NULL;
	module->Finalize     = (NFfinalizeFunc)   NULL;
	module->Initialize   = (NFinitializeFunc) NULL;
	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFmodProcessFree (NFobject_p object) {
	NFmodProcess_p module = (NFmodProcess_p) object;

	NFobjListFree (module->InputVars);
	NFobjListFree (module->OutputVars);
	NFobjListFree (module->Aliases);
	NFobjListFree (module->Parameters);
	if (module->Context != (NFcontext_p) NULL) NFcontextFree (module->Context);
}

/****************************************************************************************************************/
static void _NFmodProcessPrint (NFobject_p object) {
	size_t i;
	NFmodProcess_p module = (NFmodProcess_p) object;

	for (i = 0; i < module->Aliases->Num;    ++i) NFobjectPrint (module->Aliases->List    [i]);
	CMmsgPrint (CMmsgInfo, "Parameters Begin\n");
	CMmsgIndent (CMmsgInfo, true);
	for (i = 0; i < module->Parameters->Num; ++i) NFobjectPrint (module->Parameters->List [i]);
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "Parameters End\n");	CMmsgPrint (CMmsgInfo, "Components Begin\n");
}

/****************************************************************************************************************
 * Route module
*****************************************************************************************************************/
static CMreturn _NFmodRouteInit (NFobject_p object)  {
	NFmodRoute_p module = (NFmodRoute_p) object;

	module->Variable = (NFnumVariable_p) NULL;
	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFmodRouteFree (NFobject_p object) {
	NFmodRoute_p module = (NFmodRoute_p) object;

	if (module->Variable != (NFnumVariable_p) NULL) NFobjectFree ((NFobject_p) module->Variable);
}

/****************************************************************************************************************/
static void _NFmodRoutePrint (NFobject_p object) {
	NFmodRoute_p module = (NFmodRoute_p) object;

	if (module->Variable != (NFnumVariable_p) NULL) {
		CMmsgPrint (CMmsgInfo, "Output variable Begin\n");
		CMmsgIndent (CMmsgInfo, true);
		NFobjectPrint ((NFobject_p) module->Variable);
		CMmsgIndent (CMmsgInfo, false);
		CMmsgPrint (CMmsgInfo, "Output variable End\n");
	}
}

/****************************************************************************************************************
 * Parameter
*****************************************************************************************************************/
static CMreturn _NFnumParameterInit (NFobject_p object) {
	NFnumParameter_p parameter = (NFnumParameter_p) object;

	parameter->Value        = 0.0;
	parameter->Minimum      =  HUGE_VAL;
	parameter->Maximum      = -HUGE_VAL;
	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFnumParameterFree (NFobject_p object) {
	NFnumParameter_p parameter = (NFnumParameter_p) object;

	parameter = parameter;
}

/****************************************************************************************************************/
static void _NFnumParameterPrint (NFobject_p object) {
	NFnumParameter_p parameter = (NFnumParameter_p) object;

	CMmsgIndent (CMmsgInfo, true);
	CMmsgPrint  (CMmsgInfo, "Value = %f\n",parameter->Value);
	CMmsgIndent (CMmsgInfo, false);
}

/****************************************************************************************************************
 * Variable
*****************************************************************************************************************/
static CMreturn _NFnumVariableInit (NFobject_p object) {
	NFnumVariable_p variable = (NFnumVariable_p) object;

	variable->Route        = false;
	variable->UnitString   = (char *) NULL;
	variable->StandardName = (char *) NULL;
	variable->XMLline      = 0;
	variable->InitialValue = 0.0;
	variable->VarHandle    = NULL;
	variable->VarType      = NFvarTypeVoid;
	variable->Data.Void    = (void *) NULL;
	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFnumVariableFree (NFobject_p object) {
	NFnumVariable_p variable = (NFnumVariable_p) object;

	if (variable->UnitString   != (char *) NULL) free (variable->UnitString);
	if (variable->StandardName != (char *) NULL) free (variable->StandardName);
	if (variable->Data.Void    != (void *) NULL) free (variable->Data.Void);
}

/****************************************************************************************************************/
static void _NFnumVariablePrint (NFobject_p object) {
	NFnumVariable_p variable = (NFnumVariable_p) object;

	if (variable->UnitString   != (char *) NULL) CMmsgPrint (CMmsgInfo,"Unit: %s\n",         variable->UnitString);
	if (variable->StandardName != (char *) NULL) CMmsgPrint (CMmsgInfo,"Standard name: %s\n",variable->StandardName);
}

/****************************************************************************************************************
 * Alias
*****************************************************************************************************************/
static CMreturn _NFobjAliasInit (NFobject_p object) {
	NFobjAlias_p alias = (NFobjAlias_p) object;

	alias->Alias     = (char *) NULL;
	alias->AliasType = NFaliasVariable;
	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFobjAliasFree (NFobject_p object) {
	NFobjAlias_p alias = (NFobjAlias_p) object;

	if (alias->Alias != (char *) NULL) free (alias->Alias);
}

/****************************************************************************************************************/
static void _NFobjAliasPrint (NFobject_p object) {
	NFobjAlias_p alias = (NFobjAlias_p) object;

	if (alias->Alias != (char *) NULL) CMmsgPrint (CMmsgInfo, "Alias: %s\n", alias->Alias);
}

/****************************************************************************************************************
 * Category
*****************************************************************************************************************/
static CMreturn _NFobjCategoryInit (NFobject_p object) {
	NFobjCategory_p category = (NFobjCategory_p) object;

	category->Values   = (char *) NULL;
	category->Variable = (NFnumVariable_p) NULL;
	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFobjCategoryFree (NFobject_p object) {
	NFobjCategory_p category = (NFobjCategory_p) object;

	if (category->Values   != (char *) NULL) free (category->Values);
	if (category->Variable != (NFnumVariable_p) NULL) NFobjectFree ((NFobject_p) category->Variable);
}

/****************************************************************************************************************/
static void _NFobjCategoryPrint (NFobject_p object) {
	NFobjCategory_p category = (NFobjCategory_p) object;

	if (category->Values != (char *) NULL) CMmsgPrint (CMmsgInfo, "Values: %s\n", category->Values);
}

/****************************************************************************************************************
 * Output
*****************************************************************************************************************/
static CMreturn _NFobjOutputInit (NFobject_p object) {
	NFobjOutput_p output = (NFobjOutput_p) object;

	output->Path        = (char *)  NULL;
	output->Variables   = NFobjListCreate (object);
	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFobjOutputFree (NFobject_p object) {
	NFobjOutput_p output = (NFobjOutput_p) object;

	if (output->Path != (char *) NULL) free (output->Path);
	NFobjListFree (output->Variables);
}

/****************************************************************************************************************/
static void _NFobjOutputPrint (NFobject_p object) {
	size_t i;
	NFobjOutput_p output = (NFobjOutput_p) object;

	if (output->Path != (char *) NULL) CMmsgPrint (CMmsgInfo, "Path: %s\n", output->Path);
	for (i = 0;i < output->Variables->Num; ++i) NFobjectPrint (output->Variables->List [i]);
}

/****************************************************************************************************************
 * Plugin
*****************************************************************************************************************/
static CMreturn _NFobjPluginInit (NFobject_p object) {

	return (CMsucceeded);
}

/****************************************************************************************************************/
static void _NFobjPluginFree (NFobject_p object) {

	if (((NFobjPlugin_p) object)->Handle != (void *) NULL) dlclose (((NFobjPlugin_p) object)->Handle);
}

/****************************************************************************************************************/
static void _NFobjPluginPrint (NFobject_p object) {

	CMmsgPrint (CMmsgInfo, "Path: %s\n", object->Name);
}

/****************************************************************************************************************
 * Object
*****************************************************************************************************************/
NFobject_p NFobjectCreate (const char *name, NFobjType objType) {
	CMreturn ret;
	size_t size;
	NFobject_p object;

	if ((size = _NFobjectSize (objType)) == CMfailed) {
		CMmsgPrint (CMmsgAppError, "Invalid object in: %s:%d\n",__FILE__,__LINE__);
		return ((NFobject_p) NULL);
	}
	if ((object = (NFobject_p) malloc (size)) == (NFobject_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation in %s:%d\n",__FILE__,__LINE__);
		return ((NFobject_p) NULL);
	}
	if ((object->Name = CMstrDuplicate (name)) == (char *) NULL) {
		CMmsgPrint (CMmsgAppError,"Name string duplication error in %s:%d\n",__FILE__,__LINE__);
		free (object);
		return ((NFobject_p) NULL);
	}
	object->Type   = objType;
	object->Notes  = (char *) NULL;

	switch (objType) {
	case NFcompAggregate: ret = _NFcompAggregateInit (object); break;
	case NFcompContainer: ret = _NFcompContainerInit (object); break;
	case NFcompInput:     ret = _NFcompInputInit     (object); break;
	case NFcompModel:     ret = _NFcompModelInit     (object); break;
	case NFcompRegion:    ret = _NFcompRegionInit    (object); break;
	case NFmodDerivative: ret = _NFmodDerivativeInit (object); break;
	case NFmodEquation:   ret = _NFmodEquationInit   (object); break;
	case NFmodInterface:  ret = _NFmodInterfaceInit  (object); break;
	case NFmodProcess:    ret = _NFmodProcessInit    (object); break;
	case NFmodRoute:      ret = _NFmodRouteInit      (object); break;
	case NFnumInitial:    ret = _NFnumVariableInit   (object); break;
	case NFnumParameter:  ret = _NFnumParameterInit  (object); break;
	case NFnumVariable:   ret = _NFnumVariableInit   (object); break;
	case NFobjAlias:      ret = _NFobjAliasInit      (object); break;
	case NFobjCategory:   ret = _NFobjCategoryInit   (object); break;
	case NFobjOutput:     ret = _NFobjOutputInit     (object); break;
	case NFobjPlugin:     ret = _NFobjPluginInit     (object); break;
	}
	if (ret != CMsucceeded) {
		free (object->Name);
		return ((NFobject_p) NULL);
	}
	else
		return (object);
}

/****************************************************************************************************************/
void NFobjectFree (NFobject_p object) {

	switch (object->Type) {
	case NFcompAggregate: _NFcompAggregateFree (object); break;
	case NFcompContainer: _NFcompContainerFree (object); break;
	case NFcompInput:     _NFcompInputFree     (object); break;
	case NFcompModel:     _NFcompModelFree     (object); break;
	case NFcompRegion:    _NFcompRegionFree    (object); break;
	case NFmodDerivative: _NFmodDerivativeFree (object); break;
	case NFmodEquation:   _NFmodEquationFree   (object); break;
	case NFmodInterface:  _NFmodInterfaceFree  (object); break;
	case NFmodProcess:    _NFmodProcessFree    (object); break;
	case NFmodRoute:      _NFmodRouteFree      (object); break;
	case NFnumInitial:    _NFnumVariableFree   (object); break;
	case NFnumParameter:  _NFnumParameterFree  (object); break;
	case NFnumVariable:   _NFnumVariableFree   (object); break;
	case NFobjAlias:      _NFobjAliasFree      (object); break;
	case NFobjCategory:   _NFobjCategoryFree   (object); break;
	case NFobjOutput:     _NFobjOutputFree     (object); break;
	case NFobjPlugin:     _NFobjPluginFree     (object); break;
	}
	if (object->Name != (char *) NULL) free (object->Name);
	if (object->Notes!= (char *) NULL) free (object->Notes);
}

void NFobjectPrint (NFobject_p object) {

	CMmsgPrint (CMmsgInfo, "%s [%s] Begin\n",NFobjTypeName (object->Type), object->Name);
	CMmsgIndent (CMmsgInfo, true);
	if (object->Parent != (NFobject_p) NULL)
		CMmsgPrint (CMmsgInfo, "Parent: %s [%s]\n",NFobjTypeName (object->Parent->Type), object->Parent->Name, object->Name);
	switch (object->Type) {
	case NFcompAggregate: _NFcompAggregatePrint (object); break;
	case NFcompContainer: _NFcompContainerPrint (object); break;
	case NFcompInput:     _NFcompInputPrint     (object); break;
	case NFcompModel:     _NFcompModelPrint     (object); break;
	case NFcompRegion:    _NFcompRegionPrint    (object); break;
	case NFmodDerivative: _NFmodDerivativePrint (object); break;
	case NFmodEquation:   _NFmodEquationPrint   (object); break;
	case NFmodInterface:  _NFmodInterfacePrint  (object); break;
	case NFmodProcess:    _NFmodProcessPrint    (object); break;
	case NFmodRoute:      _NFmodRoutePrint      (object); break;
	case NFnumInitial:    _NFnumVariablePrint   (object); break;
	case NFnumParameter:  _NFnumParameterPrint  (object); break;
	case NFnumVariable:   _NFnumVariablePrint   (object); break;
	case NFobjAlias:      _NFobjAliasPrint      (object); break;
	case NFobjCategory:   _NFobjCategoryPrint   (object); break;
	case NFobjOutput:     _NFobjOutputPrint     (object); break;
	case NFobjPlugin:     _NFobjPluginPrint     (object); break;
	}
	CMmsgIndent (CMmsgInfo, false);
	CMmsgPrint (CMmsgInfo, "%s [%s] End\n",NFobjTypeName (object->Type), object->Name);
}
