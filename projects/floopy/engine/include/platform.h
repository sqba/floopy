
//#include <string.h>

// Strings
#ifndef WIN32
	#define strcmpi						strcasecmp
	#define stricmp						strcasecmp
	#define strnicmp					strncasecmp
#endif

// File system
#ifdef WIN32
	#define PATH_SEP	'\\'
#else
	#include <limits.h>					// PATH_MAX, NAME_MAX
	#define __USE_XOPEN2K				// readlink
	#include <unistd.h>					// readlink
	#define MAX_PATH	PATH_MAX
	#define MAX_FNAME	NAME_MAX
	#define PATH_SEP	'/'
#endif

// Dynamic library stuff
#ifdef WIN32
	#define PLUG_PREFIX					""
	#define PLUG_EXT					".dll"
	#define LIB_HANDLE					HMODULE
	#define PLUGIN_OPEN(filename)		LoadLibraryA(filename)
	#define PLUGIN_LOAD(handle, func)	GetProcAddress(handle, func)
	#define PLUGIN_CLOSE(handle)		FreeLibrary(handle)
	#define EXPORTED					__declspec( dllexport )
#else
	// Add linker options: -rdynamic -ldl
	#define __USE_GNU					// dladdr, DL_info
	#include <dlfcn.h>
	#define PLUG_PREFIX					"lib"
	#define PLUG_EXT					".so"
	#define LIB_HANDLE					void*
	#define PLUGIN_OPEN(filename)		dlopen( filename, RTLD_LAZY )
	#define PLUGIN_LOAD(handle, func)	dlsym( handle, func )
	#define PLUGIN_CLOSE(handle)		dlclose( handle )
	#define EXPORTED					extern
#endif

#ifdef WIN32
#define PCM_OUT	"libstd.waveout"
#else
#define PCM_OUT	"libstd.oss"
#endif

// Misc
#ifndef WIN32
  #define NULL		0
#endif
