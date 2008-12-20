#ifndef RegionDisplay_H
#define RegionDisplay_H


#include "floopyobj.h"
#include "objtypes.h"


#define SAMPLE	short int


class CRegion;


class CRegionDisplay : public IFloopyObj
{
private:
	struct Peak
	{
		SAMPLE	value1;
		SAMPLE	value2;
		int		pos;	/// Sample offset
		Peak	*next;
	};

public:
	CRegionDisplay(CRegion *region);
	virtual ~CRegionDisplay();

	int GetType()	{ return FLOOPY_REGION_DISP; }

	void DrawBG(wxDC& dc, wxRect& rc);
	void DrawFore(wxDC& dc, wxRect& rc);

	void LoadPeaks();

	int   GetPropertyCount();
	bool  GetPropertyVal(int index, float *value);
	void  SetPropertyVal(int index, float value);
	char *GetPropertyName(int index);
	char *GetPropertyDesc(int index);
	float GetPropertyMax(int index);
	float GetPropertyMin(int index);
	char *GetPropertyUnit(int index);
	float GetPropertyStep(int index);

private:
	void drawDBLines(wxDC& dc, wxRect& rc);
	void drawCenterLine(wxDC& dc, wxRect& rc);
	void drawPeaks(wxDC& dc, wxRect& rc, int start);
	int getMaxSampleValue();
	void add(CRegionDisplay::Peak *peak);
	void clear();

private:
	IFloopySoundInput	*m_pInput;

	CRegion		*m_pRegion;
	bool		m_bLoaded;

	CTrack		*m_pTrack;
	CTracks		*m_pTracks;

	bool		m_bDrawVertical;

	wxMemoryDC	m_tempDC;
	wxBitmap	m_tempBitmap;
	bool		m_bRepaint;

	Peak		*m_pFirstPeak;
	Peak		*m_pLastPeak;
	int			m_iPeakCount;

	// Properties
	float		m_fdB;
	bool		m_bDrawDBLines;
	bool		m_bDrawContour;
	int			m_iBufferLength;
};


#endif // #ifndef RegionDisplay_H
