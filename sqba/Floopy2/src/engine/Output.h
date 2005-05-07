// Output.h: interface for the COutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__8964D44D_9FFA_4F21_BA21_37E1D084CDD4__INCLUDED_)
#define AFX_OUTPUT_H__8964D44D_9FFA_4F21_BA21_37E1D084CDD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "../ifloopy.h"
#include <stdio.h>
#include "timeline.h"

/**
 * \class COutput
 * \brief Output plug-in wrapper class.
 * \author Filip Pavlovic
 * \version
 * \date 02. May 2005.
 *
 * Output plug-in wrapper class.
 */
class COutput : public IFloopySoundOutput
{
public:
	COutput(char *plugin, WAVFORMAT *fmt);
	virtual ~COutput();

	char *GetName()			{ return "outputw"; }
	char *GetDescription()	{ return "Floopy Output Plugin Wrapper"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }
/*
	char *GetName()			{ return (m_plugin ? m_plugin->GetName() : "outputwrapper"); }
	char *GetDescription()	{ return (m_plugin ? m_plugin->GetDescription() : "Floopy Output Plugin Wrapper"); }
	char *GetVersion()		{ return (m_plugin ? m_plugin->GetVersion() : "0.1"); }
	char *GetAuthor()		{ return (m_plugin ? m_plugin->GetAuthor() : "sqba"); }
/*
	BOOL Open(char *filename);
	UINT Write(BYTE *data, UINT size);
	void Close();
*/
//	IFloopySoundOutput *getPlugin() { return m_plugin; }
//	IFloopySoundOutput *getPlugin() { return GetDest(); }

	int   GetParamCount()					{ return m_plugin->GetParamCount(); }
	void  SetParam(int index, float value);//	{ m_plugin->SetParam(index, value); }
	float GetParam(int index);//				{ return m_plugin->GetParam(index); }
	char *GetParamName(int index)			{ return m_plugin->GetParamName(index); }
	char *GetParamDesc(int index)			{ return m_plugin->GetParamDesc(index); }

	UINT Write(BYTE *data, UINT size);

	void SetDest(IFloopySoundOutput *dst)
	{
		m_plugin->SetDest(dst);
	}

private:
	UINT m_offset;
	HINSTANCE m_hinst;
	IFloopySoundOutput *m_plugin;
	CTimeline m_timeline;
};

#endif // !defined(AFX_OUTPUT_H__8964D44D_9FFA_4F21_BA21_37E1D084CDD4__INCLUDED_)
