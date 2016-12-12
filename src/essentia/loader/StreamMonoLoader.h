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
