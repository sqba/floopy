// main.cpp : Defines the entry point for the DLL application.
//

#include "engine.h"
#include "globals.h"
#include "../platform.h"

/*
bool APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return true;
}
*/

#ifdef __cplusplus
extern "C" {
#endif
EXPORTED IFloopySoundEngine *CreateSoundEngine(LIB_HANDLE hModule)
{
	if(gOutputCache == NULL)
		gOutputCache = new COutputCache();
	gEngine = new CEngine(hModule, gOutputCache);
	return gEngine;
}
#ifdef __cplusplus
}
#endif
