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
    standard::Output<std::vector<AudioSample> > _audio;

    // no shared pointer necessary, network deletes registered algorithm on its own
    streaming::Algorithm *_monoLoader;
    std::shared_ptr<scheduler::Network> _network;

    std::shared_ptr<streaming::VectorOutput<AudioSample> >_audioStorage;

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
