/*
 * Copyright (C) 2006-2016  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 *
 * Modified by E. Mista, date 12 december 2016
 *
 */

#include "StreamStereoTrimmer.h"
#include "algorithmfactory.h"

namespace essentiawrapper {

StreamStereoTrimmer::StreamStereoTrimmer() : Algorithm(), _preferredSize(defaultPreferredSize)
{
    cout << "-------- create StreamStereoTrimmer --------" << endl;


    declareInput(_input, _preferredSize, "signal", "the input signal");
    declareOutput(_output, _preferredSize, "signal", "the trimmed signal");

    _output.setBufferType(streaming::BufferUsage::forAudioStream);

}

void StreamStereoTrimmer::declareParameters()
{
    cout << "-------- declare paramteres StreamStereoTrimmer --------" << endl;

    declareParameter("sampleRate", "the sampling rate of the input audio signal [Hz]", "(0,inf)", 44100.);
    declareParameter("startTime", "the start time of the slice you want to extract [s]", "[0,inf)", 0.0);
    declareParameter("endTime", "the end time of the slice you want to extract [s]", "[0,inf)", 1.0e6);
}

void StreamStereoTrimmer::configure()
{
    cout << "-------- configure StreamStereoTrimmer --------" << endl;

    Real sampleRate = parameter("sampleRate").toReal();
    _startIndex = (long long)(parameter("startTime").toReal() * sampleRate);
    _endIndex = (long long)(parameter("endTime").toReal() * sampleRate);
    if (_startIndex > _endIndex)
    {
        throw EssentiaException("Trimmer: startTime cannot be larger than endTime.");
    }
    _consumed = 0;
    _preferredSize = defaultPreferredSize;
}

streaming::AlgorithmStatus StreamStereoTrimmer::process()
{
    // cout << "-------- process StreamStereoTrimmer --------" << endl;

    if ((_consumed < _startIndex) && (_consumed + _preferredSize > _startIndex))
    {
        _input.setAcquireSize(_startIndex - _consumed);
        _input.setReleaseSize(_startIndex - _consumed);
    }

    if (_consumed == _startIndex)
    {
        _input.setAcquireSize(_preferredSize);
        _input.setReleaseSize(_preferredSize);
    }

    streaming::AlgorithmStatus status = acquireData();

    if (status != streaming::OK)
    {
        // if status == NO_OUTPUT, we should temporarily stop the framecutter,
        // return from this function so its dependencies can process the frames,
        // and reschedule the framecutter to run when all this is done.
        if (status == streaming::NO_OUTPUT)
        {
            //_reschedule = true;
            return streaming::NO_OUTPUT; // if the buffer is full, we need to have produced something!
        }

        // if shouldStop is true, that means there is no more audio, so we need
        // to take what's left to fill in the output buffer
        if (!shouldStop()) return streaming::NO_INPUT;

        int available = input("signal").available();
        if (available == 0)
        {
            shouldStop(true);
            return streaming::NO_INPUT;
        }

        _input.setAcquireSize(available);
        _input.setReleaseSize(available);
        _output.setAcquireSize(available);
        _output.setReleaseSize(available);
        _preferredSize = available;
        return process();
    }

    // get the audio input and copy it to the output
    const vector<StereoSample> &input = _input.tokens();
    vector<StereoSample> &output = _output.tokens();


    if (_consumed >= _startIndex && _consumed < _endIndex)
    {
        assert(input.size() == output.size());
        int howMany = min((long long)input.size(), _endIndex - _consumed);
        fastcopy(&*output.begin(), &*input.begin(), howMany);

        _output.setReleaseSize(howMany);
    }
    else
    {
        _output.setReleaseSize(0);
    }

    _consumed += _input.releaseSize();

    // optimization: we should also tell the parent (most of the time an
    // audio loader) to also stop, to avoid decoding an entire mp3 when only
    // 10 seconds are needed
    if (_consumed >= _endIndex)
    {
        // FIXME: does still still work with the new composites?
        shouldStop(true);
        const_cast<streaming::SourceBase *>(_input.source())->parent()->shouldStop(true);
    }

    releaseData();

    return streaming::OK;

}

void StreamStereoTrimmer::reset()
{
    cout << "-------- reset StreamStereoTrimmer --------" << endl;

    Algorithm::reset();
    _consumed = 0;
    _preferredSize = defaultPreferredSize;

    // make sure to reset I/O sizes
    _input.setAcquireSize(_preferredSize);
    _input.setReleaseSize(_preferredSize);
    _output.setAcquireSize(_preferredSize);
    _output.setReleaseSize(_preferredSize);
}

} // namespace essentiawrapper
