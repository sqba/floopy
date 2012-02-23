#ifndef PlayThread_H
#define PlayThread_H


#include "../../../ifloopy.h"
#include "../FloopyControl.h"
#include "floopyobj.h"
#include "ObjTypes.h"


class CPlayThread : public wxThread  
{
public:
	CPlayThread(CTracks *pTracks);
	virtual ~CPlayThread();

	void *Entry();

	bool Play(int sample);
	bool Pause();
	void Stop();

	void OnExit();

	bool IsPlaying();
	bool IsPaused();

	int GetPosition();

	void SetStartPos(int pos);

private:
	CTracks *m_pTracks;
	int m_iStartPos;
	bool m_bPlaying, m_bPaused;
	IFloopySoundOutput	*m_pOutput;
	IFloopySoundInput	*m_pInput;
	IFloopySoundEngine	*m_pEngine;
	int m_iBufferLength; // In samples
	int m_iPosition; // In samples
	int m_iSamplesToBytes;
};


#endif // #ifndef PlayThread_H
