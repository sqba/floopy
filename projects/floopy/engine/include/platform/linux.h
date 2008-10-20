// Strings
#define strcmpi						strcasecmp
#define stricmp						strcasecmp
#define strnicmp					strncasecmp

// Dynamic library stuff
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

// File system
#include <limits.h>					// PATH_MAX, NAME_MAX
#define __USE_XOPEN2K				// readlink
#include <unistd.h>					// readlink
#define MAX_PATH	PATH_MAX
#define MAX_FNAME	NAME_MAX
#define PATH_SEP	'/'

// Default PCM output
#define PCM_OUT	"libstd.oss"

// Misc
#define NULL		0
