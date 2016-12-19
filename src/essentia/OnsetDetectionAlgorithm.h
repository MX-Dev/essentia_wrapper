#ifndef ONSET_DETECTION_ALGORITHM_H
#define ONSET_DETECTION_ALGORITHM_H

#include "IEssentiaAlgorithm.h"

namespace essentiawrapper {

class OnsetDetectionAlgorithm : public IEssentiaAlgorithm
{
public:
    OnsetDetectionAlgorithm();
    virtual ~OnsetDetectionAlgorithm();

    // IEssentiaAlgorithm interface
public:
    /**
     * @brief Analyzes the audio delivered by the reader.
     * @param cb
     * @return
     */
    virtual std::vector<essentia_timestamp> analyze(callbacks *cb) override;
};

} // namespace essentiawrapper

#endif // ONSET_DETECTION_ALGORITHM_H
