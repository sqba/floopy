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



bool CMadxlibWrapper::Open(char *filename)
{
	in_file = fopen(filename, "rb");
	if(NULL == in_file)
		return false;

	// Initialize madxlib
	madx_init(out_buffer, &mxhouse);

	return false;
}

SOUNDFORMAT *CMadxlibWrapper::GetFormat()
{
	return NULL;
}

int CMadxlibWrapper::Read(BYTE *data, int size)
{
	// Read input, pass processed data to out_buffer
	mxsig = madx_read( in_buffer, out_buffer, &mxhouse, &mxstat );

	switch(mxsig)
	{
	case ERROR_OCCURED:			// Error
		printf("Unrecoverable error %s\n", mxstat.msg);
		break;
	case MORE_INPUT:		// Fill buffer
		{
			if (mxstat.buffstart)			// Fill partial buffer
			{
				if ( (a = fread(mxstat.buffstart, 1, mxstat.readsize, in_file)) == mxstat.readsize)
				{
					printf("Filling buffer.\n");			
				}
				else if (feof(in_file))		// EOF must be flagged
				{
					mxstat.is_eof = 1;
					mxstat.readsize = a;					
				}
				else
					printf("Error! %d\n", ferror(in_file));
			}
			else							// Read full buffer
			{
				
				if ( fread(in_buffer, 1, mxstat.readsize, in_file) == mxstat.readsize)
				{
					printf("Fill buffer full.\n");					
				}
				else if (feof(in_file))		// EOF must be flagged
				{
					mxstat.is_eof = 1;
					mxstat.readsize = a;
				}
				else
					printf("Error! %d\n", ferror(in_file));
			}
		}
		break;
	case FLUSH_BUFFER: 	// Output to file
		/*if (fwrite(out_buffer, 1, mxstat.write_size, out_file) == mxstat.write_size)
			printf("Writing buffer.\n");
		else		
			printf("fwrite() error\n");*/
		break;
	case EOF_REACHED: 		// Input file EOF
		/*if ( (a = fwrite(out_buffer,1,mxstat.write_size,out_file)) != mxstat.write_size)
		{
			printf("Error with final write! out_size:%d, fwrite: %d\n", 
			mxstat.write_size, a);
		}
		else
			printf("Finished. out_size:%d, fwrite: %d\n", mxstat.write_size, a);			*/
		break;	
	}

	return 0;
}

void CMadxlibWrapper::Close()
{
	// Clean-up
	madx_deinit(&mxhouse);
	//fclose(out_file);
}

void CMadxlibWrapper::Reset()
{
}

void CMadxlibWrapper::MoveTo(int offset)
{
}
