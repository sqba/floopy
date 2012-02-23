// MadxlibWrapper.cpp: implementation of the CMadxlibWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "MadxlibWrapper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMadxlibWrapper::CMadxlibWrapper()
{
	in_size = MADX_INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD;
}

CMadxlibWrapper::~CMadxlibWrapper()
{

}

SOUNDFORMAT *CMadxlibWrapper::GetFormat()
{
	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	if(NULL != fmt)
	{
		memcpy(&m_format, fmt, sizeof(SOUNDFORMAT));
		m_format.sampleDataType = SAMPLE_DATA_TYPE_INTEGER;
		m_format.bitsPerSample = 16;
	}
	return &m_format;
}

bool CMadxlibWrapper::Open(char *filename)
{
	m_pInFile = fopen(filename, "rb");
	if(NULL == m_pInFile)
		return false;

	// Initialize madxlib
	madx_init(out_buffer, &mxhouse);

	return false;
}

int CMadxlibWrapper::Read(BYTE *data, int size)
{
	size_t readBytes = 0;
	size_t totalReadBytes = 0;

	while(totalReadBytes < size)
	{
		// Read input, pass processed data to out_buffer
		mxsig = madx_read( in_buffer, out_buffer, &mxhouse, &mxstat );

		switch(mxsig)
		{
		case ERROR_OCCURED:	// Error
			printf("Unrecoverable error %s\n", mxstat.msg);
			return 0;
		case MORE_INPUT:	// Fill buffer
			if (mxstat.buffstart)			// Fill partial buffer
			{
				if ( (readBytes = fread(mxstat.buffstart, 1, mxstat.readsize, m_pInFile)) == mxstat.readsize)
				{
					printf("Filling buffer.\n");			
				}
				else if (feof(m_pInFile))		// EOF must be flagged
				{
					mxstat.is_eof = 1;
					mxstat.readsize = readBytes;
					totalReadBytes += readBytes;
				}
				else
					printf("Error! %d\n", ferror(m_pInFile));
			}
			else							// Read full buffer
			{
				if ( fread(in_buffer, 1, mxstat.readsize, m_pInFile) == mxstat.readsize)
				{
					printf("Fill buffer full.\n");					
				}
				else if (feof(m_pInFile))		// EOF must be flagged
				{
					mxstat.is_eof = 1;
					mxstat.readsize = readBytes;
					totalReadBytes += readBytes;
				}
				else
					printf("Error! %d\n", ferror(m_pInFile));
			}
			break;
		case FLUSH_BUFFER:	// Output to file
			/*if (fwrite(out_buffer, 1, mxstat.write_size, out_file) == mxstat.write_size)
				printf("Writing buffer.\n");
			else		
				printf("fwrite() error\n");*/
			break;
		case EOF_REACHED:	// Input file EOF
			/*if ( (a = fwrite(out_buffer,1,mxstat.write_size,out_file)) != mxstat.write_size)
			{
				printf("Error with final write! out_size:%d, fwrite: %d\n", 
				mxstat.write_size, a);
			}
			else
				printf("Finished. out_size:%d, fwrite: %d\n", mxstat.write_size, a);			*/
			break;	
		}
	}

	return 0;
}

void CMadxlibWrapper::Close()
{
	// Clean-up
	madx_deinit(&mxhouse);
	//fclose(out_file);
	fclose(m_pInFile);
}

void CMadxlibWrapper::Reset()
{
}

void CMadxlibWrapper::MoveTo(int offset)
{
}
