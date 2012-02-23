// libstd.cpp : Defines the entry point for the library.
//

#include <string.h>

#include "../ifloopy.h"
#include "../platform.h"


// Input classes
#include "dump/dump.h"
#include "xml/xml.h"
#include "cache/cache.h"
#include "echo/echo.h"
#include "loop/loop.h"
#include "mixer/mixer.h"
#include "mono2stereo/mono2stereo.h"
#include "playrgn/playregion.h"
#include "tonegen/tonegen.h"
#include "volume/volume.h"
#include "wavfile/wavfilein.h"
#include "track/track.h"
#include "reverse/reverse.h"
#include "invert/invert.h"
#include "performance/performance.h"
#include "converttofloat/converttofloat.h"
#include "float2int/float2int.h"

// Output classes
#include "wavfile/wavfileout.h"
#include "8to16bit/convert8to16bit.h"

#include "waveout/waveout.h"
#include "oss/oss.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORTED IFloopySoundInput *CreateInput(const char *name)
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
	if( 0 == stricmp(name, "reverse") )
		return new CReverse();
	if( 0 == stricmp(name, "invert") )
		return new CInvert();
	if( 0 == stricmp(name, "8to16bit") )
		return new CConvert8to16bit();
	if( 0 == stricmp(name, "performance") )
		return new CPerformance();
	if( 0 == stricmp(name, "tofloat") )
		return new CConvertToFloat();
	if( 0 == stricmp(name, "float2int") )
		return new CFloat2Int();
	//if( 0 == stricmp(name, "wavein") )
	//	return NULL;

	return NULL;
}

EXPORTED IFloopySoundOutput *CreateOutput(const char *name, SOUNDFORMAT fmt)
{
	if(!name)
		return NULL;

	if( 0 == stricmp(name, "waveout") )
#ifdef linux
		return new COSS(fmt);
#elif WIN32
		return new CWaveOut(fmt);
#else
		return NULL;
#endif

	if( 0 == stricmp(name, "wavfile") )
		return new CWavFileOut(fmt.frequency, fmt.bitsPerSample, fmt.channels);
	//if( stricmp(name, "svgfile") )
	//	return NULL;

	return NULL;
}

EXPORTED IFloopyEngineStorage *CreateStorage(const char *name)
{
	if( 0 == stricmp(name, "xml") )
		return new CXml();
	if( 0 == stricmp(name, "dump") )
		return new CDump();
	return NULL;
}

EXPORTED int GetPluginCount()
{
	return 12;
}

EXPORTED void GetPluginInfo(int index, const char *name, int *type)
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
		name = "playrgn";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 6:
		name = "tonegen";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	case 7:
		name = "volume";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 8:
		name = "wavfile";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	case 9:
		name = "track";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 10:
		name = "waveout";
		*type = TYPE_FLOOPY_SOUND_OUTPUT;
		break;
	case 11:
		name = "wavfile";
		*type = TYPE_FLOOPY_SOUND_OUTPUT;
		break;
	case 12:
		name = "reverse";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 13:
		name = "invert";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 14:
		name = "8to16bit";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 15:
		name = "performance";
		*type = TYPE_FLOOPY_SOUND_FILTER;
		break;
	case 16:
		name = "tofloat";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	case 17:
		name = "float2int";
		*type = TYPE_FLOOPY_SOUND_INPUT;
		break;
	case 18:
		name = "xml";
		*type = TYPE_FLOOPY_ENGINE_STORAGE;
		break;
	};
	return;
}


EXPORTED IFloopySoundFilter *CreateConvertor(SOUNDFORMAT fmtIn, SOUNDFORMAT fmtOut)
{
	return NULL;
}

#ifdef __cplusplus
}
#endif
