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
			// Ako je izabrana varijanta 2 u CInput::Read onda
			// sledece dve linije moraju da se odkomentarisu.
			// Da li je taj nacin dobar nije najjasnije jer,
			// sta se desava kada je len != chunkSize?
			//if(len == 0)
			//	len = chunkSize;

			// Varijanta 3?:
			//if(len!=chunkSize && !IFloopySoundFilter::IsEOF())
			//	len = chunkSize;

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
