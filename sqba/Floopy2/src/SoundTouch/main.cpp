// SoundTouch.cpp : Defines the entry point for the DLL application.
//

#include <string.h>

#include "../ifloopy.h"

#include "SoundTouch.h"


#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) IFloopySoundInput *CreateInput(char *name)
{
	return new CSoundTouch();
}

__declspec( dllexport ) int GetPluginCount()
{
	return 17;
}

__declspec( dllexport ) void GetPluginInfo(int index, char *name, int *type)
{
	name = "SoundTouch";
	*type = TYPE_FLOOPY_SOUND_FILTER;
}

#ifdef __cplusplus
}
#endif
