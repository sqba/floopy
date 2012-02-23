#ifndef _REGION_H_
#define _REGION_H_

#include "object.h"

class CRegion : public CObject
{
public:
    CRegion(CObject *pTrack);
    virtual ~CRegion();

	void DrawFrame();
};

#endif // _REGION_H_
