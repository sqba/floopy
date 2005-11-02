
#include <string.h>

#include "vstwrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) IFloopySoundInput *CreateInput(char *name)
{
	if(!name)
		return NULL;

	if( 0 == stricmp(name, "synth") )
		return new CVstWrapper();

	return NULL;
}

__declspec( dllexport ) IFloopySoundOutput *CreateOutput(char *name, SOUNDFORMAT fmt)
{
	return NULL;
}

__declspec( dllexport ) int GetPluginCount()
{
	return 1;
}

__declspec( dllexport ) void GetPluginInfo(int index, char *name, int *type)
{
	switch(index)
	{
	case 0:
		name = "synth";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	}
	return;
}

#ifdef __cplusplus
}
#endif
