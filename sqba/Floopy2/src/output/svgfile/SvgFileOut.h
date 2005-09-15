// SvgFileOut.h: interface for the CSvgFileOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SVGFILEOUT_H__7DC319B2_8648_4313_A148_75D4D6CCF502__INCLUDED_)
#define AFX_SVGFILEOUT_H__7DC319B2_8648_4313_A148_75D4D6CCF502__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

#include "../ifloopy.h"

class CSvgFileOut : public IFloopySoundOutput  
{
public:
	CSvgFileOut(SOUNDFORMAT fmt);
	virtual ~CSvgFileOut();

	bool Open(char *filename);
	int Write(BYTE *data, int size);
	void Close();

	char *GetName()			{ return "svgfile.svgout"; }
	char *GetDescription()	{ return "SVG Out"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	//int GetPosition();
	//void Reset();
	//void Flush();

private:
	FILE *m_pFile;
	int bitsPerSample, channels;
	int maxSample;
	float scale, mid;
	int counter;
	int height, width;
	int peakcount;
	int headerLen;
	int interval;
};

#endif // !defined(AFX_SVGFILEOUT_H__7DC319B2_8648_4313_A148_75D4D6CCF502__INCLUDED_)
