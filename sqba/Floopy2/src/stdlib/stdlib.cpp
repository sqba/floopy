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
	if( 0 == stricmp(name, "wavfile") )
		return new CWavFileIn();
	if( 0 == stricmp(name, "track") )
		return new CTrack();
	//if( 0 == stricmp(name, "wavein") )
	//	return NULL;

	return NULL;
}

__declspec( dllexport ) IFloopySoundOutput *CreateOutput(char *name, SOUNDFORMAT fmt)
{
	if(!name)
		return NULL;

	if( 0 == stricmp(name, "waveout") )
		return new CWaveOut(fmt);
	if( 0 == stricmp(name, "wavfile") )
		return new CWavFileOut(fmt.frequency, fmt.bitsPerSample, fmt.channels);
	//if( stricmp(name, "svgfile") )
	//	return NULL;

	return NULL;
}

__declspec( dllexport ) int GetPluginCount()
{
	return 13;
}

__declspec( dllexport ) void GetPluginInfo(int index, char *name, int *type)
{
	switch(index)
	{
	case 0:
		name = "cache";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 1:
		name = "echo";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 2:
		name = "loop";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 3:
		name = "mixer";
		*type = TYPE_FLOOPY_SOUND_MIXER;
		break;
	case 4:
		name = "mono2stereo";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 5:
		name = "mp3file";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	case 6:
		name = "playrgn";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 7:
		name = "tonegen";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	case 8:
		name = "volume";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 9:
		name = "wavfile";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	case 10:
		name = "track";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 11:
		name = "waveout";
		*type = TYPE_FLOOPY_SOUND_OUTPUT;
		break;
	case 12:
		name = "wavfile";
		*type = TYPE_FLOOPY_SOUND_OUTPUT;
		break;
	};
	return;
}

#ifdef __cplusplus
}
#endif
