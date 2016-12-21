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

#ifndef MONO_LOADER_H
#define MONO_LOADER_H

#include <memory>

#include "streaming/streamingalgorithmcomposite.h"
#include "streaming/algorithms/vectoroutput.h"
#include "scheduler/network.h"
#include "algorithm.h"
#include "essentia_wrapper.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

// Standard non-streaming algorithm comes after the streaming one as it depends on it
class MonoLoader : public standard::Algorithm
{
protected:
    standard::Output<vector<AudioSample> > _audio;

    // no shared pointer necessary, network deletes registered algorithm on its own
    streaming::Algorithm *_monoLoader;
    shared_ptr<scheduler::Network> _network;

    shared_ptr<streaming::VectorOutput<AudioSample> >_audioStorage;

    void createInnerNetwork(const callbacks* cb);

public:
    MonoLoader(const callbacks* cb);
    virtual ~MonoLoader() = default;

    virtual void declareParameters() override;
    virtual void configure() override;
    virtual void compute() override;
    virtual void reset() override;

};

} // namespace essentiawrapper

#endif // MONO_LOADER_H

