#include "BeatDetectionAlgorithm.h"

#include "loader/StreamMonoLoader.h"

#include <iostream>
#include <iomanip>

#include "streaming/algorithms/poolstorage.h"
#include "algorithmfactory.h"
#include "pool.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

BeatDetectionAlgorithm::BeatDetectionAlgorithm()
{
    essentia::init();
}

BeatDetectionAlgorithm::~BeatDetectionAlgorithm()
{
    essentia::shutdown();
}

/**
 * @brief Analyzes the audio delivered by the reader.
 * @param reader for audio (needed configuration -> 44100 Hz, 2 channels and 32 bps (float))
 * @param callback for progress, can be a nullptr
 * @return beats
 */
std::vector<float> BeatDetectionAlgorithm::analyze(callbacks *cb, const Pool &config)
{
    Pool pool;

    cout << "Beat tracker based on multifeature or degara algorithm." << endl;

    streaming::Algorithm *monoloader = new StreamMonoLoader(cb);
    monoloader->declareParameters();
    monoloader->configure("sampleRate", 44100,
                          "downmix", "mix");

    streaming::AlgorithmFactory &factory = streaming::AlgorithmFactory::instance();
    streaming::Algorithm *rhythmextractor = factory.create("RhythmExtractor2013",
                                                           "method",
                                                           //"multifeature");
                                                           "degara"); // faster, lower accuracy

    /////////// CONNECTING THE ALGORITHMS ////////////////
    cout << "-------- connecting algos --------" << endl;
    monoloader->output("audio")             >> rhythmextractor->input("signal");
    // the estimated tick locations
    rhythmextractor->output("ticks")        >> PC(pool, "rhythm.ticks");
    // confidence with which the ticks are detected
    // [0, 1)      very low confidence, the input signal is hard for the employed candidate beat trackers
    // [1, 1.5]    low confidence
    // (1.5, 3.5]  good confidence, accuracy around 80% in AMLt measure
    // (3.5, 5.32] excellent confidence"
    rhythmextractor->output("confidence")   >> PC(pool, "rhythm.ticks_confidence");
    // the tempo estimation
    rhythmextractor->output("bpm")          >> PC(pool, "rhythm.bpm");
    // the list of bpm estimates characterizing the bpm distribution for the signal
    rhythmextractor->output("estimates")    >> PC(pool, "rhythm.estimates");
    // list of beats interval
    rhythmextractor->output("bpmIntervals") >> PC(pool, "rhythm.bpmIntervals");

    /////////// STARTING THE ALGORITHMS //////////////////
    cout << "-------- start processing --------" << endl;

    scheduler::Network network(monoloader);
    network.run();

    cout << "-------- remove empty vectors --------" << endl;

    vector<Real> ticks;
    if (pool.contains<vector<Real> >("rhythm.ticks"))   // there might be empty ticks
    {
        ticks = pool.value<vector<Real> >("rhythm.ticks");
    }
    vector<Real> bpmIntervals;
    if (pool.contains<vector<Real> >("rhythm.bpmIntervals"))   // there might be empty bpmIntervals
    {
        bpmIntervals = pool.value<vector<Real> >("rhythm.bpmIntervals");
    }
    vector<Real> estimates;
    if (pool.contains<vector<Real> >("rhythm.estimates"))   // there might be empty estimates
    {
        estimates = pool.value<vector<Real> >("rhythm.estimates");
    }

    cout << "-------- finished processing --------" << endl;

    cout << setw(4) << "ticks count: " << ticks.size() << endl;
    cout << setw(4) << "bpmIntervals count: " << bpmIntervals.size() << endl;
    cout << setw(4) << "estimates count: " << estimates.size() << endl;
    cout << setw(4) << "bpm: " << pool.value<Real>("rhythm.bpm") << endl;
    cout << setw(4) << "confidence: " << pool.value<Real>("rhythm.ticks_confidence") << endl;

    cout << left;
    for (size_t i = 0; i < ticks.size(); ++i)
    {
        if (i == 0)
        {
            cout << setw(4) << i << " tick: " << setw(8) << ticks.at(i) << endl;
        }
        else
        {
            cout << setw(4) << i
                 << " tick: " << setw(8) << left << ticks.at(i)
                 << " bmpInterval: " << setw(8) << bpmIntervals.at(i - 1) << endl;
        }
    }

    for (size_t i = 0; i < estimates.size(); ++i)
    {
        cout << setw(4) << i << " estimate: " << setw(8) << estimates.at(i) << endl;
    }

    cout << "-------- convert ticks --------" << endl;
    std::vector<essentia_timestamp> poitss;
    for (size_t i = 0; i < ticks.size(); ++i)
    {
        essentia_timestamp ts;
        ts.ts = ticks[i];
        ts.type = Beat;
        //ts.name = "Essentia - BeatTracker degara";
        poitss.push_back(ts);
    }

    return ticks;
}

} // namespace essentiawrapper
