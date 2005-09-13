// Storage.h: interface for the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGE_H__D2BE1D70_0029_47BA_9E1D_599A381DB224__INCLUDED_)
#define AFX_STORAGE_H__D2BE1D70_0029_47BA_9E1D_599A381DB224__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ifloopy.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

class CStorage  
{
public:
	CStorage(IFloopySoundEngine *engine, char *plugin);
	virtual ~CStorage();

	bool Load(char *filename);
	bool Save(char *filename);

private:
	HINSTANCE m_hinst;
	IFloopySoundEngine *m_engine;
};

#endif // !defined(AFX_STORAGE_H__D2BE1D70_0029_47BA_9E1D_599A381DB224__INCLUDED_)
