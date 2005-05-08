// Output.cpp: implementation of the COutput class.
//
//////////////////////////////////////////////////////////////////////

#include "Output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


COutput::COutput(int nSamplesPerSec, int wBitsPerSample, int nChannels)
{
	m_nSamplesPerSec = nSamplesPerSec;
	m_wBitsPerSample = wBitsPerSample;
	m_nChannels = nChannels;
	m_pFile = NULL;
	height=400;
	width=65535;
	lmin.x=lmin.y=0;
	lmax.x=lmax.y=0;
	lprev.x=lprev.y=0;
	pt1.x=pt1.y=0;
	pt2.x=pt2.y=0;
	x=0;//, step=1;
	counter=0;
}

COutput::~COutput()
{
	Close();
}

BOOL COutput::Open(char *filename)
{
	m_pFile = fopen(filename, "wb");
	if(NULL != m_pFile)
	{
#ifdef _PLAIN_SVG_
	headerLen = fprintf(m_pFile, xml1, 0.0f, (float)height/2, (float)width, (float)height);
#else
	headerLen = fprintf(m_pFile, xml1, (float)width, (float)height);
#endif
		return TRUE;
	}
	return FALSE;
}

int COutput::Write(BYTE *data, int size)
{
	int written = 0;

	int interval = 512;
	int step=1;

	short int *buffer = (short int*)data;

	int maxSample = (int)pow(2, m_wBitsPerSample);// / 2;
	int scale = maxSample / height;

	if(NULL != m_pFile)
	{
		int samples = (size*8) / m_wBitsPerSample / m_nChannels;

		float mid = height/2.f;

		for(int i=0; i<samples; i+=2)
		{
			if(buffer[i] > lmax.y) {
				lmax.x = x;
				lmax.y = buffer[i];
			} else if(buffer[i] < lmin.y) {
				lmin.x = x;
				lmin.y = buffer[i];
			}

			if(counter >= interval)
			{
				if(lmin.x > lmax.x) {
					memcpy(&pt1, &lmax, sizeof(Point));
					memcpy(&pt2, &lmin, sizeof(Point));
				} else {
					memcpy(&pt1, &lmin, sizeof(Point));
					memcpy(&pt2, &lmax, sizeof(Point));
				}

				fprintf(m_pFile, "M %d,%.5f ", lprev.x,	(float)mid + lprev.y/scale);
				fprintf(m_pFile, "L %d,%.5f ", x,		(float)mid + pt1.y/scale);

				fprintf(m_pFile, "M %d,%.5f ", x,		(float)mid + pt1.y/scale);
				fprintf(m_pFile, "L %d,%.5f ", x+=step,	(float)mid + pt2.y/scale);

				//memcpy(&lprev, &pt2, sizeof(Point));
				lprev.x = x;
				lprev.y = pt2.y;
				
				memset(&lmin, 0, sizeof(Point));
				memset(&lmax, 0, sizeof(Point));

				counter = 0;

				x += step;
			} else
				counter++;
		}
	}
	return written;
}

void COutput::Close()
{
	if(NULL != m_pFile)
	{
		fprintf(m_pFile, "%s", xml2);

		width = (int)x;
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

		fclose(m_pFile);
		m_pFile = NULL;
	}
}
