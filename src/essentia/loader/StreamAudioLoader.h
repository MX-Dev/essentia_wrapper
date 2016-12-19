#ifndef STREAM_AUDIO_LOADER_H
#define STREAM_AUDIO_LOADER_H

#include <memory>

#include "streaming/algorithms/vectoroutput.h"
#include "streaming/algorithms/poolstorage.h"
#include "scheduler/network.h"
#include "algorithm.h"
#include "essentia_wrapper.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

class StreamAudioLoader : public streaming::Algorithm
{
private:
    void openAudio();
    void closeAudio();

    const callbacks *_callback;

    streaming::Source<StereoSample> _audio;
    streaming::AbsoluteSource<Real> _sampleRate;
    streaming::AbsoluteSource<int> _channels;
    streaming::AbsoluteSource<int> _bit_rate;

    int _nChannels = 0;

    bool _configured = false;


public:
    StreamAudioLoader(const callbacks *cb);
    virtual ~StreamAudioLoader();

    virtual void declareParameters() override;
    virtual void configure() override;
    virtual streaming::AlgorithmStatus process() override;
    virtual void reset() override;

};

} // namespace essentiawrapper

#endif // STREAM_AUDIO_LOADER_H
