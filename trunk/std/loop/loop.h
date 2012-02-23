#ifndef _FLOOPY_LOOP_H_
#define _FLOOPY_LOOP_H_

#include "../../ifloopy.h"

class CLoop : public IFloopySoundFilter
{
public:
	CLoop();

	const char *GetName()			{ return "std.loop"; }
	const char *GetDescription()	{ return "Looping component"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	int GetSize()				{ return SIZE_INFINITE; }
	bool CanReadSourceIfDisabled()	{ return false; }

	int Read(BYTE*, int);
	void MoveTo(int);
	void Reset();
	bool SetSource(IFloopySoundInput *src);

	const char *GetLastErrorDesc() { return NULL; }

private:
	int m_nOffset;	/** In bytes */
};

#endif // _FLOOPY_LOOP_H_
