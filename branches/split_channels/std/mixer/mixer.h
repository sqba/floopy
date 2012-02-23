// Mixer.h: interface for the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXER_H__571408E7_BE83_4237_BA26_0C5306E32472__INCLUDED_)
#define AFX_MIXER_H__571408E7_BE83_4237_BA26_0C5306E32472__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "../../ifloopy.h"

class CMixer : public IFloopySoundMixer
{
private:
	struct inputChannel
	{
		IFloopySoundInput	*input;
		inputChannel		*next;
	};

public:
	CMixer();
	virtual ~CMixer();

	const char *GetName()			{ return "std.mixer"; }
	const char *GetDescription()	{ return "Mixer Component"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	void Reset();
	void Close();

	int Read(BYTE *data, int size);
	int GetSize();
	void MoveTo(int samples);

	bool SetSource(IFloopySoundInput *src) { return (AddSource(src) != -1); }
	int AddSource(IFloopySoundInput *src);
	IFloopySoundInput *GetSource(int index);
	bool RemoveSource(IFloopySoundInput *src);
	int GetInputCount();

	bool CanReadSourceIfDisabled()	{ return false; }

	const char *GetLastErrorDesc() { return NULL; }

private:
	int sampleSize();
	void mixBuffers(BYTE *buffers, int count, BYTE *output, int size);

private:
	int				*m_nLengths;	/// Array of buffer lengths
	int				m_nLengthsSize;	/// Length of m_nLengths array
	BYTE			*m_pBuffers;	/// Array of buffers
	int				m_nBuffSize;	/// Total size of m_pBuffers, in bytes
	inputChannel	*m_pFirst;		/// Pointer to the first input channel
	int				m_nInputCount;	/// Number of input channels
};

#endif // !defined(AFX_MIXER_H__571408E7_BE83_4237_BA26_0C5306E32472__INCLUDED_)
