// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
#define AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "../ifloopy.h"
#include "timeline.h"

/**
 * \class CInput
 * \brief Input plug-in wrapper class.
 * \author Filip Pavlovic
 * \version
 * \date 02. May 2005.
 *
 * Input plug-in wrapper class.
 * Memorizes all parameter changes and the offsets
 * at which they occured. These parameters are then
 * set during execution.
 */
class CInput : public IFloopySoundInput
{
public:
	CInput(char *plugin);
	virtual ~CInput();

	char *GetName()			{ return "inputw"; }
	char *GetDescription()	{ return "Floopy Input Plugin Wrapper"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }
/*
	char *GetName()			{ return (m_plugin ? m_plugin->GetName() : "inputwrapper"); }
	char *GetDescription()	{ return (m_plugin ? m_plugin->GetDescription() : "Floopy Input Plugin Wrapper"); }
	char *GetVersion()		{ return (m_plugin ? m_plugin->GetVersion() : "0.1"); }
	char *GetAuthor()		{ return (m_plugin ? m_plugin->GetAuthor() : "sqba"); }
*/
	void MoveTo(UINT samples);
	void Reset();
	UINT Read(BYTE *data, UINT size);

	void Enable(BOOL bEnable);
	BOOL IsEnabled();

	UINT GetSize();
/*
	BOOL Open(char *filename);
	DWORD GetSize();
	UINT Read(BYTE *data, UINT size, UINT offset);
	void Close();
*/
//	IFloopySoundInput *getPlugin() { return m_plugin; }
//	IFloopySoundInput *getPlugin() { return GetSource(); }
/*
	WAVFORMAT *GetFormat()
	{
		return m_plugin->GetFormat();
	}
*/
	int   GetParamCount()					{ return m_plugin->GetParamCount(); }
	void  SetParam(int index, float value);//	{ m_plugin->SetParam(index, value); }
	float GetParam(int index);//				{ return m_plugin->GetParam(index); }
	char *GetParamName(int index)			{ return m_plugin->GetParamName(index); }
	char *GetParamDesc(int index)			{ return m_plugin->GetParamDesc(index); }

	BOOL SetSource(IFloopySoundInput *src)
	{
		return m_plugin->SetSource(src);
	}

	IFloopySoundInput *GetSource()
	{
		return m_plugin->GetSource();
	}

private:
	void applyParamsAt(UINT offset);

	void initLog(char *plugin);
	void logParamChange(UINT offset, tParam *param);
	void logParamSet(UINT offset, int index, float value);

	UINT m_offset;
	HINSTANCE m_hinst;
	IFloopySoundInput *m_plugin;
	CTimeline m_timeline;
	FILE *m_fplog;
};

#endif // !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
