// Cache.h: interface for the CCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CACHE_H__935D07E1_D3E6_461F_8BBA_006E633A56C8__INCLUDED_)
#define AFX_CACHE_H__935D07E1_D3E6_461F_8BBA_006E633A56C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

class CCache : public IFloopySoundInput
{
public:
	CCache();
	virtual ~CCache();

	char *GetName()				{ return "cache"; }
	char *GetDescription()		{ return "Stores entire source input in memory"; }
	char *GetVersion()			{ return "0.1"; }
	char *GetAuthor()			{ return "sqba"; }

	int Read(BYTE *data, int size);
	BOOL SetSource(IFloopySoundInput *src);

	void MoveTo(int samples)	{ m_nPosition = samples * samplesToBytes(); }
	int GetPos()				{ return m_nPosition / samplesToBytes(); }
	void Reset()				{ m_nPosition = 0; }
	void Close();

	BOOL ReadSourceIfDisabled()	{ return FALSE; }

private:
	int samplesToBytes();
	BOOL createBuffer();
	inline BOOL passedTheEnd()	{ return (m_nPosition > m_nSize); }
	inline BOOL bufferIsEmpty()	{ return (m_nSize == 0); }
	void clearBuffer();

private:
	BYTE *m_pBuffer;
	int m_nSize;
	int m_nPosition;
};

#endif // !defined(AFX_CACHE_H__935D07E1_D3E6_461F_8BBA_006E633A56C8__INCLUDED_)
