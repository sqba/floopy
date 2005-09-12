// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
#define AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "../ifloopy.h"
#include "timeline.h"


//#define _DEBUG_TIMER_


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
class CInput : public IFloopySoundMixer
{
public:
	CInput(UpdateCallback func);
	virtual ~CInput();

	BOOL Create(char *plugin);
	BOOL Create(IFloopySoundEngine *src);

	char *GetName()			{ return m_plugin->GetName(); }
	char *GetDescription()	{ return m_plugin->GetDescription(); }
	char *GetVersion()		{ return m_plugin->GetVersion(); }
	char *GetAuthor()		{ return m_plugin->GetAuthor(); }

	int   GetParamCount()			{ return m_plugin->GetParamCount(); }
	BOOL  GetParamVal(int index, float *value);
	void  SetParamVal(int index, float value);
	char *GetParamName(int index)	{ return m_plugin->GetParamName(index); }
	char *GetParamDesc(int index)	{ return m_plugin->GetParamDesc(index); }
	BOOL  GetParamIndex(char *name, int *index);
	float GetParamMax(int index)	{ return m_plugin->GetParamMax(index); }
	float GetParamMin(int index)	{ return m_plugin->GetParamMin(index); }
	char *GetParamUnit(int index)	{ return m_plugin->GetParamUnit(index); }
	float GetParamStep(int index)	{ return m_plugin->GetParamStep(index); }

	int   GetPropertyCount()						{ return m_plugin->GetPropertyCount(); }
	BOOL  GetPropertyVal(int index, float *value)	{ return m_plugin->GetPropertyVal(index, value); }
	void  SetPropertyVal(int index, float value)	{ m_plugin->SetPropertyVal(index, value); }
	char *GetPropertyName(int index)				{ return m_plugin->GetPropertyName(index); }
	char *GetPropertyDesc(int index)				{ return m_plugin->GetPropertyDesc(index); }
	BOOL  GetPropertyIndex(char *name, int *index);//	{ return m_plugin->GetPropertyIndex(name, index); }
	float GetPropertyMax(int index)					{ return m_plugin->GetPropertyMax(index); }
	float GetPropertyMin(int index)					{ return m_plugin->GetPropertyMin(index); }
	char *GetPropertyUnit(int index)				{ return m_plugin->GetPropertyUnit(index); }
	float GetPropertyStep(int index)				{ return m_plugin->GetPropertyStep(index); }

	BOOL GetColor(UINT *r, UINT *g, UINT *b);
	void SetColor(UINT r, UINT g, UINT b);

	void Enable(BOOL bEnable);
	BOOL IsEnabled();

	BOOL Open(char *filename);

	void MoveTo(int samples);
	void Reset();
	int  Read(BYTE *data, int size);

	int GetSize();
	int GetSourceSize();

	BOOL SetSource(IFloopySoundInput *src);
	IFloopySoundInput *GetSource();

	int GetNextOffset(int offset);
	int GetPrevOffset(int offset);

	BOOL GetParamAt(int offset, int index, float *value);
	void SetParamAt(int offset, int index, float value);
	BOOL ResetParamAt(int offset, int index, float value);
	void EnableAt(int offset, BOOL bEnable);

	void Close();

//	int GetLastError();
	BOOL GetLastError(char *str, int len);

	char *GetDisplayName() { return m_szDisplayName; }
	void SetDisplayName(char *name, int len)
	{
		memset(m_szDisplayName, 0, 50);
		memcpy(m_szDisplayName, name, (len<50?len:50));
	}

	char *GetPath() { return isEngine() ? m_source->GetPath() : m_szObjPath; }

	enumClassType GetType() { return (m_plugin ? m_plugin->GetType() : TYPE_FLOOPY); }

	BOOL MoveParam(int offset, int index, float value, int newoffset);

	void MoveAllParamsBetween(int start, int end, int offset);

	///////////////////////////////////////////
	// IFloopySoundMixer functions
	///////////////////////////////////////////
	IFloopySoundInput *GetSource(int index);
	int AddSource(IFloopySoundInput *src);
	bool RemoveSource(IFloopySoundInput *src);
	int GetInputCount();
	///////////////////////////////////////////

	void ClearAllParams();

	BOOL GetBypass();
	void SetBypass(BOOL bBypass);

private:
	inline BOOL isEngine() { return (m_source ? m_source->GetType() == TYPE_FLOOPY_SOUND_ENGINE : FALSE); }
//	inline BOOL isTrack() { return (m_plugin ? m_plugin->GetType() == TYPE_FLOOPY_SOUND_TRACK : FALSE); }
	void applyParamsAt(int offset);
	BOOL applyPreviousParams(int offset);
	int getSamplesToBytes();
	int getStartOffset();
	int getEndOffset();
	void recalcVariables();
	void recalcSourceVariables();
	inline IFloopySoundInput *getSource();

	//void _applyParamsUntil(int endoffset);
//	SOUNDFORMAT *GetFormat();


protected:
	int m_iCheck;	/** Some random number, used to check if the
						component was created by the same engine */

private:
	char m_szDisplayName[50];
	char m_szLastError[100];
	char m_szObjPath[MAX_PATH];

	UINT m_red, m_green, m_blue;

	UpdateCallback m_callback;

	int m_offset;
	HINSTANCE m_hinst;
	IFloopySoundInput *m_plugin;
	CTimeline m_timeline;

	/// Optimization variables
	int m_nStartOffset, m_nEndOffset, m_nSamplesToBytes;
};

#endif // !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
