#ifndef ESSENTIA_WRAPPER_H_
#define ESSENTIA_WRAPPER_H_

#include "essentia-wrapper_exports.h"


#ifdef __cplusplus
extern "C" {
#endif

struct audio_buffer
{
    const char* buffer;
    int buffer_size;
    int sample_count;
};

typedef void (*free_audio_buffer_fct)(audio_buffer* buffer);

typedef const void* audio_file_handle;

enum essentia_reader_sample_fmt
{
    Short,
    Float
};

typedef bool (*open_audio_fct)(audio_file_handle file, int sample_rate, int channels, essentia_reader_sample_fmt fmt);
typedef audio_buffer* (*read_audio_fct)(audio_file_handle file);
typedef void (*close_audio_file_fct)(audio_file_handle file);
typedef void (*progress_fct)(float progress);

struct callbacks
{
    audio_file_handle audio_file;
    open_audio_fct open_audio;
    read_audio_fct read_audio;
    close_audio_file_fct close_audio;
    free_audio_buffer_fct free_audio_buffer;
    progress_fct progress;
};

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

ESSENTIA_WRAPPER_API essentia_timestamp* essentia_analyze(callbacks* cb, int *count);
ESSENTIA_WRAPPER_API void free_essentia_timestamps(essentia_timestamp* ts);

#ifdef __cplusplus
}
#endif


#endif // ESSENTIA_WRAPPER_H_
