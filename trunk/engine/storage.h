// Storage.h: interface for the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(STORAGE_H)
#define STORAGE_H

#include "../ifloopy.h"
#include "../common/loader.h"

/*********************************************************************
 *! \class CStorage
 *  \brief Storage plugin wrapper.
 *  \author Filip Pavlovic
 *  \version 0.0
 *********************************************************************/
class CStorage : CLoader
{
public:
	CStorage(LIB_HANDLE hModule, IFloopySoundEngine *engine, const char *name);
	virtual ~CStorage();

	bool Load(const char *filename);
	bool Save(const char *filename);

private:
	IFloopySoundEngine *m_engine;
	IFloopyEngineStorage *m_plugin;
};

#endif // !defined(STORAGE_H)
