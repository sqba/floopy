
#include "../ifloopy.h"
#include "../platform.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
EXPORTED bool Load(IFloopySoundEngine *engine, char *filename)
{
	printf("Load(%s)\n", filename);
	return false;
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
