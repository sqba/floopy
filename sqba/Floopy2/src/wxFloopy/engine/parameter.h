#ifndef Parameter_H
#define Parameter_H


#include "floopyobj.h"
#include "ObjTypes.h"


#define SIZE_VALUE	0
#define SIZE_OFFSET	1
#define SIZE_ALL	2


class CRegion;


class CParameter : public IFloopyObj
{
//	DECLARE_DYNAMIC_CLASS(CParameter)

public:
	class CPoint : public IFloopyObj
	{
	public:
		CPoint(CParameter *parameter);
		virtual ~CPoint() {}

		wxCursor GetCursor();

		void Move(int dx, int dy);

		void Select(bool selected=true);

		IFloopySoundInput *m_pInput;
		CParameter *m_pParameter;
		int		m_offset;
		int		m_samplesPerPixel;
		float	m_fScale;
		int		m_index;
		float	m_value;
		int		m_sizing;
	};

public:
	CParameter(CRegion*, IFloopySoundInput*, int, bool, wxColor color=*wxCYAN);
	virtual ~CParameter();

	int GetType()	{ return FLOOPY_PARAMETER; }

	void Refresh();
	//void Invalidate();

	IFloopyObj *GetChildAt(int x, int y);
	void DrawFore(wxDC& dc, wxRect& rc);
//	float GetValueAt(int x);
	bool OnKeyDown(wxKeyEvent& event);
	void insertParam(int x, int y);
	bool HitTest(int x, int y);

	IFloopySoundInput *GetInput() { return m_pInput; }

private:
	CRegion	*m_pRegion;
	CTrack	*m_pTrack;
	CTracks	*m_pTracks;
	int		m_index;
//	bool	m_bEdit;
//	int		m_iSamplePos, m_iPrevSamplePos;
	CPoint	*m_pPoint;
	IFloopySoundInput *m_pInput;
	bool	m_bAfterTrack;
	float	m_fScale;
	float	m_fMin, m_fMax;
	int		m_iSamplesPerPixel;
};


#endif // #ifndef Parameter_H
