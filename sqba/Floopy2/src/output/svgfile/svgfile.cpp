// svgfile.cpp : Defines the entry point for the DLL application.
//

#include "output.h"




/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
*/

#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) IFloopySoundOutput *CreateOutput(WAVFORMAT fmt)
{
	return new COutput(fmt);
}

#ifdef __cplusplus
}
#endif
