#include "MonoLoader.h"
#include "StreamMonoLoader.h"

#include "algorithmfactory.h"

using namespace std;

namespace essentiawrapper {

MonoLoader::MonoLoader(const callbacks *cb)
{
    cout << "-------- create MonoLoader --------" << endl;

    declareOutput(_audio, "audio", "the audio signal");

    createInnerNetwork(cb);
}

void MonoLoader::createInnerNetwork(const callbacks *cb)
{
    _monoLoader = new StreamMonoLoader(cb);
    _audioStorage.reset(new streaming::VectorOutput<AudioSample>());

    connect(_monoLoader->output("audio"), _audioStorage->input("data"));

    _network.reset(new scheduler::Network(_monoLoader));
}

void MonoLoader::declareParameters()
{
    cout << "-------- declare parameters MonoLoader --------" << endl;

    _monoLoader->declareParameters();
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
    declareParameter("downmix", "the mixing type for stereo files", "{left,right,mix}", "mix");
}

void MonoLoader::configure()
{
    cout << "-------- configure MonoLoader --------" << endl;

    _monoLoader->configure(INHERIT("sampleRate"),
                                 INHERIT("downmix"));
}

void MonoLoader::compute()
{
    cout << "-------- compute MonoLoader --------" << endl;

    vector<AudioSample> &audio = _audio.get();

    // TODO: _audio.reserve(sth_meaningful);

    _audioStorage->setVector(&audio);

    _network->run();
    reset();
}

void MonoLoader::reset()
{
    cout << "-------- reset MonoLoader --------" << endl;

    _network->reset();
}


} // namespace intellcut
