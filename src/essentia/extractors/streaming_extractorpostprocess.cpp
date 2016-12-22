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

#include "streaming_extractorpostprocess.h"

#include "streaming/algorithms/poolstorage.h"
#include "algorithmfactory.h"
#include "essentiamath.h"

using namespace standard;

// Add missing descriptors which are not computed yet, but will be for the
// final release or during the 1.x cycle. However, the schema need to be
// complete before that, so just put default values for these.
// Also make sure that some descriptors that might have fucked up come out nice.
void PostProcess(Pool &pool, const Pool &options, const string &nspace)
{
    cout << "PostProcess missing descriptors" << endl;

    bool computeBeats = nspace.empty() ?
                         options.value<Real>("rhythm.beats.compute") != 0 :
                         options.value<Real>("segmentation.desc.rhythm.beats.compute") != 0;

    if (computeBeats)
    {
        string rhythmspace = "rhythm.";
        if (!nspace.empty()) rhythmspace = nspace + ".rhythm.";
        const vector<string> &descNames = pool.descriptorNames();
        if (find(descNames.begin(), descNames.end(), rhythmspace + "bpm_confidence") == descNames.end())
            pool.set(rhythmspace + "bpm_confidence", 0.0);
        if (find(descNames.begin(), descNames.end(), rhythmspace + "perceptual_tempo") == descNames.end())
            pool.set(rhythmspace + "perceptual_tempo", "unknown");
        if (options.value<Real>("rhythm.beats.loudness.compute") != 0)
        {
            if (find(descNames.begin(), descNames.end(), rhythmspace + "beats.loudness") == descNames.end())
                pool.add(rhythmspace + "beats.loudness", Real(0.0));
            if (find(descNames.begin(), descNames.end(), rhythmspace + "beats.loudness_band_ratio") == descNames.end())
                pool.add(rhythmspace + "beats.loudness_band_Ratio", vector<Real>());
        }
    }

    //if (find(descNames.begin(), descNames.end(), rhythmspace + "rubato_start") == descNames.end())
    //  pool.set(rhythmspace + "rubato_start", vector<Real>(0));
    //if (find(descNames.begin(), descNames.end(), rhythmspace + "rubato_stop") == descNames.end())
    //  pool.set(rhythmspace + "rubato_stop", vector<Real>(0));
}
