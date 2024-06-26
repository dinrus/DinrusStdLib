//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___BIT_BIT_CEIL_H
#define _LIBCPP___BIT_BIT_CEIL_H

#include <__assert>
#include <__bit/countl.h>
#include <__concepts/arithmetic.h>
#include <__config>
#include <limits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER >= 20

template <__libcpp_unsigned_integer _Tp>
_LIBCPP_HIDE_FROM_ABI constexpr _Tp bit_ceil(_Tp __t) noexcept {
  if (__t < 2)
    return 1;
  const unsigned __n = numeric_limits<_Tp>::digits - std::countl_zero((_Tp)(__t - 1u));
  _LIBCPP_ASSERT(__n != numeric_limits<_Tp>::digits, "Bad input to bit_ceil");

  if constexpr (sizeof(_Tp) >= sizeof(unsigned))
    return _Tp{1} << __n;
  else {
    const unsigned __extra = numeric_limits<unsigned>::digits - numeric_limits<_Tp>::digits;
    const unsigned __retVal = 1u << (__n + __extra);
    return (_Tp)(__retVal >> __extra);
  }
}

#endif // _LIBCPP_STD_VER >= 20

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___BIT_BIT_CEIL_H
