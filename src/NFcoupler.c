#include <NFobjects.h>

#include <string.h>

/****************************************************************************************************************
 * Coupler create
*****************************************************************************************************************/
NFcoupler_p NFcouplerGet (NFcomponent_p srcComponent, NFcomponent_p dstComponent, NFcoupler type) {
	size_t i;
	NFobject_p rootObj;
	NFcompModel_p model;
	NFcoupler_p coupler;

	rootObj = NFobjectRoot ((NFobject_p) srcComponent);
	if (rootObj != NFobjectRoot ((NFobject_p) dstComponent)) {
		CMmsgPrint (CMmsgAppError, "This is impossible in %s:%d!\n", __FILE__,__LINE__);
		return ((NFcoupler_p) NULL);
	}
	if (rootObj->Type != NFcompModel) {
		CMmsgPrint (CMmsgAppError, "This is impossible in %s:%d!\n", __FILE__,__LINE__);
		return ((NFcoupler_p) NULL);
	}
	model = (NFcompModel_p) rootObj;
	for (i = 0; i < model->Couplers->Num; ++i) {
		coupler = (NFcoupler_p) (model->Couplers->List [i]);
		if ((coupler->SrcDomain == srcComponent->Domain) &&
		    (coupler->DstDomain == dstComponent->Domain) &&
		    (coupler->Type      == type)) return (coupler);
	}

	if ((coupler = (NFcoupler_p) malloc (sizeof (NFcoupler_t))) == (NFcoupler_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((NFcoupler_p) NULL);
	}
	coupler->SrcDomain = srcComponent->Domain;
	coupler->DstDomain = dstComponent->Domain;
	coupler->Type = type;
	coupler->Mapping = (void *) NULL;

	if (NFlistAddItem (model->Couplers, (void *) coupler) != CMsucceeded) {
		CMmsgPrint (CMmsgAppError,"Coupler list error in %s:%d\n",__FILE__,__LINE__);
		NFcouplerFree (coupler);
		return ((NFcoupler_p) NULL);
	}
	return (coupler);
}

/****************************************************************************************************************
 * Coupler free
*****************************************************************************************************************/
void NFcouplerFree (NFcoupler_p coupler) {
	free (coupler);
	return;
}

static CMreturn _NFcouplerMapping (NFcoupler_p coupler, size_t item) {

	return (CMsucceeded);
}
static CMreturn _NFcouplerBuildFlux (NFcoupler_p coupler) {
	size_t item;

	for (item = 0; item < coupler->DstDomain->Num; ++item) {
		if (_NFcouplerMapping (coupler, item) == CMfailed) return (CMfailed);
	}
	return (CMsucceeded);
}

static CMreturn _NFcouplerBuildSurface (NFcoupler_p coupler) {
	return (CMsucceeded);
}

static CMreturn _NFcouplerBuildPoint (NFcoupler_p coupler) {
	return (CMsucceeded);
}

CMreturn NFcouplerBuild (NFcoupler_p coupler) {

	if ((coupler->Mapping = (void *) calloc (coupler->DstDomain->Num, sizeof (NFmapping_t))) == (void *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	switch (coupler->Type) {
	case NFcouplerFlux:    return (_NFcouplerBuildFlux    (coupler));
	case NFcouplerSurface: return (_NFcouplerBuildSurface (coupler));
	case NFcouplerPoint:   return (_NFcouplerBuildPoint   (coupler));
	}
	CMmsgPrint (CMmsgAppError,"Invalid coupler type in %s:%d\n",__FILE__,__LINE__);
	return (CMfailed);
}
