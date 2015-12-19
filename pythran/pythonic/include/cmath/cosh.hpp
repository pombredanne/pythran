#ifndef PYTHONIC_INCLUDE_CMATH_COSH_HPP
#define PYTHONIC_INCLUDE_CMATH_COSH_HPP

#include "pythonic/include/utils/functor.hpp"
#include "pythonic/include/types/complex.hpp"

#include <cmath>

namespace pythonic
{

  namespace cmath
  {
    DECLARE_FUNCTOR_2(cosh, std::cosh);
  }
}

#endif
