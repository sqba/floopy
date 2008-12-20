
#include "util.h"


/////////////////////////////////////////////////////////////////////////////
// FormatTime
//! Formats a float representing time in seconds into string (CString).
//! @param fSec [in] time in seconds
//! @param csTime [out] reference to the wxString class.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void FormatTime(float fSec, wxString &csTime)
{
	float min = fSec>60.f ? fSec/60.f : 0.f;
	float sec = min>1.f ? fSec-(int)min*60 : fSec;
	float ms  = sec*1000.f - (int)sec*1000;

	csTime.Printf(_T("%2.2d:%2.2d:%3.3d"), (int)min, (int)sec, (int)ms);
}
