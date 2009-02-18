#include <NFobjects.h>

#include <string.h>

/****************************************************************************************************************
 * Domain create
*****************************************************************************************************************/
NFdomain_p NFdomainCreate () {
	NFdomain_p domain;

	if ((domain = (NFdomain_p) malloc (sizeof (NFdomain_t))) == (NFdomain_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((NFdomain_p) NULL);
	}
	domain->Num = 0;
	domain->IO  = (NFio_p) NULL;
	return (domain);
}

/****************************************************************************************************************
 * Domain free
*****************************************************************************************************************/
void NFdomainFree (NFdomain_p domain) {
	free (domain);
	return;
}
