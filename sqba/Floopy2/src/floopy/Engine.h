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

class CEngine : public IFloopySoundEngine
{
public:
	CEngine(char *plugin);
	virtual ~CEngine();

	char *GetName()			{ return m_plugin->GetName(); }
	char *GetDescription()	{ return m_plugin->GetDescription(); }
	char *GetVersion()		{ return m_plugin->GetVersion(); }
	char *GetAuthor()		{ return m_plugin->GetAuthor(); }

	IFloopySoundInput  *CreateInput(char *plugin)
	{
		return (NULL != m_plugin ? m_plugin->CreateInput(plugin) : NULL);
	}

	IFloopySoundOutput *CreateOutput(char *plugin, SOUNDFORMAT fmt)
	{
		return (NULL != m_plugin ? m_plugin->CreateOutput(plugin, fmt) : NULL);
	}

	IFloopySoundInput *GetSource() { return (m_plugin ? m_plugin->GetSource() : NULL); }

	bool Save(char *filename) { return (m_plugin ? m_plugin->Save(filename) : false); }

	void Close() { if(m_plugin) m_plugin->Close(); }

private:
	HMODULE m_hModule;
	IFloopySoundEngine *m_plugin;
};

#endif // !defined(AFX_ENGINE_H__2971634E_ED26_4A50_ABC1_123ADF851EA5__INCLUDED_)
