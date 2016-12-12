#ifndef AUDIO_LOADER_H
#define AUDIO_LOADER_H

#include <memory>

#include "streaming/algorithms/vectoroutput.h"
#include "streaming/algorithms/poolstorage.h"
#include "scheduler/network.h"
#include "algorithm.h"

#include "essentia_wrapper.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

// Standard non-streaming algorithm comes after the streaming one as it depends on it
class AudioLoader : public standard::Algorithm
{

protected:

    std::shared_ptr<streaming::VectorOutput<essentia::StereoSample>> _audioStorage;

    // no shared pointer necessary, network deletes registered algorithm on its own
    streaming::Algorithm *_audioLoader = 0;
    std::shared_ptr<scheduler::Network> _network;

    standard::Output<vector<StereoSample> > _audio;

    standard::Output<int> _channels;
    Pool _pool;

    void createInnerNetwork(const callbacks *cb);

public:
    AudioLoader(const callbacks *cb);
    virtual ~AudioLoader() = default;

    virtual void declareParameters() override;
    virtual void configure() override;
    virtual void compute() override;
    virtual void reset() override;

};

} // namespace essentiawrapper

#endif // AUDIO_LOADER_H
