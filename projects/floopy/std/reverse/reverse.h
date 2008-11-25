#ifndef _FLOOPY_REVERSE_H_
#define _FLOOPY_REVERSE_H_

#include "../../ifloopy.h"

class CReverse : public IFloopySoundFilter
{
public:
	CReverse();
	virtual ~CReverse();

	const char *GetName()			{ return "libstd.reverse"; }
	const char *GetDescription()	{ return "Reverse"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	bool CanReadSourceIfDisabled()	{ return true; }

	int Read(BYTE*, int);
	void MoveTo(int);
	void Reset();
	int GetPosition();
	bool SetSource(IFloopySoundInput*);

private:
	void reverse(BYTE*, int);

private:
	int m_nPosition;
	BYTE *m_pBuffer;
	int m_nBuffSize;
	int m_nSamplesToBytes;
};

#endif // _FLOOPY_REVERSE_H_
