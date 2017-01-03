#include "essentia_wrapper.h"
#include <algorithm>
#include <cstring>
#include <cstddef>
#include "essentia/AllDetectionAlgorithms.h"
#include "pool.h"

namespace {

essentia::Pool &configPool()
{
    static essentia::Pool pool;
    return pool;
}

template<class T>
bool setPoolValue(const char *name, T value)
{
    auto &pool =  configPool();
    try
    {
        pool.set(name, value);
        return true;
    }
    catch (essentia::EssentiaException &)
    {
    }

    return false;
}

template<class T>
bool addPoolValue(const char *name, T value)
{
    auto &pool =  configPool();
    try
    {
        pool.add(name, value);
        return true;
    }
    catch (essentia::EssentiaException &)
    {
    }

    return false;
}

}

void free_essentia_timestamps(essentia_timestamps *ts)
{
    if (!ts)
    {
        return;
    }

    if(ts->ts) {
        delete[] ts->ts;
    }

    delete[] ts;
}

void convertAndAdd(std::vector<essentia_timestamps> &et_vec, const std::vector<float> &resVec, essentia_ts_type type)
{
    if (resVec.empty())
    {
        return;
    }

    const size_t size = resVec.size();

    essentia_timestamps et;
    et.tsCount = size;
    et.type = type;
    et.ts = new float[size];
    for (size_t i = 0; i < size; ++i)
    {
        et.ts[i] = resVec[i];
    }

    et_vec.push_back(et);
}

essentia_timestamps *essentia_analyze(callbacks *cb, uint32_t *count)
{
    if (count == nullptr)
    {
        return nullptr;
    }

    essentiawrapper::AllDetectionAlgorithms algo;

    essentia::Pool localConfigPool = configPool();

    bool neqloud = localConfigPool.contains<essentia::Real>("nequalLoudness") && localConfigPool.value<essentia::Real>("nequalLoudness");

    algo.analyze(cb, localConfigPool);

    std::vector<essentia_timestamps> et_vec;

    convertAndAdd(et_vec, algo.get("rhythm.beats.position", !neqloud), Beats);
    convertAndAdd(et_vec, algo.get("rhythm.bpm", !neqloud), BPM);
    convertAndAdd(et_vec, algo.get("segmentation.timestamps", !neqloud), Segments);
    convertAndAdd(et_vec, algo.get("fades.fadeIns", !neqloud), FadeIns);
    convertAndAdd(et_vec, algo.get("fades.fadeOuts", !neqloud), FadeOuts);
    convertAndAdd(et_vec, algo.get("rhythm.onset_times", !neqloud), Onsets);
    convertAndAdd(et_vec, algo.get("average_loudness", !neqloud), AverageLoudness);
    convertAndAdd(et_vec, algo.get("rhythm.danceability", !neqloud), Danceability);

    const size_t size = et_vec.size();

    *count = static_cast<uint32_t>(size);

    essentia_timestamps *timestamps = new essentia_timestamps[size];
    std::copy(std::begin(et_vec), std::end(et_vec), timestamps);

    return timestamps;
}

bool essentia_set_config_value_f(const char *name, float value)
{
    if (!name)
    {
        return false;
    }

    return setPoolValue(name, value);
}

bool essentia_set_config_value_s(const char *name, const char *value)
{
    if (!name || !value)
    {
        return false;
    }

    return setPoolValue(name, value);
}

bool essentia_set_config_value_b(const char *name, bool value)
{
    if (!name)
    {
        return false;
    }

    return setPoolValue(name, value);
}

bool essentia_add_config_value_f(const char *name, float value)
{
    if (!name)
    {
        return false;
    }

    return addPoolValue(name, value);
}

bool essentia_add_config_value_s(const char *name, const char *value)
{
    if (!name || !value)
    {
        return false;
    }

    return addPoolValue(name, value);
}

bool essentia_add_config_value_b(const char *name, bool value)
{
    if (!name)
    {
        return false;
    }

    return addPoolValue(name, value);
}
