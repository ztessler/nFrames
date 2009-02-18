#include <NFobjects.h>

#include <string.h>

/****************************************************************************************************************
 * Coupler create
*****************************************************************************************************************/
NFcoupler_p NFcouplerGet (NFdomain_p srcDomain, NFdomain_p dstDomain) {
	NFcoupler_p coupler;

	if ((coupler = (NFcoupler_p) malloc (sizeof (NFcoupler_t))) == (NFcoupler_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((NFcoupler_p) NULL);
	}
	coupler->SrcDomain = srcDomain;
	coupler->DstDomain = dstDomain;
	return (coupler);
}

/****************************************************************************************************************
 * Coupler free
*****************************************************************************************************************/
void NFcouplerFree (NFcoupler_p coupler) {
	free (coupler);
	return;
}
