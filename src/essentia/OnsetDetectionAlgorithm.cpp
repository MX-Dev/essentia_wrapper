#include "OnsetDetectionAlgorithm.h"
#include "loader/MonoLoader.h"
#include <iostream>
#include <iomanip>
#include "algorithmfactory.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

OnsetDetectionAlgorithm::OnsetDetectionAlgorithm()
{
    essentia::init();
}

OnsetDetectionAlgorithm::~OnsetDetectionAlgorithm()
{
    essentia::shutdown();
}

std::vector<essentia_timestamp> OnsetDetectionAlgorithm::analyze(callbacks *cb)
{
    cout << "Multifeature beat tracker based on BeatTrackerMultiFeature algorithm." << endl;

    Real onsetRate;
    vector<Real> audio, onsets;

    std::shared_ptr<standard::Algorithm> monoloader(new MonoLoader(cb));
    monoloader->declareParameters();
    monoloader->configure("sampleRate", 44100,
                          "downmix", "mix");

    std::shared_ptr<standard::Algorithm> extractoronsetrate(standard::AlgorithmFactory::create("OnsetRate"));

    monoloader->output("audio").set(audio);

    extractoronsetrate->input("signal").set(audio);
    extractoronsetrate->output("onsets").set(onsets);
    extractoronsetrate->output("onsetRate").set(onsetRate);

    cout << "-------- start processing --------" << endl;

    monoloader->compute();
    extractoronsetrate->compute();

    cout << "-------- finished processing --------" << endl;

    cout << setw(4) << "onsets count: " << onsets.size() << endl;
    cout << setw(4) << "onset rate: " << onsetRate << endl;

    cout << left;
    for (size_t i = 0; i < onsets.size(); ++i)
    {
        cout << setw(4) << i << " onset: " << setw(8) << onsets.at(i) << endl;
    }

    cout << "-------- convert onsets --------" << endl;
    std::vector<essentia_timestamp> poitss;
    for (size_t i = 0; i < onsets.size(); ++i)
    {
        essentia_timestamp ts;
        ts.ts = onsets[i];
        ts.type = OnSet;
        //ts.name = "Essentia - OnsetDetector";
        poitss.push_back(ts);
    }

    return poitss;
}

} // namespace essentiawrapper
