#ifndef FADE_DETECTION_ALGORITHM_H
#define FADE_DETECTION_ALGORITHM_H

#include "EssentiaAlgorithm.h"

namespace essentiawrapper {

class FadeDetectionAlgorithm : public IEssentiaAlgorithm
{
public:
    FadeDetectionAlgorithm();
    virtual ~FadeDetectionAlgorithm();

    // IEssentiaAlgorithm interface
    virtual std::vector<essentia_timestamp> analyze(callbacks *cb) override;
};

} // namespace essentiawrapper

#endif // FADE_DETECTION_ALGORITHM_H
