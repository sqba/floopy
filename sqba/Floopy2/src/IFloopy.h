#ifndef _IFLOOPY_H_
#define _IFLOOPY_H_


//! Sound format information
typedef struct SoundFormat
{
	INT		frequency;		// Audio frequency in samples per second
	WORD	format;			// Audio data format
	BYTE	channels;		// Number of channels: 1-mono, 2-stereo
	DWORD	bitsPerSample;	// Sample size, in bits
} SOUNDFORMAT;


/*********************************************************************
 *! \class IFloopy
 *  \brief Main object interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *********************************************************************/
class IFloopy
{
public:
	IFloopy()			{ m_bEnabled = TRUE; m_nLastError = 0; }
	virtual ~IFloopy()	{ }

	//! Used for runtime identification, do not override in plugins!
	//virtual int GetType()			{ return typeFloopy; }

	// Component description
	virtual char *GetName()			{ return "IFloopy"; }
	virtual char *GetDescription()	{ return "IFloopy interface"; }
	virtual char *GetVersion()		{ return "0.1"; }
	virtual char *GetAuthor()		{ return "sqba"; }

	// Component parameters
	virtual int   GetParamCount()					{ return 0; }
	virtual void  SetParam(int index, float value)	{ }
	virtual float GetParam(int index)				{ return 0.f; }
	virtual char *GetParamName(int index)			{ return NULL; }
	virtual char *GetParamDesc(int index)			{ return NULL; }
	virtual BOOL  GetParam(int index, float *value)	{ return FALSE; }
	virtual int   GetParamIndex(char *name)			{ return -1; }

	virtual void  Enable(BOOL bEnabled)				{ m_bEnabled = bEnabled; }
	virtual BOOL  IsEnabled()						{ return m_bEnabled; }

	virtual int   GetLastError()					{ return m_nLastError; }
	virtual BOOL  GetErrorDesc(char *str, int len)	{ return FALSE; }

	virtual char *GetDisplayName()					{ return NULL; }
	virtual void  SetDisplayName(char *name, int len){ }

private:
	int m_nLastError;
	BOOL m_bEnabled;
};


/*********************************************************************
 *! \class IFloopySound
 *  \brief Main sound object interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *
 *  Interface implemented and used by all sound objects.
 *********************************************************************/
class IFloopySound : public IFloopy
{
public:
	IFloopySound() {}
	virtual ~IFloopySound() {}

	//! Used for runtime identification, do not override in plugins!
	//virtual int GetType() { return typeFloopySound; }

	// Component description
	virtual char *GetName()					{ return "IFloopySound"; }
	virtual char *GetDescription()			{ return "IFloopySound interface"; }

	virtual BOOL Open(char *filename)		{ return FALSE; }
	virtual void Close()					{ }

	virtual int GetNextOffset(int offset)	{ return 0; }
};


/*********************************************************************
 *! \class IFloopySoundInput
 *  \brief Main sound input interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *
 *  Interface implemented and used by all sound input objects
 *  such as sound source objects and filter (effect) objects.
 *********************************************************************/
class IFloopySoundInput : public IFloopySound
{
public:
	IFloopySoundInput()
	{
		m_source = NULL;
		memset(&m_format, 0, sizeof(SOUNDFORMAT));
		//m_pos = 0;
	}
	virtual ~IFloopySoundInput() {}

	//! Used for runtime identification, do not override in plugins!
	//int GetType() { return typeFloopySoundInput; }

	// Component description
	virtual char *GetName()			{ return "IFloopySoundInput"; }
	virtual char *GetDescription()	{ return "IFloopySoundInput interface"; }

	/**
	 * Open source file.
	 */
	virtual BOOL Open(char *filename)
	{
		return (NULL != m_source ? m_source->Open(filename) : FALSE);
	}

	/**
	 * Return total sample count.
	 * @return number of samples.
	 */
	virtual int GetSize()
	{
		return (NULL != m_source ? m_source->GetSize() : 0);
	}

	virtual BOOL SetSource(IFloopySoundInput *src)
	{
		m_source = src;
		return TRUE;
	}

	virtual IFloopySoundInput *GetSource()
	{
		return m_source;
	}

	/**
	 * Fills given buffer with sound data.
	 * @param data pointer to the buffer to be filled.
	 * @param size size of the buffer in bytes.
	 * @param offset number of bytes to skip.
	 * @return number of bytes read from data.
	 */
	virtual int Read(BYTE *data, int size)
	{
		//int len = (NULL != m_source ? m_source->Read(data, size) : 0);
		//m_pos += len;
		//return len;
		return (NULL != m_source ? m_source->Read(data, size) : 0);
	}

	/**
	 * Moves the starting position.
	 * @param samples number of samples to move from the start.
	 */
	virtual void MoveTo(int samples)
	{
		if(NULL != m_source)
			m_source->MoveTo(samples);
		//SOUNDFORMAT *fmt = GetFormat();
		//m_pos = samples * ( (fmt->size / 8) * fmt->channels );
	}

	virtual int GetPos()
	{
		if(NULL != m_source)
			return m_source->GetPos();
		else
			return 0;
		//	return m_pos;
	}

	/**
	 * Moves to the starting position.
	 */
	virtual void Reset()
	{
		if(NULL != m_source)
			m_source->Reset();
		//m_pos = 0;
	}

	virtual SOUNDFORMAT *GetFormat()
	{
		return (NULL != m_source ? m_source->GetFormat() : &m_format);
	}

	//////////////////////////////////////////////////////////////////
	// Mixer specific functions
	//////////////////////////////////////////////////////////////////
	virtual int AddSource(IFloopySoundInput *src)
	{
		return (NULL != m_source ? m_source->AddSource(src) : 0);
	}

	virtual IFloopySoundInput *GetSource(int index)
	{
		return (index > 0 ? NULL : m_source);
	}

	virtual void RemoveSource(IFloopySoundInput *src)
	{
		if(NULL != m_source)
			m_source->RemoveSource(src);
	}

	virtual int GetInputCount()			{ return (NULL != m_source ? 1 : 0); }

	virtual BOOL ReadSourceIfDisabled()	{ return TRUE; }

	// Utility
	/*int SamplesToBytes(int samples)
	{
		SOUNDFORMAT *fmt = GetFormat();
		return samples * ( (fmt->bitsPerSample / 8) * fmt->channels );
	}*/

protected:
	IFloopySoundInput *m_source;
	SOUNDFORMAT m_format;
	//int m_pos;
};

/*********************************************************************
 *! \class IFloopySoundOutput
 *  \brief Main sound output interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *
 *  Interface implemented and used by all sound output objects
 *  such as sound mixer, encoders and other output objects.
 *********************************************************************/
class IFloopySoundOutput : public IFloopySound
{
public:
	IFloopySoundOutput() { m_dest = NULL; }
	IFloopySoundOutput(SOUNDFORMAT fmt) {}
	virtual ~IFloopySoundOutput() {}

	//! Used for runtime identification, do not override in plugins!
	//int GetType() { return typeFloopySoundOutput; }

	virtual char *GetName()			{ return "IFloopySoundOutput"; }
	virtual char *GetDescription()	{ return "IFloopySoundOutput interface"; }

	/**
	 * Opens destination (file).
	 */
	virtual BOOL Open(char *filename)
	{
		return (NULL != m_dest ? m_dest->Open(filename) : FALSE);
	}

	/**
	 * Closes destination (file).
	 */
	virtual void Close()
	{
		if(NULL != m_dest)
			m_dest->Close();
	}

	virtual void SetDest(IFloopySoundOutput *dst)	{ m_dest = dst; }
	virtual IFloopySoundOutput *GetDest()			{ return m_dest; }

	/**
	 * Writes a buffer to the output, whatever it may be.
	 * @param size size of the data in bytes.
	 * @param data pointer to the buffer written into output.
	 * @return number of bytes read from data into output.
	 */
	virtual int Write(BYTE *data, int size)
	{
		return (NULL != m_dest ? m_dest->Write(data, size) : 0);
	}

protected:
	IFloopySoundOutput *m_dest;
};


/*********************************************************************
 *! \class IFloopyEngine
 *  \brief Floopy engine interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *********************************************************************/
class IFloopyEngine : public IFloopySoundInput
{
public:
	//! Used for runtime identification, do not override in plugins!
	//int GetType() { return typeFloopyEngine; }

	virtual char *GetName()			{ return "IFloopySoundEngine"; }
	virtual char *GetDescription()	{ return "IFloopySoundEngine interface"; }

	virtual IFloopySoundInput  *CreateInput(char *plugin)  { return NULL; }
	virtual IFloopySoundOutput *CreateOutput(char *plugin, SOUNDFORMAT fmt) { return NULL; }

	virtual BOOL Save(char *filename) { return FALSE; }

	// offset is in samples!
	virtual void SetParamAt(IFloopy *obj, int offset, int index, float value) {}
	virtual void ResetParamAt(IFloopy *obj, int offset, int index) {}
	virtual void EnableAt(IFloopy *obj, int offset, BOOL bEnable) {}
};


#endif //_IFLOOPY_H_
