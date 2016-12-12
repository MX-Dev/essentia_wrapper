#include "essentia_wrapper.h"

void free_essentia_timestamps(essentia_timestamp **ts)
{
    if(!ts || !(*ts))
    {
        return;
    }

    while(*ts)
    {
        delete *ts;
        ts++;
    }

    delete ts;
}

essentia_timestamp *analyze(callbacks *cb)
{
    // call the timestamp detection here
}
