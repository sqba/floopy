// engine.cpp : Defines the entry point for the DLL application.
//

#include "engine.h"
#include "globals.h"

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
__declspec( dllexport ) IFloopySoundEngine *CreateSoundEngine(HMODULE hModule)
{
	if(gOutputCache == NULL)
		gOutputCache = new COutputCache();
	gEngine = new CEngine(hModule, gOutputCache);
	return gEngine;
}
#ifdef __cplusplus
}
#endif
