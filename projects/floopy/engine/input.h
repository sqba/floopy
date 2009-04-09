// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
#define AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../platform.h"
#include "../ifloopy.h"
#include "timeline.h"
#include "../common/loader.h"
#include "outputcache.h"

/**
 * \class CInput
 * \brief Input plug-in wrapper class.
 * \author Filip Pavlovic
 * \version
 * \date 02. May 2005.
 *
 * Input plug-in wrapper class.
 * Memorizes all parameter changes and the offsets at which they occured.
 * These parameters are then set during execution.
 */
class CInput : public IFloopySoundMixer, CLoader
{
public:
	CInput(LIB_HANDLE hModule, UpdateCallback func, COutputCache *outputCache);
	virtual ~CInput();

	bool Create(const char *plugin);
	bool Create(IFloopySoundEngine *src);

	const char *GetName()							{ return m_plugin->GetName(); }
	const char *GetDescription()					{ return m_plugin->GetDescription(); }
	const char *GetVersion()						{ return m_plugin->GetVersion(); }
	const char *GetAuthor()							{ return m_plugin->GetAuthor(); }

	int   GetParamCount()							{ return m_plugin->GetParamCount(); }
	bool  GetParamVal(int index, float *value);
	void  SetParamVal(int index, float value);
	const char *GetParamName(int index);
	const char *GetParamDesc(int index);
	bool  GetParamIndex(const char *name, int *index);
	float GetParamMax(int index)					{ return m_plugin->GetParamMax(index); }
	float GetParamMin(int index)					{ return m_plugin->GetParamMin(index); }
	const char *GetParamUnit(int index)				{ return m_plugin->GetParamUnit(index); }
	float GetParamStep(int index)					{ return m_plugin->GetParamStep(index); }

	int   GetPropertyCount()						{ return m_plugin->GetPropertyCount(); }
	bool  GetPropertyVal(int index, float *value)	{ return m_plugin->GetPropertyVal(index, value); }
	void  SetPropertyVal(int index, float value)	{ m_plugin->SetPropertyVal(index, value); }
	const char *GetPropertyName(int index)			{ return m_plugin->GetPropertyName(index); }
	const char *GetPropertyDesc(int index)			{ return m_plugin->GetPropertyDesc(index); }
	bool  GetPropertyIndex(const char *name, int *index);//	{ return m_plugin->GetPropertyIndex(name, index); }
	float GetPropertyMax(int index)					{ return m_plugin->GetPropertyMax(index); }
	float GetPropertyMin(int index)					{ return m_plugin->GetPropertyMin(index); }
	const char *GetPropertyUnit(int index)			{ return m_plugin->GetPropertyUnit(index); }
	float GetPropertyStep(int index)				{ return m_plugin->GetPropertyStep(index); }

	int GetType()									{ return (m_plugin ? m_plugin->GetType() : TYPE_FLOOPY); }

	const char *GetPath()							{ return isEngine() ? m_source->GetPath() : m_szObjPath; }

	const char *GetDisplayName()					{ return m_szDisplayName; }
	void SetDisplayName(const char*, int);

	int GetPosition()								{ return m_offset / m_nSamplesToBytes; }

	void SetEngine(IFloopySoundEngine *pEngine)		{ m_plugin->SetEngine(pEngine); }

	bool IsEOF()									{ return m_plugin->IsEOF(); }

	bool GetColor(UINT *r, UINT *g, UINT *b);
	void SetColor(UINT r, UINT g, UINT b);

	void Enable(bool bEnable);
	bool IsEnabled();

	bool Open(const char*);
	void Close();

	void MoveTo(int samples);
	void Reset();
	int  Read(BYTE *data, int size);

	int GetSize();
	int GetSourceSize();

	bool SetSource(IFloopySoundInput *src);
	IFloopySoundInput *GetSource();

	int GetNextOffset(int);
	int GetPrevOffset(int);

	int GetNextOffset(int, int);
	int GetPrevOffset(int, int);

	bool GetParamAt(int, int, float*);
	bool SetParamAt(int, int, float);
	bool ResetParamAt(int, int, float);
	bool EnableAt(int, bool);
	bool MoveParam(int, int, float, int);
	bool MoveAllParamsBetween(int, int, int);


	bool GetLastError(char*, int);
	const char *GetLastErrorDesc() { return NULL; }

	const char *GetSignature();



	///////////////////////////////////////////
	// IFloopySoundMixer functions
	///////////////////////////////////////////
	IFloopySoundInput *GetSource(int index);
	int AddSource(IFloopySoundInput *src);
	bool RemoveSource(IFloopySoundInput *src);
	int GetInputCount();
	///////////////////////////////////////////

	bool ClearAllParams();

	bool GetBypass();
	void SetBypass(bool);

	bool CanReadSourceIfDisabled();

private:
	inline IFloopySoundInput *getSource();
	bool isEngine();
	bool isFilter();
	bool isEndOfTrack();
	bool isMixer();

	void	applyParamsAt(int);
	int		applyPreviousParams(int);
	int		getSamplesToBytes();
	int		getStartOffset();
	int		getEndOffset();
	void	recalcVariables();
	void	recalcSourceVariables();
	int		skipChunk(int);

	void	loadDefaultParams();
	int		readFromCache(BYTE *data, int size);
	void	createSignature();

private:
	char m_szDisplayName[50];
	char m_szLastError[100];
	char m_szObjPath[MAX_PATH];

	float *m_pDefaultParams;
	int m_iParamCount;

	UINT m_red, m_green, m_blue;

	UpdateCallback		m_callback;	/** Pointer to the function called after every
										parameter change during reading */

	int					m_offset;	/** Current position, in bytes */
	IFloopySoundInput*	m_plugin;	/** Pointer to the plugin component */
	CTimeline			m_timeline;	/** Array of parameter changes and their offsets */

	bool m_bFadeEdges;				/** Fade edges when enabling/disabling plugin
										to prevent clicks */

	/// Optimization variables
	int m_nStartOffset;				/** Offset, in bytes, at which the track begins */
	int m_nEndOffset;				/** Offset, in bytes, at which the track ends */
	int m_nSamplesToBytes;			/** For converting samples to bytes (samples*m_nSamplesToBytes)
										and vice versa (bytes/m_nSamplesToBytes) */

	int m_nSkipBytes;				/** Optimization, to avoid calling MoveTo() in the loop */

	COutputCache *m_pOutputCache;
	char *m_pSignature;
};

#endif // !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
