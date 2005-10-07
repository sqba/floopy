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

#include <math.h>
#include <assert.h>
#include <string.h>
#include "SoundTouch/include/FIFOSampleBuffer.h"
//#include "WavFile.h"
#include "PeakFinder.h"
#include "BPMDetect.h"

#define INPUT_BLOCK_SAMPLES       2048
#define DECIMATED_BLOCK_SAMPLES   1024

typedef unsigned short ushort;
typedef unsigned long ulong;



BPMDetect::BPMDetect()
{
    xcorr = NULL;

    buffer = new FIFOSampleBuffer();

    decimateSum = 0;
    decimateCount = 0;
    decimateBy = 0;

    envelopeAccu = 0;
    RMSVolumeAccu = (3000 * 3000) / (1 - .99986f);   // initialize RMS value to 3000
}



BPMDetect::~BPMDetect()
{
    delete[] xcorr;
    delete buffer;
}


// low-pass filter & decimate to about 500 Hz. return number of outputted samples.
//
// Decimation is used to remove the unnecessary frequencies and thus to reduce 
// the amount of data needed to be processed as calculating autocorrelation 
// function is a very-very heavy operation.
//
// Anti-alias filtering is done simply by averaging the samples. This is really a 
// poor-man's anti-alias filtering, but it's not so critical in this kind of application
// (it'd also be difficult to design a filter with steep cut-off at very narrow band)
int BPMDetect::decimate(short *dest, const short *src, int numsamples)
{
    int count, outcount, iout;

    assert(decimateBy != 0);
    outcount = 0;
    for (count = 0; count < numsamples; count ++) 
    {
        decimateSum += src[count];

        decimateCount ++;
        if (decimateCount >= decimateBy) 
        {
            // Store every Nth sample only
            iout = decimateSum / decimateBy;
            decimateSum = 0;
            decimateCount = 0;
            // check ranges (shouldn't be necessary)
            if (iout > 32767) 
            {
                iout = 32767;
            } 
            else if (iout < -32768) 
            {
                iout = -32768;
            }
            dest[outcount] = iout;
            outcount ++;
        }
    }
    return outcount;
}



// Calculates autocorrelation function of the sample history buffer
void BPMDetect::updateXCorr(const int process_slots)
{
    int offs;
    short *pBuffer;
    
    assert(buffer->numSamples() >= (uint)(process_slots + windowLen));

    pBuffer = buffer->ptrBegin();
    for (offs = windowStart; offs < windowLen; offs ++) 
    {
        ulong sum;
        int i;

        sum = 0;
        for (i = 0; i < process_slots; i ++) 
        {
            sum += pBuffer[i] * pBuffer[i + offs];    // scaling the sub-result shouldn't be necessary
        }
//        xcorr[offs] *= xcorr_decay;   // decay 'xcorr' here with suitable coefficients 
                                        // if it's desired that the system adapts automatically to
                                        // various bpms, e.g. in processing continouos music stream
                                        // (notice that decay should depend on 'process_slots' value)

        xcorr[offs] += sum;
    }
}



// Calculates envelope of the sample data
void BPMDetect::calcEnvelope(short *samples, int numsamples) 
{
    const float decay = 0.7f;               // decay constant for smoothing the envelope
    const float norm = (1 - decay);

    const float avgdecay = 0.99986f;        // decay constant for calculating sliding RMS volume 
                                            // (time constant of about 10 sec)
    const float avgnorm = (1 - avgdecay);

    int i;
    int out;
    float val;

    for (i = 0; i < numsamples; i ++) {

        // calc average RMS volume
        RMSVolumeAccu *= avgdecay;
        val = (float)(abs(samples[i]));
        RMSVolumeAccu += val * val;

        // cut amplitudes that are below 2 times average RMS volume
        // (we're interested in peak values, not the silent moments)
        val -= 2 * (float)sqrt(RMSVolumeAccu * avgnorm);
        val = (val > 0) ? val : 0;

        // smooth amplitude envelope
        envelopeAccu *= decay;
        envelopeAccu += val;
        out = (int)(envelopeAccu * norm);

        // cut peaks (shouldn't be necessary though)
        if (out > 32767) out = 32767;
        samples[i] = out;
    }
}



// reads a block of samples from the file, anti-alias filter and decimate.
// returns number of samples outputted.
int BPMDetect::inputSamples(IFloopySoundInput *source, short *outBuffer)
{
    int numChannels;
    short samples[INPUT_BLOCK_SAMPLES];
    int numBytes;
    int numSamples;

    // read a block of samples
    numBytes = source->Read((unsigned char*)samples, INPUT_BLOCK_SAMPLES * sizeof(short));
	if(numBytes <= 0)
		return 0;
	SOUNDFORMAT *fmt = source->GetFormat();
    numChannels = fmt->channels;
    numSamples = numBytes / (numChannels * sizeof(short));

    // convert from stereo to mono if necessary
    if (numChannels == 2)
    {
        int i;

        for (i = 0; i < numSamples; i ++)
        {
            samples[i] = (samples[i * 2] + samples[i * 2 + 1]) / 2;
        }
    }
    
    // decimate
    return decimate(outBuffer, samples, numSamples);
}


void BPMDetect::processBlock(short *samples, int numSamples)
{
    // envelope new samples and add them to buffer
    calcEnvelope(samples, numSamples);
    buffer->putSamples(samples, numSamples);

    // when the buffer has enought samples for processing...
    if ((int)buffer->numSamples() > windowLen) 
    {
        int processLength;

        // how many samples are processed
        processLength = buffer->numSamples() - windowLen;

        // ... calculate autocorrelations for oldest samples...
        updateXCorr(processLength);
        // ... and remove them from the buffer
        buffer->receiveSamples(processLength);
    }
}



void BPMDetect::init(IFloopySoundInput *source)
{
    int sampleRate;

	SOUNDFORMAT *fmt = source->GetFormat();
    assert(fmt->bitsPerSample == 16);    // ensure we've got 16 bit samples

    sampleRate = fmt->frequency;

    // choose decimation factor so that result is approx. 500 Hz
    decimateBy = sampleRate / 500;
    assert(decimateBy > 0);

    // Calculate window length & starting item according to desired min & max bpms
    windowLen = (60 * sampleRate) / (decimateBy * MIN_BPM);
    windowStart = (60 * sampleRate) / (decimateBy * MAX_BPM);

    assert(windowLen > windowStart);

    // allocate new working objects
    xcorr = new float[windowLen];
    memset(xcorr, 0, windowLen * sizeof(float));

    // we do processing in mono mode
    buffer->setChannels(1);
    buffer->clear();
}


float BPMDetect::detectBpm(IFloopySoundInput *source)
{
    short decimated[DECIMATED_BLOCK_SAMPLES];
    int dsamples;
    int sampleRate;
    float peakPos;
    PeakFinder *peakFinder;

    // Initialize processing constants etc.
    init(source);

    // process the file
    //while (wavInFile->eof() == 0)
    do {
        // input a block of samples
        dsamples = inputSamples(source, decimated);
        assert(dsamples <= DECIMATED_BLOCK_SAMPLES);
        // process block
        processBlock(decimated, dsamples);
    } while (dsamples > 0);

    // find peak position
    peakFinder = new PeakFinder(windowStart, windowLen);
    peakPos = peakFinder->detectPeak(xcorr);
    delete peakFinder;

    // delete xcorr array
    delete[] xcorr;
    xcorr = NULL;

    // calculate BPM
	SOUNDFORMAT *fmt = source->GetFormat();
    sampleRate = fmt->frequency;
    return 60.0f * (((float)sampleRate / (float)decimateBy) / peakPos);
}
