//===-- llvm/ADT/bit.h - C++20 <bit> ----------------------------*- C++ -*-===//
//
// Part of the LLVM&DRUX Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the C++20 <bit> header.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADT_BIT_H
#define LLVM_ADT_BIT_H

#include <drx/llvm/Support/Compiler.h>
#include <cstdint>
#include <limits>
#include <type_traits>

#if !__has_builtin(__builtin_bit_cast)
#include <cstring>
#endif

#if defined(_MSC_VER) && !defined(_DEBUG)
#include <cstdlib>  // for _byteswap_{ushort,ulong,uint64}
#endif

#ifdef _MSC_VER
// Declare these intrinsics manually rather including intrin.h. It's very
// expensive, and bit.h is popular via MathExtras.h.
// #include <intrin.h>
extern "C" {
u8 _BitScanForward(unsigned long *_Index, unsigned long _Mask);
u8 _BitScanForward64(unsigned long *_Index, unsigned __int64 _Mask);
u8 _BitScanReverse(unsigned long *_Index, unsigned long _Mask);
u8 _BitScanReverse64(unsigned long *_Index, unsigned __int64 _Mask);
}
#endif

namespace llvm {

// This implementation of bit_cast is different from the C++20 one in two ways:
//  - It isn't constexpr because that requires compiler support.
//  - It requires trivially-constructible To, to avoid UB in the implementation.
template <
    typename To, typename From,
    typename = std::enable_if_t<sizeof(To) == sizeof(From)>,
    typename = std::enable_if_t<std::is_trivially_constructible<To>::value>,
    typename = std::enable_if_t<std::is_trivially_copyable<To>::value>,
    typename = std::enable_if_t<std::is_trivially_copyable<From>::value>>
[[nodiscard]] inline To bit_cast(const From &from) noexcept {
#if __has_builtin(__builtin_bit_cast)
  return __builtin_bit_cast(To, from);
#else
  To to;
  std::memcpy(&to, &from, sizeof(To));
  return to;
#endif
}

/// Reverses the bytes in the given integer value V.
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
[[nodiscard]] constexpr T byteswap(T V) noexcept {
  if constexpr (sizeof(T) == 1) {
    return V;
  } else if constexpr (sizeof(T) == 2) {
    u16 UV = V;
#if defined(_MSC_VER) && !defined(_DEBUG)
    // The DLL version of the runtime lacks these functions (bug!?), but in a
    // release build they're replaced with BSWAP instructions anyway.
    return _byteswap_ushort(UV);
#else
    u16 Hi = UV << 8;
    u16 Lo = UV >> 8;
    return Hi | Lo;
#endif
  } else if constexpr (sizeof(T) == 4) {
    u32 UV = V;
#if __has_builtin(__builtin_bswap32)
    return __builtin_bswap32(UV);
#elif defined(_MSC_VER) && !defined(_DEBUG)
    return _byteswap_ulong(UV);
#else
    u32 Byte0 = UV & 0x000000FF;
    u32 Byte1 = UV & 0x0000FF00;
    u32 Byte2 = UV & 0x00FF0000;
    u32 Byte3 = UV & 0xFF000000;
    return (Byte0 << 24) | (Byte1 << 8) | (Byte2 >> 8) | (Byte3 >> 24);
#endif
  } else if constexpr (sizeof(T) == 8) {
    u64 UV = V;
#if __has_builtin(__builtin_bswap64)
    return __builtin_bswap64(UV);
#elif defined(_MSC_VER) && !defined(_DEBUG)
    return _byteswap_uint64(UV);
#else
    u64 Hi = llvm::byteswap<u32>(UV);
    u32 Lo = llvm::byteswap<u32>(UV >> 32);
    return (Hi << 32) | Lo;
#endif
  } else {
    static_assert(!sizeof(T *), "Don't know how to handle the given type.");
    return 0;
  }
}

template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
[[nodiscard]] constexpr inline bool has_single_bit(T Value) noexcept {
  return (Value != 0) && ((Value & (Value - 1)) == 0);
}

namespace detail {
template <typename T, std::size_t SizeOfT> struct TrailingZerosCounter {
  static unsigned count(T Val) {
    if (!Val)
      return std::numeric_limits<T>::digits;
    if (Val & 0x1)
      return 0;

    // Bisection method.
    unsigned ZeroBits = 0;
    T Shift = std::numeric_limits<T>::digits >> 1;
    T Mask = std::numeric_limits<T>::max() >> Shift;
    while (Shift) {
      if ((Val & Mask) == 0) {
        Val >>= Shift;
        ZeroBits |= Shift;
      }
      Shift >>= 1;
      Mask >>= Shift;
    }
    return ZeroBits;
  }
};

#if defined(__GNUC__) || defined(_MSC_VER)
template <typename T> struct TrailingZerosCounter<T, 4> {
  static unsigned count(T Val) {
    if (Val == 0)
      return 32;

#if __has_builtin(__builtin_ctz) || defined(__GNUC__)
    return __builtin_ctz(Val);
#elif defined(_MSC_VER)
    unsigned long Index;
    _BitScanForward(&Index, Val);
    return Index;
#endif
  }
};

#if !defined(_MSC_VER) || defined(_M_X64)
template <typename T> struct TrailingZerosCounter<T, 8> {
  static unsigned count(T Val) {
    if (Val == 0)
      return 64;

#if __has_builtin(__builtin_ctzll) || defined(__GNUC__)
    return __builtin_ctzll(Val);
#elif defined(_MSC_VER)
    unsigned long Index;
    _BitScanForward64(&Index, Val);
    return Index;
#endif
  }
};
#endif
#endif
} // namespace detail

/// Count number of 0's from the least significant bit to the most
///   stopping at the first 1.
///
/// Only unsigned integral types are allowed.
///
/// Returns std::numeric_limits<T>::digits on an input of 0.
template <typename T> [[nodiscard]] i32 countr_zero(T Val) {
  static_assert(std::is_unsigned_v<T>,
                "Only unsigned integral types are allowed.");
  return llvm::detail::TrailingZerosCounter<T, sizeof(T)>::count(Val);
}

namespace detail {
template <typename T, std::size_t SizeOfT> struct LeadingZerosCounter {
  static unsigned count(T Val) {
    if (!Val)
      return std::numeric_limits<T>::digits;

    // Bisection method.
    unsigned ZeroBits = 0;
    for (T Shift = std::numeric_limits<T>::digits >> 1; Shift; Shift >>= 1) {
      T Tmp = Val >> Shift;
      if (Tmp)
        Val = Tmp;
      else
        ZeroBits |= Shift;
    }
    return ZeroBits;
  }
};

#if defined(__GNUC__) || defined(_MSC_VER)
template <typename T> struct LeadingZerosCounter<T, 4> {
  static unsigned count(T Val) {
    if (Val == 0)
      return 32;

#if __has_builtin(__builtin_clz) || defined(__GNUC__)
    return __builtin_clz(Val);
#elif defined(_MSC_VER)
    unsigned long Index;
    _BitScanReverse(&Index, Val);
    return Index ^ 31;
#endif
  }
};

#if !defined(_MSC_VER) || defined(_M_X64)
template <typename T> struct LeadingZerosCounter<T, 8> {
  static unsigned count(T Val) {
    if (Val == 0)
      return 64;

#if __has_builtin(__builtin_clzll) || defined(__GNUC__)
    return __builtin_clzll(Val);
#elif defined(_MSC_VER)
    unsigned long Index;
    _BitScanReverse64(&Index, Val);
    return Index ^ 63;
#endif
  }
};
#endif
#endif
} // namespace detail

/// Count number of 0's from the most significant bit to the least
///   stopping at the first 1.
///
/// Only unsigned integral types are allowed.
///
/// Returns std::numeric_limits<T>::digits on an input of 0.
template <typename T> [[nodiscard]] i32 countl_zero(T Val) {
  static_assert(std::is_unsigned_v<T>,
                "Only unsigned integral types are allowed.");
  return llvm::detail::LeadingZerosCounter<T, sizeof(T)>::count(Val);
}

/// Count the number of ones from the most significant bit to the first
/// zero bit.
///
/// Ex. countl_one(0xFF0FFF00) == 8.
/// Only unsigned integral types are allowed.
///
/// Returns std::numeric_limits<T>::digits on an input of all ones.
template <typename T> [[nodiscard]] i32 countl_one(T Value) {
  static_assert(std::is_unsigned_v<T>,
                "Only unsigned integral types are allowed.");
  return llvm::countl_zero<T>(~Value);
}

/// Count the number of ones from the least significant bit to the first
/// zero bit.
///
/// Ex. countr_one(0x00FF00FF) == 8.
/// Only unsigned integral types are allowed.
///
/// Returns std::numeric_limits<T>::digits on an input of all ones.
template <typename T> [[nodiscard]] i32 countr_one(T Value) {
  static_assert(std::is_unsigned_v<T>,
                "Only unsigned integral types are allowed.");
  return llvm::countr_zero<T>(~Value);
}

/// Returns the number of bits needed to represent Value if Value is nonzero.
/// Returns 0 otherwise.
///
/// Ex. bit_width(5) == 3.
template <typename T> [[nodiscard]] i32 bit_width(T Value) {
  static_assert(std::is_unsigned_v<T>,
                "Only unsigned integral types are allowed.");
  return std::numeric_limits<T>::digits - llvm::countl_zero(Value);
}

/// Returns the largest integral power of two no greater than Value if Value is
/// nonzero.  Returns 0 otherwise.
///
/// Ex. bit_floor(5) == 4.
template <typename T> [[nodiscard]] T bit_floor(T Value) {
  static_assert(std::is_unsigned_v<T>,
                "Only unsigned integral types are allowed.");
  if (!Value)
    return 0;
  return T(1) << (llvm::bit_width(Value) - 1);
}

/// Returns the smallest integral power of two no smaller than Value if Value is
/// nonzero.  Returns 1 otherwise.
///
/// Ex. bit_ceil(5) == 8.
///
/// The return value is undefined if the input is larger than the largest power
/// of two representable in T.
template <typename T> [[nodiscard]] T bit_ceil(T Value) {
  static_assert(std::is_unsigned_v<T>,
                "Only unsigned integral types are allowed.");
  if (Value < 2)
    return 1;
  return T(1) << llvm::bit_width<T>(Value - 1u);
}

namespace detail {
template <typename T, std::size_t SizeOfT> struct PopulationCounter {
  static i32 count(T Value) {
    // Generic version, forward to 32 bits.
    static_assert(SizeOfT <= 4, "Not implemented!");
#if defined(__GNUC__)
    return (i32)__builtin_popcount(Value);
#else
    u32 v = Value;
    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    return i32(((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24);
#endif
  }
};

template <typename T> struct PopulationCounter<T, 8> {
  static i32 count(T Value) {
#if defined(__GNUC__)
    return (i32)__builtin_popcountll(Value);
#else
    u64 v = Value;
    v = v - ((v >> 1) & 0x5555555555555555ULL);
    v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
    v = (v + (v >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return i32((u64)(v * 0x0101010101010101ULL) >> 56);
#endif
  }
};
} // namespace detail

/// Count the number of set bits in a value.
/// Ex. popcount(0xF000F000) = 8
/// Returns 0 if the word is zero.
template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
[[nodiscard]] inline i32 popcount(T Value) noexcept {
  return detail::PopulationCounter<T, sizeof(T)>::count(Value);
}

} // namespace llvm

#endif
