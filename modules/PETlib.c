#include <NF.h>
#include <math.h>

enum { VectorLen = 20 };

typedef struct HamonData_s {
	NFfloat Parameter0;
	NFfloat Parameter1;
	NFfloat Airtemp;
	NFfloat VaporPressure;
	NFfloat Vector[VectorLen];
	NFfloat PotEvap;
} HamonData_t;

void Initialize_Hamon (NFcontext_p context) {
	static struct NFparameter_s parameters [] = {{ "param0",  0.5, 0.2,  0.8,       NFscalar(HamonData_t,Parameter0)},
	                                               { "param1", 10.0, 5.0, 20.0,       NFscalar(HamonData_t,Parameter1)}};
	static struct NFvariable_s  variables  [] = {{ "airtemp", NFinput,  "degC",   NFscalar(HamonData_t,Airtemp),       NFmissingVal},
			                                       { "vpress",  NFinput,  "kPa",    NFscalar(HamonData_t,VaporPressure), NFmissingVal},
			                                       { "Vector",  NFoutput, "m",      NFvector(HamonData_t,Vector),        NFmissingVal},
			                                       { "pet",     NFoutput, "mm/day", NFscalar(HamonData_t,PotEvap),       NFmissingVal}};
	context->TimeStepUnit = "day";
	context->MinTimeStep  = context->MaxTimeStep  = 1;
	context->ParameterNum = NFparameterNum (parameters);
	context->Parameters   = parameters;
	context->VariableNum  = NFvariableNum  (variables);
	context->Variables    = variables;
	context->UserDataSize = sizeof (HamonData_t);          // User's responsibility
}

void Execute_Hamon  (void *userData) {
	size_t i;
	HamonData_t *data = (HamonData_t *) userData;

	data->Parameter0    = 1.0;
	data->Parameter1    = 2.0;
	data->Airtemp       = 3.0;
	data->VaporPressure = 4.0;
	data->PotEvap       = 5.0;

	for (i = 0;i < VectorLen; ++i) data->Vector [i] = i + 6.0;
}

void Finalize_Hamon (void *userData) {
	size_t i;
	HamonData_t *data = (HamonData_t *) userData;

	CMmsgPrint (CMmsgUsrError, "Parameter0: %lf\n",    data->Parameter0);
	CMmsgPrint (CMmsgUsrError, "Parameter1: %lf\n",    data->Parameter1);
	CMmsgPrint (CMmsgUsrError, "Airtemp: %lf\n",       data->Airtemp);
	CMmsgPrint (CMmsgUsrError, "VaporPressure: %lf\n", data->VaporPressure);
	CMmsgPrint (CMmsgUsrError, "PotEvap: %lf\n",       data->PotEvap);
	for (i = 0;i < VectorLen; ++i) CMmsgPrint (CMmsgInfo, "Vector: %lf\n", data->Vector [i]);
}

float VPressSat (float airT) {
/* calculates saturated vp from airt temperature Murray (1967)
 * airT      - air temperature [degree C] */
	return ((airT >= 0.0) ? 0.61078 * exp (17.26939 * airT / (airT + 237.3)) :
   							   0.61078 * exp (21.87456 * airT / (airT + 265.5)));
}

float VPressDelta (float airT) {
/* calculates saturated vp delta from airt temperature Murray (1967)
 * airT      - air temperature [degree C] */
	return ((airT >= 0.0) ?
			   4098 * VPressSat (airT) / ((airT + 237.3) * (airT + 237.3)) :
			   5808 * VPressSat (airT) / ((airT + 265.5) * (airT + 265.5)));
}

#define MDConstC1           0.25   // intercept in actual/potential solar radiation to sunshine duration
#define MDConstC2           0.5    // slope in actual/potential solar radiation to sunshine duration
#define MDConstC3           0.2    // longwave cloud correction coefficient
#define MDConstSIGMA      5.67E-08 // Stefan-Boltzmann constant, W/(m2/K4)

float SRadNETLong (float i0hDay,float airT,float solRad,float ea) {
/* net longwave radiation,  W/m2 emissivity of the surface taken as 1.0
 * to also account for reflected all equations and parameters from Brutsaert (1982)
 * airT      - air temperature [degree C]
 * ea        - vapor pressure  [kPa] */
	float effem;  /* effective emissivity from clear sky */
	float novern; /* sunshine duration fraction of daylength */
	float cldcor; /* cloud cover correction to net longwave under clear sky */

/* Brutsaert method */
	effem = 1.24 * pow (ea * 10. / (airT + 273.15),1.0 / 7.0);
/*
	additional methods not used
	Brunt method

	bruntA = .44;    - Brunt intercept
	bruntB = .253;   - Brunt EA coefficient, for kPa (value for mb * �10)
	effem = bruntA + bruntB * sqrt(ea);

	Satterlund method
	effem = 1.08 * (1 - exp (pow (-(10.0 * ea),(airT + 273.15) / 2016.)))

	Swinbank method
	effem = 0.0000092 * (airT + 273.15) * (airT + 273.15);

	Idso-Jackson method
	effem = 1.0 - 0.261 * exp (-.000777 * airT * airT);
*/
   novern = i0hDay > 0.0 ? (solRad / i0hDay - MDConstC1) / MDConstC2 : (1.0 - MDConstC1) / MDConstC2;
   if (novern > 1.0) novern = 1.0;
   if (novern < 0.0) novern = 0.0;
   cldcor = MDConstC3 + (1.0 - MDConstC3) * novern; 
   return ((effem - 1.0) * cldcor * MDConstSIGMA * pow (airT + 273.15,4.0));
}

typedef struct PsTaylorData_s {
	NFfloat IGrate;
	NFfloat PSgamma;
	NFfloat PTalpha;
	NFfloat EtoM;

	NFfloat Albedo;
	NFfloat DayLength;
	NFfloat I0HDay;
	NFfloat AirTemp;
	NFfloat SolRad;
	NFfloat VaporP;
	NFfloat Pet;
} PsTaylorData_t;

void Initialize_PsTaylor (NFcontext_p context) {
	static struct NFparameter_s parameters [] = {{ "IGrate",   0.5, 0.2,  0.8,      NFscalar(PsTaylorData_t,IGrate)},
	                                               { "PSgamma", 10.0, 5.0, 20.0,      NFscalar(PsTaylorData_t,PSgamma)},
	                                               { "PTalpha", 10.0, 5.0, 20.0,      NFscalar(PsTaylorData_t,PTalpha)},
	                                               { "EtoM",    10.0, 5.0, 20.0,      NFscalar(PsTaylorData_t,EtoM)}};
	static struct NFvariable_s  variables  [] = {{ "airtemp", NFinput,  "degC",   NFscalar(PsTaylorData_t,AirTemp), NFmissingVal},
			                                       { "vpress",  NFinput,  "kPa",    NFscalar(PsTaylorData_t,VaporP),  NFmissingVal},
			                                       { "solrad",  NFinput,  "MJ/m2",  NFscalar(PsTaylorData_t,SolRad),  NFmissingVal},
			                                       { "pet",     NFoutput, "mm/day", NFscalar(PsTaylorData_t,Pet), NFmissingVal}};
	context->TimeStepUnit = "day";
	context->MinTimeStep  = context->MaxTimeStep  = 1;
	context->ParameterNum = NFparameterNum (parameters);
	context->Parameters   = parameters;
	context->VariableNum  = NFvariableNum  (variables);
	context->Variables    = variables;
	context->UserDataSize = sizeof (HamonData_t);          // User's responsibility
}

void Execute_PsTaylor  (void *userData) {
	PsTaylorData_t *data = (PsTaylorData_t *) userData;
	float solNet;  // average net solar radiation for daytime [W/m2]
	float lngNet;  // average net longwave radiation for day  [W/m2]
	float aa;      // available energy [W/m2]
	float es;      // vapor pressure at airT [kPa]
	float delta;   // dEsat/dTair [kPa/K]
	float dd;      // vapor pressure deficit [kPa]
	float le;      // latent heat [W/m2]
	float sHeat = 0.0;  // average subsurface heat storage for day [W/m2]

	solNet = (1.0 - data->Albedo) * data->SolRad / data->IGrate;
	lngNet = SRadNETLong (data->I0HDay,data->AirTemp,data->SolRad,data->VaporP);

	aa     = solNet + lngNet - sHeat;
	es     = VPressSat  (data->AirTemp);
	delta  = VPressDelta (data->AirTemp);

	dd     = es - data->VaporP; 
	le     = data->PTalpha * delta * aa / (delta + data->PSgamma);

	data->Pet = data->EtoM * data->IGrate * le;
}

void Finalize_PsTaylor (void *userData) {
	PsTaylorData_t *data = (PsTaylorData_t *) userData;

	data = data;
}
