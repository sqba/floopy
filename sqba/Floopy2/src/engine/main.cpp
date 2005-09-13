// engine.cpp : Defines the entry point for the DLL application.
//

#include "engine.h"
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
	return new CEngine(hModule);
}
#ifdef __cplusplus
}
#endif
