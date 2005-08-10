// Loop.h: interface for the CLoop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOP_H__A48C578C_A373_4452_8A5C_F979D26A4D87__INCLUDED_)
#define AFX_LOOP_H__A48C578C_A373_4452_8A5C_F979D26A4D87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include "../../ifloopy.h"

class CLoop : public IFloopySoundFilter
{
public:
	CLoop();
	virtual ~CLoop();

	char *GetName()			{ return "stdlib.loop"; }
	char *GetDescription()	{ return "Looping component"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int Read(BYTE *data, int size);
	int GetSize();	//{ return -1; }
	void MoveTo(int samples);
	void Reset();

	BOOL ReadSourceIfDisabled()	{ return FALSE; }

private:
	int samplesToBytes();

private:
//	int m_nLoops, m_nMaxLoops;
	int m_nPosition;//, m_pos;
};

#endif // !defined(AFX_LOOP_H__A48C578C_A373_4452_8A5C_F979D26A4D87__INCLUDED_)
