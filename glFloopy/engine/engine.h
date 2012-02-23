#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "object.h"
#include "track.h"
#include <vector>

class CEngine : public CObject
{
public:
    CEngine();
    virtual ~CEngine();

    void DrawFrame();
    CTrack *AddTrack();

private:
    std::vector<CTrack> m_Tracks;
};

#endif // _ENGINE_H_

