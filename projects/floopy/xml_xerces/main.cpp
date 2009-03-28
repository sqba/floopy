
#include "../ifloopy.h"
#include "../platform.h"
#include "storage.h"

#ifdef __cplusplus
extern "C" {
#endif
EXPORTED bool Load(IFloopySoundEngine *engine, char *filename)
{
	CStorage *pStorage;
	pStorage = new CStorage( engine );
	bool bResult = pStorage->Load( filename );
	return bResult;
}

EXPORTED bool Save(IFloopySoundEngine *engine, char *filename)
{
	return false;
}

EXPORTED char *GetExtension()
{
	return "XML";
}
#ifdef __cplusplus
}
#endif
