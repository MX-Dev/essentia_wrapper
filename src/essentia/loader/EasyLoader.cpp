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

#include "EasyLoader.h"
#include "StreamEasyLoader.h"

#include "algorithmfactory.h"

namespace essentiawrapper {

EasyLoader::EasyLoader(const callbacks *cb)
{
    cout << "-------- create EasyLoader --------" << endl;

    declareOutput(_audio, "audio", "the audio signal");

    createInnerNetwork(cb);
}

void EasyLoader::createInnerNetwork(const callbacks *cb)
{
    _easyLoader = new StreamEasyLoader(cb);
    _audioStorage.reset(new streaming::VectorOutput<AudioSample>());

    connect(_easyLoader->output("audio"), _audioStorage->input("data"));

    _network.reset(new scheduler::Network(_easyLoader));
}

void EasyLoader::declareParameters()
{
    cout << "-------- declare parameters EasyLoader --------" << endl;

    _easyLoader->declareParameters();
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
    declareParameter("startTime", "the start time of the slice to be extracted [s]", "[0,inf)", 0.0);
    declareParameter("endTime", "the end time of the slice to be extracted [s]", "[0,inf)", 1e6);
    declareParameter("replayGain", "the value of the replayGain that should be used to normalize the signal [dB]", "(-inf,inf)", -6.0);
    declareParameter("downmix", "the mixing type for stereo files", "{left,right,mix}", "mix");
}

void EasyLoader::configure()
{
    cout << "-------- configure EasyLoader --------" << endl;

    _easyLoader->configure(INHERIT("sampleRate"),
                           INHERIT("startTime"),
                           INHERIT("endTime"),
                           INHERIT("replayGain"),
                           INHERIT("downmix"));
}

void EasyLoader::compute()
{
    cout << "-------- compute EasyLoader --------" << endl;

    vector<AudioSample> &audio = _audio.get();
    audio.clear();
    // TODO: somehow retrieve the audioFileLength from the internal loader at
    // configure time
    // _audio.reserve( sampleRate*( min(endTime, audioFileLength) - startTime ) );

    _audioStorage->setVector(&audio);

    _network->run();
    reset();
}

void EasyLoader::reset()
{
    cout << "-------- reset EasyLoader --------" << endl;

    _network->reset();
}


} // namespace essentiawrapper
