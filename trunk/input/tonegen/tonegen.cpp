// tonegen.cpp : Defines the entry point for the DLL application.
//

#include "input.h"


/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

*/

#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) IFloopySoundInput *CreateInput(char *name)
{
	return new CInput();
}
#ifdef __cplusplus
}
#endif
