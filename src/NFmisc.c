#include <NF.h>

const char *NFvarTypeString (NFvarType varType) {
	switch (varType) {
	default: return ("void");
	case NFvarTypeByte:   return ("Byte");
	case NFvarTypeShort:  return ("Short");
	case NFvarTypeFloat:  return ("Float");
	case NFvarTypeDouble: return ("Double");
	}
	return ((char *) NULL);
}
