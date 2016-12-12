#include "StreamAudioLoader.h"
#include "algorithmfactory.h"
#include <functional>

namespace essentiawrapper {

StreamAudioLoader::StreamAudioLoader(const callbacks *cb)
    : Algorithm()
    , _callback(cb)
{
    cout << "-------- create StreamAudioLoader --------" << endl;

    declareOutput(_audio, 1, "audio", "the input audio signal");
    declareOutput(_channels, 0, "numberChannels", "the number of channels");

    _audio.setBufferType(streaming::BufferUsage::forLargeAudioStream);

}

StreamAudioLoader::~StreamAudioLoader()
{

}

void StreamAudioLoader::declareParameters()
{
    cout << "-------- declare paramteres StreamAudioLoader --------" << endl;
    declareParameter("sampleRate", "the desired output sampling rate [Hz]", "(0,inf)", 44100.);
}

void StreamAudioLoader::configure()
{
    cout << "-------- configure StreamAudioLoader --------" << endl;

    reset();
}

streaming::AlgorithmStatus StreamAudioLoader::process()
{
    cout << "-------- process StreamAudioLoader --------" << endl;

    int32_t size = -1;

    audio_buffer* audioBuffer = nullptr;
    const char* data = nullptr;

    if(_callback)
    {
        audioBuffer = _callback->read_audio(_callback->audio_file);
        if(audioBuffer)
        {
            size = static_cast<int32_t>(audioBuffer->sample_count);
            data = audioBuffer->buffer;
        }
    }

    if (size > 0)
    {
        int nSamples = size;

        cout << "audioloader read " << nSamples << endl;

        bool ok = _audio.acquire(nSamples);
        if (!ok)
        {
            throw EssentiaException("AudioLoader: could not acquire output for audio");
        }

        vector<StereoSample> &audio = *((vector<StereoSample> *)_audio.getTokens());

        const float *sampleBuffer = reinterpret_cast<const float*>(data);
        if (_nChannels == 1)
        {
            for (int i = 0; i < nSamples; i++)
            {
                audio[i].left() = *sampleBuffer;
                sampleBuffer++;
            }
        }
        else // _nChannels == 2
        {
            for (int i = 0; i < nSamples; i++)
            {
                audio[i].left() = *sampleBuffer;
                sampleBuffer++;
                audio[i].right() = *sampleBuffer;
                sampleBuffer++;
            }
        }

        _audio.release(nSamples);

        return streaming::OK;
    }

    if(audioBuffer && _callback)
    {
        _callback->free_audio_buffer(audioBuffer);
    }

    shouldStop(true);
    return streaming::FINISHED;

}

void StreamAudioLoader::reset()
{
    cout << "-------- reset StreamAudioLoader --------" << endl;

    Algorithm::reset();
    _nChannels = 2;
    _channels.push(_nChannels);
    _sampleRate.push(static_cast<Real>(44100));
}

} // namespace essentiawrapper
