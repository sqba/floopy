#ifndef RegionDisplay_H
#define RegionDisplay_H


#include "floopyobj.h"
#include "ObjTypes.h"


struct Peak
{
	short int prev;
	short int value;
	int pos;			// Sample offset
};


WX_DECLARE_OBJARRAY(Peak, PeaksArray);


class CRegion;

class CRegionDisplay : public IFloopyObj
{
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
	void loadPeaks();
	void loadPeaksChunked();
	int getLengthNotLooped();
	int getMaxSampleValue();

private:
	IFloopySoundInput	*m_pInput;
//	IFloopySoundInput	*m_pTrackInput;

	CRegion		*m_pRegion;
	bool		m_bLoaded;
	PeaksArray	m_peaks;

	CTrack		*m_pTrack;
	CTracks		*m_pTracks;

	bool		m_bDrawVertical;

	wxMemoryDC	m_tempDC;
	wxBitmap	m_tempBitmap;
	bool		m_bRepaint;

	// Properties
	float		m_fdB;
	bool		m_bDrawDBLines;
	bool		m_bDrawContour;
};


#endif // #ifndef RegionDisplay_H
