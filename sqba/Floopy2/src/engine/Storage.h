// Storage.h: interface for the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(STORAGE_H)
#define STORAGE_H

#include "../ifloopy.h"
#include "PluginLoader.h"

/*********************************************************************
 *! \class CStorage
 *  \brief Storage plugin wrapper.
 *  \author Filip Pavlovic
 *  \version 0.0
 *********************************************************************/
class CStorage : CPluginLoader
{
public:
	CStorage(IFloopySoundEngine *engine, char *plugin);
	virtual ~CStorage();

	bool Load(char *filename);
	bool Save(char *filename);

private:
	IFloopySoundEngine *m_engine;
};

#endif // !defined(STORAGE_H)
