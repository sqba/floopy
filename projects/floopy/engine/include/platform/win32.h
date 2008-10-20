// Dynamic library stuff
#define PLUG_PREFIX					""
#define PLUG_EXT					".dll"
#define LIB_HANDLE					HMODULE
#define PLUGIN_OPEN(filename)		LoadLibraryA(filename)
#define PLUGIN_LOAD(handle, func)	GetProcAddress(handle, func)
#define PLUGIN_CLOSE(handle)		FreeLibrary(handle)
#define EXPORTED					__declspec( dllexport )

// File system
#define PATH_SEP	'\\'

// Default PCM output
#define PCM_OUT	"libstd.waveout"
