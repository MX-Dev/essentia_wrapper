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

#ifndef STREAM_EASY_LOADER_H
#define STREAM_EASY_LOADER_H

#include <memory>

#include "streaming/streamingalgorithmcomposite.h"
#include "streaming/algorithms/vectoroutput.h"
#include "scheduler/network.h"
#include "algorithm.h"
#include "essentia_wrapper.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

class StreamEasyLoader : public streaming::AlgorithmComposite
{
protected:
    shared_ptr<streaming::Algorithm> _monoLoader;
    shared_ptr<streaming::Algorithm> _trimmer;
    shared_ptr<streaming::Algorithm> _scale;

    streaming::SourceProxy<AudioSample> _audio;

    bool _configured = false;

public:
    StreamEasyLoader(const callbacks *cb);
    virtual ~StreamEasyLoader() = default;

    virtual void declareProcessOrder() override
    {
        declareProcessStep(streaming::ChainFrom(_monoLoader.get()));
    }

    virtual void declareParameters() override;
    virtual void configure() override;
};

} // namespace essentiawrapper

#endif // STREAM_EASY_LOADER_H
