#ifndef ESSENTIA_WRAPPER_H_
#define ESSENTIA_WRAPPER_H_

#include "essentia-wrapper_exports.h"


#ifdef __cplusplus
extern "C" {
#endif

struct audio_buffer
{
    char* buffer;
    int buffer_size;
    int sample_count;
};

typedef void (*free_audio_buffer_fct)(audio_buffer** buffer);

struct audio_file_handle;
typedef audio_buffer* (*read_audio_fct)(audio_file_handle* file);
typedef audio_file_handle* (*reset_audio_file_fct)(audio_file_handle* file);
typedef void (*progress_fct)(float progress);

struct callbacks
{
    audio_file_handle* audio_file;
    free_audio_buffer_fct free_audio_buffer;
    read_audio_fct read_audio;
    reset_audio_file_fct reset_audio_file;
    progress_fct progress;
};

struct essentia_timestamp
{
    float ts;
};

ESSENTIA_WRAPPER_API essentia_timestamp* analyze(callbacks* cb);
ESSENTIA_WRAPPER_API void free_essentia_timestamps(essentia_timestamp** ts);

#ifdef __cplusplus
}
#endif


#endif // ESSENTIA_WRAPPER_H_
