#include "AudioLoader.h"
#include "StreamAudioLoader.h"

#include "algorithmfactory.h"

namespace essentiawrapper {

AudioLoader::AudioLoader(const callbacks *cb)
{
    cout << "-------- create AudioLoader --------" << endl;

    declareOutput(_audio, "audio", "the input audio signal");
    declareOutput(_channels, "numberChannels", "the number of channels");

    createInnerNetwork(cb);
}

void AudioLoader::createInnerNetwork(const callbacks *cb)
{
    _audioLoader = new StreamAudioLoader(cb);
    _audioStorage.reset(new streaming::VectorOutput<StereoSample>());

    _audioLoader->output("audio") >> _audioStorage->input("data");
    _audioLoader->output("numberChannels")  >>  PC(_pool, "internal.numberChannels");

    _network.reset(new scheduler::Network(_audioLoader));
}

void AudioLoader::declareParameters()
{
    cout << "-------- declare parameters AudioLoader --------" << endl;

    _audioLoader->declareParameters();
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
}

void AudioLoader::configure()
{
    cout << "-------- create AudioLoader --------" << endl;

    _audioLoader->configure(INHERIT("sampleRate"));
}

void AudioLoader::compute()
{
    cout << "-------- compute AudioLoader --------" << endl;

    int &numberChannels = _channels.get();
    vector<StereoSample> &audio = _audio.get();

    _audioStorage->setVector(&audio);
    // TODO: is using VectorInput indeed faster than using Pool?

    // FIXME:
    // _audio.reserve(sth_meaningful);

    _network->run();

    numberChannels = (int) _pool.value<Real>("internal.numberChannels");

    // reset, so it is ready to load audio again
    reset();
}

void AudioLoader::reset()
{
    cout << "-------- reset AudioLoader --------" << endl;

    _network->reset();

    _pool.remove("internal.numberChannels");
}

} // namespace essentiawrapper
