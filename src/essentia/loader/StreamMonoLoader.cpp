#include "StreamMonoLoader.h"
#include "StreamAudioLoader.h"
#include "algorithmfactory.h"

using namespace std;

namespace essentiawrapper {

StreamMonoLoader::StreamMonoLoader(const callbacks *cb) : AlgorithmComposite()
{
    cout << "-------- create StreamMonoLoader --------" << endl;

    declareOutput(_audio, "audio", "the mono audio signal");

    _audioLoader.reset(new StreamAudioLoader(cb));

    streaming::AlgorithmFactory &factory = streaming::AlgorithmFactory::instance();
    _mixer.reset(factory.create("MonoMixer"));

    _audioLoader->output("audio")           >>  _mixer->input("audio");
    _audioLoader->output("numberChannels")  >>  _mixer->input("numberChannels");

    attach(_mixer->output("audio"), _audio);
}

void StreamMonoLoader::declareParameters()
{
    cout << "-------- declare parameters StreamMonoLoader --------" << endl;

    _audioLoader->declareParameters();
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
    declareParameter("downmix", "the mixing type for stereo files", "{left,right,mix}", "mix");
}

void StreamMonoLoader::configure()
{
    cout << "-------- configure StreamMonoLoader --------" << endl;

    _audioLoader->configure(INHERIT("sampleRate"));

    _mixer->configure("type", parameter("downmix"));
}

} // namespace intellcut
