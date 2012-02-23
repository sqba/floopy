
//#include <string.h>

#include "../ifloopy.h"
#include "../platform.h"

#include "svg.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORTED IFloopySoundInput *CreateInput(const char *name)
{
	return NULL;
}

EXPORTED IFloopySoundOutput *CreateOutput(const char *name, SOUNDFORMAT fmt)
{
	if(!name)
		return NULL;

	if( 0 == stricmp(name, "svgout") )
		return new CSvgFileOut( fmt );

	return NULL;
}

EXPORTED int GetPluginCount()
{
	return 1;
}

EXPORTED void GetPluginInfo(int index, char *name, int *type)
{
	switch(index)
	{
	case 0:
		strcpy(name, "svgout");
		*type = TYPE_FLOOPY_SOUND_OUTPUT;
		break;
	}
	return;
}

#ifdef __cplusplus
}
#endif
