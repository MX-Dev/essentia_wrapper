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

#ifndef STREAM_MONO_LOADER_H
#define STREAM_MONO_LOADER_H

#include <memory>

#include "streaming/streamingalgorithmcomposite.h"
#include "streaming/algorithms/vectoroutput.h"
#include "scheduler/network.h"
#include "algorithm.h"
#include "essentia_wrapper.h"

using namespace essentia;

namespace essentiawrapper {

class StreamMonoLoader : public streaming::AlgorithmComposite
{
protected:
    std::shared_ptr<streaming::Algorithm> _audioLoader;
    std::shared_ptr<streaming::Algorithm> _mixer;

    streaming::SourceProxy<AudioSample> _audio;

    bool _configured = false;

public:
    StreamMonoLoader(const callbacks *cb);
    virtual ~StreamMonoLoader() = default;

    virtual void declareProcessOrder() override
    {
        declareProcessStep(streaming::ChainFrom(_audioLoader.get()));
    }

    virtual void declareParameters() override;
    virtual void configure() override;
};

} // namespace essentiawrapper

#endif // STREAM_MONO_LOADER_H
