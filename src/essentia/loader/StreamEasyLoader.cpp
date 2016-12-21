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

#include "StreamEasyLoader.h"

#include "StreamMonoLoader.h"
#include "algorithmfactory.h"
#include "essentiamath.h"

namespace essentiawrapper {

StreamEasyLoader::StreamEasyLoader(const callbacks *cb) : AlgorithmComposite()
{
    cout << "-------- create StreamEasyLoader --------" << endl;

    declareOutput(_audio, "audio", "the output audio signal");

    _monoLoader.reset(new StreamMonoLoader(cb));

    streaming::AlgorithmFactory &factory = streaming::AlgorithmFactory::instance();

    _trimmer.reset(factory.create("Trimmer"));
    _scale.reset(factory.create("Scale"));

    _monoLoader->output("audio")  >>  _trimmer->input("signal");
    _trimmer->output("signal")    >>  _scale->input("signal");

    attach(_scale->output("signal"), _audio);
}

void StreamEasyLoader::declareParameters()
{
    cout << "-------- declare parameters StreamEasyLoader --------" << endl;

    _monoLoader->declareParameters();
    declareParameter("sampleRate", "the output sampling rate [Hz]", "{32000,44100,48000}", 44100.);
    declareParameter("startTime", "the start time of the slice to be extracted [s]", "[0,inf)", 0.0);
    declareParameter("endTime", "the end time of the slice to be extracted [s]", "[0,inf)", 1e6);
    declareParameter("replayGain", "the value of the replayGain that should be used to normalize the signal [dB]", "(-inf,inf)", -6.0);
    declareParameter("downmix", "the mixing type for stereo files", "{left,right,mix}", "mix");
}

void StreamEasyLoader::configure()
{
    cout << "-------- configure StreamEasyLoader --------" << endl;

    _monoLoader->configure(INHERIT("sampleRate"),
                           INHERIT("downmix"));

    _trimmer->configure(INHERIT("sampleRate"),
                        INHERIT("startTime"),
                        INHERIT("endTime"));

    // apply a 6dB preamp, as done by all audio players.
    Real scalingFactor = db2amp(parameter("replayGain").toReal() + 6.0);

    _scale->configure("factor", scalingFactor);
}

} // namespace essentiawrapper
