
#ifndef _XML_STORAGE_H_
#define _XML_STORAGE_H_


#include "../ifloopy.h"
#include "../platform.h"

class CStorage
{
public:
	CStorage(IFloopySoundEngine *engine);
	virtual ~CStorage();

	bool Load(char *filename);
	bool Save(char *filename);
	void Close();

private:
	IFloopySoundEngine *m_pEngine;
};

#endif // _XML_STORAGE_H_
