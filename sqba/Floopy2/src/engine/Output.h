// Output.h: interface for the COutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__8964D44D_9FFA_4F21_BA21_37E1D084CDD4__INCLUDED_)
#define AFX_OUTPUT_H__8964D44D_9FFA_4F21_BA21_37E1D084CDD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ifloopy.h"
#include <stdio.h>
#include "timeline.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

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
	COutput();
	virtual ~COutput();

	bool Create(char *plugin, SOUNDFORMAT fmt);

	char *GetName()			{ return m_plugin->GetName(); }
	char *GetDescription()	{ return m_plugin->GetDescription(); }
	char *GetVersion()		{ return m_plugin->GetVersion(); }
	char *GetAuthor()		{ return m_plugin->GetAuthor(); }

	int   GetParamCount()					{ return m_plugin->GetParamCount(); }
	bool  GetParamVal(int index, float *value);
	void  SetParamVal(int index, float value);//	{ m_plugin->SetParam(index, value); }
	char *GetParamName(int index)			{ return m_plugin->GetParamName(index); }
	char *GetParamDesc(int index)			{ return m_plugin->GetParamDesc(index); }

	int Write(BYTE *data, int size);

	void SetDest(IFloopySoundOutput *dst)
	{
		m_plugin->SetDest(dst);
	}

	char *GetLastErrorDesc() { return m_szLastError; }

	int GetWrittenSamples();
	void Reset();

private:
	char m_name[50];
	char m_szLastError[100];

	int m_samplesToBytes;
	int m_offset;
	HINSTANCE m_hinst;
	IFloopySoundOutput *m_plugin;
	CTimeline m_timeline;
};

#endif // !defined(AFX_OUTPUT_H__8964D44D_9FFA_4F21_BA21_37E1D084CDD4__INCLUDED_)
