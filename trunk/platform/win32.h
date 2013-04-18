// Dynamic library stuff
#define PLUG_PREFIX					""
#define PLUG_EXT					".dll"
#define LIB_HANDLE					HMODULE
#define PLUGIN_OPEN(p)				LoadLibraryA(p)
#define PLUGIN_LOAD(h, p)			GetProcAddress(h, p)
#define PLUGIN_CLOSE(h)				FreeLibrary(h)
#define EXPORTED					__declspec( dllexport )

// File system
//#define MAX_PATH	                _MAX_PATH
#define MAX_FNAME	                _MAX_FNAME
#define PATH_SEP	'\\'

// Default PCM output
#define PCM_OUT	"libstd.waveout"
