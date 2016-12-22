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

#ifndef BEAT_DETECTION_ALGORITHM_H
#define BEAT_DETECTION_ALGORITHM_H

#include "IEssentiaAlgorithm.h"

namespace essentiawrapper {

class BeatDetectionAlgorithm : public IEssentiaAlgorithm
{
public:
    BeatDetectionAlgorithm();
    virtual ~BeatDetectionAlgorithm();

    // IEssentiaAlgorithm interface
    virtual std::vector<essentia_timestamp> analyze(callbacks *cb, const essentia::Pool& config) override;
};

} // namespace essentiawrapper

#endif // BEAT_DETECTION_ALGORITHM_H
