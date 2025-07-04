/**
 * @file Debug.cpp
 * @copyright Released under the terms of the BSD 3-Clause License
 * @date 2023
 */
#include <OsqpEigen/Debug.hpp>

namespace OsqpEigen
{

// Taken from https://stackoverflow.com/a/8243866/2702753
class NullStream : public std::ostream
{
public:
    NullStream()
        : std::ostream(nullptr)
    {
    }
    NullStream(const NullStream&)
        : std::ostream(nullptr)
    {
    }
};

template <class T> const NullStream& operator<<(NullStream&& os, const T&)
{
    return os;
}

NullStream theStream;

std::ostream& debugStream()
{
#ifdef OSQP_EIGEN_DEBUG_OUTPUT
    return std::cerr;
#else
    return theStream;
#endif
}
} // namespace OsqpEigen
