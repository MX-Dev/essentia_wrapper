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

#include "StreamAudioLoader.h"
#include "algorithmfactory.h"
#include <functional>

namespace essentiawrapper {

StreamAudioLoader::StreamAudioLoader(const callbacks *cb)
    : Algorithm()
    , _callback(cb)
{
    cout << "-------- create StreamAudioLoader --------" << endl;

    declareOutput(_audio, 1, "audio", "the input audio signal");
    declareOutput(_channels, 0, "numberChannels", "the number of channels");

    _audio.setBufferType(streaming::BufferUsage::forLargeAudioStream);

}

StreamAudioLoader::~StreamAudioLoader()
{
    closeAudio();
}

void StreamAudioLoader::declareParameters()
{
    cout << "-------- declare paramteres StreamAudioLoader --------" << endl;
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
}

void StreamAudioLoader::configure()
{
    cout << "-------- configure StreamAudioLoader --------" << endl;

    reset();
}

streaming::AlgorithmStatus StreamAudioLoader::process()
{
    // cout << "-------- process StreamAudioLoader --------" << endl;

    int32_t size = -1;

    std::shared_ptr<audio_buffer> audioBuffer;
    const char* data = nullptr;

    if(_callback)
    {
        audioBuffer.reset(_callback->read_audio(_callback->audio_file), _callback->free_audio_buffer);
        if(audioBuffer)
        {
            size = static_cast<int32_t>(audioBuffer->sample_count);
            data = reinterpret_cast<const char*>(audioBuffer->buffer);
        }
    }

    if (size > 0)
    {
        int nSamples = size;

        bool ok = _audio.acquire(nSamples);
        if (!ok)
        {
            throw EssentiaException("AudioLoader: could not acquire output for audio");
        }

        vector<StereoSample> &audio = *((vector<StereoSample> *)_audio.getTokens());

        const float *sampleBuffer = reinterpret_cast<const float*>(data);
        if (_nChannels == 1)
        {
            for (int i = 0; i < nSamples; i++)
            {
                audio[i].left() = *sampleBuffer;
                sampleBuffer++;
            }
        }
        else // _nChannels == 2
        {
            for (int i = 0; i < nSamples; i++)
            {
                audio[i].left() = *sampleBuffer;
                sampleBuffer++;
                audio[i].right() = *sampleBuffer;
                sampleBuffer++;
            }
        }

        _audio.release(nSamples);

        return streaming::OK;
    }

    shouldStop(true);
    return streaming::FINISHED;

}

void StreamAudioLoader::reset()
{
    cout << "-------- reset StreamAudioLoader --------" << endl;

    Algorithm::reset();
    _nChannels = 2;
    _channels.push(_nChannels);
    _sampleRate.push(static_cast<Real>(44100));

    closeAudio();
    openAudio();
}

void StreamAudioLoader::openAudio()
{
    if(_callback)
    {
        _callback->open_audio(_callback->audio_file, 44100, 2, Float);
    }
}

void StreamAudioLoader::closeAudio()
{
    if(_callback)
    {
        _callback->close_audio(_callback->audio_file);
    }
}



} // namespace essentiawrapper

