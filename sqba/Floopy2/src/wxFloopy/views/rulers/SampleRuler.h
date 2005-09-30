#ifndef SAMPLE_RULER_H
#define SAMPLE_RULER_H

#include "../rulerview.h"
#include "../../engine/tracks.h"

class CSampleRuler : public CHorizontalRuler  
{
public:
	CSampleRuler(CRulerView *parent, CTracks *tracks);
	virtual ~CSampleRuler();

	void OnDraw(wxDC &dc);

private:
	void drawFrame(wxDC &dc);

private:
	CTracks *m_pTracks;
};

#endif // #ifndef SAMPLE_RULER_H
