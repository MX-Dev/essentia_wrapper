#ifndef ESSENTIA_WRAPPER_H_
#define ESSENTIA_WRAPPER_H_

#include <stdint.h>
#include "essentia-wrapper_exports.h"


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
    Beat,
    FadeIn,
    FadeOut,
    OnSet
};

struct essentia_timestamp
{
    float ts;
    essentia_ts_type type;
    const char* algo_name;
};

/**
 * @brief essentia_analyze
 * @param cb The filled callback struct
 * @param count The count of the returned timestamps.
 * @return An array of timestamps.
 */
ESSENTIA_WRAPPER_API essentia_timestamp* essentia_analyze(callbacks* cb, uint32_t *count);

/**
 * @brief free_essentia_timestamps Frees the timestamp array returned by essentia_analyze.
 * @param ts The timestamp array.
 */
ESSENTIA_WRAPPER_API void free_essentia_timestamps(essentia_timestamp* ts);

#ifdef __cplusplus
}
#endif


#endif // ESSENTIA_WRAPPER_H_
