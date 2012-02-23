#include "object.h"
#include "region.h"
#include <vector>

class CTrack : public CObject
{
public:
    CTrack(CObject *pEngine);
    virtual ~CTrack();

	CRegion *AddRegion();
	void DrawFrame();

private:
    std::vector<CRegion> m_Regions;
};
