// SoundTouch.cpp : Defines the entry point for the DLL application.
//

#include <string.h>

#include "../ifloopy.h"

#include "Wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) IFloopySoundInput *CreateInput(char *name)
{
	if(!name)
		return NULL;

	if( 0 == stricmp(name, "wrapper") )
		return new CWrapper();

	return NULL;
}

__declspec( dllexport ) int GetPluginCount()
{
	return 17;
}

__declspec( dllexport ) void GetPluginInfo(int index, char *name, int *type)
{
	switch(index)
	{
	case 0:
		name = "wrapper";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	}
	return;
}

#ifdef __cplusplus
}
#endif
