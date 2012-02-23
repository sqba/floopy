#ifndef TimeRuler_H
#define TimeRuler_H

#include "floopyhruler.h"

class CTimeRuler : public CFloopyHRuler
{
public:
	CTimeRuler(CRulerView *parent, CTracks *tracks);
	virtual ~CTimeRuler();

	void OnDraw(wxDC &dc);
};

#endif // #ifndef TimeRuler_H
