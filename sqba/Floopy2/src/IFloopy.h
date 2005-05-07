#ifndef _IFLOOPY_H_
#define _IFLOOPY_H_

//#include "floopyutil.h"



//! Portable waveform structure
typedef struct structWAVFormat
{
	INT   freq;		// Sampling frequency
	WORD  format;
	BYTE  channels;	// Mono/Stereo - 1/2
	BYTE  silence;
	WORD  samples;
	DWORD size;
} WAVFORMAT;

/*
//! Object types definition, used for runtime identification
enum objType
{
	typeFloopy,
	typeFloopySound,
	typeFloopySoundInput,
	typeFloopySoundOutput,
	typeFloopyEngine
//	typeFloopySoundMixer,
//	typeFloopyEvents,
//	typeFloopyParameters,
//	typeFloopySoundEffect,
//	typeFloopyGUI,
//	typeFloopyEvent,
//	typeFloopyTrack,
//	typeFloopyTracks,
//	typeFloopyGUIApp
};
*/


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
//	IFloopy() { m_config = NULL; }
	IFloopy() { m_bEnabled = TRUE; }
	virtual ~IFloopy() {}

	//! Used for runtime identification, do not override in plugins!
//	virtual int GetType()			{ return typeFloopy; }

	virtual char *GetName()			{ return "IFloopy"; }
	virtual char *GetDescription()	{ return "IFloopy interface"; }
	virtual char *GetVersion()		{ return "0.1"; }
	virtual char *GetAuthor()		{ return "sqba"; }

	virtual int   GetParamCount()			{ return 0; }
	virtual void  SetParam(int index, float value) { }
	virtual float GetParam(int index)		{ return 0.f; }
	virtual char *GetParamName(int index)	{ return NULL; }
	virtual char *GetParamDesc(int index)	{ return NULL; }

	virtual void Enable(BOOL bEnabled) { m_bEnabled = bEnabled; }
	virtual BOOL IsEnabled() { return m_bEnabled; }

//	virtual BOOL Initialize(IFloopyConfig *config) { m_config = config; return true; }
//	virtual void UnInitialize() {}

//	IFloopyConfig *GetConfig() { return m_config; }

	//virtual void SetSource(IFloopy *src) {}

private:
	BOOL m_bEnabled;
//	IFloopyConfig *m_config;

protected:
//	int m_type = objFloopy;

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
//	virtual int GetType() { return typeFloopySound; }

	virtual char *GetName()			{ return "IFloopySound"; }
	virtual char *GetDescription()	{ return "IFloopySound interface"; }
	virtual char *GetVersion()		{ return "0.1"; }
	virtual char *GetAuthor()		{ return "sqba"; }

//	BOOL SetSource(IFloopySound *src) { m_source = src; return true; }
//	IFloopySound *GetSource() { return m_source; }

	//virtual void operator << (IFloopy *src) { SetSource(src); }
	//virtual void operator >> (IFloopy *dst) { dst->SetSource(this); }
	//virtual void operator = (const IFloopy& copy) 

//private:
//	IFloopySound *m_source = NULL;
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
		//m_pos = 0;
		memset(&m_wavformat, 0, sizeof(WAVFORMAT));
	}
	virtual ~IFloopySoundInput() {}

	//! Used for runtime identification, do not override in plugins!
//	int GetType() { return typeFloopySoundInput; }

	virtual char *GetName()			{ return "IFloopySoundInput"; }
	virtual char *GetDescription()	{ return "IFloopySoundInput interface"; }
	virtual char *GetVersion()		{ return "0.1"; }
	virtual char *GetAuthor()		{ return "sqba"; }

	/**
	 * Opens source file.
	 */
	virtual BOOL Open(char *filename)
	{
		return (NULL != m_source ? m_source->Open(filename) : FALSE);
	}

	/**
	 * Opens source file.
	 */
	virtual void Close()
	{
		if(NULL != m_source)
			m_source->Close();
	}

	/**
	 * Returns total sample count.
	 * @return number of samples.
	 */
	virtual UINT GetSize()
	{
		return (NULL != m_source ? m_source->GetSize() : 0);
	}

	/**
	 * Sets total sample count.
	 * @param size number of samples.
	 */
	virtual void SetSize(DWORD size)
	{
		if(NULL != m_source)
			m_source->SetSize(size);
	}
/*
	virtual FLOAT GetLength()
	{
		return (NULL != m_source ? m_source->GetLength() : 0.f);
	};
*/
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
	virtual UINT Read(BYTE *data, UINT size)
	{
		//UINT len = (NULL != m_source ? m_source->Read(data, size) : 0);
		//m_pos += len;
		//return len;
		return (NULL != m_source ? m_source->Read(data, size) : 0);
	}

	/**
	 * Moves the starting position.
	 * @param samples number of samples to move from the start.
	 */
	virtual void MoveTo(UINT samples)
	{
		if(NULL != m_source)
			m_source->MoveTo(samples);
		//WAVFORMAT *fmt = GetFormat();
		//m_pos = samples * ( (fmt->size / 8) * fmt->channels );
	}

	virtual UINT GetPos()
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

	virtual WAVFORMAT *GetFormat()
	{
		return (NULL != m_source ? m_source->GetFormat() : &m_wavformat);
	}

	virtual BOOL SetFormat(WAVFORMAT *fmt)
	{
		if(NULL != m_source)
			return m_source->SetFormat(fmt);
		else
		{
			memcpy(&m_wavformat, fmt, sizeof(WAVFORMAT));
			return TRUE;
		}
	}

	// Utility
	/*UINT SamplesToBytes(UINT samples)
	{
		WAVFORMAT *fmt = GetFormat();
		return samples * ( (fmt->size / 8) * fmt->channels );
	}*/


	// Mixer specific functions
	virtual int AddSource(IFloopySoundInput *src)
	{
		return (NULL != m_source ? m_source->AddSource(src) : 0);
	}

	virtual IFloopySoundInput *GetSource(int index)
	{
		//return (NULL != m_source ? m_source->GetSource(index) : NULL);
		return (index > 0 ? NULL : m_source);
	}

	virtual void RemoveSource(IFloopySoundInput *src)
	{
		if(NULL != m_source)
			m_source->RemoveSource(src);
	}

	virtual int GetInputCount()
	{
		//return (NULL != m_source ? m_source->GetInputCount() : 0);
		return (NULL != m_source ? 1 : 0);
	}

protected:
	IFloopySoundInput *m_source;
	WAVFORMAT m_wavformat;
	//UINT m_pos;
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
//	IFloopySoundOutput(IFloopySoundInput *source) {}
	IFloopySoundOutput(int nSamplesPerSec, int wBitsPerSample, int nChannels) {}
	virtual ~IFloopySoundOutput() {}

	//! Used for runtime identification, do not override in plugins!
//	int GetType() { return typeFloopySoundOutput; }

	virtual char *GetName()			{ return "IFloopySoundOutput"; }
	virtual char *GetDescription()	{ return "IFloopySoundOutput interface"; }
	virtual char *GetVersion()		{ return "0.1"; }
	virtual char *GetAuthor()		{ return "sqba"; }

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

	virtual void SetDest(IFloopySoundOutput *dst)
	{
		m_dest = dst;
	}

	virtual IFloopySoundOutput *GetDest()
	{
		return m_dest;
	}

	/**
	 * Writes a buffer to the output, whatever it may be.
	 * @param size size of the data in bytes.
	 * @param data pointer to the buffer written into output.
	 * @return number of bytes read from data into output.
	 */
	virtual UINT Write(BYTE *data, UINT size)
	{
		return (NULL != m_dest ? m_dest->Write(data, size) : 0);
	}

protected:
	IFloopySoundOutput *m_dest;
};


class IFloopyEngine : public IFloopySoundInput
{
public:
	//! Used for runtime identification, do not override in plugins!
//	int GetType() { return typeFloopyEngine; }

	virtual char *GetName()			{ return "IFloopySoundEngine"; }
	virtual char *GetDescription()	{ return "IFloopySoundEngine interface"; }
	virtual char *GetVersion()		{ return "0.1"; }
	virtual char *GetAuthor()		{ return "sqba"; }

	virtual IFloopySoundInput  *CreateInput(char *plugin)  { return NULL; }
	virtual IFloopySoundOutput *CreateOutput(char *plugin, WAVFORMAT *fmt) { return NULL; }
};


/*class IFloopySoundMixer : public IFloopySoundInput
{
public:
	IFloopySoundMixer() { m_source = NULL; }
	virtual ~IFloopySoundMixer() {}

	//! Used for runtime identification, do not override in plugins!
	int GetType() { return typeFloopySoundMixer; }

	virtual char *GetName()			{ return "IFloopySoundMixer"; }
	virtual char *GetDescription()	{ return "IFloopySoundMixer interface"; }
	virtual char *GetVersion()		{ return "0.1"; }
	virtual char *GetAuthor()		{ return "sqba"; }

	virtual int AddSource(IFloopySoundInput *src)
	{
		// Add to the list and return index
		//m_last = new CInput(src, m_last);
		m_source = src;
		return 0;
	}

	virtual IFloopySoundInput *GetSource(int index)
	{
		return m_source;
	}

	virtual void RemoveSource(IFloopySoundInput *src)
	{
		// Remove from the list
		m_source = NULL;
	}

	virtual void RemoveSource(int index)
	{
		// Remove from the list
		m_source = NULL;
	}

	virtual int GetInputCount() { return (m_source ? 1 : 0); }

private:
//	CInput *m_last;
};*/

/*class IFloopyParameters// : public IFloopy
{
public:
	//! Used for runtime identification, do not override in plugins!
	virtual int GetType() { return typeFloopyParameters; }
	
	virtual int GetCount() { return 0; }
	virtual void Set(int index, float value) { }
	virtual float Get(int index) { return 0.f; }
};*/

/*
class IFloopyEvents// : public IFloopy
{
public:
	//! Used for runtime identification, do not override in plugins!
	virtual int GetType() { return typeFloopyEvents; }
	
	virtual int GetCount() { return 0; }
	virtual int Add(UINT position, IFloopy *src, int paramIndex, float paramValue)
	{ return -1; }

	virtual BOOL Get(int index, UINT *position, IFloopy **src, int *paramIndex, float *paramValue)
	{ return 0; }

	virtual void Remove(int index) { }
};
*/

/*********************************************************************
 *! \class IFloopyGUI
 *  \brief Main GUI object interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *
 *  Interface implemented and all objects used by the GUI.
 *********************************************************************/
/*class IFloopyGUI : public IFloopy
{
public:
	IFloopyGUI()
	{
		m_enabled  = TRUE;
		m_selected = FALSE;
		m_parent = NULL;
		m_colour.R = m_colour.G = m_colour.B = 255;
	}
	virtual ~IFloopyGUI() {}

	//! Used for runtime identification, do not override in plugins!
	virtual int GetType() { return typeFloopyGUI; }

	BOOL IsEnabled() { return m_enabled; }
	void Enable(BOOL enable) { m_enabled = enable; }

	BOOL IsSelected() { return m_selected; }
	void Select(BOOL sel) { m_selected = sel; }

	IFloopyGUI *GetParent() { return m_parent; }
	void SetParent(IFloopyGUI *parent) { m_parent = parent; }

	COLOUR &GetColour() { return m_colour; }
	void SetColour(COLOUR colour) { m_colour = colour; }

public:
	virtual int GetChildCount() { return 0; }
	virtual IFloopyGUI *GetChild(int index) { return NULL; }

	virtual void DrawBack(HDC hDC, RECT *rect, int pps) {}
	virtual void DrawFore(HDC hDC, RECT *rect, int pps) {}

private:
	BOOL m_enabled, m_selected;
	IFloopyGUI *m_parent;
	COLOUR m_colour;
};*/


/*********************************************************************
 *! \class IFloopyGUIApp
 *  \brief Interface of the main GUI application object.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *********************************************************************/
/*class IFloopyGUIApp : public IFloopyGUI
{
public:
	IFloopyGUIApp() {}
	virtual ~IFloopyGUIApp() {}

	//! Used for runtime identification, do not override in plugins!
	virtual int GetType() { return typeFloopyGUIApp; }

	//! Instantiates main GUI application
	virtual void Run(IFloopyTracks *tracks) { m_tracks = tracks; }

	virtual void OnDisplayText(char *text) {}
	virtual void OnSetProgress(int pos, int range) {}
	virtual void OnUpdate(IFloopy *pSender) {}

private:
	IFloopyTracks *m_tracks;
};*/

/*********************************************************************
 *! \class IFloopyEvent
 *  \brief Event object interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *********************************************************************/
/*class IFloopyEvent : public IFloopyGUI
{
public:
	IFloopyEvent() { m_start = m_end = 0.f; }
	virtual ~IFloopyEvent() {}

	//! Used for runtime identification, do not override in plugins!
	virtual int GetType() { return typeFloopyEvent; }

private:
	//IFloopyTrack *m_track = NULL; // parent
	float m_start, m_end;
};*/


/*********************************************************************
 *! \class IFloopyTrack
 *  \brief Track object interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *********************************************************************/
/*class IFloopyTrack : public IFloopyGUI
{
public:
	IFloopyTrack() { m_source = NULL; m_height = 50; }
	virtual ~IFloopyTrack() {}

	//! Used for runtime identification, do not override in plugins!
	virtual int GetType() { return typeFloopyTrack; }

	void SetSource(IFloopySoundInput *src) { m_source = src; }
	IFloopySoundInput *GetSource() { return m_source; }

	int GetHeight() { return m_height; }
	void SetHeight(int height) { m_height = height; }

	virtual int GetEventCount() { return 0; }
	virtual IFloopyEvent *GetEvent(int index) { return NULL; }

private:
	IFloopySoundInput *m_source;
	int m_height; //! Track height in pixels
};*/


/*********************************************************************
 *! \class IFloopyTracks
 *  \brief Tracks object interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *********************************************************************/
/*class IFloopyTracks : public IFloopyGUI
{
public:
	IFloopyTracks() { m_pps = 16; }
	virtual ~IFloopyTracks() {}

	//! Used for runtime identification, do not override in plugins!
	virtual int GetType() { return typeFloopyTracks; }

	virtual int GetCount() { return 0; }
	virtual IFloopyTrack *GetTrack(int index) { return NULL; }

	int GetVRes() { return m_pps; }

	virtual void Play()  {}
	virtual void Pause() {}
	virtual void Stop()  {}
	virtual void Reset() {}

	virtual void OnUpdate() {}

	virtual float GetCursorPosition() { return 0.f; }
	virtual void SetCursorPosition(float time) {}

	virtual float GetTempo() { return 0.f; }
	virtual void SetTempo(float fTempo) {}

private:
	int m_pps; //! Vertical resolution - pixels per second
};*/


/*********************************************************************
 *! \class IFloopySoundEffect
 *  \brief Main sound effect interface.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 13. april 2005
 *
 *  Interface implemented and used by sound objects such as effects
 *	and visualisation
 *********************************************************************/
/*class IFloopySoundEffect : public IFloopySound
{
public:
	IFloopySoundOutput() {}
	virtual ~IFloopySoundOutput() {}

	//! Used for runtime identification, do not override in plugins!
	virtual int GetType() { return typeFloopySoundEffect; }
*/
	/**
	 * Processes the buffer.
	 * @param data pointer to the buffer to be filled.
	 * @param size size of the data in bytes.
	 * return 
	 */
/*	virtual BOOL Process(BYTE *data, UINT size) { return FALSE; };
};*/

/*
class IFloopyGraphics : public IFloopy
{
public:
	virtual void SetSource(IFloopySoundInput *src) {}

	virtual void DrawBG  (HDC hDC, RECT *rect, int pps) {}
	virtual void DrawFore(HDC hDC, RECT *rect, int pps) {}
};
*/

#endif //_IFLOOPY_H_
