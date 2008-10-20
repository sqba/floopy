#ifdef linux

#ifndef OSS_H_
#define OSS_H_

#include "../../engine/include/ifloopy.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/soundcard.h>

class COSS : public IFloopySoundOutput
{
public:
	COSS(SOUNDFORMAT fmt);
	virtual ~COSS();

	int Write(BYTE *data, int size);

	const char *GetName()			{ return "libstd.oss"; }
	const char *GetDescription()	{ return "OSS Out"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	int GetPosition();
	void Reset();
	void Flush();
	void Pause();
	
private:
	int open_audio_device (char *name, int mode);
	
private:
	int m_sample_rate;
};

#endif /*OSS_H_*/

#endif // linux
