// Cache.h: interface for the CCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CACHE_H__935D07E1_D3E6_461F_8BBA_006E633A56C8__INCLUDED_)
#define AFX_CACHE_H__935D07E1_D3E6_461F_8BBA_006E633A56C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

class CCache : public IFloopySoundFilter
{
private:
	struct sourceParam
	{
		float value;
		sourceParam *next;
	};

public:
	CCache();
	virtual ~CCache();

	const char *GetName()			{ return "std.cache"; }
	const char *GetDescription()	{ return "Stores entire source input in memory"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	int Read(BYTE *data, int size);
	int Read2(BYTE **data, int channels, int samples);
	bool SetSource(IFloopySoundInput *src);

	void MoveTo(int samples);//	{ m_nPosition = samples * samplesToBytes(); }
	int GetPosition();//			{ return m_nPosition / samplesToBytes(); }
	void Reset();//				{ m_nPosition = 0; }
	void Close();

	int GetSize();

	bool CanReadSourceIfDisabled()	{ return false; }

	const char *GetLastErrorDesc() { return NULL; }

private:
	int samplesToBytes();
	bool createBuffer();
	inline bool passedTheEnd()	{ return (m_nPosition > m_nSize); }
	inline bool bufferIsEmpty()	{ return (m_nSize == 0); }
	bool sourceParameterChanged();
	void clearBuffer();
	void loadSourceParams(IFloopySoundInput *src);

private:
	BYTE *m_pBuffer;
	int m_nSize;
	int m_nPosition;
	sourceParam *m_pSourceParams;
};

#endif // !defined(AFX_CACHE_H__935D07E1_D3E6_461F_8BBA_006E633A56C8__INCLUDED_)
