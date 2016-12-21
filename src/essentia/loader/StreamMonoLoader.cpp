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

#include "StreamMonoLoader.h"
#include "StreamAudioLoader.h"
#include "algorithmfactory.h"

using namespace std;

namespace essentiawrapper {

StreamMonoLoader::StreamMonoLoader(const callbacks *cb) : AlgorithmComposite()
{
    cout << "-------- create StreamMonoLoader --------" << endl;

    declareOutput(_audio, "audio", "the mono audio signal");

    _audioLoader.reset(new StreamAudioLoader(cb));

    streaming::AlgorithmFactory &factory = streaming::AlgorithmFactory::instance();
    _mixer.reset(factory.create("MonoMixer"));

    _audioLoader->output("audio")           >>  _mixer->input("audio");
    _audioLoader->output("numberChannels")  >>  _mixer->input("numberChannels");

    attach(_mixer->output("audio"), _audio);
}

void StreamMonoLoader::declareParameters()
{
    cout << "-------- declare parameters StreamMonoLoader --------" << endl;

    _audioLoader->declareParameters();
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
    declareParameter("downmix", "the mixing type for stereo files", "{left,right,mix}", "mix");
}

void StreamMonoLoader::configure()
{
    cout << "-------- configure StreamMonoLoader --------" << endl;

    _audioLoader->configure(INHERIT("sampleRate"));

    _mixer->configure("type", parameter("downmix"));
}

} // namespace intellcut
