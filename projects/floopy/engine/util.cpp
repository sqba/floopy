
#include <string.h>
#include "../platform.h"

/**
 * Extracts the library name from full plugin name (library.plugin).
 * @param fullname pointer to buffer containing full plugin name.
 * @param name pointer to the buffer to receive the library name.
 * @return void.
 */
void get_library_name(const char *fullname, char *name)
{
	char *sep = strrchr(fullname, '.');
	int len = sep ? sep-fullname : strlen(fullname);

	int extlen = strlen(PLUG_EXT);
	if( len >= MAX_PATH-extlen-1 )
		len = MAX_PATH-extlen-1;

	strncpy(name, fullname, len);
//	strcat(name, PLUG_EXT);
}

/**
 * Extracts the plugin name from full plugin name (library.plugin).
 * @param fullname pointer to buffer containing full plugin name.
 * @param name pointer to the buffer to receive the plugin name.
 * @return void.
 */
void get_plugin_name(const char *fullname, char *name)
{
	const char *path = strrchr(fullname, PATH_SEP);
	if(path)
		path++;
	else
		path = fullname;

	char *sep = strrchr(path, '.');
	const char *tmp = sep ? sep+1 : path;

	if( strlen(tmp) <= MAX_FNAME )
		strcpy(name, tmp);
	else
		strncpy(name, tmp, MAX_FNAME);
}

