// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINE_H__621A6F07_09D1_41D0_A981_DB32D29DA57A__INCLUDED_)
#define AFX_ENGINE_H__621A6F07_09D1_41D0_A981_DB32D29DA57A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "../ifloopy.h"
#include "input.h"
#include "output.h"

class CEngine : public IFloopyEngine
{
public:
	CEngine();
	virtual ~CEngine();

//	UINT Read(BYTE *data, UINT size, UINT offset);

	char *GetName()			{ return "engine"; }
	char *GetDescription()	{ return "Floopy Engine Plugin Wrapper"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	IFloopySoundInput  *CreateInput(char *plugin)
		{ return new CInput(plugin); }
	IFloopySoundOutput *CreateOutput(char *plugin, WAVFORMAT *fmt)
		{ return new COutput(plugin, fmt); }

private:
//	void printPath(CInput *input, COutput *output);
//	void process(CInput *input, COutput *output);

	void test();

//	CInput *m_master;
};

#endif // !defined(AFX_ENGINE_H__621A6F07_09D1_41D0_A981_DB32D29DA57A__INCLUDED_)
