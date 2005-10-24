#include "../../../ifloopy.h"
#include "../FloopyControl.h"
#include "floopyobj.h"
#include "ObjTypes.h"
#include "PathCtrl.h"

class CTrack;

class CLabel : public IFloopyObj
{
public:
	/*! \class CLoopButton
	 *  \brief 
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *
	 *  
	 */
	class CLoopButton : public CFloopyControl
	{
	public:
		CLoopButton(CTrack *track) : CFloopyControl(track) {}
		virtual ~CLoopButton() {}

		void DrawFore(wxDC& dc, wxRect& rc);

	private:
		CTrack *getTrack() { return (CTrack*)GetParent(); }
	};

	/*! \class CCacheButton
	 *  \brief 
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *
	 *  
	 */
	class CCacheButton : public CFloopyControl
	{
	public:
		CCacheButton(CTrack *track) : CFloopyControl(track) {}
		virtual ~CCacheButton() {}

		void DrawFore(wxDC& dc, wxRect& rc);

	private:
		CTrack *getTrack() { return (CTrack*)GetParent(); }
	};

public:
	CLabel(CTrack *track);
	virtual ~CLabel();

	void DrawBG   (wxDC& dc, wxRect& rc);
	void DrawFore (wxDC& dc, wxRect& rc);

	bool HitTest(int y);

private:
	inline CTrack  *getTrack()	{ return (CTrack*)GetParent(); }
	void drawLoopSign(wxDC& dc, wxRect& rc);
	void drawCacheSign(wxDC& dc, wxRect& rc);

private:
	wxRect m_rcLabel;
//	CLoopButton		*m_pButtonLoop;
//	CCacheButton	*m_pButtonCache;
	CPathCtrl		*m_pPathCtrl;
};
