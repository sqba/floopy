// Dynamic library stuff
#define PLUG_PREFIX					""
#define PLUG_EXT					".dll"
#define LIB_HANDLE					HMODULE
#define PLUGIN_OPEN(p)				LoadLibraryA(p)
#define PLUGIN_LOAD(h, p)			GetProcAddress(h, f)
#define PLUGIN_CLOSE(h)				FreeLibrary(h)
#define EXPORTED					__declspec( dllexport )

// File system
#define PATH_SEP	'\\'

// Default PCM output
#define PCM_OUT	"libstd.waveout"
