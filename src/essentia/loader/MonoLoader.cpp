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

#include "MonoLoader.h"
#include "StreamMonoLoader.h"

#include "algorithmfactory.h"

namespace essentiawrapper {

MonoLoader::MonoLoader(const callbacks *cb)
{
    cout << "-------- create MonoLoader --------" << endl;

    declareOutput(_audio, "audio", "the audio signal");

    createInnerNetwork(cb);
}

void MonoLoader::createInnerNetwork(const callbacks *cb)
{
    _monoLoader = new StreamMonoLoader(cb);
    _audioStorage.reset(new streaming::VectorOutput<AudioSample>());

    connect(_monoLoader->output("audio"), _audioStorage->input("data"));

    _network.reset(new scheduler::Network(_monoLoader));
}

void MonoLoader::declareParameters()
{
    cout << "-------- declare parameters MonoLoader --------" << endl;

    _monoLoader->declareParameters();
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
    declareParameter("downmix", "the mixing type for stereo files", "{left,right,mix}", "mix");
}

void MonoLoader::configure()
{
    cout << "-------- configure MonoLoader --------" << endl;

    _monoLoader->configure(INHERIT("sampleRate"),
                                 INHERIT("downmix"));
}

void MonoLoader::compute()
{
    cout << "-------- compute MonoLoader --------" << endl;

    vector<AudioSample> &audio = _audio.get();

    // TODO: _audio.reserve(sth_meaningful);

    _audioStorage->setVector(&audio);

    _network->run();
    reset();
}

void MonoLoader::reset()
{
    cout << "-------- reset MonoLoader --------" << endl;

    _network->reset();
}


} // namespace intellcut
