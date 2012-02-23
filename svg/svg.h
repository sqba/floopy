// SvgFileOut.h: interface for the CSvgFileOut class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "../ifloopy.h"

class CSvgFileOut : public IFloopySoundOutput
{
public:
	CSvgFileOut(SOUNDFORMAT fmt);
	virtual ~CSvgFileOut();

	bool Open(const char *filename);
	int Write(BYTE *data, int size);
	void Close();

	const char *GetName()			{ return "svg.svgout"; }
	const char *GetDescription()	{ return "SVG Out"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()		    { return "sqba"; }

	//int GetPosition();
	//void Reset();
	//void Flush();

	const char *GetLastErrorDesc() { return NULL; }

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
	bool m_bDrawVertical;
};

