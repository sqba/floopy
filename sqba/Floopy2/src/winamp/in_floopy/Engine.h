// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINE_H__2971634E_ED26_4A50_ABC1_123ADF851EA5__INCLUDED_)
#define AFX_ENGINE_H__2971634E_ED26_4A50_ABC1_123ADF851EA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ifloopy.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

class CEngine : public IFloopyEngine
{
public:
	CEngine();
	virtual ~CEngine();

	char *GetName()			{ return m_plugin->GetName(); }
	char *GetDescription()	{ return m_plugin->GetDescription(); }
	char *GetVersion()		{ return m_plugin->GetVersion(); }
	char *GetAuthor()		{ return m_plugin->GetAuthor(); }

	BOOL Create (char *plugin);

	IFloopySoundInput  *CreateInput(char *plugin)
	{
		return (NULL != m_plugin ? m_plugin->CreateInput(plugin) : NULL);
	}

	IFloopySoundOutput *CreateOutput(char *plugin, SOUNDFORMAT fmt)
	{
		return (NULL != m_plugin ? m_plugin->CreateOutput(plugin, fmt) : NULL);
	}

	IFloopySoundInput *GetSource() { return (m_plugin ? m_plugin->GetSource() : NULL); }

	BOOL Save(char *filename) { return (m_plugin ? m_plugin->Save(filename) : FALSE); }

	void Close() { if(m_plugin) m_plugin->Close(); }

	char *GetDisplayName() { return (m_plugin ? m_plugin->GetDisplayName() : NULL); }
	void SetDisplayName(char *name, int len) { m_plugin->SetDisplayName(name, len); }

	void init();

	char *GetLastErrorDesc() { return m_szLastError; }

private:
	HMODULE m_hModule;
	IFloopyEngine *m_plugin;
	char m_path[MAX_PATH];
	char m_szLastError[100];
};

#endif // !defined(AFX_ENGINE_H__2971634E_ED26_4A50_ABC1_123ADF851EA5__INCLUDED_)
