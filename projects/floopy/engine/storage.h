// Storage.h: interface for the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(STORAGE_H)
#define STORAGE_H

#include "include/ifloopy.h"
#include "pluginloader.h"

/*********************************************************************
 *! \class CStorage
 *  \brief Storage plugin wrapper.
 *  \author Filip Pavlovic
 *  \version 0.0
 *********************************************************************/
class CStorage : CPluginLoader
{
public:
	CStorage(IFloopySoundEngine *engine, const char *plugin);
	virtual ~CStorage();

	bool Load(const char *filename);
	bool Save(const char *filename);

private:
	IFloopySoundEngine *m_engine;
};

#endif // !defined(STORAGE_H)
