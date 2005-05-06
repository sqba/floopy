// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINE_H__2971634E_ED26_4A50_ABC1_123ADF851EA5__INCLUDED_)
#define AFX_ENGINE_H__2971634E_ED26_4A50_ABC1_123ADF851EA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "../ifloopy.h"

class CEngine : public IFloopyEngine
{
public:
	CEngine(char *plugin);
	virtual ~CEngine();
/*
	char *GetName()			{ return (m_plugin ? m_plugin->GetName() : "enginewrapper"); }
	char *GetDescription()	{ return (m_plugin ? m_plugin->GetDescription() : "Floopy Engine Plugin Wrapper"); }
	char *GetVersion()		{ return (m_plugin ? m_plugin->GetVersion() : "0.1"); }
	char *GetAuthor()		{ return (m_plugin ? m_plugin->GetAuthor() : "sqba"); }
*/
//	IFloopyEngine *getPlugin() { return m_plugin; }

	IFloopySoundInput  *CreateInput(char *plugin)
	{
		return (NULL != m_plugin ? m_plugin->CreateInput(plugin) : NULL);
	}

	IFloopySoundOutput *CreateOutput(char *plugin, WAVFORMAT *fmt)
	{
		return (NULL != m_plugin ? m_plugin->CreateOutput(plugin, fmt) : NULL);
	}

	IFloopySoundInput *GetSource() { return (m_plugin ? m_plugin->GetSource() : NULL); }

private:
	HINSTANCE m_hinst;
	IFloopyEngine *m_plugin;
};

#endif // !defined(AFX_ENGINE_H__2971634E_ED26_4A50_ABC1_123ADF851EA5__INCLUDED_)
