#include "FadeDetectionAlgorithm.h"

#include "loader/MonoLoader.h"

#include <iostream>

#include "streaming/algorithms/poolstorage.h"
#include "algorithmfactory.h"
#include "utils/tnt/tnt2vector.h"
#include "pool.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

typedef TNT::Array2D<Real> array2d;

FadeDetectionAlgorithm::FadeDetectionAlgorithm()
{
    essentia::init();
}

FadeDetectionAlgorithm::~FadeDetectionAlgorithm()
{
    essentia::shutdown();
}

void addToPool(const array2d &a2d,
               const string &desc, // descriptor names
               Pool &pool)
{
    vector<vector<Real> > v2d =  array2DToVecvec(a2d);
    for (size_t i = 0; i < v2d.size(); ++i)
        pool.add(desc, v2d[i]);
}

/**
 * @brief Analyzes the audio delivered by the reader.
 * @param reader for audio (needed configuration -> 44100 Hz, 2 channels and 32 bps (float))
 * @param callback for progress, can be a nullptr
 * @return fades
 */
std::vector<essentia_timestamp> FadeDetectionAlgorithm::analyze(callbacks *cb)
{
    cout << "Multifeature beat tracker based on BeatTrackerMultiFeature algorithm." << endl;

    int sr = 44100;

    std::shared_ptr<standard::Algorithm> monoloader(new MonoLoader(cb));
    monoloader->declareParameters();
    monoloader->configure("sampleRate", sr,
                          "downmix", "mix");

    int framesize = sr / 4;
    int hopsize = 256;
    Real frameRate = Real(sr) / Real(hopsize);

    standard::AlgorithmFactory &factory = standard::AlgorithmFactory::instance();
    std::shared_ptr<standard::Algorithm> frameCutter(factory.create("FrameCutter",
            "frameSize", framesize,
            "hopSize", hopsize));

    std::shared_ptr<standard::Algorithm> rms(factory.create("RMS"));

    std::shared_ptr<standard::Algorithm> fadeDetect(factory.create("FadeDetection",
            "minLength", 3.,
            "cutoffHigh", 0.85,
            "cutoffLow", 0.20,
            "frameRate", frameRate));

    // create a pool for fades' storage:
    Pool pool;

    // set audio:
    vector<Real> audio_mono;
    monoloader->output("audio").set(audio_mono);

    // set frameCutter:
    vector<Real> frame;
    frameCutter->input("signal").set(audio_mono);
    frameCutter->output("frame").set(frame);

    // set rms:
    Real rms_value;
    rms->input("array").set(frame);
    rms->output("rms").set(rms_value);

    // we need a vector to store rms values:
    std::vector<Real> rms_vector;

    cout << "-------- start processing --------" << endl;

    // load audio:
    monoloader->compute();

    // compute and store rms first and will compute fade detection later:
    while (true)
    {
        frameCutter->compute();
        if (frame.empty())
            break;

        rms->compute();
        rms_vector.push_back(rms_value);
    }

    // set fade detection:
    array2d fade_in;
    array2d fade_out;
    fadeDetect->input("rms").set(rms_vector);
    fadeDetect->output("fadeIn").set(fade_in);
    fadeDetect->output("fadeOut").set(fade_out);

    // compute fade detection:
    fadeDetect->compute();

    cout << "-------- finished processing --------" << endl;

    // Exemplifying how to add/retrieve values from the pool in order to output them into stdout
    if (fade_in.dim1())
    {
        addToPool(fade_in, "high_level.fade_in", pool);
        vector<vector<Real> > fadeIn = pool.value<vector<vector<Real> > > ("high_level.fade_in");
        cout << "fade ins: ";
        for (size_t i = 0; i < fadeIn.size(); ++i)
            cout << fadeIn[i] << endl;
    }
    else cout << "No fades in found" << endl;

    if (fade_out.dim1())
    {
        addToPool(fade_out, "high_level.fade_out", pool);
        vector<vector<Real> > fadeOut = pool.value<vector<vector<Real> > > ("high_level.fade_out");
        cout << "fade outs: ";
        for (size_t i = 0; i < fadeOut.size(); ++i)
            cout << fadeOut[i] << endl;
    }
    else cout << "No fades out found" << endl ;

    cout << "-------- convert fades --------" << endl;

    // TODO currently no output format for two related timestamps implemented
    std::vector<essentia_timestamp> poitss;
//    for (size_t i = 0; i < ticks.size(); ++i)
//    {
//        poitss.push_back(POITimeStamp(POIType::Fade_in, ticks.at(i) * 1000000000, "Essentia - Fade"));
//    }

    return poitss;
}

} // namespace essentiawrapper
