// SvgFileOut.cpp: implementation of the CSvgFileOut class.
//
//////////////////////////////////////////////////////////////////////

//#include <fcntl.h>
//#include <io.h>
#include <math.h>
#include <string>

#include "svg.h"






#define _PLAIN_SVG_


#ifdef _PLAIN_SVG_

const char *xml1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n\
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

const char *xml2 = "\" id=\"path909\"\n\
        style=\"stroke:#000000;stroke-opacity:1.0000000;\" />\n</svg>\n";

#else

const char *xml1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n\
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

const char *xml2 = "\" id=\"path909\" />\n</svg>\n";

#endif



struct Peak
{
	int x;
	float value;
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSvgFileOut::CSvgFileOut(SOUNDFORMAT fmt) : IFloopySoundOutput(fmt)
{
	height			= 400;
	width			= 65535;
	m_pFile			= NULL;
	bitsPerSample	= fmt.bitsPerSample; //sizeof(short) * 8;
	maxSample		= (int)pow(2, bitsPerSample);// / 2;
	scale			= (float)maxSample / (float)height;
	channels		= fmt.channels;
	headerLen		= 0;
	mid				= height/2.f;
	m_bDrawVertical	= true;
}

CSvgFileOut::~CSvgFileOut()
{
	Close();
}

bool CSvgFileOut::Open(const char *filename)
{
	counter			= 0;
	peakcount		= 0;
	interval		= 128; // samples per pixel
	counter			= interval;
	mid				= height/2.f;

	m_pFile = fopen(filename, "w");

	if(NULL != m_pFile)
	{
#ifdef _PLAIN_SVG_
	headerLen = fprintf(m_pFile, xml1, 0.0f, (float)height/2, (float)width, (float)height);
#else
	headerLen = fprintf(m_pFile, xml1, (float)width, (float)height);
#endif
	}

	return (NULL != m_pFile);
}

int CSvgFileOut::Write(BYTE *data, int size)
{
	int samples = size / (bitsPerSample / 8);
	short int *buffer = (short int*)data;

	int ch=0;

	static short int min[2]={0}, max[2]={0};
	static Peak peak[2];
	static Peak prev[2];
	memset(peak, 0, sizeof(peak));
	memset(prev, 0, sizeof(prev));

	for(int pos=channels; pos<samples; pos+=channels)
	{
		//for(ch=0; ch<channels; ch++)
		{
			short int sample = buffer[pos-channels+ch];

			if(sample > max[ch])
				max[ch] = sample;
			else if(sample < min[ch])
				min[ch] = sample;
		}

		if(counter >= interval)
		{
			//for(ch=0; ch<channels; ch++)
			{
				short int sample = buffer[pos-channels+ch];

				int x1=0, x2=0;
				float y1=0.f, y2=0.f;

				if(!m_bDrawVertical)
					{
					if(max[ch] == 0 && min[ch] != 0)
						max[ch] = min[ch];
					else if(min[ch] == 0 && max[ch] != 0)
						min[ch] = max[ch];

					peak[ch].value = ( (peakcount % 2) == 0 ? max[ch] : min[ch] );
					peak[ch].value /= scale;

					peak[ch].x = peakcount;

					x1 = prev[ch].x;
					x2 = peak[ch].x;
					y1 = mid - prev[ch].value;
					y2 = mid - peak[ch].value;

					prev[ch].x = peak[ch].x;
					prev[ch].value = peak[ch].value;
				}
				else
				{
					x1 = x2 = peakcount;
					y1 = mid - max[ch]/scale;	// Top point
					y2 = mid - min[ch]/scale;	// Bottom point
				}

				fprintf(m_pFile, "M %d,%.5f ", x1,	y1);
				fprintf(m_pFile, "L %d,%.5f ", x2,	y2);

				max[ch] = min[ch] = sample;
			}
			counter = 0;
			peakcount++;
		} else
			counter++;
	}
	return size;
}

int CSvgFileOut::Write2(BYTE **data, int channels, int samples)
{
	return 0;
}

void CSvgFileOut::Close()
{
	if(NULL == m_pFile)
		return;


	fprintf(m_pFile, "%s", xml2);

	width = (int)peakcount;
	fseek(m_pFile, 0, SEEK_SET);
	int headerLen1;
#ifdef _PLAIN_SVG_
	headerLen1 = fprintf(m_pFile, xml1, 0.0f, (float)height/2, (float)width, (float)height);
#else
	headerLen1 = fprintf(m_pFile, xml1, (float)width, (float)height);
#endif

	if(headerLen > headerLen1)
	{
		for(int n=0; n<headerLen-headerLen1; n++)
			fputc(' ', m_pFile);
	}


	fclose( m_pFile );
	m_pFile = NULL;
}
