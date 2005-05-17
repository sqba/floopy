// Output.h: interface for the COutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__F92C4FCF_3973_4C92_8FD9_3248B8EC974B__INCLUDED_)
#define AFX_OUTPUT_H__F92C4FCF_3973_4C92_8FD9_3248B8EC974B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <math.h>
#include "../../ifloopy.h"





#define _PLAIN_SVG_


#ifdef _PLAIN_SVG_

static char *xml1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n\
<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\"\n\
\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n\
<!-- Created with Inkscape (http://www.inkscape.org/) -->\n\
<svg\n\
   xmlns=\"http://www.w3.org/2000/svg\"\n\
   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n\
   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n\
   xmlns:cc=\"http://web.resource.org/cc/\"\n\
   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n\
   version=\"1.0\"\n\
   x=\"%.5f\"\n\
   y=\"%.5f\"\n\
   width=\"%.5f\"\n\
   height=\"%.5f\"\n\
   id=\"svg1\">\n\
  <defs id=\"defs3\" />\n\
  <metadata id=\"metadata4\">\n\
    <rdf:RDF id=\"RDF5\">\n\
      <cc:Work rdf:about=\"\" id=\"Work6\">\n\
        <dc:format id=\"format7\">image/svg+xml</dc:format>\n\
        <dc:type id=\"type9\"\n\
           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n\
      </cc:Work>\n\
    </rdf:RDF>\n\
  </metadata>\n\
  <path d=\"";

static char *xml2 = "\" id=\"path909\"\n\
        style=\"stroke:#000000;stroke-opacity:1.0000000;\" />\n</svg>\n";

#else

static char *xml1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n\
<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\"\n\
\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n\
<!-- Created with Inkscape (http://www.inkscape.org/) -->\n\
<svg\n\
   id=\"svg1\"\n\
   sodipodi:version=\"0.32\"\n\
   inkscape:version=\"0.39cvs\"\n\
   xmlns=\"http://www.w3.org/2000/svg\"\n\
   xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"\n\
   xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n\
   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n\
   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n\
   xmlns:cc=\"http://web.resource.org/cc/\"\n\
   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n\
   width=\"%.5f\"\n\
   height=\"%.5f\"\n\
   sodipodi:docbase=\"C:\\Documents and Settings\\sqba\\Desktop\"\n\
   sodipodi:docname=\"drawing.svg\">\n\
  <defs\n\
     id=\"defs3\" />\n\
  <sodipodi:namedview\n\
     id=\"base\"\n\
     pagecolor=\"#ffffff\"\n\
     bordercolor=\"#666666\"\n\
     borderopacity=\"1.0\"\n\
     inkscape:pageopacity=\"0.0\"\n\
     inkscape:pageshadow=\"2\"\n\
     inkscape:zoom=\"0.43415836\"\n\
     inkscape:cx=\"305.25953\"\n\
     inkscape:cy=\"417.84947\"\n\
     inkscape:window-width=\"640\"\n\
     inkscape:window-height=\"524\"\n\
     inkscape:window-x=\"66\"\n\
     inkscape:window-y=\"66\"\n />		\
  <metadata\n\
     id=\"metadata4\">\n\
    <rdf:RDF\n\
       id=\"RDF5\">\n\
      <cc:Work\n\
         rdf:about=\"\"\n\
         id=\"Work6\">\n\
        <dc:format\n\
           id=\"format7\">image/svg+xml</dc:format>\n\
        <dc:type\n\
           id=\"type9\"\n\
           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n\
      </cc:Work>\n\
    </rdf:RDF>\n\
  </metadata>\n\
  <path\n\
     style=\"fill:none;fill-opacity:0.75000000;fill-rule:evenodd;stroke:#3929af;stroke-width:1.0000000pt;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1.0000000;\"\n\
     d=\"";

static char *xml2 = "\" id=\"path909\" />\n</svg>\n";

#endif



class COutput : public IFloopySoundOutput
{
private:
	struct Point
	{
		int   x;
		short y;
	};

	/*struct Param
	{
		int val;
		char *name;
		char *desc;
	} params[] = {
		512, "interval", "Interval", 
		1, "step", "Step"
	};*/

public:
	COutput(SOUNDFORMAT fmt);
	virtual ~COutput();

	BOOL Open(char *filename);
	int Write(BYTE *data, int size);
	void Close();

	char *GetName()			{ return "svgfile"; }
	char *GetDescription()	{ return "SVG File Writer"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }
/*
	int   GetParamCount()			{ return 2; }
	void  SetParam(int index, float value) {  }
	float GetParam(int index)		{ return (float)interval; }
	char *GetParamName(int index)	{ return "Frequency"; }
	char *GetParamDesc(int index)	{ return "Tone Frequency"; }
*/
private:
	FILE *m_pFile;
	int m_nSamplesPerSec, m_wBitsPerSample, m_nChannels;
	int height, width;
	Point lmin, lmax, lprev;
	Point pt1, pt2;
	int x;//, step=1;
	int counter;
	int headerLen;
//	int interval;
//	int step;
};

#endif // !defined(AFX_OUTPUT_H__F92C4FCF_3973_4C92_8FD9_3248B8EC974B__INCLUDED_)
