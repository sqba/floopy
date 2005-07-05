// playrgn.cpp : Defines the entry point for the DLL application.
//

#include <string.h>

#include "../ifloopy.h"


// Input classes
#include "cache/cache.h"
#include "echo/echo.h"
#include "loop/loop.h"
#include "mixer/mixer.h"
#include "mono2stereo/mono2stereo.h"
#include "mp3file/mp3file.h"
#include "playrgn/playregion.h"
#include "tonegen/tonegen.h"
#include "volume/volume.h"
#include "wavfile/wavfilein.h"
#include "track/track.h"

// Output classes
#include "wavfile/wavfileout.h"
#include "waveout/waveout.h"


#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) IFloopySoundInput *CreateInput(char *name)
{
	if(!name)
		return NULL;

	if( 0 == stricmp(name, "cache") )
		return new CCache();
	if( 0 == stricmp(name, "echo") )
		return new CEcho();
	if( 0 == stricmp(name, "loop") )
		return new CLoop();
	if( 0 == stricmp(name, "mixer") )
		return new CMixer();
	if( 0 == stricmp(name, "mono2stereo") )
		return new CMono2Stereo();
	if( 0 == stricmp(name, "mp3file") )
		return new CMp3File();
	if( 0 == stricmp(name, "playrgn") )
		return new CPlayRegion();
	if( 0 == stricmp(name, "tonegen") )
		return new CToneGen();
	if( 0 == stricmp(name, "volume") )
		return new CVolume();
	//if( 0 == stricmp(name, "wavein") )
	//	return NULL;
	if( 0 == stricmp(name, "wavfile") )
		return new CWavFileIn();
	if( 0 == stricmp(name, "track") )
		return new CTrack();

	return NULL;
}

__declspec( dllexport ) IFloopySoundOutput *CreateOutput(char *name, SOUNDFORMAT fmt)
{
	if(!name)
		return NULL;

	//if( stricmp(name, "svgfile") )
	//	return NULL;
	if( 0 == stricmp(name, "waveout") )
		return new CWaveOut(fmt);
	if( 0 == stricmp(name, "wavfile") )
		return new CWavFileOut(fmt.frequency, fmt.bitsPerSample, fmt.channels);

	return NULL;
}
#ifdef __cplusplus
}
#endif
