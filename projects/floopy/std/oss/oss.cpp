#ifdef linux

#include "oss.h"

COSS::COSS(SOUNDFORMAT fmt) : IFloopySoundOutput(fmt)
{
	m_sample_rate	= 44100;
	m_num_channels	= 2;

	/*
	Use /dev/dsp as the default device because the system administrator 
	may select the device using the ossctl program or some other methods
	*/
	const char *name_out = "/dev/dsp";
  
	/*
	It's mandatory to use O_WRONLY in programs that do only playback. 
	Other modes may cause increased resource (memory) usage in the driver.
	It may also prevent other applications from using the same device for 
	recording at the same time.
	*/
	fd_out = open_audio_device(name_out, O_WRONLY);
}

COSS::~COSS()
{
}

int COSS::Write(BYTE *data, int size)
{
	return write (fd_out, data, size);
}

int COSS::GetPosition()
{
	return 0;
}

void COSS::Reset()
{
}

void COSS::Flush()
{
}

void COSS::Pause()
{
}

int COSS::open_audio_device (const char *name, int mode)
{
	int tmp, fd;
	
	if ((fd = open (name, mode, 0)) == -1)
	{
		perror (name);
		exit (-1);
	}
	/*
	Setup the device. Note that it's important to set the sample format, 
	number of channels and sample rate exactly in this order. Some 
	devices depend on the order.
	*/
	
	/* Set the sample format */
	tmp = AFMT_S16_NE;            /* Native 16 bits */
	if (ioctl (fd, SNDCTL_DSP_SETFMT, &tmp) == -1)
	{
		perror ("SNDCTL_DSP_SETFMT");
		exit (-1);
	}
	if (tmp != AFMT_S16_NE)
	{
		fprintf (stderr, "The device doesn't support the 16 bit sample format.\n");
		exit (-1);
	}

	/* Set the number of channels */
	if (ioctl (fd, SNDCTL_DSP_CHANNELS, &m_num_channels) == -1)
	{
		perror ("SNDCTL_DSP_CHANNELS");
		exit (-1);
	}
	if (tmp != m_num_channels)
	{
		fprintf (stderr, "The device doesn't support %d channels.\n", m_num_channels);
		exit (-1);
	}
	
	/* Set the sample rate */
	if (ioctl (fd, SNDCTL_DSP_SPEED, &m_sample_rate) == -1)
	{
		perror ("SNDCTL_DSP_SPEED");
		exit (-1);
	}
	
	/*
	No need for error checking because we will automatically adjust the
	signal based on the actual sample rate. However most application must 
	check the value of sample_rate and compare it to the requested rate.
	Small differences between the rates (10% or less) are normal and the 
	applications should usually tolerate them. However larger differences 
	may cause annoying pitch problems (Mickey Mouse).
	*/
	
	return fd;
}

#endif // linux
