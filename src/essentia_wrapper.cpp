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

void free_essentia_timestamps(essentia_timestamp *ts)
{
    if (!ts)
    {
        return;
    }

    delete[] ts;
}

void convertAndAdd(std::vector<essentia_timestamp> &et_vec, std::vector<float> resVec, essentia_ts_type type, std::string algoName)
{
    if (resVec.empty())
    {
        return;
    }

    const size_t size = resVec.size();

    essentia_timestamp et;
    et.tsCount = size;
    et.type = type;
    et.algo_name = algoName.c_str();
    et.ts = new float[size];
    for (size_t i = 0; i < size; i++)
    {
        et.ts[i] = resVec[i];
    }

    et_vec.push_back(et);
}

essentia_timestamp *essentia_analyze(callbacks *cb, uint32_t *count)
{
    if (count == nullptr)
    {
        return nullptr;
    }

    essentiawrapper::AllDetectionAlgorithms algo;

    algo.analyze(cb, configPool());

    std::vector<essentia_timestamp> et_vec;

    convertAndAdd(et_vec, algo.get("rhythm.beats.position", true), Beats, "Essentia - AllDetect");
    convertAndAdd(et_vec, algo.get("rhythm.bpm", true), BPM, "Essentia - AllDetect");
    convertAndAdd(et_vec, algo.get("segmentation.timestamps", true), Segments, "Essentia - AllDetect");
    convertAndAdd(et_vec, algo.get("fades.fadeIns", true), FadeIns, "Essentia - AllDetect");
    convertAndAdd(et_vec, algo.get("fades.fadeOuts", true), FadeOuts, "Essentia - AllDetect");
    convertAndAdd(et_vec, algo.get("rhythm.onset_times", true), Onsets,  "Essentia - AllDetect");
    convertAndAdd(et_vec, algo.get("average_loudness", true), AverageLoudness, "Essentia - AllDetect");
    convertAndAdd(et_vec, algo.get("rhythm.danceability", true), Danceability, "Essentia - AllDetect");

    const size_t size = et_vec.size();

    *count = static_cast<uint32_t>(size);

    essentia_timestamp *timestamps = new essentia_timestamp[size];
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
