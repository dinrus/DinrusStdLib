// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___FUNCTIONAL_RANGES_OPERATIONS_H
#define _LIBCPP___FUNCTIONAL_RANGES_OPERATIONS_H

#include <__concepts/equality_comparable.h>
#include <__concepts/totally_ordered.h>
#include <__config>
#include <__utility/forward.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17

namespace ranges {

struct equal_to {
  template <class _Tp, class _Up>
  requires equality_comparable_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(_VSTD::forward<_Tp>(__t) == _VSTD::forward<_Up>(__u)))) {
    return _VSTD::forward<_Tp>(__t) == _VSTD::forward<_Up>(__u);
  }

  using is_transparent = void;
};

struct not_equal_to {
  template <class _Tp, class _Up>
  requires equality_comparable_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(!(_VSTD::forward<_Tp>(__t) == _VSTD::forward<_Up>(__u))))) {
    return !(_VSTD::forward<_Tp>(__t) == _VSTD::forward<_Up>(__u));
  }

  using is_transparent = void;
};

struct less {
  template <class _Tp, class _Up>
  requires totally_ordered_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(_VSTD::forward<_Tp>(__t) < _VSTD::forward<_Up>(__u)))) {
    return _VSTD::forward<_Tp>(__t) < _VSTD::forward<_Up>(__u);
  }

  using is_transparent = void;
};

struct less_equal {
  template <class _Tp, class _Up>
  requires totally_ordered_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(!(_VSTD::forward<_Up>(__u) < _VSTD::forward<_Tp>(__t))))) {
    return !(_VSTD::forward<_Up>(__u) < _VSTD::forward<_Tp>(__t));
  }

  using is_transparent = void;
};

struct greater {
  template <class _Tp, class _Up>
  requires totally_ordered_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(_VSTD::forward<_Up>(__u) < _VSTD::forward<_Tp>(__t)))) {
    return _VSTD::forward<_Up>(__u) < _VSTD::forward<_Tp>(__t);
  }

  using is_transparent = void;
};

struct greater_equal {
  template <class _Tp, class _Up>
  requires totally_ordered_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(!(_VSTD::forward<_Tp>(__t) < _VSTD::forward<_Up>(__u))))) {
    return !(_VSTD::forward<_Tp>(__t) < _VSTD::forward<_Up>(__u));
  }

  using is_transparent = void;
};

} // namespace ranges

#endif // _LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___FUNCTIONAL_RANGES_OPERATIONS_H
