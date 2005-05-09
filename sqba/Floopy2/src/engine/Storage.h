// Storage.h: interface for the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGE_H__D2BE1D70_0029_47BA_9E1D_599A381DB224__INCLUDED_)
#define AFX_STORAGE_H__D2BE1D70_0029_47BA_9E1D_599A381DB224__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "../ifloopy.h"

class CStorage  
{
public:
	CStorage(IFloopyEngine *engine, char *plugin);
	virtual ~CStorage();

	BOOL Load(char *filename);
	BOOL Save(char *filename);

private:
	HINSTANCE m_hinst;
	IFloopyEngine *m_plugin;
	IFloopyEngine *m_engine;
};

#endif // !defined(AFX_STORAGE_H__D2BE1D70_0029_47BA_9E1D_599A381DB224__INCLUDED_)
