#ifndef TimeRuler_H
#define TimeRuler_H

#include "../rulerview.h"
#include "../../engine/tracks.h"

class CTimeRuler : public CHorizontalRuler  
{
public:
	CTimeRuler(CRulerView *parent, CTracks *tracks);
	virtual ~CTimeRuler();

	void OnDraw(wxDC &dc);

private:
	void FormatTime(float fSec, wxString &csTime);

private:
	CTracks *m_pTracks;
};

#endif // #ifndef TimeRuler_H
