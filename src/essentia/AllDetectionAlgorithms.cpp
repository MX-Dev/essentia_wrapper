/*
 * Copyright (C) 2006-2016  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 *
 * Modified by E. Mista, date 12 december 2016
 *
 */

#include "AllDetectionAlgorithms.h"

#include <string>
#include <memory>

#include "loader/StreamAudioLoader.h"
#include "loader/StreamEasyLoader.h"
#include "loader/EasyLoader.h"
#include "loader/StreamEqloudLoader.h"
#include "standard/StreamStereoTrimmer.h"

#include "algorithmfactory.h"
#include "essentiamath.h"
#include "streaming/algorithms/poolstorage.h"
#include "essentiautil.h"
#include "scheduler/network.h"

// helper functions
#include "configuration/config_util.h"
#include "extractors/streaming_extractorlowlevel.h"
#include "extractors/streaming_extractorsfx.h"
#include "extractors/streaming_extractortonal.h"
#include "extractors/streaming_extractorpostprocess.h"

#include "utils/tnt/tnt2vector.h"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

namespace essentiawrapper {


void compute(const callbacks *cb, Pool &neqloudPool, Pool &eqloudPool, const Pool &options);
void computeSegments(Pool &neqloudPool, Pool &eqloudPool, const Pool &options);
void computeReplayGain(const callbacks *cb, Pool &neqloudPool, Pool &eqloudPool, const Pool &options);
void computeLowLevel(const callbacks *cb, Pool &neqloudPool, Pool &eqloudPool, const Pool &options, Real startTime, Real endTime, const string &nspace = "");
void computeMidLevel(const callbacks *cb, Pool &neqloudPool, Pool &eqloudPool, const Pool &options, Real startTime, Real endTime, const string &nspace = "");
void computePanning(const callbacks *cb, Pool &neqloudPool, Pool &eqloudPool, const Pool &options, Real startTime, Real endTime, const string &nspace = "");
void computeFades(const callbacks *cb, Pool &neqloudPool, Pool &eqloudPool, const Pool &options, Real startTime, Real endTime, const string &nspace = "");
void computeHighlevel(Pool &pool, const Pool &options, const string &nspace = "");
void addSVMDescriptors(Pool &pool);

AllDetectionAlgorithms::AllDetectionAlgorithms()
{
    essentia::init();
}

AllDetectionAlgorithms::~AllDetectionAlgorithms()
{
    essentia::shutdown();
}

/**
 * @brief Analyzes the audio delivered by the reader.
 * @param reader for audio (needed configuration -> 44100 Hz, 2 channels and 32 bps (float))
 * @param callback for progress, can be a nullptr
 * @return
 */
vector<essentia_timestamp> AllDetectionAlgorithms::analyze(callbacks *cb, const Pool &config)
{
    Pool tmpOptions = config;
    Pool mergedOptions;

    setDefaultOptions(mergedOptions);

    mergedOptions.merge(tmpOptions, "replace");

    // pool for storing results
    Pool neqloudPool; // non equal loudness pool
    Pool eqloudPool; // equal loudness pool

    bool neqloud = mergedOptions.value<Real>("nequalLoudness") != 0;
    bool eqloud =  mergedOptions.value<Real>("equalLoudness")  != 0;

    if (!eqloud && !neqloud)
    {
        throw EssentiaException("Configuration for both equal loudness and non\
           equal loudness is set to false. At least one must set to true");
    }

    // beat detection needed if beatsloudness or bpmhistogram detection is requested
    if (mergedOptions.value<Real>("rhythm.beats.loudness.compute") != 0 ||
            mergedOptions.value<Real>("rhythm.bpmhistogram.compute") != 0)
    {
        mergedOptions.set("rhythm.beats.compute", true);
    }

    cout << "-------- start processing --------" << endl;

    compute(cb, neqloudPool, eqloudPool, mergedOptions);

    cout << "-------- finished processing --------" << endl;

    vector<essentia_timestamp> et_vec;

    if (eqloudPool.contains<vector<Real> >("rhythm.beats.position"))
    {
        // return eqloud results
        vector<Real> ticks = eqloudPool.value<vector<Real>>("rhythm.beats.position");

        for (size_t i = 0; i < ticks.size(); ++i)
        {
            essentia_timestamp ts;
            ts.ts = ticks[i];
            ts.type = Beat;
            et_vec.push_back(ts);
        }
    }

    return et_vec;
}

void compute(const callbacks *cb, Pool &neqloudPool, Pool &eqloudPool, const Pool &options)
{

    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud = options.value<Real>("equalLoudness") != 0;

    if (neqloud) neqloudPool.set("metadata.audio_properties.equal_loudness", false);
    if (eqloud) eqloudPool.set("metadata.audio_properties.equal_loudness", true);

    // what to compute:
    bool lowlevel = options.value<Real>("lowlevel.compute")             ||
                    options.value<Real>("average_loudness.compute")     ||
                    options.value<Real>("tonal.compute")                ||
                    options.value<Real>("sfx.compute")                  ||
                    options.value<Real>("rhythm.beats.compute")         ||
                    options.value<Real>("rhythm.onset.compute")         ||
                    options.value<Real>("rhythm.danceability.compute")  ||
                    options.value<Real>("segmentation.compute") != 0;
    bool midlevel = options.value<Real>("tonal.compute") ||
                    options.value<Real>("rhythm.beats.loudness.compute");
    bool panning  = options.value<Real>("panning.compute") != 0;
    bool fades    = options.value<Real>("fades.compute") != 0;

    // compute features for the whole song
    computeReplayGain(cb, neqloudPool, eqloudPool, options);
    Real startTime = options.value<Real>("startTime");
    Real endTime = options.value<Real>("endTime");
    if (eqloud)
    {
        if (endTime > eqloudPool.value<Real>("metadata.audio_properties.length"))
        {
            endTime = eqloudPool.value<Real>("metadata.audio_properties.length");
        }
    }
    else
    {
        if (endTime > neqloudPool.value<Real>("metadata.audio_properties.length"))
        {
            endTime = neqloudPool.value<Real>("metadata.audio_properties.length");
        }
    }
    if (lowlevel) computeLowLevel(cb, neqloudPool, eqloudPool, options, startTime, endTime);
    if (midlevel) computeMidLevel(cb, neqloudPool, eqloudPool, options, startTime, endTime);
    if (panning) computePanning(cb, neqloudPool, eqloudPool, options, startTime, endTime);
    if (fades) computeFades(cb, neqloudPool, eqloudPool, options, startTime, endTime);
    if (neqloud) computeHighlevel(neqloudPool, options);
    if (eqloud) computeHighlevel(eqloudPool, options);

    bool segLowlevel = options.value<Real>("segmentation.desc.lowlevel.compute")         ||
                       options.value<Real>("segmentation.desc.average_loudness.compute") ||
                       options.value<Real>("segmentation.desc.tonal.compute")            ||
                       options.value<Real>("segmentation.desc.sfx.compute")              ||
                       options.value<Real>("segmentation.desc.rhythm.beats.compute")     ||
                       options.value<Real>("segmentation.desc.rhythm.onset.compute")     ||
                       options.value<Real>("segmentation.desc.rhythm.danceability.compute");
    bool segMidlevel = options.value<Real>("segmentation.desc.tonal.compute") ||
                       options.value<Real>("segmentation.desc.rhythm.beats.loudness.compute");
    bool segPanning  = options.value<Real>("segmentation.desc.panning.compute") != 0;
    bool segFades    = options.value<Real>("segmentation.desc.fades.compute") != 0;

    vector<Real> segments;
    if (options.value<Real>("segmentation.compute") != 0)
    {
        computeSegments(neqloudPool, eqloudPool, options);

        segments = eqloudPool.value<vector<Real> >("segmentation.timestamps");
        for (int i = 0; i < int(segments.size() - 1); ++i)
        {
            Real start = segments[i];
            Real end = segments[i + 1];
            cout << "\n**************************************************************************";
            cout << "\nSegment " << i << ": processing audio from " << start << "s to " << end << "s";
            cout << "\n**************************************************************************" << endl;

            // set segment name
            ostringstream ns;
            ns << "segment_" << i;
            string sn = ns.str();
            ns.str("");
            ns << "segments." << sn << ".name";
            if (neqloud) neqloudPool.set(ns.str(), sn);
            if (eqloud) eqloudPool.set(ns.str(), sn);

            // set segment scope
            ns.str("");
            ns << "segments." << sn << ".scope";
            vector<Real> scope(2, 0);
            scope[0] = start;
            scope[1] = end;
            if (neqloud) neqloudPool.set(ns.str(), scope);
            if (eqloud) eqloudPool.set(ns.str(), scope);

            // compute descriptors
            ns.str("");
            ns << "segments.segment_" << i << ".desc";

            if (segLowlevel) computeLowLevel(cb, neqloudPool, eqloudPool, options, start, end, ns.str());
            if (segMidlevel) computeMidLevel(cb, neqloudPool, eqloudPool, options, start, end, ns.str());
            if (segPanning) computePanning(cb, neqloudPool, eqloudPool, options, start, end, ns.str());
            if (segFades) computeFades(cb, neqloudPool, eqloudPool, options, start, end, ns.str());
            if (neqloud) computeHighlevel(neqloudPool, options, ns.str());
            if (eqloud) computeHighlevel(eqloudPool, options, ns.str());

            cout << "\n**************************************************************************\n";
        }
    }

    if (neqloud)
    {
        Pool stats = computeAggregation(neqloudPool, options, segments.size());
        //if (options.value<Real>("svm.compute") != 0) addSVMDescriptors(stats); //not available
        cleanUp(stats, options);
        outputToFile(stats, options.value<string>("nequalOutputPath"), options);
        neqloudPool.remove("metadata.audio_properties.downmix");
    }

    if (eqloud)
    {
        Pool stats = computeAggregation(eqloudPool, options, segments.size());
        if (options.value<Real>("svm.compute") != 0) addSVMDescriptors(stats);
        cleanUp(stats, options);
        outputToFile(stats, options.value<string>("equalOutputPath"), options);
        eqloudPool.remove("metadata.audio_properties.downmix");
    }
}

void computeSegments(Pool &neqloudPool, Pool &eqloudPool, const Pool &options)
{

    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud =  options.value<Real>("equalLoudness")  != 0;

    int minimumSegmentsLength = int(options.value<Real>("segmentation.minimumSegmentsLength"));
    int size1 = int(options.value<Real>("segmentation.size1"));
    int inc1  = int(options.value<Real>("segmentation.inc1"));
    int size2 = int(options.value<Real>("segmentation.size2"));
    int inc2  = int(options.value<Real>("segmentation.inc2"));
    int cpw   = int(options.value<Real>("segmentation.cpw"));

    vector<vector<Real> > features;
    try
    {
        if (eqloud)
            features = eqloudPool.value<vector<vector<Real> > >("lowlevel.mfcc");
        else features = neqloudPool.value<vector<vector<Real> > >("lowlevel.mfcc");
    }
    catch (const EssentiaException &)
    {
        cerr << "Error: could not find MFCC features in low level pool. Aborting..." << endl;
        exit(3);
    }

    TNT::Array2D<Real> featuresArray(features[0].size(), features.size());
    for (int frame = 0; frame < int(features.size()); ++frame)
    {
        for (int mfcc = 0; mfcc < int(features[0].size()); ++mfcc)
        {
            featuresArray[mfcc][frame] = features[frame][mfcc];
        }
    }
    // only BIC segmentation available
    standard::Algorithm *sbic = standard::AlgorithmFactory::create("SBic", "size1", size1, "inc1", inc1,
                                "size2", size2, "inc2", inc2, "cpw", cpw,
                                "minLength", minimumSegmentsLength);
    vector<Real> segments;
    sbic->input("features").set(featuresArray);
    sbic->output("segmentation").set(segments);
    sbic->compute();
    Real analysisSampleRate = options.value<Real>("analysisSampleRate");
    Real step = options.value<Real>("lowlevel.hopSize");

    for (int i = 0; i < int(segments.size()); ++i)
    {
        segments[i] *= step / analysisSampleRate;
        if (neqloud) neqloudPool.add("segmentation.timestamps", segments[i]);
        if (eqloud) eqloudPool.add("segmentation.timestamps", segments[i]);
    }
}

void computeReplayGain(const callbacks *cb, Pool &neqloudPool,
                       Pool &eqloudPool, const Pool &options)
{

    streaming::AlgorithmFactory &factory = streaming::AlgorithmFactory::instance();

    /*************************************************************************
     *    1st pass: get metadata and replay gain                             *
     *************************************************************************/

    cout << "Process step 1: Replay Gain" << endl;

    Real analysisSampleRate = options.value<Real>("analysisSampleRate");

    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud =  options.value<Real>("equalLoudness")  != 0;

    Real startTime = options.value<Real>("startTime");
    Real endTime = options.value<Real>("endTime");

    string downmix = "mix";
    Real replayGain = 0.0;
    bool tryReallyHard = true;
    int length = 0;

    while (tryReallyHard)
    {

        Algorithm *streamEqloudloader = new StreamEqloudLoader(cb);
        streamEqloudloader->declareParameters();
        streamEqloudloader->configure("sampleRate", analysisSampleRate,
                                      "startTime",  startTime,
                                      "endTime",    endTime,
                                      "downmix",    downmix);

        Algorithm *rgain   = factory.create("ReplayGain",
                                            "applyEqloud", false);

        if (neqloud)
        {
            neqloudPool.set("metadata.audio_properties.analysis_sample_rate", streamEqloudloader->parameter("sampleRate").toReal());
            neqloudPool.set("metadata.audio_properties.downmix", downmix);
        }
        if (eqloud)
        {
            eqloudPool.set("metadata.audio_properties.analysis_sample_rate", streamEqloudloader->parameter("sampleRate").toReal());
            eqloudPool.set("metadata.audio_properties.downmix", downmix);
        }

        streamEqloudloader->output("audio")  >>  rgain->input("signal");
        if (neqloud)
            rgain->output("replayGain")  >>  PC(neqloudPool, "metadata.audio_properties.replay_gain");
        if (eqloud)
            rgain->output("replayGain")  >>  PC(eqloudPool, "metadata.audio_properties.replay_gain");

        cout << "Process step 1: Replay Gain" << endl;
        try
        {
            Network network(streamEqloudloader);
            network.run();
            length = streamEqloudloader->output("audio").totalProduced();
            tryReallyHard = false;
        }

        catch (const EssentiaException &)
        {
            if (downmix == "mix")
            {
                downmix = "left";
                try
                {
                    neqloudPool.remove("metadata.audio_properties.downmix");
                    neqloudPool.remove("metadata.audio_properties.replay_gain");
                    eqloudPool.remove("metadata.audio_properties.downmix");
                    eqloudPool.remove("metadata.audio_properties.replay_gain");
                }
                catch (EssentiaException &) {}

                continue;
            }
            else
            {
                cerr << "ERROR: File looks like a completely silent file... Aborting..." << endl;
                exit(4);
            }
        }

        if (eqloud) replayGain = eqloudPool.value<Real>("metadata.audio_properties.replay_gain");
        else replayGain = neqloudPool.value<Real>("metadata.audio_properties.replay_gain");

        // very high value for replayGain, we are probably analyzing a silence even
        // though it is not a pure digital silence
        if (replayGain > 40.0)   // before it was set to 20 but it was found too conservative
        {
            // NB: except if it was some electro music where someone thought it was smart
            //     to have opposite left and right channels... Try with only the left
            //     channel, then.
            if (downmix == "mix")
            {
                downmix = "left";
                tryReallyHard = true;
                neqloudPool.remove("metadata.audio_properties.downmix");
                neqloudPool.remove("metadata.audio_properties.replay_gain");
                eqloudPool.remove("metadata.audio_properties.downmix");
                eqloudPool.remove("metadata.audio_properties.replay_gain");
            }
            else
            {
                cerr << "ERROR: File looks like a completely silent file... Aborting..." << endl;
                exit(5);
            }
        }
    }
    // set length (actually duration) of the file:
    if (neqloud) neqloudPool.set("metadata.audio_properties.length", length / analysisSampleRate);
    if (eqloud) eqloudPool.set("metadata.audio_properties.length", length / analysisSampleRate);

    cout.precision(10);
}

void computeLowLevel(const callbacks *cb, Pool &neqloudPool, Pool &eqloudPool,
                     const Pool &options, Real startTime, Real endTime, const string &nspace)
{
    /*************************************************************************
     *    2nd pass: normalize the audio with replay gain, compute as         *
     *              many lowlevel descriptors as possible                    *
     *************************************************************************/

    cout << "Process step 2: Low Level" << endl;

    // namespace:
    string llspace = "lowlevel.";
    string rhythmspace = "rhythm.";
    if (!nspace.empty())
    {
        llspace = nspace + ".lowlevel.";
        rhythmspace = nspace + ".rhythm.";
    }

    bool shortsound = options.value<Real>("shortSound")  != 0;

    Real analysisSampleRate = options.value<Real>("analysisSampleRate");
    Real replayGain = 0;
    string downmix = "mix";

    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud  = options.value<Real>("equalLoudness")  != 0;

    if (eqloud)
    {
        replayGain = eqloudPool.value<Real>("metadata.audio_properties.replay_gain");
        downmix = eqloudPool.value<string>("metadata.audio_properties.downmix");
    }
    else
    {
        replayGain = neqloudPool.value<Real>("metadata.audio_properties.replay_gain");
        downmix = neqloudPool.value<string>("metadata.audio_properties.downmix");
    }

    streaming::AlgorithmFactory &factory = streaming::AlgorithmFactory::instance();

    Algorithm *streamEasyLoader = new StreamEasyLoader(cb);
    streamEasyLoader->declareParameters();
    streamEasyLoader->configure("sampleRate", analysisSampleRate,
                                "startTime",  startTime,
                                "endTime",    endTime,
                                "replayGain", replayGain,
                                "downmix",    downmix);

    SourceBase &neqloudSource = streamEasyLoader->output("audio");
    Algorithm *eqloud2 = factory.create("EqualLoudness");
    connect(streamEasyLoader->output("audio"), eqloud2->input("signal"));
    SourceBase &eqloudSource = eqloud2->output("signal");

    if (neqloud)
    {
        LowLevelSpectral(neqloudSource, neqloudPool, options, nspace);

        // Low-Level Spectral Equal Loudness Descriptors
        // expects the audio source to already be equal-loudness filtered, so it
        // must use the eqloudSouce instead of neqloudSource
        LowLevelSpectralEqLoud(eqloudSource, neqloudPool, options, nspace);

        // Level Descriptor
        // expects the audio source to already be equal-loudness filtered, so it
        // must use the eqloudSouce instead of neqloudSource
        // results needed for average loudness
        bool computeAverageLoudness = nspace.empty() ?
                                      options.value<Real>("average_loudness.compute") != 0 :
                                      options.value<Real>("segmentation.desc.average_loudness.compute") != 0;
        if (!shortsound || computeAverageLoudness)
            Level(eqloudSource, neqloudPool, options, nspace);

        // Tuning Frequency
        bool computeTonal = nspace.empty() ?
                            options.value<Real>("tonal.compute") != 0 :
                            options.value<Real>("segmentation.desc.tonal.compute") != 0;
        if (computeTonal)
            TuningFrequency(neqloudSource, neqloudPool, options, nspace);

        // Rhythm descriptor - beats
        bool computeBeats = nspace.empty() ?
                            options.value<Real>("rhythm.beats.compute") != 0 :
                            options.value<Real>("segmentation.desc.rhythm.beats.compute") != 0;
        if (computeBeats)
        {

            Algorithm *rhythmExtractor = factory.create("RhythmExtractor2013");
            rhythmExtractor->configure("method", options.value<string>("rhythm.beats.method"),
                                       "maxTempo", options.value<Real>("rhythm.beats.maxTempo"),
                                       "minTempo", options.value<Real>("rhythm.beats.minTempo"));

            connect(neqloudSource, rhythmExtractor->input("signal"));
            connect(rhythmExtractor->output("ticks"),        neqloudPool, rhythmspace + "beats.position");
            connect(rhythmExtractor->output("bpm"),          neqloudPool, rhythmspace + "bpm");
            connect(rhythmExtractor->output("estimates"),    neqloudPool, rhythmspace + "bpm_estimates");
            connect(rhythmExtractor->output("bpmIntervals"), neqloudPool, rhythmspace + "bpm_intervals");
            // discard dummy value for confidence as 'degara' beat tracker is not able to compute it
            rhythmExtractor->output("confidence") >> NOWHERE;

            // Rhythm descriptor - bmp histogram
            bool computeBpmHistogram = nspace.empty() ?
                                       options.value<Real>("rhythm.bpmhistogram.compute") != 0 :
                                       options.value<Real>("segmentation.desc.rhythm.bpmhistogram.compute") != 0;
            if (computeBpmHistogram)
            {

                // BPM Histogram descriptors
                Algorithm *bpmhist = factory.create("BpmHistogramDescriptors");
                connect(rhythmExtractor->output("bpmIntervals"), bpmhist->input("bpmIntervals"));
                connectSingleValue(bpmhist->output("firstPeakBPM"),     neqloudPool, rhythmspace + "first_peak_bpm");
                connectSingleValue(bpmhist->output("firstPeakWeight"),  neqloudPool, rhythmspace + "first_peak_weight");
                connectSingleValue(bpmhist->output("firstPeakSpread"),  neqloudPool, rhythmspace + "first_peak_spread");
                connectSingleValue(bpmhist->output("secondPeakBPM"),    neqloudPool, rhythmspace + "second_peak_bpm");
                connectSingleValue(bpmhist->output("secondPeakWeight"), neqloudPool, rhythmspace + "second_peak_weight");
                connectSingleValue(bpmhist->output("secondPeakSpread"), neqloudPool, rhythmspace + "second_peak_spread");
                bpmhist->output("histogram") >> NOWHERE;
            }
        }

        // Rhythm descriptor - onset
        bool computeOnsets = nspace.empty() ?
                             options.value<Real>("rhythm.onset.compute") != 0 :
                             options.value<Real>("segmentation.desc.rhythm.onset.compute") != 0;
        if (computeOnsets)
        {
            // Onset Detection
            Algorithm *onset = factory.create("OnsetRate");
            connect(neqloudSource, onset->input("signal"));
            connect(onset->output("onsetTimes"), neqloudPool, rhythmspace + "onset_times");
            connect(onset->output("onsetRate"), NOWHERE);  //pool, rhythmspace + "onset_rate"); // this is done later
        }

        // Rhythm descriptor - danceability
        bool computeDanceability = nspace.empty() ?
                                   options.value<Real>("rhythm.danceability.compute") != 0 :
                                   options.value<Real>("segmentation.desc.rhythm.danceability.compute") != 0;
        if (computeDanceability)
        {
            Algorithm *danceability = factory.create("Danceability",
                                      "minTau", options.value<Real>("rhythm.danceability.minTau"),
                                      "maxTau", options.value<Real>("rhythm.danceability.maxTau"),
                                      "tauMultiplier", options.value<Real>("rhythm.danceability.tauMultiplier"),
                                      "sampleRate", options.value<Real>("analysisSampleRate"));
            connect(neqloudSource, danceability->input("signal"));
            connect(danceability->output("danceability"), neqloudPool, rhythmspace + "danceability");
        }
    }

    if (eqloud)
    {

        // Low-Level Spectral Descriptors
        LowLevelSpectral(eqloudSource, eqloudPool, options, nspace);

        // Low-Level Spectral Equal Loudness Descriptors
        // expects the audio source to already be equal-loudness filtered
        LowLevelSpectralEqLoud(eqloudSource, eqloudPool, options, nspace);

        // Level Descriptor
        // expects the audio source to already be equal-loudness filtered
        // results needed for average_loudness
        bool computeAverageLoudness = nspace.empty() ?
                                      options.value<Real>("average_loudness.compute") != 0 :
                                      options.value<Real>("segmentation.desc.average_loudness.compute") != 0;
        if (!shortsound || computeAverageLoudness)
            Level(eqloudSource, eqloudPool, options, nspace);

        // Tuning Frequency
        bool computeTonal = nspace.empty() ?
                            options.value<Real>("tonal.compute") != 0 :
                            options.value<Real>("segmentation.desc.tonal.compute") != 0;
        if (computeTonal)
            TuningFrequency(eqloudSource, eqloudPool, options, nspace);

        // Rhythm descriptor - beats
        bool computeBeats = nspace.empty() ?
                            options.value<Real>("rhythm.beats.compute") != 0 :
                            options.value<Real>("segmentation.desc.rhythm.beats.compute") != 0;
        if (computeBeats)
        {

            Algorithm *rhythmExtractor = factory.create("RhythmExtractor2013");
            rhythmExtractor->configure("method", options.value<string>("rhythm.beats.method"),
                                       "maxTempo", options.value<Real>("rhythm.beats.maxTempo"),
                                       "minTempo", options.value<Real>("rhythm.beats.minTempo"));

            connect(eqloudSource, rhythmExtractor->input("signal"));
            connect(rhythmExtractor->output("ticks"),        eqloudPool, rhythmspace + "beats.position");
            connect(rhythmExtractor->output("bpm"),          eqloudPool, rhythmspace + "bpm");
            connect(rhythmExtractor->output("estimates"),    eqloudPool, rhythmspace + "bpm_estimates");
            connect(rhythmExtractor->output("bpmIntervals"), eqloudPool, rhythmspace + "bpm_intervals");
            // discard dummy value for confidence as 'degara' beat tracker is not able to compute it
            rhythmExtractor->output("confidence") >> NOWHERE;

            // Rhythm descriptor - bmp histogram
            bool computeBpmHistogram = nspace.empty() ?
                                       options.value<Real>("rhythm.bpmhistogram.compute") != 0 :
                                       options.value<Real>("segmentation.desc.rhythm.bpmhistogram.compute") != 0;
            if (computeBpmHistogram)
            {
                // BPM Histogram descriptors
                Algorithm *bpmhist = factory.create("BpmHistogramDescriptors");
                connect(rhythmExtractor->output("bpmIntervals"), bpmhist->input("bpmIntervals"));
                connectSingleValue(bpmhist->output("firstPeakBPM"),     eqloudPool, rhythmspace + "first_peak_bpm");
                connectSingleValue(bpmhist->output("firstPeakWeight"),  eqloudPool, rhythmspace + "first_peak_weight");
                connectSingleValue(bpmhist->output("firstPeakSpread"),  eqloudPool, rhythmspace + "first_peak_spread");
                connectSingleValue(bpmhist->output("secondPeakBPM"),    eqloudPool, rhythmspace + "second_peak_bpm");
                connectSingleValue(bpmhist->output("secondPeakWeight"), eqloudPool, rhythmspace + "second_peak_weight");
                connectSingleValue(bpmhist->output("secondPeakSpread"), eqloudPool, rhythmspace + "second_peak_spread");
                bpmhist->output("histogram") >> NOWHERE;
            }
        }

        // Rhythm descriptor - onset
        bool computeOnsets = nspace.empty() ?
                             options.value<Real>("rhythm.onset.compute") != 0 :
                             options.value<Real>("segmentation.desc.rhythm.onset.compute") != 0;
        if (computeOnsets)
        {
            // Onset Detection
            Algorithm *onset = factory.create("OnsetRate");
            connect(eqloudSource, onset->input("signal"));
            connect(onset->output("onsetTimes"), eqloudPool, rhythmspace + "onset_times");
            connect(onset->output("onsetRate"), NOWHERE);  //pool, rhythmspace + "onset_rate"); // this is done later
        }

        // Rhythm descriptor - danceability
        bool computeDanceability = nspace.empty() ?
                                   options.value<Real>("rhythm.danceability.compute") != 0 :
                                   options.value<Real>("segmentation.desc.rhythm.danceability.compute") != 0;
        if (computeDanceability)
        {
            Algorithm *danceability = factory.create("Danceability",
                                      "minTau", options.value<Real>("rhythm.danceability.minTau"),
                                      "maxTau", options.value<Real>("rhythm.danceability.maxTau"),
                                      "tauMultiplier", options.value<Real>("rhythm.danceability.tauMultiplier"),
                                      "sampleRate", options.value<Real>("analysisSampleRate"));
            connect(eqloudSource, danceability->input("signal"));
            connect(danceability->output("danceability"), eqloudPool, rhythmspace + "danceability");
        }
    }

    Network network(streamEasyLoader);
    network.run();

    if (!shortsound)
    {
        // check if we processed enough audio for it to be useful, in particular did
        // we manage to get an estimation for the loudness (2 seconds required)
        try
        {
            if (eqloud) eqloudPool.value<vector<Real> >(llspace + "loudness")[0];
            else neqloudPool.value<vector<Real> >(llspace + "loudness")[0];
        }
        catch (EssentiaException &)
        {
            cerr << "ERROR: File is too short (< 2sec)... Aborting..." << endl;
            exit(6);
        }
    }

    bool computeOnsets = nspace.empty() ?
                         options.value<Real>("rhythm.onset.compute") != 0 :
                         options.value<Real>("segmentation.desc.rhythm.onset.compute") != 0;
    if (computeOnsets)
    {
        // compute onset rate = len(onsets) / len(audio)
        if (neqloud)
        {
            neqloudPool.set(rhythmspace + "onset_rate", neqloudPool.value<vector<Real> >(rhythmspace + "onset_times").size()
                            / (Real)streamEasyLoader->output("audio").totalProduced()
                            * neqloudPool.value<Real>("metadata.audio_properties.analysis_sample_rate"));
        }
        if (eqloud)
        {
            eqloudPool.set(rhythmspace + "onset_rate", eqloudPool.value<vector<Real> >(rhythmspace + "onset_times").size()
                           / (Real)streamEasyLoader->output("audio").totalProduced()
                           * eqloudPool.value<Real>("metadata.audio_properties.analysis_sample_rate"));
        }
    }


    // delete network only now, because we needed streamEasyLoader->output("audio") to
    // compute the onset rate on the previous line.
    //deleteNetwork(streamEasyLoader);
}

void computeMidLevel(const callbacks *cb, Pool &neqloudPool,
                     Pool &eqloudPool, const Pool &options,
                     Real startTime, Real endTime, const string &nspace)
{

    /*************************************************************************
     *    4th pass: HPCP & beats loudness (depend on some descriptors that   *
     *              have been computed during the 2nd pass)                  *
     *************************************************************************/

    cout << "Process step 4: Mid Level" << endl;

    Real analysisSampleRate = options.value<Real>("analysisSampleRate");
    Real replayGain = 0;
    string downmix = "mix";

    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud  = options.value<Real>("equalLoudness")  != 0;

    if (eqloud)
    {
        replayGain = eqloudPool.value<Real>("metadata.audio_properties.replay_gain");
        downmix = eqloudPool.value<string>("metadata.audio_properties.downmix");
    }
    else
    {
        replayGain = neqloudPool.value<Real>("metadata.audio_properties.replay_gain");
        downmix = neqloudPool.value<string>("metadata.audio_properties.downmix");
    }

    streaming::AlgorithmFactory &factory = streaming::AlgorithmFactory::instance();

    Algorithm *streamEasyLoader = new StreamEasyLoader(cb);
    streamEasyLoader->declareParameters();
    streamEasyLoader->configure("sampleRate", analysisSampleRate,
                                "startTime",  startTime,
                                "endTime",    endTime,
                                "replayGain", replayGain,
                                "downmix",    downmix);

    if (neqloud)
    {
        SourceBase &neqloudSource = streamEasyLoader->output("audio");
        // Compute Tonal descriptors (needed TuningFrequency before)
        bool computeTonal = nspace.empty() ?
                            options.value<Real>("tonal.compute") != 0 :
                            options.value<Real>("segmentation.desc.tonal.compute") != 0;
        if (computeTonal)
            TonalDescriptors(neqloudSource, neqloudPool, options, nspace);

        // Compute the loudness at the beats position (needed beats position)
        bool computeBeats = nspace.empty() ?
                            options.value<Real>("rhythm.beats.loudness.compute") != 0 :
                            options.value<Real>("segmentation.desc.rhythm.beats.loudness.compute") != 0;
        if (computeBeats)
        {
            string rhythmspace = "rhythm.";
            if (!nspace.empty()) rhythmspace = nspace + ".rhythm.";
            vector<Real> ticks = neqloudPool.value<vector<Real> >(rhythmspace + "beats.position");

            Algorithm *beatsLoudness = factory.create("BeatsLoudness",
                                       "sampleRate", analysisSampleRate,
                                       "beats", ticks);

            connect(neqloudSource, beatsLoudness->input("signal"));
            connect(beatsLoudness->output("loudness"), neqloudPool, rhythmspace + "beats.loudness");
            connect(beatsLoudness->output("loudnessBandRatio"), neqloudPool, rhythmspace + "beats.loudness_band_ratio");
        }
    }
    if (eqloud)
    {
        Algorithm *eqloud3 = factory.create("EqualLoudness");
        connect(streamEasyLoader->output("audio"), eqloud3->input("signal"));
        SourceBase &eqloudSource = eqloud3->output("signal");
        // Compute Tonal descriptors (needed TuningFrequency before)
        bool computeTonal = nspace.empty() ?
                            options.value<Real>("tonal.compute") != 0 :
                            options.value<Real>("segmentation.desc.tonal.compute") != 0;
        if (computeTonal)
            TonalDescriptors(eqloudSource, eqloudPool, options, nspace);

        // Compute the loudness at the beats position (needed beats position)
        bool computeBeats = nspace.empty() ?
                            options.value<Real>("rhythm.beats.loudness.compute") != 0 :
                            options.value<Real>("segmentation.desc.rhythm.beats.loudness.compute") != 0;
        if (computeBeats)
        {
            string rhythmspace = "rhythm.";
            if (!nspace.empty()) rhythmspace = nspace + ".rhythm.";
            vector<Real> ticks = eqloudPool.value<vector<Real> >(rhythmspace + "beats.position");

            Algorithm *beatsLoudness = factory.create("BeatsLoudness",
                                       "sampleRate", analysisSampleRate,
                                       "beats", ticks);

            connect(eqloudSource, beatsLoudness->input("signal"));
            connect(beatsLoudness->output("loudness"), eqloudPool, rhythmspace + "beats.loudness");
            connect(beatsLoudness->output("loudnessBandRatio"), eqloudPool, rhythmspace + "beats.loudness_band_ratio");
        }
    }

    Network network(streamEasyLoader);
    network.run();
}

void computePanning(const callbacks *cb, Pool &neqloudPool,
                    Pool &eqloudPool, const Pool &options,
                    Real startTime, Real endTime, const string &nspace)
{

    /*************************************************************************
     *    5th pass: Panning                                                  *
     *                                                                       *
     *************************************************************************/

    cout << "Process step 5: Panning" << endl;

    Real analysisSampleRate = options.value<Real>("analysisSampleRate");
    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud =  options.value<Real>("equalLoudness")  != 0;

    Algorithm *streamAudioLoader = new StreamAudioLoader(cb);
    streamAudioLoader->declareParameters();
    streamAudioLoader->configure("sampleRate", analysisSampleRate);


    Algorithm *stereoTrimmer = new StreamStereoTrimmer();
    stereoTrimmer->declareParameters();
    stereoTrimmer->configure("startTime", startTime,
                             "endTime", endTime);

    connect(streamAudioLoader->output("audio"), stereoTrimmer->input("signal"));
    connect(streamAudioLoader->output("numberChannels"), NOWHERE);

    // namespace
    string panningspace = "panning.";
    if (!nspace.empty()) panningspace = nspace + ".panning.";

    Real sampleRate = options.value<Real>("analysisSampleRate");
    int frameSize   = int(options.value<Real>("panning.frameSize"));
    int hopSize     = int(options.value<Real>("panning.hopSize"));
    int averageFrames = int(options.value<Real>("panning.averageFrames"));
    int panningBins   = int(options.value<Real>("panning.panningBins"));
    int numCoeffs     = int(options.value<Real>("panning.numCoeffs"));
    int numBands      = int(options.value<Real>("panning.numBands"));
    bool warpedPanorama = options.value<Real>("panning.warpedPanorama");
    int zeroPadding     = int(options.value<Real>("panning.zeroPadding"));
    string silentFrames = options.value<string>("panning.silentFrames");
    string windowType   = options.value<string>("panning.windowType");

    streaming::AlgorithmFactory &factory = streaming::AlgorithmFactory::instance();

    Algorithm *demuxer = factory.create("StereoDemuxer");

    Algorithm *fc_left = factory.create("FrameCutter",
                                        "frameSize", frameSize,
                                        "hopSize", hopSize,
                                        "startFromZero", false,
                                        "silentFrames", silentFrames);

    Algorithm *fc_right  = factory.create("FrameCutter",
                                          "frameSize", frameSize,
                                          "hopSize", hopSize,
                                          "startFromZero", false,
                                          "silentFrames", silentFrames);

    Algorithm *w_left = factory.create("Windowing",
                                       "size", frameSize,
                                       "zeroPadding", zeroPadding,
                                       "type", windowType);

    Algorithm *w_right = factory.create("Windowing",
                                        "size", frameSize,
                                        "zeroPadding", zeroPadding,
                                        "type", windowType);

    Algorithm *spec_left = factory.create("Spectrum");

    Algorithm *spec_right = factory.create("Spectrum");

    Algorithm *pan = factory.create("Panning",
                                    "sampleRate", sampleRate,
                                    "averageFrames", averageFrames,
                                    "panningBins", panningBins,
                                    "numCoeffs", numCoeffs,
                                    "numBands", numBands,
                                    "warpedPanorama", warpedPanorama);

    connect(stereoTrimmer->output("signal"), demuxer->input("audio"));
    connect(demuxer->output("left"), fc_left->input("signal"));
    connect(demuxer->output("right"), fc_right->input("signal"));
    // left channel
    connect(fc_left->output("frame"), w_left->input("frame"));
    connect(w_left->output("frame"), spec_left->input("frame"));
    connect(spec_left->output("spectrum"), pan->input("spectrumLeft"));
    // right channel
    connect(fc_right->output("frame"), w_right->input("frame"));
    connect(w_right->output("frame"), spec_right->input("frame"));
    connect(spec_right->output("spectrum"), pan->input("spectrumRight"));

    // no difference between eqloud and neqloud, both are taken as non eqloud
    if (neqloud) connect(pan->output("panningCoeffs"), neqloudPool, panningspace + "panning_coefficients");
    if (eqloud) connect(pan->output("panningCoeffs"), eqloudPool, panningspace + "panning_coefficients");

    Network network(streamAudioLoader);
    network.run();
}

typedef TNT::Array2D<Real> array2d;

void addToPool(const array2d &a2d,
               const string &desc, // descriptor names
               Pool &pool)
{
    if (a2d.dim1() > 0)
    {
        vector<vector<Real> > v2d =  array2DToVecvec(a2d);
        for (size_t i = 0; i < v2d.size(); ++i)
            pool.add(desc, v2d[i]);
    }
}

void computeFades(const callbacks *cb, Pool &neqloudPool,
                  Pool &eqloudPool, const Pool &options,
                  Real startTime, Real endTime, const string &nspace)
{

    /*************************************************************************
     *    6th pass: Fades                                                    *
     *                                                                       *
     *************************************************************************/

    cout << "Process step 6: Fades" << endl;

    Real analysisSampleRate = options.value<Real>("analysisSampleRate");
    Real replayGain = 0;
    string downmix = "mix";

    bool neqloud = options.value<Real>("nequalLoudness") != 0;
    bool eqloud  = options.value<Real>("equalLoudness")  != 0;

    if (eqloud)
    {
        replayGain = eqloudPool.value<Real>("metadata.audio_properties.replay_gain");
        downmix = eqloudPool.value<string>("metadata.audio_properties.downmix");
    }
    else
    {
        replayGain = neqloudPool.value<Real>("metadata.audio_properties.replay_gain");
        downmix = neqloudPool.value<string>("metadata.audio_properties.downmix");
    }

    // namespace
    string fadesspace = "fades.";
    if (!nspace.empty()) fadesspace = nspace + ".fades.";

    int frameSize   = int(options.value<Real>("fades.frameSize"));
    int hopSize     = int(options.value<Real>("fades.hopSize"));
    int frameRate   = int(options.value<Real>("fades.frameRate"));
    int minLength   = int(options.value<Real>("fades.minLength"));
    Real cutoffHigh = options.value<Real>("fades.cutoffHigh");
    Real cutoffLow  = options.value<Real>("fades.cutoffLow");

    standard::AlgorithmFactory &factory = standard::AlgorithmFactory::instance();

    standard::Algorithm *easyLoader = new EasyLoader(cb);
    easyLoader->declareParameters();
    easyLoader->configure("sampleRate", analysisSampleRate,
                          "startTime",  startTime,
                          "endTime",    endTime,
                          "replayGain", replayGain,
                          "downmix",    downmix);


    shared_ptr<standard::Algorithm> frameCutter(factory.create("FrameCutter",
            "frameSize", frameSize,
            "hopSize", hopSize));

    shared_ptr<standard::Algorithm> rms(factory.create("RMS"));

    shared_ptr<standard::Algorithm> fadeDetect(factory.create("FadeDetection",
            "minLength", minLength,
            "cutoffHigh", cutoffHigh,
            "cutoffLow", cutoffLow,
            "frameRate", frameRate));

    vector<Real> audio_easy;
    easyLoader->output("audio").set(audio_easy);

    // set frameCutter:
    vector<Real> frame;
    frameCutter->input("signal").set(audio_easy);
    frameCutter->output("frame").set(frame);

    // set rms:
    Real rms_value;
    rms->input("array").set(frame);
    rms->output("rms").set(rms_value);

    // we need a vector to store rms values:
    std::vector<Real> rms_vector;

    cout << "-------- start processing --------" << endl;

    // load audio:
    easyLoader->compute();

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

    if (neqloud)
    {
        addToPool(fade_in, fadesspace + "fadeIns", neqloudPool);
        addToPool(fade_out, fadesspace + "fadeOuts", neqloudPool);
    }
    if (eqloud)
    {
        addToPool(fade_in, fadesspace + "fadeIns", eqloudPool);
        addToPool(fade_out, fadesspace + "fadeOuts", eqloudPool);
    }
}

void computeHighlevel(Pool &pool, const Pool &options, const string &nspace)
{

    /*************************************************************************
     *    7th pass: High-level descriptors that depend on others, but we     *
     *              don't need to stream the audio anymore                   *
     *************************************************************************/

    cout << "Process step 7: High Level" << endl;

    // Average Level
    bool computeAverageLoudness = nspace.empty() ?
                                  options.value<Real>("average_loudness.compute") != 0 :
                                  options.value<Real>("segmentation.desc.average_loudness.compute") != 0;
    if (computeAverageLoudness)
        LevelAverage(pool, nspace);

    // SFX Descriptors
    bool computeSfx = nspace.empty() ?
                      options.value<Real>("sfx.compute") != 0 :
                      options.value<Real>("segmentation.desc.sfx.compute") != 0;
    if (computeSfx)
        SFXPitch(pool, nspace);

    // Tuning System Features
    bool computeTonal = nspace.empty() ?
                        options.value<Real>("tonal.compute") != 0 :
                        options.value<Real>("segmentation.desc.tonal.compute") != 0;
    if (computeTonal)
    {
        TuningSystemFeatures(pool, nspace);
        // Pool Cleaning (remove temporary descriptors)
        TonalPoolCleaning(pool, nspace);
    }

    // Add missing descriptors which are not computed yet, but will be for the
    // final release or during the 1.x cycle. However, the schema need to be
    // complete before that, so just put default values for these.
    PostProcess(pool, options, nspace);
}

void addSVMDescriptors(Pool &pool)
{
    cout << "Process step 7: SVM Models" << endl;
    //const char* svmModels[] = {}; // leave this empty if you don't have any SVM models
    const char *svmModels[] = { "genre_tzanetakis", "genre_dortmund",
                                "genre_electronica", "genre_rosamerica",
                                "mood_acoustic", "mood_aggressive",
                                "mood_electronic", "mood_happy",
                                "mood_party", "mood_relaxed", "mood_sad",
                                "perceptual_speed", "timbre",
                                "culture", "gender", "live_studio",
                                "mirex-moods", "ballroom",
                                "voice_instrumental", "speech_music"
                              };

    string pathToSvmModels;

#ifdef OS_WIN32
    pathToSvmModels = "svm_models\\";
#else
    pathToSvmModels = "svm_models/";
#endif

    for (int i = 0; i < (int)ARRAY_SIZE(svmModels); i++)
    {
        //cout << "adding HL desc: " << svmModels[i] << endl;
        string modelFilename = pathToSvmModels + string(svmModels[i]) + ".history";
        shared_ptr<standard::Algorithm> svm(standard::AlgorithmFactory::create("GaiaTransform",
                                            "history", modelFilename));

        svm->input("pool").set(pool);
        svm->output("pool").set(pool);
        svm->compute();
    }
}

} // namespace essentiawrapper
