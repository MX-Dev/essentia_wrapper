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

#include "config_util.h"

#include "../writer/YamlOutput.h"

#include "streaming/algorithms/poolstorage.h"
#include "scheduler/network.h"

void setDefaultOptions(Pool &pool)
{
    // general
    pool.set("equalLoudness", true);                        // {false,true}                     | use equal loudness
    pool.set("nequalLoudness", false);                      // {false,true}                     | use non equal loudness
    pool.set("shortSound", false);                          // {false,true}                     | set to true if duration is below 2 seconds
    pool.set("startTime", 0);                               // [0,end)                          | analyse from (seconds)
    pool.set("endTime", 2000.0);                            // (0,end]                          | analyse to (seconds)
    pool.set("analysisSampleRate", 44100.0);                // (0,inf)                          | the sampling rate of the audio signal [Hz]

    pool.set("equalOutputPath", "");                        // string (default="")              | equal result output to file
    pool.set("nequalOutputPath", "");                       // string (default="")              | nequal result output to file
    pool.set("outputFormat", "json");                       // {yaml,json}                      | result output format

    // svm
    pool.set("svm.compute", false);                         // not supported yet (gaia)

    // segmentation
    pool.set("segmentation.compute", false);                // {false,true}                     | compute segments
    pool.set("segmentation.size1", 300);                    // [1,inf)                          | first pass window size [frames]
    pool.set("segmentation.inc1", 60);                      // [1,inf)                          | first pass increment [frames]
    pool.set("segmentation.size2", 200);                    // [1,inf)                          | second pass window size [frames]
    pool.set("segmentation.inc2", 20);                      // [1,inf)                          | second pass increment [frames]
    pool.set("segmentation.cpw", 1.5);                      // [0,inf)                          | complexity penalty weight
    pool.set("segmentation.minimumSegmentsLength", 10);     // [1,inf)                          | minimum length of a segment [frames]
    pool.set("segmentation.desc.lowlevel.compute", false);  // {false,true}                     | compute lowlevel descriptors for segments
    pool.set("segmentation.desc.average_loudness.compute", false);      // {false,true}         | compute average_loudness descriptors for segments
    pool.set("segmentation.desc.rhythm.beats.compute", false);          // {false,true}         | compute rhythm descriptors for segments
    pool.set("segmentation.desc.rhythm.beats.loudness.compute", false); // {false,true}         | compute loudness of beats for segments
    pool.set("segmentation.desc.rhythm.bpmhistogram.compute", false);   // {false,true}         | compute a bpm histogram for segments
    pool.set("segmentation.desc.rhythm.onset.compute", false);          // {false,true}         | compute onsets for segments
    pool.set("segmentation.desc.rhythm.danceability.compute", false);   // {false,true}         | compute danceability for segments
    pool.set("segmentation.desc.tonal.compute", false);     // {false,true}                     | compute tonal descriptors for segments
    pool.set("segmentation.desc.sfx.compute", false);       // {false,true}                     | compute sfx descriptors for segments
    pool.set("segmentation.desc.panning.compute", false);   // {false,true}                     | compute panning descriptors for segments
    pool.set("segmentation.desc.fades.compute", false);     // {false,true}                     | compute fades descriptors for segments

    // lowlevel
    pool.set("lowlevel.compute", false);                    // {false,true}                     | compute some low level things
    pool.set("lowlevel.frameSize", 2048);                   // [1,inf)                          | the size of the frame to cut
    pool.set("lowlevel.hopSize", 1024);                     // [1,inf)                          | the number of samples to jump after a frame is output
    pool.set("lowlevel.zeroPadding", 0);                    // [0,inf)                          | the size of the zero-padding
    pool.set("lowlevel.windowType", "blackmanharris62");    // {hamming,hann,triangular,square, | the window type
    //  blackmanharris62/70/74/92}
    pool.set("lowlevel.silentFrames", "noise");             // {drop,keep,noise}                | whether to [keep/drop/add noise to] silent frames

    // average_loudness
    pool.set("average_loudness.compute", false);            // {false,true}                     | compute the average loudness
    pool.set("average_loudness.frameSize", 88200);          // [1,inf)                          | the size of the frame to cut
    pool.set("average_loudness.hopSize", 44100);            // [1,inf)                          | the number of samples to jump after a frame is output
    pool.set("average_loudness.windowType", "hann");        // {hamming,hann,triangular,square, | the window type
    //  blackmanharris62/70/74/92}
    pool.set("average_loudness.silentFrames", "noise");     // {drop,keep,noise}                | whether to [keep/drop/add noise to] silent frames

    // rhythm
    pool.set("rhythm.beats.compute", true);                 // {false,true}                     | compute beats
    pool.set("rhythm.beats.method", "degara");              // {multifeature,degara}            | the method used for beat tracking
    pool.set("rhythm.beats.minTempo", 40);                  // [40,180]                         | the fastest tempo to detect [bpm]
    pool.set("rhythm.beats.maxTempo", 208);                 // [60,250]                         | the slowest tempo to detect [bpm]

    pool.set("rhythm.beats.loudness.compute", false);       // {false,true}                     | compute loudness of beats
    pool.set("rhythm.bpmhistogram.compute", false);         // {false,true}                     | compute a bpm histogram
    pool.set("rhythm.onset.compute", false);                // {false,true}                     | compute onsets

    pool.set("rhythm.danceability.compute", false);         // {false,true}                     | compute a normal value in range from 0 to ~3. The higher, the more danceable
    pool.set("rhythm.danceability.minTau", 310.);           // (0,inf)                          | minimum segment length to consider [ms]
    pool.set("rhythm.danceability.maxTau", 8800.);          // (0,inf)                          | maximum segment length to consider [ms]
    pool.set("rhythm.danceability.tauMultiplier", 1.1);     // [1,inf)                          | multiplier to increment from min to max tau

    // tonal
    pool.set("tonal.compute", false);                       // {false,true}                     | compute some tonal things
    pool.set("tonal.frameSize", 4096);                      // [1,inf)                          | the size of the frame to cut
    pool.set("tonal.hopSize", 2048);                        // [1,inf)                          | the number of samples to jump after a frame is output
    pool.set("tonal.zeroPadding", 0);                       // [0,inf)                          | the size of the zero-padding
    pool.set("tonal.windowType", "blackmanharris62");       // {hamming,hann,triangular,square, | the window type
    //  blackmanharris62/70/74/92}
    pool.set("tonal.silentFrames", "noise");                // {drop,keep,noise}                | whether to [keep/drop/add noise to] silent frames

    // sfx
    pool.set("sfx.compute", false);                         // {false,true}                     | compute some sfx things

    // panning
    pool.set("panning.compute", false);                     // {false,true}                     | compute panning
    pool.set("panning.frameSize", 4096);                    // [1,inf)                          | the size of the frame to cut
    pool.set("panning.hopSize", 2048);                      // [1,inf)                          | the number of samples to jump after a frame is output
    pool.set("panning.averageFrames", 43);                  // [0,inf) 2 seconds * sr/hopsize   | number of frames to take into account for averaging
    pool.set("panning.panningBins", 512);                   // (1,inf)                          | size of panorama histogram (in bins)
    pool.set("panning.numCoeffs", 20);                      // (0,inf)                          | number of coefficients used to define the panning curve at each frame
    pool.set("panning.numBands", 1);                        // [1,inf)                          | number of mel bands
    pool.set("panning.warpedPanorama", true);               // {false,true}                     | if true, warped panorama is applied, having more resolution in the center area
    pool.set("panning.zeroPadding", 8192);                  // [0,inf)                          | the size of the zero-padding
    pool.set("panning.windowType", "hann");                 // {hamming,hann,triangular,square, | the window type
    //  blackmanharris62/70/74/92}
    pool.set("panning.silentFrames", "noise");              // {drop,keep,noise}                | whether to [keep/drop/add noise to] silent frames

    // fades
    pool.set("fades.compute", false);                       // {false,true}                     | computes fade ins and fade outs
    pool.set("fades.frameSize", 11025);                     // [1,inf)                          | the size of the frame to cut
    pool.set("fades.hopSize", 256);                         // [1,inf)                          | the number of samples to jump after a frame is output
    pool.set("fades.frameRate", 172.265625);                // (0,inf) - sampleRate/hopSize     | the rate of frames used in calculation of the RMS [frames/s]
    pool.set("fades.minLength", 3);                         // (0,inf)                          | the minimum length to consider a fade-in/out [s]
    pool.set("fades.cutoffHigh", 0.85);                     // (0,1]                            | fraction of the average RMS to define the maximum threshold
    pool.set("fades.cutoffLow", 0.20);                      // [0,1)                            | fraction of the average RMS to define the minimum threshold
    pool.set("fades.silentFrames", "noise");                // {drop,keep,noise}                | whether to [keep/drop/add noise to] silent frames

    // stats
    // const char *statsArray[] = { "mean", "var", "median", "min", "max", "dmean", "dmean2", "dvar", "dvar2" };
    const char *statsArray[] = {"mean", "var", "median", "min", "max", "dmean", "dmean2", "dvar", "dvar2"};

    // compute cov and icov matrix can lead to an exception if matrix is singular...
    // const char *mfccStatsArray[] = { "mean", "cov", "icov"};
    const char *mfccStatsArray[] = {"mean"};
    vector<string> stats = arrayToVector<string>(statsArray);
    vector<string> mfccStats = arrayToVector<string>(mfccStatsArray);
    for (int i = 0; i < (int)stats.size(); i++)
    {
        pool.add("lowlevel.stats", stats[i]);
        pool.add("tonal.stats", stats[i]);
        pool.add("rhythm.stats", stats[i]);
        pool.add("sfx.stats", stats[i]);
    }
    for (int i = 0; i < (int)mfccStats.size(); i++)
        pool.add("lowlevel.mfccStats", mfccStats[i]);
    pool.add("panning.stats", "copy");
    pool.add("fades.stats", "copy");
}

void mergeOptionsAndResults(Pool &results, const Pool &options)
{
    // merges the configuration results with results pool
    results.set("configuration.general.equalLoudness",        options.value<Real>("equalLoudness"));
    results.set("configuration.general.nequalLoudness",       options.value<Real>("nequalLoudness"));
    results.set("configuration.general.shortSound",           options.value<Real>("shortSound"));
    results.set("configuration.general.startTime",            options.value<Real>("startTime"));
    results.set("configuration.general.endTime",              options.value<Real>("endTime"));
    results.set("configuration.general.analysisSampleRate",   options.value<Real>("analysisSampleRate"));

    // svm
    results.set("configuration.svm.compute",          options.value<Real>("svm.compute"));

    // segmentation
    results.set("configuration.segmentation.compute",               options.value<Real>("segmentation.compute"));
    results.set("configuration.segmentation.size1",                 options.value<Real>("segmentation.size1"));
    results.set("configuration.segmentation.inc1",                  options.value<Real>("segmentation.inc1"));
    results.set("configuration.segmentation.size2",                 options.value<Real>("segmentation.size2"));
    results.set("configuration.segmentation.inc2",                  options.value<Real>("segmentation.inc2"));
    results.set("configuration.segmentation.cpw",                   options.value<Real>("segmentation.cpw"));
    results.set("configuration.segmentation.minimumSegmentsLength", options.value<Real>("segmentation.minimumSegmentsLength"));
    results.set("configuration.segmentation.desc.lowlevel.compute",               options.value<Real>("segmentation.desc.lowlevel.compute"));
    results.set("configuration.segmentation.desc.average_loudness.compute",       options.value<Real>("segmentation.desc.average_loudness.compute"));
    results.set("configuration.segmentation.desc.rhythm.beats.compute",           options.value<Real>("segmentation.desc.rhythm.beats.compute"));
    results.set("configuration.segmentation.desc.rhythm.beats.loudness.compute",  options.value<Real>("segmentation.desc.rhythm.beats.loudness.compute"));
    results.set("configuration.segmentation.desc.rhythm.bpmhistogram.compute",    options.value<Real>("segmentation.desc.rhythm.bpmhistogram.compute"));
    results.set("configuration.segmentation.desc.rhythm.onset.compute",           options.value<Real>("segmentation.desc.rhythm.onset.compute"));
    results.set("configuration.segmentation.desc.rhythm.danceability.compute",    options.value<Real>("segmentation.desc.rhythm.danceability.compute"));
    results.set("configuration.segmentation.desc.tonal.compute",                  options.value<Real>("segmentation.desc.tonal.compute"));
    results.set("configuration.segmentation.desc.sfx.compute",                    options.value<Real>("segmentation.desc.sfx.compute"));
    results.set("configuration.segmentation.desc.panning.compute",                options.value<Real>("segmentation.desc.panning.compute"));
    results.set("configuration.segmentation.desc.fades.compute",                  options.value<Real>("segmentation.desc.fades.compute"));

    // lowlevel
    results.set("configuration.lowlevel.compute",      options.value<Real>("lowlevel.compute"));
    results.set("configuration.lowlevel.frameSize",    options.value<Real>("lowlevel.frameSize"));
    results.set("configuration.lowlevel.hopSize",      options.value<Real>("lowlevel.hopSize"));
    results.set("configuration.lowlevel.zeroPadding",  options.value<Real>("lowlevel.zeroPadding"));
    results.set("configuration.lowlevel.windowType",   options.value<string>("lowlevel.windowType"));
    results.set("configuration.lowlevel.silentFrames", options.value<string>("lowlevel.silentFrames"));

    // average_loudness
    results.set("configuration.average_loudness.compute",      options.value<Real>("average_loudness.compute"));
    results.set("configuration.average_loudness.frameSize",    options.value<Real>("average_loudness.frameSize"));
    results.set("configuration.average_loudness.hopSize",      options.value<Real>("average_loudness.hopSize"));
    results.set("configuration.average_loudness.windowType",   options.value<string>("average_loudness.windowType"));
    results.set("configuration.average_loudness.silentFrames", options.value<string>("average_loudness.silentFrames"));

    // rhythm
    results.set("configuration.rhythm.beats.compute",   options.value<Real>("rhythm.beats.compute"));
    results.set("configuration.rhythm.beats.method",    options.value<string>("rhythm.beats.method"));
    results.set("configuration.rhythm.beats.minTempo",  options.value<Real>("rhythm.beats.minTempo"));
    results.set("configuration.rhythm.beats.maxTempo",  options.value<Real>("rhythm.beats.maxTempo"));

    results.set("configuration.rhythm.beats.loudness.compute",   options.value<Real>("rhythm.beats.loudness.compute"));
    results.set("configuration.rhythm.bpmhistogram.compute",    options.value<Real>("rhythm.bpmhistogram.compute"));
    results.set("configuration.rhythm.onset.compute",           options.value<Real>("rhythm.onset.compute"));

    results.set("configuration.rhythm.danceability.compute",        options.value<Real>("rhythm.danceability.compute"));
    results.set("configuration.rhythm.danceability.minTau",         options.value<Real>("rhythm.danceability.minTau"));
    results.set("configuration.rhythm.danceability.maxTau",         options.value<Real>("rhythm.danceability.maxTau"));
    results.set("configuration.rhythm.danceability.tauMultiplier",  options.value<Real>("rhythm.danceability.tauMultiplier"));

    // tonal
    results.set("configuration.tonal.compute",      options.value<Real>("tonal.compute"));
    results.set("configuration.tonal.frameSize",    options.value<Real>("tonal.frameSize"));
    results.set("configuration.tonal.hopSize",      options.value<Real>("tonal.hopSize"));
    results.set("configuration.tonal.zeroPadding",  options.value<Real>("tonal.zeroPadding"));
    results.set("configuration.tonal.windowType",   options.value<string>("tonal.windowType"));
    results.set("configuration.tonal.silentFrames", options.value<string>("tonal.silentFrames"));

    // sfx
    results.set("configuration.sfx.compute", options.value<Real>("sfx.compute"));

    // panning
    results.set("configuration.panning.compute",        options.value<Real>("panning.compute"));
    results.set("configuration.panning.frameSize",      options.value<Real>("panning.frameSize"));
    results.set("configuration.panning.hopSize",        options.value<Real>("panning.hopSize"));
    results.set("configuration.panning.averageFrames",  options.value<Real>("panning.averageFrames"));
    results.set("configuration.panning.panningBins",    options.value<Real>("panning.panningBins"));
    results.set("configuration.panning.numCoeffs",      options.value<Real>("panning.numCoeffs"));
    results.set("configuration.panning.numBands",       options.value<Real>("panning.numBands"));
    results.set("configuration.panning.warpedPanorama", options.value<Real>("panning.warpedPanorama"));
    results.set("configuration.panning.zeroPadding",    options.value<Real>("panning.zeroPadding"));
    results.set("configuration.panning.windowType",     options.value<string>("panning.windowType"));
    results.set("configuration.panning.silentFrames",   options.value<string>("panning.silentFrames"));

    // fades
    results.set("configuration.fades.compute",        options.value<Real>("fades.compute"));
    results.set("configuration.fades.frameSize",      options.value<Real>("fades.frameSize"));
    results.set("configuration.fades.hopSize",        options.value<Real>("fades.hopSize"));
    results.set("configuration.fades.minLength",      options.value<Real>("fades.minLength"));
    results.set("configuration.fades.cutoffHigh",     options.value<Real>("fades.cutoffHigh"));
    results.set("configuration.fades.cutoffLow",      options.value<Real>("fades.cutoffLow"));
    results.set("configuration.fades.silentFrames",   options.value<string>("fades.silentFrames"));

    // stats
    vector<string> lowlevelStats = options.value<vector<string> >("lowlevel.stats");
    for (int i = 0; i < (int)lowlevelStats.size(); i++) results.add("configuration.lowlevel.stats", lowlevelStats[i]);

    vector<string> tonalStats = options.value<vector<string> >("tonal.stats");
    for (int i = 0; i < (int)tonalStats.size(); i++) results.add("configuration.tonal.stats", tonalStats[i]);

    vector<string> rhythmStats = options.value<vector<string> >("rhythm.stats");
    for (int i = 0; i < (int)rhythmStats.size(); i++) results.add("configuration.rhythm.stats", rhythmStats[i]);

    vector<string> sfxStats = options.value<vector<string> >("sfx.stats");
    for (int i = 0; i < (int)sfxStats.size(); i++) results.add("configuration.sfx.stats", sfxStats[i]);

    vector<string> mfccStats = options.value<vector<string> >("lowlevel.mfccStats");
    for (int i = 0; i < (int)mfccStats.size(); i++) results.add("configuration.lowlevel.mfccStats", mfccStats[i]);

    vector<string> panningStats = options.value<vector<string> >("panning.stats");
    for (int i = 0; i < (int)panningStats.size(); i++) results.add("configuration.panning.stats", panningStats[i]);

    vector<string> fadeStats = options.value<vector<string> >("fades.stats");
    for (int i = 0; i < (int)fadeStats.size(); i++) results.add("configuration.fades.stats", fadeStats[i]);

}

Pool computeAggregation(Pool &pool, const Pool &options, int nSegments)
{
    cout << "Process step 8: Aggregation" << endl;

    // choose which descriptors stats to output
    const char *defaultStats[] = { "mean", "var", "min", "max", "dmean", "dmean2", "dvar", "dvar2" };

    map<string, vector<string> > exceptions;
    const vector<string> &descNames = pool.descriptorNames();
    for (int i = 0; i < (int)descNames.size(); i++)
    {
        if (descNames[i].find("lowlevel.mfcc") != string::npos)
        {
            exceptions[descNames[i]] = options.value<vector<string> >("lowlevel.mfccStats");
            continue;
        }
        if (descNames[i].find("lowlevel.") != string::npos)
        {
            exceptions[descNames[i]] = options.value<vector<string> >("lowlevel.stats");
            continue;
        }
        if (descNames[i].find("rhythm.") != string::npos)
        {
            exceptions[descNames[i]] = options.value<vector<string> >("rhythm.stats");
            continue;
        }
        if (descNames[i].find("tonal.") != string::npos)
        {
            exceptions[descNames[i]] = options.value<vector<string> >("tonal.stats");
            continue;
        }
        if (descNames[i].find("sfx.") != string::npos)
        {
            exceptions[descNames[i]] = options.value<vector<string> >("sfx.stats");
            continue;
        }
        if (descNames[i].find("panning.") != string::npos)
        {
            exceptions[descNames[i]] = options.value<vector<string> >("panning.stats");
            continue;
        }
        if (descNames[i].find("fades.") != string::npos)
        {
            exceptions[descNames[i]] = options.value<vector<string> >("fades.stats");
            continue;
        }
    }

    // in case there is segmentation:
    if (nSegments > 0)
    {
        vector<string> value(1, "copy");
        exceptions["segmentation.timestamps"] = value;
    }

    shared_ptr<standard::Algorithm> aggregator(standard::AlgorithmFactory::create("PoolAggregator",
            "defaultStats", arrayToVector<string>(defaultStats),
            "exceptions", exceptions));
    Pool poolStats;
    aggregator->input("input").set(pool);
    aggregator->output("output").set(poolStats);

    aggregator->compute();

    return poolStats;
}

void cleanUp(Pool &pool, const Pool &options)
{

    cout << "clean up " << endl;
    // some descriptors depend on lowlevel descriptors but it might be that the
    // config file was set lowlevel.compute: false. In this case, the ouput
    // should not contain lowlevel features. The rest of namespaces should
    // only be computed if they were set explicitly in the config file

    // TODO code below currently not working

//    if (options.value<Real>("lowlevel.compute") == 0)
//    {
//        if (options.value<Real>("average_loudness.compute") != 0)
//        {
//            Real al = options.value<Real>("lowlevel.average_loudness");
//            pool.removeNamespace("lowlevel");
//            pool.set("lowlevel.average_loudness", al);
//        }
//        else pool.removeNamespace("lowlevel");
//    }

//    if (options.value<Real>("segmentation.desc.lowlevel.compute") == 0)
//    {
//        ostringstream ns;
//        vector<Real> segments = pool.value<vector<Real> >("segmentation.timestamps");
//        for (int i = 0; i < int(segments.size() - 1); ++i)
//        {
//            ns << "";
//            if (options.value<Real>("segmentation.desc.average_loudness.compute") != 0)
//            {
//                ns << "segments.segment_" << i << ".desc.lowlevel.average_loudness";
//                Real al = options.value<Real>(ns.str());
//                pool.removeNamespace("lowlevel");
//                pool.set(ns.str(), al);
//            }
//            else
//            {
//                ns << "segments.segment_" << i << ".desc.lowlevel";
//                pool.removeNamespace(ns.str());
//            }
//        }
//    }

    mergeOptionsAndResults(pool, options);

}

void outputToFile(Pool &pool, const string &outputFilename, const Pool &options)
{
    if (!outputFilename.empty())
    {
        cout << "Writing results to file " << outputFilename << endl;
        // some descriptors depend on lowlevel descriptors but it might be that the
        // config file was set lowlevel.compute: false. In this case, the ouput
        // file should not contain lowlevel features. The rest of namespaces should
        // only be computed if they were set explicitly in the config file
        if (options.value<Real>("lowlevel.compute") == 0) pool.removeNamespace("lowlevel");

        // TODO: merge results pool with options pool so configuration is also
        // available in the output file
        mergeOptionsAndResults(pool, options);

        ostringstream ns(options.value<string>("outputFormat"));
        string format = ns.str();

        shared_ptr<standard::Algorithm> output(new essentiawrapper::YamlOutput());
        output->declareParameters();
        output->configure("filename", outputFilename,
                          "doubleCheck", true,
                          "format", format);
        output->input("pool").set(pool);
        output->compute();
    }
}
