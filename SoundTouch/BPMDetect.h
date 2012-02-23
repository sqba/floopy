/******************************************************************************
 *
 * Beats-per-minute (BPM) detection routine
 *
 * Author        : Copyright (c) Olli Parviainen 2003
 * Author e-mail : oparviai@iki.fi
 * File created  : 11-Jan-2003
 * Last modified : 12-Jan-2003
 *
 * License :
 *
 *  This file is part of SoundTouch sound processing library.
 *
 *  SoundTouch is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SoundTouch is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SoundTouch; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****************************************************************************/

#ifndef _BPMDetect_H_
#define _BPMDetect_H_

#define MIN_BPM 45      // max. allowed BPM
#define MAX_BPM 230     // min. allowed BPM

#include "../ifloopy.h"

class FIFOSampleBuffer;
class PeakFinder;

class BPMDetect
{
protected:
    float *xcorr;
    float envelopeAccu;
    float RMSVolumeAccu;

    int decimateCount;
    int decimateSum;
    int decimateBy;

    int windowLen;
    int windowStart;
 
    FIFOSampleBuffer *buffer;

    void init(IFloopySoundInput *source);
    void updateXCorr(const int process_slots);
    int decimate(short *dest, const short *src, int numsamples);
    int inputSamples(IFloopySoundInput *source, short *outBuffer);
    void processBlock(short *samples, int numSamples);
    void calcEnvelope(short *samples, int numsamples);

public:

	BPMDetect();
	virtual ~BPMDetect();

	float detectBpm(IFloopySoundInput *source);
};

#endif // _BPMDetect_H_
