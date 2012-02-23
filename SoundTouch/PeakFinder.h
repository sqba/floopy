/******************************************************************************
 *
 * Peak detection routine. Detects highest value on an array of values and 
 * calculates the precise peak location as a mass-center of the peak 'hump'.
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

#ifndef _PeakFinder_H_
#define _PeakFinder_H_

class PeakFinder
{
protected:
    int minPos, maxPos;

    float calcMassCenter(const float *data, int firstPos, int lastPos) const;
    int   findCrossingLevel(const float *data, float level, int peakpos, int direction) const;
    int   findGround(const float *data, int peakpos, int direction) const;

public:
	PeakFinder(int minPos, int maxPos);

	float detectPeak(const float *data) const;
};

#endif // _PeakFinder_H_
