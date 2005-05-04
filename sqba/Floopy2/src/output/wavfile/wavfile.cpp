// wavfile.cpp : Defines the entry point for the DLL application.
//

#include "input.h"
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
__declspec( dllexport ) IFloopySoundInput *CreateInput()
{
	return new CInput();
}

__declspec( dllexport ) IFloopySoundOutput *CreateOutput(int nSamplesPerSec, int wBitsPerSample, int nChannels)
{
	return new COutput(nSamplesPerSec, wBitsPerSample, nChannels);
}

#ifdef __cplusplus
}
#endif
