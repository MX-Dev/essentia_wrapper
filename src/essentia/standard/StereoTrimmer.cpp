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

#include "StereoTrimmer.h"

#include "algorithmfactory.h"

namespace essentiawrapper {

StereoTrimmer::StereoTrimmer()
{
    cout << "-------- create StereoTrimmer --------" << endl;

    declareInput(_input, "signal", "the input signal");
    declareOutput(_output, "signal", "the trimmed signal");
}

void StereoTrimmer::declareParameters()
{
    cout << "-------- declare parameters StereoTrimmer --------" << endl;

    declareParameter("sampleRate", "the sampling rate of the input audio signal [Hz]", "(0,inf)", 44100.);
    declareParameter("startTime", "the start time of the slice you want to extract [s]", "[0,inf)", 0.0);
    declareParameter("endTime", "the end time of the slice you want to extract [s]", "[0,inf)", 1.0e6);
}

void StereoTrimmer::configure()
{
    cout << "-------- configure StereoTrimmer --------" << endl;

    Real sampleRate = parameter("sampleRate").toReal();
    _startIndex = (long long)(parameter("startTime").toReal() * sampleRate);
    _endIndex = (long long)(parameter("endTime").toReal() * sampleRate);
    if (_startIndex > _endIndex)
    {
        throw EssentiaException("Trimmer: startTime cannot be larger than endTime.");
    }
}

void StereoTrimmer::compute()
{
    cout << "-------- compute StereoTrimmer --------" << endl;

    const vector<StereoSample> &input = _input.get();
    vector<StereoSample> &output = _output.get();
    int size = input.size();

    if (_startIndex < 0) _startIndex = 0;
    if (_startIndex > size)
    {
        //throw EssentiaException("Trimmer: cannot trim beyond the size of the input signal");
        _startIndex = size;
    }
    if (_endIndex > size) _endIndex = size;

    size = _endIndex - _startIndex;
    output.resize(size);
    memcpy(&output[0], &input[0] + _startIndex, size * sizeof(StereoSample));
}

} // namespace intellcut
