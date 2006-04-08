// FloopyHRuler.h: interface for the CFloopyHRuler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOOPYHRULER_H__4778C646_CA6A_4FD2_8B07_C077DD680589__INCLUDED_)
#define AFX_FLOOPYHRULER_H__4778C646_CA6A_4FD2_8B07_C077DD680589__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../rulerview.h"
#include "../../engine/tracks.h"
#include "../../util/util.h"

class CFloopyHRuler : public CHorizontalRuler  
{
public:
	CFloopyHRuler(CRulerView *parent, CTracks *tracks);
	virtual ~CFloopyHRuler();

	void OnDrawBG(wxDC &dc);

protected:
	CTracks		*m_pTracks;
	bool		m_bDrawAquaBG;
};

#endif // !defined(AFX_FLOOPYHRULER_H__4778C646_CA6A_4FD2_8B07_C077DD680589__INCLUDED_)
