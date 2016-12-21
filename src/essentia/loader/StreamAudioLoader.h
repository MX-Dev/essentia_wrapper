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

#ifndef STREAM_AUDIO_LOADER_H
#define STREAM_AUDIO_LOADER_H

#include <memory>

#include "streaming/algorithms/vectoroutput.h"
#include "streaming/algorithms/poolstorage.h"
#include "scheduler/network.h"
#include "algorithm.h"
#include "essentia_wrapper.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

class StreamAudioLoader : public streaming::Algorithm
{
private:
    void openAudio();
    void closeAudio();

    const callbacks *_callback;

    streaming::Source<StereoSample> _audio;
    streaming::AbsoluteSource<Real> _sampleRate;
    streaming::AbsoluteSource<int> _channels;
    streaming::AbsoluteSource<int> _bit_rate;

    int _nChannels = 0;

    bool _configured = false;


public:
    StreamAudioLoader(const callbacks *cb);
    virtual ~StreamAudioLoader();

    virtual void declareParameters() override;
    virtual void configure() override;
    virtual streaming::AlgorithmStatus process() override;
    virtual void reset() override;

};

} // namespace essentiawrapper

#endif // STREAM_AUDIO_LOADER_H
