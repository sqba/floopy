#ifndef _FLOOPY_LOOP_H_
#define _FLOOPY_LOOP_H_

#include "../../ifloopy.h"

class CLoop : public IFloopySoundFilter
{
public:
	CLoop();

	char *GetName()				{ return "stdlib.loop"; }
	char *GetDescription()		{ return "Looping component"; }
	char *GetVersion()			{ return "0.1"; }
	char *GetAuthor()			{ return "sqba"; }

	int GetSize()				{ return SIZE_INFINITE; }
	bool ReadSourceIfDisabled()	{ return false; }

	int Read(BYTE*, int);
	void MoveTo(int);
	void Reset();

private:
	int m_nOffset;	/** In bytes */
};

#endif // _FLOOPY_LOOP_H_
