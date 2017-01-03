#ifndef ESSENTIA_WRAPPER_H_
#define ESSENTIA_WRAPPER_H_

#include <stdint.h>
#include "essentia-wrapper_exports.h"

/*
    copy from config_util.cpp

    // overview -> http://essentia.upf.edu/documentation/algorithms_overview.html

    // general
    pool.set("equalLoudness", true);                        // {false,true}                     | use equal loudness
    pool.set("nequalLoudness", false);                      // {false,true}                     | use non equal loudness, if its true results were taken from this pool

    pool.set("startTime", 0);                               // [0,end)                          | analyse from (seconds)
    pool.set("endTime", 2000.0);                            // (0,end]                          | analyse to (seconds), automatically set to file length during analyse if config value is higher
    pool.set("analysisSampleRate", 44100.0);                // (0,inf)                          | the sampling rate of the audio signal [Hz], should not be changed, most algorithms need 44100 Hz

    pool.set("equalOutputPath", "");                        // string                           | equal result output to file
    pool.set("nequalOutputPath", "");                       // string                           | nequal result output to file
    pool.set("outputFormat", "json");                       // {yaml,json}                      | result output format

    pool.set("skipReplayGain", false);                      // {false,true}                     | if true use standard values, saves some time, possibly different results

    // segmentation
    // http://essentia.upf.edu/documentation/reference/streaming_SBic.html
    pool.set("segmentation.compute", false);                // {false,true}                     | compute segments
    pool.set("segmentation.size1", 300);                    // [1,inf)                          | first pass window size [frames]
    pool.set("segmentation.inc1", 60);                      // [1,inf)                          | first pass increment [frames]
    pool.set("segmentation.size2", 200);                    // [1,inf)                          | second pass window size [frames]
    pool.set("segmentation.inc2", 20);                      // [1,inf)                          | second pass increment [frames]
    pool.set("segmentation.cpw", 1.5);                      // [0,inf)                          | complexity penalty weight
    pool.set("segmentation.minimumSegmentsLength", 10);     // [1,inf)                          | minimum length of a segment [frames]

    // average_loudness
    // http://essentia.upf.edu/documentation/reference/streaming_Loudness.html
    pool.set("average_loudness.compute", false);            // {false,true}                     | compute the average loudness
    pool.set("average_loudness.frameSize", 88200);          // [1,inf)                          | the size of the frame to cut
    pool.set("average_loudness.hopSize", 44100);            // [1,inf)                          | the number of samples to jump after a frame is output
    pool.set("average_loudness.windowType", "hann");        // {hamming,hann,triangular,square, | the window type
                                                            //  blackmanharris62/70/74/92}
    pool.set("average_loudness.silentFrames", "noise");     // {drop,keep,noise}                | whether to [keep/drop/add noise to] silent frames

    // rhythm
    // http://essentia.upf.edu/documentation/reference/streaming_BeatTrackerDegara.html
    // http://essentia.upf.edu/documentation/reference/streaming_BeatTrackerMultiFeature.html
    pool.set("rhythm.beats.compute", false);                // {false,true}                     | compute beats
    pool.set("rhythm.beats.method", "degara");              // {multifeature,degara}            | the method used for beat tracking
    pool.set("rhythm.beats.minTempo", 40);                  // [40,180]                         | the fastest tempo to detect [bpm]
    pool.set("rhythm.beats.maxTempo", 208);                 // [60,250]                         | the slowest tempo to detect [bpm]

    // http://essentia.upf.edu/documentation/reference/streaming_BeatsLoudness.html
    pool.set("rhythm.beats.loudness.compute", false);       // {false,true}                     | compute loudness of beats
    pool.set("rhythm.bpmhistogram.compute", false);         // {false,true}                     | compute a bpm histogram
    pool.set("rhythm.onset.compute", false);                // {false,true}                     | compute onsets

    // http://essentia.upf.edu/documentation/reference/streaming_Danceability.html
    pool.set("rhythm.danceability.compute", false);         // {false,true}                     | compute a normal value in range from 0 to ~3. The higher, the more danceable
    pool.set("rhythm.danceability.minTau", 310.);           // (0,inf)                          | minimum segment length to consider [ms]
    pool.set("rhythm.danceability.maxTau", 8800.);          // (0,inf)                          | maximum segment length to consider [ms]
    pool.set("rhythm.danceability.tauMultiplier", 1.1);     // [1,inf)                          | multiplier to increment from min to max tau

    // fades
    // http://essentia.upf.edu/documentation/reference/streaming_FadeDetection.html
    pool.set("fades.compute", false);                       // {false,true}                     | computes fade ins and fade outs
    pool.set("fades.frameSize", 11025);                     // [1,inf)                          | the size of the frame to cut
    pool.set("fades.hopSize", 256);                         // [1,inf)                          | the number of samples to jump after a frame is output
    pool.set("fades.frameRate", 172.265625);                // (0,inf) - sampleRate/hopSize     | the rate of frames used in calculation of the RMS [frames/s]
    pool.set("fades.minLength", 3);                         // (0,inf)                          | the minimum length to consider a fade-in/out [s]
    pool.set("fades.cutoffHigh", 0.85);                     // (0,1]                            | fraction of the average RMS to define the maximum threshold
    pool.set("fades.cutoffLow", 0.20);                      // [0,1)                            | fraction of the average RMS to define the minimum threshold
    pool.set("fades.silentFrames", "noise");                // {drop,keep,noise}                | whether to [keep/drop/add noise to] silent frames

    // below values currently only accessable via yaml/json file output

    // lowlevel
    pool.set("lowlevel.compute", false);                    // {false,true}                     | compute some low level things
    pool.set("lowlevel.frameSize", 2048);                   // [1,inf)                          | the size of the frame to cut
    pool.set("lowlevel.hopSize", 1024);                     // [1,inf)                          | the number of samples to jump after a frame is output
    pool.set("lowlevel.zeroPadding", 0);                    // [0,inf)                          | the size of the zero-padding
    pool.set("lowlevel.windowType", "blackmanharris62");    // {hamming,hann,triangular,square, | the window type
                                                            //  blackmanharris62/70/74/92}
    pool.set("lowlevel.silentFrames", "noise");             // {drop,keep,noise}                | whether to [keep/drop/add noise to] silent frames

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

    // svm
    pool.set("svm.compute", false);                         // not supported yet (gaia)

    // segment descriptors
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

*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The audio_buffer struct represents one audio buffer.
 */
struct audio_buffer
{
    const uint8_t* buffer; //!< A pointer to the buffer with audio samples.
    uint32_t sample_count; //!< The count of audio samples in buffer
};

/**
 * @brief audio_file_handle The handle to the used audio file.
 *
 * It is set in by the client.
 */
typedef const void* audio_file_handle;

/**
 * @brief The essentia_reader_sample_fmt enum
 */
enum essentia_reader_sample_fmt
{
    Short, //!< The format of the audio samples is 16 bit signed (short)
    Float  //!< The format of the audio samples is float
};

/**
 * @brief open_audio_fct Callback to open an audio file identified by file handle.
 * @param file The file handle for the audio file.
 * @param sample_rate The sample rate needed by essentia.
 * @param channels The channel count needed by essentia.
 * @param fmt The sample format needed by essentia.
 * @return
 */
typedef bool (*open_audio_fct)(audio_file_handle file, uint32_t sample_rate, uint32_t channels, essentia_reader_sample_fmt fmt);

/**
 * @brief read_audio_fct Callback function to read audio samples from audio file.
 * @param file The file handle for the audio file.
 * @return
 */
typedef audio_buffer* (*read_audio_fct)(audio_file_handle file);

/**
 * @brief get_file_length_ns_fct Callback function to get file length from audio file.
 * @param file The file handle for the audio file.
 * @return
 */
typedef uint64_t (*get_file_length_ns_fct)(audio_file_handle file);

/**
 * @brief close_audio_file_fct Callback to close the audio file opened by open_audio_fct.
 * @param file The file handle for the audio file.
 */
typedef void (*close_audio_file_fct)(audio_file_handle file);

/**
 * @brief free_audio_buffer_fct Callback for freeing one audio buffer read by read_audio_fct.
 * @param buffer
 */
typedef void (*free_audio_buffer_fct)(audio_buffer* buffer);

/**
 * @brief progress_fct Callback for showing the progress of the essentia analysis.
 * @param progress
 */
typedef void (*progress_fct)(float progress);

/**
 * @brief The callbacks struct is used to handle all client callbacks.
 */
struct callbacks
{
    audio_file_handle audio_file;
    open_audio_fct open_audio;
    read_audio_fct read_audio;
    get_file_length_ns_fct get_file_length;
    close_audio_file_fct close_audio;
    free_audio_buffer_fct free_audio_buffer;
    progress_fct progress;
};

/**
 * @brief The essentia_ts_type enum
 *
 */
enum essentia_ts_type
{
    Beats,
    BPM,
    Segments,
    FadeIns,
    FadeOuts,
    Onsets,
    AverageLoudness,
    Danceability
};

struct essentia_timestamps
{
    float* ts;
    uint32_t tsCount;
    essentia_ts_type type;
};

/**
 * Adds @e value to the configuration under @e name
 * @param name a descriptor name that identifies the collection of data to add
 *             @e value to
 * @param value the value to add to the collection of data that @e name points
 *              to
 * @param validityCheck indicates whether @e value should be checked for NaN or Inf values. If
 *                      true, an exception is thrown if @e value is (or contains) a NaN or Inf.
 * @remark If @e name already exists in the configuration and points to data with the
 *         same data type as @e value, then @e value is concatenated to the
 *         vector stored therein. If, however, @e name already exists in the
 *         configuration and points to a @b different data type than @e value, then
 *         this can cause unwanted behavior for the rest of the member
 *         functions of configuration. To avoid this, do not add data into the configuration
 *         whose descriptor name already exists in the configuration and points to a
 *         @b different data type than @e value.
 *
 * @remark If @e name has child descriptor names, this function will return false.
 *         For example, if "foo.bar" exists in the configuration, this
 *         function can no longer be called with "foo" as its @e name
 *         parameter, because "bar" is a child descriptor name of "foo".
 */
ESSENTIA_WRAPPER_API bool essentia_add_config_value_f(const char* name, float value);

/** @copydoc essentia_add_config_value_f(const char* name, float value) */
ESSENTIA_WRAPPER_API bool essentia_add_config_value_s(const char* name, const char* value);

/** @copydoc essentia_add_config_value_f(const char* name, float value) */
ESSENTIA_WRAPPER_API bool essentia_add_config_value_b(const char* name, bool value);

/**
 * @brief Sets the value of a descriptor name.
 *
 * @details This function is different than the add functions because set does not
 * append data to the existing data under a given descriptor name, it sets it.
 * Thus there can only be one datum associated with a descriptor name
 * introduced to the configuration via the set function. This function is useful when
 * there is only one value associated with a given descriptor name.
 *
 * @param name is the descriptor name of the datum to set
 * @param value is the datum to associate with @e name
 ' @return True if the value was set correctly, otherwise false.
 *
 * @remark The set function cannot be used to override the data of a
 *         descriptor name that was introduced to the Configuration via the add
 *         function. False will be returned if the given
 *         descriptor name already exists in the Configuration and was put there via a
 *         call to an add function.
 */
ESSENTIA_WRAPPER_API bool essentia_set_config_value_f(const char* name, float value);

/** @copydoc essentia_set_config_value_f(const char* name, float value) */
ESSENTIA_WRAPPER_API bool essentia_set_config_value_s(const char* name, const char* value);

/** @copydoc essentia_set_config_value_f(const char* name, float value) */
ESSENTIA_WRAPPER_API bool essentia_set_config_value_b(const char* name, bool value);

/**
 * @brief essentia_analyze
 * @param cb The filled callback struct
 * @param count The count of the returned timestamps.
 * @return An array of timestamps.
 */
ESSENTIA_WRAPPER_API essentia_timestamps* essentia_analyze(callbacks* cb, uint32_t *count);

/**
 * @brief free_essentia_timestamps Frees the timestamp array returned by essentia_analyze.
 * @param ts The timestamp array.
 */
ESSENTIA_WRAPPER_API void free_essentia_timestamps(essentia_timestamps* ts);

#ifdef __cplusplus
}
#endif


#endif // ESSENTIA_WRAPPER_H_
