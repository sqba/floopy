#include "loop.h"


CLoop::CLoop()
{
	m_nOffset = 0;
}

int CLoop::Read(BYTE *data, int size)
{
	int len		= 0;
	int result	= 0;

	while(result < size)
	{
		int chunkSize = size - result;
		len = IFloopySoundFilter::Read(data, chunkSize);

		if(EOF == len)
		{
			len = 0;
			Reset();
		}
		else
		{
			m_nOffset	+= len;
			result		+= len;
			data		+= len;
		}
	}

	return result;
}

void CLoop::MoveTo(int samples)
{
	int size = IFloopySoundFilter::GetSize();

	if(samples > size)
	{
		samples = samples % size;
		IFloopySoundFilter::Reset();
	}

	IFloopySoundFilter::MoveTo( samples );
}

void CLoop::Reset()
{
	m_nOffset = 0;
	IFloopySoundFilter::Reset();
}
