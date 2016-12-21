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

#include "AudioLoader.h"
#include "StreamAudioLoader.h"

#include "algorithmfactory.h"

namespace essentiawrapper {

AudioLoader::AudioLoader(const callbacks *cb)
{
    cout << "-------- create AudioLoader --------" << endl;

    declareOutput(_audio, "audio", "the input audio signal");
    declareOutput(_channels, "numberChannels", "the number of channels");

    createInnerNetwork(cb);
}

void AudioLoader::createInnerNetwork(const callbacks *cb)
{
    _audioLoader = new StreamAudioLoader(cb);
    _audioStorage.reset(new streaming::VectorOutput<StereoSample>());

    _audioLoader->output("audio") >> _audioStorage->input("data");
    _audioLoader->output("numberChannels")  >>  PC(_pool, "internal.numberChannels");

    _network.reset(new scheduler::Network(_audioLoader));
}

void AudioLoader::declareParameters()
{
    cout << "-------- declare parameters AudioLoader --------" << endl;

    _audioLoader->declareParameters();
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
}

void AudioLoader::configure()
{
    cout << "-------- create AudioLoader --------" << endl;

    _audioLoader->configure(INHERIT("sampleRate"));
}

void AudioLoader::compute()
{
    cout << "-------- compute AudioLoader --------" << endl;

    int &numberChannels = _channels.get();
    vector<StereoSample> &audio = _audio.get();

    _audioStorage->setVector(&audio);
    // TODO: is using VectorInput indeed faster than using Pool?

    // FIXME:
    // _audio.reserve(sth_meaningful);

    _network->run();

    numberChannels = (int) _pool.value<Real>("internal.numberChannels");

    // reset, so it is ready to load audio again
    reset();
}

void AudioLoader::reset()
{
    cout << "-------- reset AudioLoader --------" << endl;

    _network->reset();

    _pool.remove("internal.numberChannels");
}

} // namespace essentiawrapper
