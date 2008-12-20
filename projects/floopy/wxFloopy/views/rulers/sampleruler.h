#ifndef SAMPLE_RULER_H
#define SAMPLE_RULER_H

#include "floopyhruler.h"

class CSampleRuler : public CFloopyHRuler
{
public:
	CSampleRuler(CRulerView *parent, CTracks *tracks);
	virtual ~CSampleRuler();

	void OnDraw(wxDC &dc);
};

#endif // #ifndef SAMPLE_RULER_H
