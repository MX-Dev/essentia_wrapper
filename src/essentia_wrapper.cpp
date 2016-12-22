#include "essentia_wrapper.h"
#include <algorithm>
#include <cstring>
#include <cstddef>
#include "essentia/BeatDetectionAlgorithm.h"
#include "essentia/AllDetectionAlgorithms.h"
#include "pool.h"

namespace {

essentia::Pool& configPool()
{
    static essentia::Pool pool;
    return pool;
}

template<class T>
bool setPoolValue(const char* name, T value)
{
    auto& pool =  configPool();
    try
    {
        pool.set(name, value);
        return true;
    }
    catch(essentia::EssentiaException&)
    {
    }

    return false;
}

template<class T>
bool addPoolValue(const char* name, T value)
{
    auto& pool =  configPool();
    try
    {
        pool.add(name, value);
        return true;
    }
    catch(essentia::EssentiaException&)
    {
    }

    return false;
}

}

void free_essentia_timestamps(essentia_timestamp *ts)
{
    if(!ts)
    {
        return;
    }

    delete[] ts;
}

essentia_timestamp *essentia_analyze(callbacks *cb, uint32_t *count)
{
    if(count == nullptr)
    {
        return nullptr;
    }

    // ToDo: filter out if we only want to make a beat detection.
    //  Then it would be better to use the BeatDetectionAlgorithm.

    essentiawrapper::AllDetectionAlgorithms algo;
    std::vector<essentia_timestamp> vec = algo.analyze(cb, configPool());
    const size_t size = vec.size();
    *count = static_cast<uint32_t>(size);

    essentia_timestamp* timestamps = new essentia_timestamp[size];
    std::copy(std::begin(vec), std::end(vec), timestamps);

    return timestamps;
}

bool essentia_set_config_value_f(const char *name, float value)
{
    if(!name)
    {
        return false;
    }

    return setPoolValue(name, value);
}

bool essentia_set_config_value_s(const char *name, const char *value)
{
    if(!name || !value)
    {
        return false;
    }

    return setPoolValue(name, value);
}

bool essentia_set_config_value_b(const char *name, bool value)
{
    if(!name)
    {
        return false;
    }

    return setPoolValue(name, value);
}

bool essentia_add_config_value_f(const char *name, float value)
{
    if(!name)
    {
        return false;
    }

    return addPoolValue(name, value);
}

bool essentia_add_config_value_s(const char *name, const char *value)
{
    if(!name || !value)
    {
        return false;
    }

    return addPoolValue(name, value);
}

bool essentia_add_config_value_b(const char *name, bool value)
{
    if(!name)
    {
        return false;
    }

    return addPoolValue(name, value);
}
