#ifndef IESSENTIA_ALGORITHM
#define IESSENTIA_ALGORITHM

#include <memory>
#include <vector>
#include "essentia_wrapper.h"
#include "pool.h"

namespace essentiawrapper {

class IEssentiaAlgorithm
{
public:
    virtual ~IEssentiaAlgorithm() = default;
    virtual std::vector<essentia_timestamp> analyze(callbacks* cb, const essentia::Pool& config) = 0;
};

typedef std::shared_ptr<IEssentiaAlgorithm> IEssentiaAlgorithmPtr_t;

}

#endif // IESSENTIA_ALGORITHM
