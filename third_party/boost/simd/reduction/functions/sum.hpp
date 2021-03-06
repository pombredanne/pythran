//==============================================================================
//         Copyright 2003 - 2012   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2012   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef BOOST_SIMD_REDUCTION_FUNCTIONS_SUM_HPP_INCLUDED
#define BOOST_SIMD_REDUCTION_FUNCTIONS_SUM_HPP_INCLUDED
#include <boost/simd/include/functor.hpp>
#include <boost/dispatch/include/functor.hpp>
#include <boost/simd/operator/functions/plus.hpp>
#include <boost/simd/constant/constants/zero.hpp>

namespace boost { namespace simd { namespace tag
  {
   /*!
     @brief sum generic tag

     Represents the sum function in generic contexts.

     @par Models:
        Hierarchy
   **/
    struct sum_ : ext::reduction_<sum_, tag::plus_, tag::Zero>
    {
      /// @brief Parent hierarchy
      typedef ext::reduction_<sum_, tag::plus_, tag::Zero> parent;
    };
  }
  /*!
    Returns the sum of the elements of the SIMD vector

    @par Semantic:

    For every parameter of type T0

    @code
    scalar<T0> r = sum(a0);
    @endcode

    is similar to:

    @code
    scalar<T0> r = Zero;
    for(result_type i = 0; i != meta::cardinal_of<T0>; ++i)
      r += a0[i];
    @endcode

    @param a0

    @return a value of the scalar type associated to the parameter
  **/
  BOOST_DISPATCH_FUNCTION_IMPLEMENTATION(tag::sum_, sum, 1)
  BOOST_DISPATCH_FUNCTION_IMPLEMENTATION(tag::sum_, sum, 2)

} }
#endif
