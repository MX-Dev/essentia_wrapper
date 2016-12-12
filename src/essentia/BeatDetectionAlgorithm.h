#ifndef BEAT_DETECTION_ALGORITHM_H
#define BEAT_DETECTION_ALGORITHM_H

#include "EssentiaAlgorithm.h"

namespace essentiawrapper {

class BeatDetectionAlgorithm : public IEssentiaAlgorithm
{
public:
    BeatDetectionAlgorithm();
    virtual ~BeatDetectionAlgorithm();

    // IEssentiaAlgorithm interface
    virtual std::vector<essentia_timestamp> analyze(callbacks *cb) override;
};

} // namespace essentiawrapper

#endif // BEAT_DETECTION_ALGORITHM_H
