#include "essentia_wrapper.h"
#include <algorithm>
#include <cstring>
#include <cstddef>
#include "essentia/BeatDetectionAlgorithm.h"

void free_essentia_timestamps(essentia_timestamp *ts)
{
    if(!ts)
    {
        return;
    }

    delete[] ts;
}

essentia_timestamp *essentia_analyze(callbacks *cb, int *count)
{
    if(count == nullptr)
    {
        return nullptr;
    }

    essentiawrapper::BeatDetectionAlgorithm algo;
    std::vector<essentia_timestamp> vec = algo.analyze(cb);
    const size_t size = vec.size();
    *count = static_cast<int>(size);

    essentia_timestamp* timestamps = new essentia_timestamp[size];
    std::copy(std::begin(vec), std::end(vec), timestamps);

    return timestamps;
}
