#ifndef _IFLOOPY_H_
#define _IFLOOPY_H_


#include <memory.h>	// for memset()

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#include <windows.h>



#define TIMELINE_PARAM_ENABLE		-1 /// Index of the Enable/Disable parameter
#define PARAM_VALUE_ENABLED		1000.f /// Value of the Enable parameter
#define PARAM_VALUE_DISABLED	2000.f /// Value of the Disable parameter
#define TIMELINE_PARAM_MOVETO		-2 /// Index of the MoveTo parameter

#define SIZE_INFINITE				-1


#define WAVE_FORMAT_PCM     1



//////////////////////////////////////////////////////////////////////
// Defines from windows.h
//////////////////////////////////////////////////////////////////////
typedef int                 INT;	/// 32-bit signed integer.
typedef unsigned int        UINT;	/// Unsigned INT.
typedef int                 BOOL;	/// Boolean variable (should be TRUE or FALSE).
typedef unsigned short      WORD;	/// 16-bit unsigned integer.
typedef unsigned char       BYTE;	/// Byte (8 bits).
typedef unsigned long       DWORD;	/// 32-bit unsigned integer.

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif
//////////////////////////////////////////////////////////////////////



//#define EOF     (-1)	// defined in stdio

/*********************************************************************
 *! \struct SoundFormat
 *  \brief Sound format information.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. April 2005.
 *********************************************************************/
typedef struct SoundFormat
{
	INT		frequency;		/** Audio frequency (samples per second)*/
	WORD	format;			/** Audio data format					*/
	BYTE	channels;		/** Number of channels: 1-mono, 2-stereo*/
	DWORD	bitsPerSample;	/** Sample size, in bits				*/
//	INT		format;			/** Waveform-audio format type			*/
} SOUNDFORMAT;


/*********************************************************************
 *! \enum enumClassType
 *  \brief Runtime class information.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 30. May 2005.
 *
 *  Returned by IFloopy::GetType().
 *********************************************************************/
enum enumClassType
{
	TYPE_FLOOPY = 0,			/** IFloopy				*/
	TYPE_FLOOPY_SOUND_INPUT,	/** IFloopySoundInput	*/
	TYPE_FLOOPY_SOUND_FILTER,	/** IFloopySoundFilter	*/
	TYPE_FLOOPY_SOUND_MIXER,	/** IFloopySoundMixer	*/
	TYPE_FLOOPY_SOUND_OUTPUT,	/** IFloopySound		*/
	TYPE_FLOOPY_SOUND_ENGINE,	/** IFloopySoundEngine	*/
	TYPE_FLOOPY_SOUND_TRACK		/** Just a track */
};


/*********************************************************************
 *! \class IFloopyCallback
 *  \brief Main callback interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. April 2005.
 *
 *  Passed to all components.
 *********************************************************************/
/*class IFloopyCallback
{
public:
	IFloopyCallback()	{ }
	virtual ~IFloopyCallback()	{ }

	void OnParameterUpdate(IFloopy *src, int offset, int param)
};*/


/*********************************************************************
 *! \class IFloopyProperty
 *  \brief Functions for manipulating component's properties.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 11. september 2005.
 *
 *  Not to be directly overriden in implementations.
 *
 *  Properties are not connected to the timeline.
 *********************************************************************/
class IFloopyProperty
{
public:
	virtual int   GetPropertyCount()			{ return 0; }

	virtual BOOL  GetPropertyIndex(char *name, int *index)	{ return FALSE; }

	virtual BOOL  GetPropertyVal(int index, float *value)	{ return FALSE; }
	virtual void  SetPropertyVal(int index, float value)	{ }

	virtual float GetPropertyMin(int index)		{ return 0.f; }
	virtual float GetPropertyMax(int index)		{ return 0.f; }
	virtual float GetPropertyStep(int index)	{ return 0.f; }
	virtual char *GetPropertyName(int index)	{ return NULL; }
	virtual char *GetPropertyDesc(int index)	{ return NULL; }
	virtual char *GetPropertyUnit(int index)	{ return NULL; }
};

/*********************************************************************
 *! \class IFloopyParam
 *  \brief Functions for manipulating component's parameters.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 14. June 2005.
 *
 *  Not to be directly overriden in implementations.
 *********************************************************************/
class IFloopyParam : public IFloopyProperty
{
public:
	IFloopyParam()			{ m_bEnabled = TRUE; }

	virtual int   GetParamCount()					{ return 0; }

	virtual BOOL  GetParamIndex(char *name, int *index)	{ return FALSE; }

	virtual BOOL  GetParamVal(int index, float *value)	{ return FALSE; }
	virtual void  SetParamVal(int index, float value)	{ }

	virtual float GetParamMin(int index)	{ return 0.f; }
	virtual float GetParamMax(int index)	{ return 0.f; }
	virtual float GetParamStep(int index)	{ return 0.f; }
	virtual char *GetParamName(int index)	{ return NULL; }
	virtual char *GetParamDesc(int index)	{ return NULL; }
	virtual char *GetParamUnit(int index)	{ return NULL; }

	virtual void  Enable(BOOL bEnabled)		{ m_bEnabled = bEnabled; }
	virtual BOOL  IsEnabled()				{ return m_bEnabled; }

private:
	BOOL m_bEnabled;	/** Is this component disabled or enabled. */
};


/*********************************************************************
 *! \class IFloopyTimeline
 *  \brief Functions for manipulating component's timeline.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 14. June 2005.
 *
 *  Not to be directly overriden in implementations.
 *********************************************************************/
class IFloopyTimeline : public IFloopyParam
{
public:
	/**
	 * Returns next sample position at which a parameter change occurs.
	 * Do not override in implementations, handled by the engine.
	 * @param offset current position, in samples.
	 * @return next position (number of samples).
	 */
	virtual int GetNextOffset(int offset)			{ return 0; }

	virtual int GetPrevOffset(int offset)			{ return 0; }

	/**
	 * Schedules particular parameter's value for change at specific offset.
	 * Do not override in implementations, handled by the engine.
	 * @param offset position in samples at which the change occurs.
	 * @param index index of the parameter.
	 * @param value new parameter value.
	 */
	virtual void SetParamAt(int offset, int index, float value) {}

	/**
	 * Schedules particular parameter's value for change at specific offset.
	 * Do not override in implementations, handled by the engine.
	 * @param offset position in samples at which the change occurs.
	 * @param index index of the parameter.
	 * @param value new parameter value.
	 */
	virtual BOOL GetParamAt(int offset, int index, float *value) { return FALSE; }

	/**
	 * Removes scheduled parameter change.
	 * Do not override in implementations, handled by the engine.
	 * @param offset position in samples at which the change was supposed to occur.
	 * @param index index of the parameter.
	 */
	virtual BOOL ResetParamAt(int offset, int index, float value) { return FALSE; }

	/**
	 * Enables/disables object at specific offset.
	 * Do not override in implementations, handled by the engine.
	 * @param offset position in samples at which the change was supposed to occur.
	 * @param bEnable enable or disable.
	 */
	virtual void EnableAt(int offset, BOOL bEnable) {}

	virtual void ClearAllParams() {}

	/**
	 * Moves all parameters between start and end sample.
	 * @param start start offset, in samples.
	 * @param end end offset, in samples.
	 * @param offset number of samples. If -1 then the parameters are removed.
	 */
	virtual void MoveAllParamsBetween(int start, int end, int offset) {}

	virtual BOOL MoveParam(int offset, int index, float value, int newoffset) { return FALSE; }
};


/*********************************************************************
 *! \class IFloopyDisplay
 *  \brief Functions used by the GUI.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 14. June 2005.
 *
 *  Not to be directly overriden in implementations.
 *********************************************************************/
class IFloopyDisplay : public IFloopyTimeline
{
public:
	/** Do not override in implementations, handled by the engine */
	virtual char *GetDisplayName()					{ return NULL; }

	/** Do not override in implementations, handled by the engine */
	virtual void  SetDisplayName(char *name, int len){ }

	/** Do not override in implementations, handled by the engine */
	virtual BOOL GetColor(UINT *red, UINT *green, UINT *blue) { return FALSE; }

	/** Do not override in implementations, handled by the engine */
	virtual void  SetColor(UINT red, UINT green, UINT blue){ }
};


/*********************************************************************
 *! \class IFloopy
 *  \brief Main object interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. April 2005.
 *
 *  Not to be directly overriden in implementations.
 *********************************************************************/
class IFloopy : public IFloopyDisplay
{
public:
	IFloopy()			{ m_nLastError = 0; }
	virtual ~IFloopy()	{ }

	/**
	 * Returns class type identificator.
	 * Used for runtime class identification.
	 * Do not override in implementations!!!
	 * @return class type identificator.
	 */
	virtual enumClassType GetType()	{ return TYPE_FLOOPY; }

	// Component description
	virtual char *GetName()			{ return "IFloopy"; }
	virtual char *GetDescription()	{ return "IFloopy interface"; }
	virtual char *GetVersion()		{ return "0.1"; }
	virtual char *GetAuthor()		{ return "sqba"; }

			int   GetLastError()			{ return m_nLastError; }
	virtual char *GetLastErrorDesc()		{ return NULL; }
	//virtual BOOL  GetLastError(char *str, int len)	{ return FALSE; }

	/** Do not override in implementations, handled by the engine */
	virtual char *GetPath()							{ return NULL; }

	virtual BOOL Open(char *filename)				{ return FALSE; }
	virtual void Close()							{ }

//	virtual void SetCallback(IFloopyCallback *cbk) { m_callback = cbk; }
//	IFloopyCallback *m_callback;

protected:
	int m_nLastError;	/** Last error code. */
};


/*********************************************************************
 *! \class IFloopySoundInput
 *  \brief Main sound input interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *
 *  Interface implemented and used by all sound
 *  input and filter (effect) objects.
 *********************************************************************/
class IFloopySoundInput : public IFloopy
{
public:
	IFloopySoundInput() : IFloopy()
	{
		memset(&m_format, 0, sizeof(SOUNDFORMAT));
	}
	virtual ~IFloopySoundInput() {}

	enumClassType GetType()	{ return TYPE_FLOOPY_SOUND_INPUT; }

	// Component description
	char *GetName()			{ return "IFloopySoundInput"; }
	char *GetDescription()	{ return "IFloopySoundInput interface"; }

	/**
	 * Open source file.
	 */
	BOOL Open(char *filename)				{ return FALSE; }

	/**
	 * Return total track size.
	 * @return number of samples.
	 */
	virtual int GetSize()=0;//					{ return 0; }

	/**
	 * Fills given buffer with sound data.
	 * @param data pointer to the buffer to be filled.
	 * @param size size of the buffer in bytes.
	 * @return number of bytes read.
	 */
	virtual int Read(BYTE *data, int size)=0;//	{ return 0; }

	/**
	 * Moves the starting position.
	 * @param samples number of samples to move from the start.
	 */
	virtual void MoveTo(int samples)		{}

	/**
	 * Returns current sample position.
	 */
	virtual int GetPos()					{ return 0; }

	/**
	 * Moves to the starting position.
	 */
	virtual void Reset()					{}

	virtual SOUNDFORMAT *GetFormat()		{ return &m_format; }


	/**
	 * Specifies whether the source should be read and passed on 
	 * if this component is disabled. If false then nothing
	 * will be read.
	 */
	virtual BOOL ReadSourceIfDisabled()	{ return TRUE; }

	// Utility
	/*int SamplesToBytes(int samples)
	{
		SOUNDFORMAT *fmt = GetFormat();
		return samples * ( (fmt->bitsPerSample / 8) * fmt->channels );
	}*/

	/** Do not override in implementations! */
	virtual BOOL IsFilter()				{ return FALSE; }

protected:
	SOUNDFORMAT m_format;
	//int m_pos;
};


/*********************************************************************
 *! \class IFloopySoundFilter
 *  \brief Main sound filter interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 26. june 2005
 *
 *  Interface implemented and used by all sound objects that process
 *  input from some other source instead of generating it.
 *  IFloopySoundInput::IsFilter() function is called by objects that
 *  need to know if there is some other source before this object.
 *********************************************************************/
class IFloopySoundFilter : public IFloopySoundInput
{
public:
	IFloopySoundFilter() : IFloopySoundInput()	{ m_source = NULL; m_bBypass = FALSE; }

	enumClassType GetType()	{ return TYPE_FLOOPY_SOUND_FILTER; }


	/** Do not override in implementations! */
	virtual BOOL IsFilter()				{ return TRUE; }

	/**
	 * Return source size.
	 * @return number of samples.
	 */
	virtual int GetSourceSize()
	{
		return GetSize();
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


	virtual BOOL Open(char *filename)
	{
		return (NULL != m_source ? m_source->Open(filename) : FALSE);
	}

	virtual int GetSize()
	{
		return (NULL != m_source ? m_source->GetSize() : 0);
	}

	virtual int Read(BYTE *data, int size)
	{
		return (NULL != m_source ? m_source->Read(data, size) : 0);
	}

	virtual void MoveTo(int samples)
	{
		if(NULL != m_source) m_source->MoveTo(samples);
	}

	virtual int GetPos()
	{
		return (NULL != m_source ? m_source->GetPos() : 0);
	}

	virtual void Reset()
	{
		if(NULL != m_source) m_source->Reset();
	}

	virtual SOUNDFORMAT *GetFormat()
	{
		return (NULL != m_source ? m_source->GetFormat() : &m_format);
	}

	/**
	 * Bypas the component without affecting the timeline.
	 */
	virtual BOOL GetBypass() { return m_bBypass; }
	virtual void SetBypass(BOOL bBypass) { m_bBypass = bBypass; }

protected:
	IFloopySoundInput *m_source;
	BOOL m_bBypass;
};


/*********************************************************************
 *! \class IFloopySoundMixer
 *  \brief Main mixer interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 20. june 2005
 *
 *  Derived from IFloopySoundInput, IFloopySoundMixer differs from it
 *  just in the fact that it can have several inputs.
 *********************************************************************/
class IFloopySoundMixer : public IFloopySoundFilter
{
public:
	char *GetName()			{ return "IFloopySoundMixer"; }
	char *GetDescription()	{ return "IFloopySoundMixer interface"; }

	enumClassType GetType()	{ return TYPE_FLOOPY_SOUND_MIXER; }

	virtual int AddSource(IFloopySoundInput *src)
	{
		if(NULL != m_source)
		{
			m_source = src;
			return 0;
		}
		else
			return -1;
	}

	virtual IFloopySoundInput *GetSource(int index)
	{
		return (index > 0 ? NULL : m_source);
	}

	virtual bool RemoveSource(IFloopySoundInput *src)	{ return false; }

	virtual int GetInputCount()
	{
		return (NULL != m_source ? 1 : 0);
	}
};


/*********************************************************************
 *! \class IFloopySoundOutput
 *  \brief Main sound output interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *
 *  Interface implemented and used by all sound output objects
 *  such as file writers and encoders.
 *********************************************************************/
class IFloopySoundOutput : public IFloopy
{
public:
	IFloopySoundOutput() : IFloopy()				{ m_dest = NULL; }
	IFloopySoundOutput(SOUNDFORMAT fmt) : IFloopy() { m_dest = NULL; }
	virtual ~IFloopySoundOutput() {}

	enumClassType GetType()	{ return TYPE_FLOOPY_SOUND_OUTPUT; }

	char *GetName()			{ return "IFloopySoundOutput"; }
	char *GetDescription()	{ return "IFloopySoundOutput interface"; }

	/**
	 * Opens destination (file).
	 */
	BOOL Open(char *filename)
	{
		return (NULL != m_dest ? m_dest->Open(filename) : FALSE);
	}

	/**
	 * Closes destination (file).
	 */
	void Close()
	{
		if(NULL != m_dest)
			m_dest->Close();
	}

	/**
	 * Writes a buffer to the output, whatever it may be.
	 * @param size size of the data buffer in bytes.
	 * @param data pointer to the buffer to be written into output.
	 * @return number of bytes read from data into output.
	 */
	virtual int Write(BYTE *data, int size)
	{
		return (NULL != m_dest ? m_dest->Write(data, size) : 0);
	}

	virtual void SetDest(IFloopySoundOutput *dst)	{ m_dest = dst; }
	virtual IFloopySoundOutput *GetDest()			{ return m_dest; }

	virtual int GetWrittenSamples()
	{
		return (NULL != m_dest ? m_dest->GetWrittenSamples() : 0);
	}

	virtual void Reset()
	{
		if(NULL != m_dest)
			m_dest->Reset();
	}

private:
	IFloopySoundOutput *m_dest;
};


typedef void (*UpdateCallback)(IFloopy *src, int offset, int param);


/*********************************************************************
 *! \class IFloopyEngine
 *  \brief Floopy engine interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *
 *  Floopy engine is the main object responsible for creation and
 *  destruction of plugins, as well as all parameter changes on
 *  the timeline.
 *********************************************************************/
class IFloopySoundEngine : public IFloopySoundFilter
{
public:
	enumClassType GetType() { return TYPE_FLOOPY_SOUND_ENGINE; }

	char *GetName()			{ return "IFloopySoundEngine"; }
	char *GetDescription()	{ return "IFloopySoundEngine interface"; }

	/**
	 * Creates a sound source or filter.
	 * @param name Component name (without extension).
	 */
	virtual IFloopySoundInput  *CreateInput(char *name)
	{ return NULL; }

	/**
	 * Creates a sound source or filter with apropriate convertors.
	 * @param name Component name (without extension).
	 */
	virtual IFloopySoundInput  *CreateTrack(char *name)
	{ return NULL; }

	/**
	 * Creates a sound output.
	 * @param name Component name (without extension).
	 */
	virtual IFloopySoundOutput *CreateOutput(char *name, SOUNDFORMAT fmt)
	{ return NULL; }

	/**
	 * Destroys the object created by CreateInput() or CreateOutput().
	 * @param obj Pointer to the object being deleted.
	 */
	virtual void DeleteObject(IFloopy *obj) {}

	/**
	 * Serializes current engine state into a file.
	 * @param filename Project file name.
	 */
	virtual BOOL Save(char *filename) { return FALSE; }

	/**
	 * Sets the callback function that is called on parameter changes.
	 * @param func Pointer to the callback function.
	 */
	virtual void RegisterUpdateCallback(UpdateCallback func) {}

	/**
	 * Fills given buffer with silence.
	 * @param data pointer to the buffer to be filled.
	 * @param size size of the buffer in bytes.
	 * @return number of bytes written.
	 */
	virtual int EmptyBuffer(BYTE *data, int size) { return 0; }
};


#endif //_IFLOOPY_H_
