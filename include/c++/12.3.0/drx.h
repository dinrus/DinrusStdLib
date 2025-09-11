#undef linux

#include <stdarg.h>
#include <stddef.h>

#if defined(_MSC_VER)
#define drx_msvc
#elif defined(__clang__)
#define drx_clang
#define drx_gnuc
#elif defined(__GNUC__)
#define drx_gcc
#define drx_gnuc
#elif defined(__IBMC__)
#define drx_xlc
#elif defined(__TINYC__)
#define drx_tinyc
#elif defined(__slimcc__)
#define drx_slimcc
#endif

#if defined(__aarch64__) || (defined(drx_msvc) && defined(_M_ARM64))
#define drx_aarch64
#elif defined(__thumb__) || (defined(drx_msvc) && defined(_M_ARM))
#define drx_thumb
#define drx_arm
#elif defined(__arm__)
#define drx_arm
#elif defined(__hexagon__)
#define drx_hexagon
#elif defined(__loongarch32)
#define drx_loongarch32
#define drx_loongarch
#elif defined(__loongarch64)
#define drx_loongarch64
#define drx_loongarch
#elif defined(__mips64)
#define drx_mips64
#define drx_mips
#elif defined(__mips__)
#define drx_mips32
#define drx_mips
#elif defined(__powerpc64__)
#define drx_powerpc64
#define drx_powerpc
#elif defined(__powerpc__)
#define drx_powerpc32
#define drx_powerpc
#elif defined(__riscv) && __riscv_xlen == 32
#define drx_riscv32
#define drx_riscv
#elif defined(__riscv) && __riscv_xlen == 64
#define drx_riscv64
#define drx_riscv
#elif defined(__s390x__)
#define drx_s390x
#elif defined(__sparc__) && defined(__arch64__)
#define drx_sparc64
#define drx_sparc
#elif defined(__sparc__)
#define drx_sparc32
#define drx_sparc
#elif defined(__wasm32__)
#define drx_wasm32
#define drx_wasm
#elif defined(__wasm64__)
#define drx_wasm64
#define drx_wasm
#elif defined(__i386__) || (defined(drx_msvc) && defined(_M_IX86))
#define drx_x86_32
#define drx_x86
#elif defined (__x86_64__) || (defined(drx_msvc) && defined(_M_X64))
#define drx_x86_64
#define drx_x86
#endif

#if defined(drx_msvc) || __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define drx_little_endian 1
#define drx_big_endian 0
#else
#define drx_little_endian 0
#define drx_big_endian 1
#endif

#if defined(_AIX)
#define drx_aix
#elif defined(__MACH__)
#define drx_darwin
#elif defined(__DragonFly__)
#define drx_dragonfly
#define drx_bsd
#elif defined(__EMSCRIPTEN__)
#define drx_emscripten
#elif defined(__FreeBSD__)
#define drx_freebsd
#define drx_bsd
#elif defined(__Fuchsia__)
#define drx_fuchsia
#elif defined(__HAIKU__)
#define drx_haiku
#elif defined(__gnu_hurd__)
#define drx_hurd
#elif defined(__linux__)
#define drx_linux
#elif defined(__NetBSD__)
#define drx_netbsd
#define drx_bsd
#elif defined(__OpenBSD__)
#define drx_openbsd
#define drx_bsd
#elif defined(__SVR4)
#define drx_solaris
#elif defined(__wasi__)
#define drx_wasi
#elif defined(_WIN32)
#define drx_windows
#elif defined(__MVS__)
#define drx_zos
#endif

#if defined(drx_windows)
#define drx_coff
#elif defined(__ELF__)
#define drx_elf
#elif defined(drx_zos)
#define drx_goff
#elif defined(drx_darwin)
#define drx_macho
#elif defined(drx_aix)
#define drx_xcoff
#endif

#define drx_concat(lhs, rhs) lhs##rhs
#define drx_expand_concat(lhs, rhs) drx_concat(lhs, rhs)

#if defined(__has_include)
#define drx_has_include(include) __has_include(include)
#else
#define drx_has_include(include) 0
#endif

#if defined(__has_builtin)
#define drx_has_builtin(builtin) __has_builtin(__builtin_##builtin)
#else
#define drx_has_builtin(builtin) 0
#endif
#define drx_expand_has_builtin(b) drx_has_builtin(b)

#if defined(__has_attribute)
#define drx_has_attribute(attribute) __has_attribute(attribute)
#else
#define drx_has_attribute(attribute) 0
#endif

#if __STDC_VERSION__ >= 202311L
#define drx_threadlocal thread_local
#elif __STDC_VERSION__ >= 201112L
#define drx_threadlocal _Thread_local
#elif defined(drx_gnuc) || defined(drx_slimcc)
#define drx_threadlocal __thread
#elif defined(drx_msvc)
#define drx_threadlocal __declspec(thread)
#else
#define drx_threadlocal drx_threadlocal_unavailable
#endif

#if defined(drx_msvc)
#define drx_const_arr
#define drx_callconv(c) __##c
#else
#define drx_const_arr static const
#define drx_callconv(c) __attribute__((c))
#endif

#if drx_has_attribute(naked) || defined(drx_gcc)
#define drx_naked_decl __attribute__((naked))
#define drx_naked __attribute__((naked))
#elif defined(drx_msvc)
#define drx_naked_decl
#define drx_naked __declspec(naked)
#else
#define drx_naked_decl drx_naked_unavailable
#define drx_naked drx_naked_unavailable
#endif

#if drx_has_attribute(cold)
#define drx_cold __attribute__((cold))
#else
#define drx_cold
#endif

#if drx_has_attribute(flatten)
#define drx_maybe_flatten __attribute__((flatten))
#else
#define drx_maybe_flatten
#endif

#if drx_has_attribute(noinline)
#define drx_never_inline __attribute__((noinline)) drx_maybe_flatten
#elif defined(drx_msvc)
#define drx_never_inline __declspec(noinline) drx_maybe_flatten
#else
#define drx_never_inline drx_never_inline_unavailable
#endif

#if drx_has_attribute(not_tail_called)
#define drx_never_tail __attribute__((not_tail_called)) drx_never_inline
#else
#define drx_never_tail drx_never_tail_unavailable
#endif

#if drx_has_attribute(musttail)
#define drx_always_tail __attribute__((musttail))
#else
#define drx_always_tail drx_always_tail_unavailable
#endif

#if __STDC_VERSION__ >= 199901L
#define drx_restrict restrict
#elif defined(drx_gnuc) || defined(drx_tinyc)
#define drx_restrict __restrict
#else
#define drx_restrict
#endif

#if drx_has_attribute(no_builtin)
#define drx_no_builtin __attribute__((no_builtin))
#else
#define drx_no_builtin
#endif

#if drx_has_attribute(aligned) || defined(drx_tinyc)
#define drx_under_align(alignment) __attribute__((aligned(alignment)))
#elif defined(drx_msvc)
#define drx_under_align(alignment) __declspec(align(alignment))
#else
#define drx_under_align drx_align_unavailable
#endif

#if __STDC_VERSION__ >= 202311L
#define drx_align(alignment) alignas(alignment)
#elif __STDC_VERSION__ >= 201112L
#define drx_align(alignment) _Alignas(alignment)
#else
#define drx_align(alignment) drx_under_align(alignment)
#endif

#if drx_has_attribute(aligned) || defined(drx_tinyc)
#define drx_align_fn(alignment) __attribute__((aligned(alignment)))
#elif defined(drx_msvc)
#define drx_align_fn(alignment)
#else
#define drx_align_fn drx_align_fn_unavailable
#endif

#if drx_has_attribute(packed) || defined(drx_tinyc)
#define drx_packed(definition) __attribute__((packed)) definition
#elif defined(drx_msvc)
#define drx_packed(definition) __pragma(pack(1)) definition __pragma(pack())
#else
#define drx_packed(definition) drx_packed_unavailable
#endif

#if drx_has_attribute(section) || defined(drx_tinyc)
#define drx_linksection(name) __attribute__((section(name)))
#define drx_linksection_fn drx_linksection
#elif defined(drx_msvc)
#define drx_linksection(name) __pragma(section(name, read, write)) __declspec(allocate(name))
#define drx_linksection_fn(name) __pragma(section(name, read, execute)) __declspec(code_seg(name))
#else
#define drx_linksection(name) drx_linksection_unavailable
#define drx_linksection_fn drx_linksection
#endif

#if drx_has_builtin(unreachable) || defined(drx_gcc) || defined(drx_tinyc)
#define drx_unreachable() __builtin_unreachable()
#elif defined(drx_msvc)
#define drx_unreachable() __assume(0)
#else
#define drx_unreachable()
#endif

#if defined(__cplusplus)
#define drx_extern extern "C"
#else
#define drx_extern extern
#endif

#if defined(drx_msvc)
#if defined(drx_x86_64)
#define drx_mangle_c(symbol) symbol
#else /* drx_x86_64 */
#define drx_mangle_c(symbol) "_" symbol
#endif /* drx_x86_64 */
#else /* drx_msvc */
#if defined(drx_macho)
#define drx_mangle_c(symbol) "_" symbol
#else /* drx_macho */
#define drx_mangle_c(symbol) symbol
#endif /* drx_macho */
#endif /* drx_msvc */

#if defined(drx_msvc)
#define drx_export(symbol, name) ; \
    __pragma(comment(linker, "/alternatename:" drx_mangle_c(name) "=" drx_mangle_c(symbol)))
#elif (drx_has_attribute(alias) || defined(drx_tinyc)) && !defined(drx_macho)
#define drx_export(symbol, name) __attribute__((alias(symbol)))
#else
#define drx_export(symbol, name) ; \
    __asm(drx_mangle_c(name) " = " drx_mangle_c(symbol))
#endif

#define drx_mangled_tentative drx_mangled
#define drx_mangled_final drx_mangled
#if defined(drx_msvc)
#define drx_mangled(mangled, unmangled) ; \
    drx_export(#mangled, unmangled)
#define drx_mangled_export(mangled, unmangled, symbol) \
    drx_export(unmangled, #mangled) \
    drx_export(symbol, unmangled)
#else /* drx_msvc */
#define drx_mangled(mangled, unmangled) __asm(drx_mangle_c(unmangled))
#define drx_mangled_export(mangled, unmangled, symbol) \
    drx_mangled_final(mangled, unmangled) \
    drx_export(symbol, unmangled)
#endif /* drx_msvc */

#if defined(drx_msvc)
#define drx_import(Type, fn_name, libc_name, sig_args, call_args) drx_extern Type fn_name sig_args;\
    __pragma(comment(linker, "/alternatename:" drx_mangle_c(#fn_name) "=" drx_mangle_c(#libc_name)));
#define drx_import_builtin(Type, fn_name, libc_name, sig_args, call_args) drx_import(Type, fn_name, libc_name, sig_args, call_args)
#else /* drx_msvc */
#define drx_import(Type, fn_name, libc_name, sig_args, call_args) drx_extern Type fn_name sig_args __asm(drx_mangle_c(#libc_name));
#define drx_import_builtin(Type, fn_name, libc_name, sig_args, call_args) drx_extern Type libc_name sig_args; \
    static inline Type fn_name sig_args { return libc_name call_args; }
#endif

#define drx_expand_import_0(Type, fn_name, libc_name, sig_args, call_args) drx_import(Type, fn_name, libc_name, sig_args, call_args)
#define drx_expand_import_1(Type, fn_name, libc_name, sig_args, call_args) drx_import_builtin(Type, fn_name, libc_name, sig_args, call_args)

#if drx_has_attribute(weak) || defined(drx_gcc) || defined(drx_tinyc)
#define drx_weak_linkage __attribute__((weak))
#define drx_weak_linkage_fn __attribute__((weak))
#elif defined(drx_msvc)
#define drx_weak_linkage __declspec(selectany)
#define drx_weak_linkage_fn
#else
#define drx_weak_linkage drx_weak_linkage_unavailable
#define drx_weak_linkage_fn drx_weak_linkage_unavailable
#endif

#if defined(drx_gnuc) || defined(drx_tinyc) || defined(drx_slimcc)
#define drx_gnuc_asm
#endif

#if drx_has_builtin(trap)
#define drx_trap() __builtin_trap()
#elif defined(drx_msvc)

#if defined(drx_x86)
#define drx_trap() __ud2()
#else
#define drx_trap() __fastfail(7)
#endif

#elif defined(drx_gnuc_asm)

#if defined(drx_thumb)
#define drx_trap() __asm__ volatile("udf #0xfe")
#elif defined(drx_arm) || defined(drx_aarch64)
#define drx_trap() __asm__ volatile("udf #0xfdee")
#elif defined(drx_hexagon)
#define drx_trap() __asm__ volatile("r27:26 = memd(#0xbadc0fee)")
#elif defined(drx_loongarch) || defined(drx_powerpc)
#define drx_trap() __asm__ volatile(".word 0x0")
#elif defined(drx_mips)
#define drx_trap() __asm__ volatile(".word 0x3d")
#elif defined(drx_riscv)
#define drx_trap() __asm__ volatile("unimp")
#elif defined(drx_s390x)
#define drx_trap() __asm__ volatile("j 0x2")
#elif defined(drx_sparc)
#define drx_trap() __asm__ volatile("illtrap")
#elif defined(drx_x86)
#define drx_trap() __asm__ volatile("ud2")
#else
#define drx_trap() drx_trap_unavailable
#endif

#else
#define drx_trap() drx_trap_unavailable
#endif

#if drx_has_builtin(debugtrap)
#define drx_breakpoint() __builtin_debugtrap()
#elif defined(drx_msvc)
#define drx_breakpoint() __debugbreak()
#elif defined(drx_gnuc_asm)

#if defined(drx_arm)
#define drx_breakpoint() __asm__ volatile("bkpt #0x0")
#elif defined(drx_aarch64)
#define drx_breakpoint() __asm__ volatile("brk #0xf000")
#elif defined(drx_hexagon)
#define drx_breakpoint() __asm__ volatile("brkpt")
#elif defined(drx_loongarch)
#define drx_breakpoint() __asm__ volatile("break 0x0")
#elif defined(drx_mips)
#define drx_breakpoint() __asm__ volatile("break")
#elif defined(drx_powerpc)
#define drx_breakpoint() __asm__ volatile("trap")
#elif defined(drx_riscv)
#define drx_breakpoint() __asm__ volatile("ebreak")
#elif defined(drx_s390x)
#define drx_breakpoint() __asm__ volatile("j 0x6")
#elif defined(drx_sparc)
#define drx_breakpoint() __asm__ volatile("ta 0x1")
#elif defined(drx_x86)
#define drx_breakpoint() __asm__ volatile("int $0x3")
#else
#define drx_breakpoint() drx_breakpoint_unavailable
#endif

#else
#define drx_breakpoint() drx_breakpoint_unavailable
#endif

#if drx_has_builtin(return_address) || defined(drx_gcc) || defined(drx_tinyc)
#define drx_return_address() __builtin_extract_return_addr(__builtin_return_address(0))
#elif defined(drx_msvc)
#define drx_return_address() _ReturnAddress()
#else
#define drx_return_address() 0
#endif

#if drx_has_builtin(frame_address) || defined(drx_gcc) || defined(drx_tinyc)
#define drx_frame_address() __builtin_frame_address(0)
#elif defined(drx_msvc)
#define drx_frame_address() _AddressOfReturnAddress()
#else
#define drx_frame_address() 0
#endif

#if drx_has_builtin(prefetch) || defined(drx_gcc)
#define drx_prefetch(addr, rw, locality) __builtin_prefetch(addr, rw, locality)
#else
#define drx_prefetch(addr, rw, locality)
#endif

#if drx_has_builtin(memory_size) && drx_has_builtin(memory_grow)
#define drx_wasm_memory_size(index) __builtin_wasm_memory_size(index)
#define drx_wasm_memory_grow(index, delta) __builtin_wasm_memory_grow(index, delta)
#else
#define drx_wasm_memory_size(index) drx_unimplemented()
#define drx_wasm_memory_grow(index, delta) drx_unimplemented()
#endif

#if __STDC_VERSION__ >= 202311L
#define drx_noreturn [[noreturn]]
#elif __STDC_VERSION__ >= 201112L
#define drx_noreturn _Noreturn
#elif drx_has_attribute(noreturn) || defined(drx_gcc) || defined(drx_tinyc)
#define drx_noreturn __attribute__((noreturn))
#elif defined(drx_msvc)
#define drx_noreturn __declspec(noreturn)
#else
#define drx_noreturn
#endif

#define drx_compiler_rt_abbrev_uint32_t si
#define drx_compiler_rt_abbrev_int32_t  si
#define drx_compiler_rt_abbrev_uint64_t di
#define drx_compiler_rt_abbrev_int64_t  di
#define drx_compiler_rt_abbrev_drx_u128 ti
#define drx_compiler_rt_abbrev_drx_i128 ti
#define drx_compiler_rt_abbrev_drx_f16  hf
#define drx_compiler_rt_abbrev_drx_f32  sf
#define drx_compiler_rt_abbrev_drx_f64  df
#define drx_compiler_rt_abbrev_drx_f80  xf
#define drx_compiler_rt_abbrev_drx_f128 tf

drx_extern void *memcpy (void *drx_restrict, void const *drx_restrict, size_t)
                                                      __THROW __nonnull ((1, 2));
drx_extern void *memset (void *, int, size_t)__THROW __nonnull ((1));

/* ================ Bool and 8/16/32/64-bit Integer Support ================= */

#include <limits.h>

#define drx_bitSizeOf(T) (CHAR_BIT * sizeof(T))

#if __STDC_VERSION__ >= 202311L
/* bool, true, and false are provided by the language. */
#elif __STDC_VERSION__ >= 199901L || drx_has_include(<stdbool.h>)
#include <stdbool.h>
#else
typedef char bool;
#define false 0
#define true  1
#endif

#if __STDC_VERSION__ >= 199901L || defined(drx_msvc) || drx_has_include(<stdint.h>)
#include <stdint.h>
#else
#if SCHAR_MIN == ~0x7F && SCHAR_MAX == 0x7F && UCHAR_MAX == 0xFF
typedef unsigned      char uint8_t;
typedef   signed      char  int8_t;
#define  INT8_C(c) c
#define UINT8_C(c) c##U
#elif SHRT_MIN == ~0x7F && SHRT_MAX == 0x7F && USHRT_MAX == 0xFF
typedef unsigned     short uint8_t;
typedef   signed     short  int8_t;
#define  INT8_C(c) c
#define UINT8_C(c) c##U
#elif INT_MIN == ~0x7F && INT_MAX == 0x7F && UINT_MAX == 0xFF
typedef unsigned       int uint8_t;
typedef   signed       int  int8_t;
#define  INT8_C(c) c
#define UINT8_C(c) c##U
#elif LONG_MIN == ~0x7F && LONG_MAX == 0x7F && ULONG_MAX == 0xFF
typedef unsigned      long uint8_t;
typedef   signed      long  int8_t;
#define  INT8_C(c) c##L
#define UINT8_C(c) c##LU
#elif LLONG_MIN == ~0x7F && LLONG_MAX == 0x7F && ULLONG_MAX == 0xFF
typedef unsigned long long uint8_t;
typedef   signed long long  int8_t;
#define  INT8_C(c) c##LL
#define UINT8_C(c) c##LLU
#endif
#define  INT8_MIN (~INT8_C(0x7F))
#define  INT8_MAX ( INT8_C(0x7F))
#define UINT8_MAX ( INT8_C(0xFF))

#if SCHAR_MIN == ~0x7FFF && SCHAR_MAX == 0x7FFF && UCHAR_MAX == 0xFFFF
typedef unsigned      char uint16_t;
typedef   signed      char  int16_t;
#define  INT16_C(c) c
#define UINT16_C(c) c##U
#elif SHRT_MIN == ~0x7FFF && SHRT_MAX == 0x7FFF && USHRT_MAX == 0xFFFF
typedef unsigned     short uint16_t;
typedef   signed     short  int16_t;
#define  INT16_C(c) c
#define UINT16_C(c) c##U
#elif INT_MIN == ~0x7FFF && INT_MAX == 0x7FFF && UINT_MAX == 0xFFFF
typedef unsigned       int uint16_t;
typedef   signed       int  int16_t;
#define  INT16_C(c) c
#define UINT16_C(c) c##U
#elif LONG_MIN == ~0x7FFF && LONG_MAX == 0x7FFF && ULONG_MAX == 0xFFFF
typedef unsigned      long uint16_t;
typedef   signed      long  int16_t;
#define  INT16_C(c) c##L
#define UINT16_C(c) c##LU
#elif LLONG_MIN == ~0x7FFF && LLONG_MAX == 0x7FFF && ULLONG_MAX == 0xFFFF
typedef unsigned long long uint16_t;
typedef   signed long long  int16_t;
#define  INT16_C(c) c##LL
#define UINT16_C(c) c##LLU
#endif
#define  INT16_MIN (~INT16_C(0x7FFF))
#define  INT16_MAX ( INT16_C(0x7FFF))
#define UINT16_MAX ( INT16_C(0xFFFF))

#if SCHAR_MIN == ~0x7FFFFFFF && SCHAR_MAX == 0x7FFFFFFF && UCHAR_MAX == 0xFFFFFFFF
typedef unsigned      char uint32_t;
typedef   signed      char  int32_t;
#define  INT32_C(c) c
#define UINT32_C(c) c##U
#elif SHRT_MIN == ~0x7FFFFFFF && SHRT_MAX == 0x7FFFFFFF && USHRT_MAX == 0xFFFFFFFF
typedef unsigned     short uint32_t;
typedef   signed     short  int32_t;
#define  INT32_C(c) c
#define UINT32_C(c) c##U
#elif INT_MIN == ~0x7FFFFFFF && INT_MAX == 0x7FFFFFFF && UINT_MAX == 0xFFFFFFFF
typedef unsigned       int uint32_t;
typedef   signed       int  int32_t;
#define  INT32_C(c) c
#define UINT32_C(c) c##U
#elif LONG_MIN == ~0x7FFFFFFF && LONG_MAX == 0x7FFFFFFF && ULONG_MAX == 0xFFFFFFFF
typedef unsigned      long uint32_t;
typedef   signed      long  int32_t;
#define  INT32_C(c) c##L
#define UINT32_C(c) c##LU
#elif LLONG_MIN == ~0x7FFFFFFF && LLONG_MAX == 0x7FFFFFFF && ULLONG_MAX == 0xFFFFFFFF
typedef unsigned long long uint32_t;
typedef   signed long long  int32_t;
#define  INT32_C(c) c##LL
#define UINT32_C(c) c##LLU
#endif
#define  INT32_MIN (~INT32_C(0x7FFFFFFF))
#define  INT32_MAX ( INT32_C(0x7FFFFFFF))
#define UINT32_MAX ( INT32_C(0xFFFFFFFF))

#if SCHAR_MIN == ~0x7FFFFFFFFFFFFFFF && SCHAR_MAX == 0x7FFFFFFFFFFFFFFF && UCHAR_MAX == 0xFFFFFFFFFFFFFFFF
typedef unsigned      char uint64_t;
typedef   signed      char  int64_t;
#define  INT64_C(c) c
#define UINT64_C(c) c##U
#elif SHRT_MIN == ~0x7FFFFFFFFFFFFFFF && SHRT_MAX == 0x7FFFFFFFFFFFFFFF && USHRT_MAX == 0xFFFFFFFFFFFFFFFF
typedef unsigned     short uint64_t;
typedef   signed     short  int64_t;
#define  INT64_C(c) c
#define UINT64_C(c) c##U
#elif INT_MIN == ~0x7FFFFFFFFFFFFFFF && INT_MAX == 0x7FFFFFFFFFFFFFFF && UINT_MAX == 0xFFFFFFFFFFFFFFFF
typedef unsigned       int uint64_t;
typedef   signed       int  int64_t;
#define  INT64_C(c) c
#define UINT64_C(c) c##U
#elif LONG_MIN == ~0x7FFFFFFFFFFFFFFF && LONG_MAX == 0x7FFFFFFFFFFFFFFF && ULONG_MAX == 0xFFFFFFFFFFFFFFFF
typedef unsigned      long uint64_t;
typedef   signed      long  int64_t;
#define  INT64_C(c) c##L
#define UINT64_C(c) c##LU
#elif LLONG_MIN == ~0x7FFFFFFFFFFFFFFF && LLONG_MAX == 0x7FFFFFFFFFFFFFFF && ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
typedef unsigned long long uint64_t;
typedef   signed long long  int64_t;
#define  INT64_C(c) c##LL
#define UINT64_C(c) c##LLU
#endif
#define  INT64_MIN (~INT64_C(0x7FFFFFFFFFFFFFFF))
#define  INT64_MAX ( INT64_C(0x7FFFFFFFFFFFFFFF))
#define UINT64_MAX ( INT64_C(0xFFFFFFFFFFFFFFFF))

typedef size_t uintptr_t;
typedef ptrdiff_t intptr_t;

#endif

#define drx_minInt_i8    INT8_MIN
#define drx_maxInt_i8    INT8_MAX
#define drx_minInt_u8   UINT8_C(0)
#define drx_maxInt_u8   UINT8_MAX
#define drx_minInt_i16  INT16_MIN
#define drx_maxInt_i16  INT16_MAX
#define drx_minInt_u16 UINT16_C(0)
#define drx_maxInt_u16 UINT16_MAX
#define drx_minInt_i32  INT32_MIN
#define drx_maxInt_i32  INT32_MAX
#define drx_minInt_u32 UINT32_C(0)
#define drx_maxInt_u32 UINT32_MAX
#define drx_minInt_i64  INT64_MIN
#define drx_maxInt_i64  INT64_MAX
#define drx_minInt_u64 UINT64_C(0)
#define drx_maxInt_u64 UINT64_MAX

#define drx_intLimit(s, w, limit, bits) drx_shr_##s##w(drx_##limit##Int_##s##w, w - (bits))
#define drx_minInt_i(w, bits) drx_intLimit(i, w, min, bits)
#define drx_maxInt_i(w, bits) drx_intLimit(i, w, max, bits)
#define drx_minInt_u(w, bits) drx_intLimit(u, w, min, bits)
#define drx_maxInt_u(w, bits) drx_intLimit(u, w, max, bits)

#define drx_operator(Type, RhsType, operation, operator) \
    static inline Type drx_##operation(Type lhs, RhsType rhs) { \
        return lhs operator rhs; \
    }
#define drx_basic_operator(Type, operation, operator) \
    drx_operator(Type,    Type, operation, operator)
#define drx_shift_operator(Type, operation, operator) \
    drx_operator(Type, uint8_t, operation, operator)
#define drx_int_helpers(w, PromotedUnsigned) \
    drx_basic_operator(uint##w##_t, and_u##w,  &) \
    drx_basic_operator( int##w##_t, and_i##w,  &) \
    drx_basic_operator(uint##w##_t,  or_u##w,  |) \
    drx_basic_operator( int##w##_t,  or_i##w,  |) \
    drx_basic_operator(uint##w##_t, xor_u##w,  ^) \
    drx_basic_operator( int##w##_t, xor_i##w,  ^) \
    drx_shift_operator(uint##w##_t, shl_u##w, <<) \
    drx_shift_operator( int##w##_t, shl_i##w, <<) \
    drx_shift_operator(uint##w##_t, shr_u##w, >>) \
\
    static inline int##w##_t drx_shr_i##w(int##w##_t lhs, uint8_t rhs) { \
        int##w##_t sign_mask = lhs < INT##w##_C(0) ? -INT##w##_C(1) : INT##w##_C(0); \
        return ((lhs ^ sign_mask) >> rhs) ^ sign_mask; \
    } \
\
    static inline uint##w##_t drx_not_u##w(uint##w##_t val, uint8_t bits) { \
        return val ^ drx_maxInt_u(w, bits); \
    } \
\
    static inline int##w##_t drx_not_i##w(int##w##_t val, uint8_t bits) { \
        (void)bits; \
        return ~val; \
    } \
\
    static inline uint##w##_t drx_wrap_u##w(uint##w##_t val, uint8_t bits) { \
        return val & drx_maxInt_u(w, bits); \
    } \
\
    static inline int##w##_t drx_wrap_i##w(int##w##_t val, uint8_t bits) { \
        return (val & UINT##w##_C(1) << (bits - UINT8_C(1))) != 0 \
            ? val | drx_minInt_i(w, bits) : val & drx_maxInt_i(w, bits); \
    } \
\
    static inline uint##w##_t drx_abs_i##w(int##w##_t val) { \
        return (val < 0) ? -(uint##w##_t)val : (uint##w##_t)val; \
    } \
\
    drx_basic_operator(uint##w##_t, div_floor_u##w, /) \
\
    static inline int##w##_t drx_div_floor_i##w(int##w##_t lhs, int##w##_t rhs) { \
        return lhs / rhs + (lhs % rhs != INT##w##_C(0) ? drx_shr_i##w(lhs ^ rhs, UINT8_C(w) - UINT8_C(1)) : INT##w##_C(0)); \
    } \
\
    drx_basic_operator(uint##w##_t, mod_u##w, %) \
\
    static inline int##w##_t drx_mod_i##w(int##w##_t lhs, int##w##_t rhs) { \
        int##w##_t rem = lhs % rhs; \
        return rem + (rem != INT##w##_C(0) ? rhs & drx_shr_i##w(lhs ^ rhs, UINT8_C(w) - UINT8_C(1)) : INT##w##_C(0)); \
    } \
\
    static inline uint##w##_t drx_shlw_u##w(uint##w##_t lhs, uint8_t rhs, uint8_t bits) { \
        return drx_wrap_u##w(drx_shl_u##w(lhs, rhs), bits); \
    } \
\
    static inline int##w##_t drx_shlw_i##w(int##w##_t lhs, uint8_t rhs, uint8_t bits) { \
        return drx_wrap_i##w((int##w##_t)drx_shl_u##w((uint##w##_t)lhs, rhs), bits); \
    } \
\
    static inline uint##w##_t drx_addw_u##w(uint##w##_t lhs, uint##w##_t rhs, uint8_t bits) { \
        return drx_wrap_u##w(lhs + rhs, bits); \
    } \
\
    static inline int##w##_t drx_addw_i##w(int##w##_t lhs, int##w##_t rhs, uint8_t bits) { \
        return drx_wrap_i##w((int##w##_t)((uint##w##_t)lhs + (uint##w##_t)rhs), bits); \
    } \
\
    static inline uint##w##_t drx_subw_u##w(uint##w##_t lhs, uint##w##_t rhs, uint8_t bits) { \
        return drx_wrap_u##w(lhs - rhs, bits); \
    } \
\
    static inline int##w##_t drx_subw_i##w(int##w##_t lhs, int##w##_t rhs, uint8_t bits) { \
        return drx_wrap_i##w((int##w##_t)((uint##w##_t)lhs - (uint##w##_t)rhs), bits); \
    } \
\
    static inline uint##w##_t drx_mulw_u##w(uint##w##_t lhs, uint##w##_t rhs, uint8_t bits) { \
        return drx_wrap_u##w((PromotedUnsigned)lhs * rhs, bits); \
    } \
\
    static inline int##w##_t drx_mulw_i##w(int##w##_t lhs, int##w##_t rhs, uint8_t bits) { \
        return drx_wrap_i##w((int##w##_t)((uint##w##_t)lhs * (uint##w##_t)rhs), bits); \
    }
#if UINT8_MAX <= UINT_MAX
drx_int_helpers(8, unsigned int)
#elif UINT8_MAX <= ULONG_MAX
drx_int_helpers(8, unsigned long)
#elif UINT8_MAX <= ULLONG_MAX
drx_int_helpers(8, unsigned long long)
#else
drx_int_helpers(8, uint8_t)
#endif
#if UINT16_MAX <= UINT_MAX
drx_int_helpers(16, unsigned int)
#elif UINT16_MAX <= ULONG_MAX
drx_int_helpers(16, unsigned long)
#elif UINT16_MAX <= ULLONG_MAX
drx_int_helpers(16, unsigned long long)
#else
drx_int_helpers(16, uint16_t)
#endif
#if UINT32_MAX <= UINT_MAX
drx_int_helpers(32, unsigned int)
#elif UINT32_MAX <= ULONG_MAX
drx_int_helpers(32, unsigned long)
#elif UINT32_MAX <= ULLONG_MAX
drx_int_helpers(32, unsigned long long)
#else
drx_int_helpers(32, uint32_t)
#endif
#if UINT64_MAX <= UINT_MAX
drx_int_helpers(64, unsigned int)
#elif UINT64_MAX <= ULONG_MAX
drx_int_helpers(64, unsigned long)
#elif UINT64_MAX <= ULLONG_MAX
drx_int_helpers(64, unsigned long long)
#else
drx_int_helpers(64, uint64_t)
#endif

static inline bool drx_addo_u32(uint32_t *res, uint32_t lhs, uint32_t rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow) || defined(drx_gcc)
    uint32_t full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u32(full_res, bits);
    return overflow || full_res < drx_minInt_u(32, bits) || full_res > drx_maxInt_u(32, bits);
#else
    *res = drx_addw_u32(lhs, rhs, bits);
    return *res < lhs;
#endif
}

drx_extern int32_t  __addosi4(int32_t lhs, int32_t rhs, int *overflow);
static inline bool drx_addo_i32(int32_t *res, int32_t lhs, int32_t rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow) || defined(drx_gcc)
    int32_t full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    int32_t full_res = __addosi4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i32(full_res, bits);
    return overflow || full_res < drx_minInt_i(32, bits) || full_res > drx_maxInt_i(32, bits);
}

static inline bool drx_addo_u64(uint64_t *res, uint64_t lhs, uint64_t rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow) || defined(drx_gcc)
    uint64_t full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u64(full_res, bits);
    return overflow || full_res < drx_minInt_u(64, bits) || full_res > drx_maxInt_u(64, bits);
#else
    *res = drx_addw_u64(lhs, rhs, bits);
    return *res < lhs;
#endif
}

drx_extern int64_t  __addodi4(int64_t lhs, int64_t rhs, int *overflow);
static inline bool drx_addo_i64(int64_t *res, int64_t lhs, int64_t rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow) || defined(drx_gcc)
    int64_t full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    int64_t full_res = __addodi4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i64(full_res, bits);
    return overflow || full_res < drx_minInt_i(64, bits) || full_res > drx_maxInt_i(64, bits);
}

static inline bool drx_addo_u8(uint8_t *res, uint8_t lhs, uint8_t rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow) || defined(drx_gcc)
    uint8_t full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u8(full_res, bits);
    return overflow || full_res < drx_minInt_u(8, bits) || full_res > drx_maxInt_u(8, bits);
#else
    uint32_t full_res;
    bool overflow = drx_addo_u32(&full_res, lhs, rhs, bits);
    *res = (uint8_t)full_res;
    return overflow;
#endif
}

static inline bool drx_addo_i8(int8_t *res, int8_t lhs, int8_t rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow) || defined(drx_gcc)
    int8_t full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_i8(full_res, bits);
    return overflow || full_res < drx_minInt_i(8, bits) || full_res > drx_maxInt_i(8, bits);
#else
    int32_t full_res;
    bool overflow = drx_addo_i32(&full_res, lhs, rhs, bits);
    *res = (int8_t)full_res;
    return overflow;
#endif
}

static inline bool drx_addo_u16(uint16_t *res, uint16_t lhs, uint16_t rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow) || defined(drx_gcc)
    uint16_t full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u16(full_res, bits);
    return overflow || full_res < drx_minInt_u(16, bits) || full_res > drx_maxInt_u(16, bits);
#else
    uint32_t full_res;
    bool overflow = drx_addo_u32(&full_res, lhs, rhs, bits);
    *res = (uint16_t)full_res;
    return overflow;
#endif
}

static inline bool drx_addo_i16(int16_t *res, int16_t lhs, int16_t rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow) || defined(drx_gcc)
    int16_t full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_i16(full_res, bits);
    return overflow || full_res < drx_minInt_i(16, bits) || full_res > drx_maxInt_i(16, bits);
#else
    int32_t full_res;
    bool overflow = drx_addo_i32(&full_res, lhs, rhs, bits);
    *res = (int16_t)full_res;
    return overflow;
#endif
}

static inline bool drx_subo_u32(uint32_t *res, uint32_t lhs, uint32_t rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow) || defined(drx_gcc)
    uint32_t full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u32(full_res, bits);
    return overflow || full_res < drx_minInt_u(32, bits) || full_res > drx_maxInt_u(32, bits);
#else
    *res = drx_subw_u32(lhs, rhs, bits);
    return *res > lhs;
#endif
}

drx_extern int32_t  __subosi4(int32_t lhs, int32_t rhs, int *overflow);
static inline bool drx_subo_i32(int32_t *res, int32_t lhs, int32_t rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow) || defined(drx_gcc)
    int32_t full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    int32_t full_res = __subosi4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i32(full_res, bits);
    return overflow || full_res < drx_minInt_i(32, bits) || full_res > drx_maxInt_i(32, bits);
}

static inline bool drx_subo_u64(uint64_t *res, uint64_t lhs, uint64_t rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow) || defined(drx_gcc)
    uint64_t full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u64(full_res, bits);
    return overflow || full_res < drx_minInt_u(64, bits) || full_res > drx_maxInt_u(64, bits);
#else
    *res = drx_subw_u64(lhs, rhs, bits);
    return *res > lhs;
#endif
}

drx_extern int64_t  __subodi4(int64_t lhs, int64_t rhs, int *overflow);
static inline bool drx_subo_i64(int64_t *res, int64_t lhs, int64_t rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow) || defined(drx_gcc)
    int64_t full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    int64_t full_res = __subodi4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i64(full_res, bits);
    return overflow || full_res < drx_minInt_i(64, bits) || full_res > drx_maxInt_i(64, bits);
}

static inline bool drx_subo_u8(uint8_t *res, uint8_t lhs, uint8_t rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow) || defined(drx_gcc)
    uint8_t full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u8(full_res, bits);
    return overflow || full_res < drx_minInt_u(8, bits) || full_res > drx_maxInt_u(8, bits);
#else
    uint32_t full_res;
    bool overflow = drx_subo_u32(&full_res, lhs, rhs, bits);
    *res = (uint8_t)full_res;
    return overflow;
#endif
}

static inline bool drx_subo_i8(int8_t *res, int8_t lhs, int8_t rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow) || defined(drx_gcc)
    int8_t full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_i8(full_res, bits);
    return overflow || full_res < drx_minInt_i(8, bits) || full_res > drx_maxInt_i(8, bits);
#else
    int32_t full_res;
    bool overflow = drx_subo_i32(&full_res, lhs, rhs, bits);
    *res = (int8_t)full_res;
    return overflow;
#endif
}

static inline bool drx_subo_u16(uint16_t *res, uint16_t lhs, uint16_t rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow) || defined(drx_gcc)
    uint16_t full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u16(full_res, bits);
    return overflow || full_res < drx_minInt_u(16, bits) || full_res > drx_maxInt_u(16, bits);
#else
    uint32_t full_res;
    bool overflow = drx_subo_u32(&full_res, lhs, rhs, bits);
    *res = (uint16_t)full_res;
    return overflow;
#endif
}

static inline bool drx_subo_i16(int16_t *res, int16_t lhs, int16_t rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow) || defined(drx_gcc)
    int16_t full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_i16(full_res, bits);
    return overflow || full_res < drx_minInt_i(16, bits) || full_res > drx_maxInt_i(16, bits);
#else
    int32_t full_res;
    bool overflow = drx_subo_i32(&full_res, lhs, rhs, bits);
    *res = (int16_t)full_res;
    return overflow;
#endif
}

static inline bool drx_mulo_u32(uint32_t *res, uint32_t lhs, uint32_t rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow) || defined(drx_gcc)
    uint32_t full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u32(full_res, bits);
    return overflow || full_res < drx_minInt_u(32, bits) || full_res > drx_maxInt_u(32, bits);
#else
    *res = drx_mulw_u32(lhs, rhs, bits);
    return rhs != UINT32_C(0) && lhs > drx_maxInt_u(32, bits) / rhs;
#endif
}

drx_extern int32_t  __mulosi4(int32_t lhs, int32_t rhs, int *overflow);
static inline bool drx_mulo_i32(int32_t *res, int32_t lhs, int32_t rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow) || defined(drx_gcc)
    int32_t full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    int32_t full_res = __mulosi4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i32(full_res, bits);
    return overflow || full_res < drx_minInt_i(32, bits) || full_res > drx_maxInt_i(32, bits);
}

static inline bool drx_mulo_u64(uint64_t *res, uint64_t lhs, uint64_t rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow) || defined(drx_gcc)
    uint64_t full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u64(full_res, bits);
    return overflow || full_res < drx_minInt_u(64, bits) || full_res > drx_maxInt_u(64, bits);
#else
    *res = drx_mulw_u64(lhs, rhs, bits);
    return rhs != UINT64_C(0) && lhs > drx_maxInt_u(64, bits) / rhs;
#endif
}

drx_extern int64_t  __mulodi4(int64_t lhs, int64_t rhs, int *overflow);
static inline bool drx_mulo_i64(int64_t *res, int64_t lhs, int64_t rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow) || defined(drx_gcc)
    int64_t full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    int64_t full_res = __mulodi4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i64(full_res, bits);
    return overflow || full_res < drx_minInt_i(64, bits) || full_res > drx_maxInt_i(64, bits);
}

static inline bool drx_mulo_u8(uint8_t *res, uint8_t lhs, uint8_t rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow) || defined(drx_gcc)
    uint8_t full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u8(full_res, bits);
    return overflow || full_res < drx_minInt_u(8, bits) || full_res > drx_maxInt_u(8, bits);
#else
    uint32_t full_res;
    bool overflow = drx_mulo_u32(&full_res, lhs, rhs, bits);
    *res = (uint8_t)full_res;
    return overflow;
#endif
}

static inline bool drx_mulo_i8(int8_t *res, int8_t lhs, int8_t rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow) || defined(drx_gcc)
    int8_t full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_i8(full_res, bits);
    return overflow || full_res < drx_minInt_i(8, bits) || full_res > drx_maxInt_i(8, bits);
#else
    int32_t full_res;
    bool overflow = drx_mulo_i32(&full_res, lhs, rhs, bits);
    *res = (int8_t)full_res;
    return overflow;
#endif
}

static inline bool drx_mulo_u16(uint16_t *res, uint16_t lhs, uint16_t rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow) || defined(drx_gcc)
    uint16_t full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u16(full_res, bits);
    return overflow || full_res < drx_minInt_u(16, bits) || full_res > drx_maxInt_u(16, bits);
#else
    uint32_t full_res;
    bool overflow = drx_mulo_u32(&full_res, lhs, rhs, bits);
    *res = (uint16_t)full_res;
    return overflow;
#endif
}

static inline bool drx_mulo_i16(int16_t *res, int16_t lhs, int16_t rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow) || defined(drx_gcc)
    int16_t full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_i16(full_res, bits);
    return overflow || full_res < drx_minInt_i(16, bits) || full_res > drx_maxInt_i(16, bits);
#else
    int32_t full_res;
    bool overflow = drx_mulo_i32(&full_res, lhs, rhs, bits);
    *res = (int16_t)full_res;
    return overflow;
#endif
}

#define drx_int_builtins(w) \
    static inline bool drx_shlo_u##w(uint##w##_t *res, uint##w##_t lhs, uint8_t rhs, uint8_t bits) { \
        *res = drx_shlw_u##w(lhs, rhs, bits); \
        return lhs > drx_maxInt_u(w, bits) >> rhs; \
    } \
\
    static inline bool drx_shlo_i##w(int##w##_t *res, int##w##_t lhs, uint8_t rhs, uint8_t bits) { \
        *res = drx_shlw_i##w(lhs, rhs, bits); \
        int##w##_t mask = (int##w##_t)(UINT##w##_MAX << (bits - rhs - 1)); \
        return (lhs & mask) != INT##w##_C(0) && (lhs & mask) != mask; \
    } \
\
    static inline uint##w##_t drx_shls_u##w(uint##w##_t lhs, uint##w##_t rhs, uint8_t bits) { \
        uint##w##_t res; \
        if (rhs >= bits) return lhs != UINT##w##_C(0) ? drx_maxInt_u(w, bits) : lhs; \
        return drx_shlo_u##w(&res, lhs, (uint8_t)rhs, bits) ? drx_maxInt_u(w, bits) : res; \
    } \
\
    static inline int##w##_t drx_shls_i##w(int##w##_t lhs, int##w##_t rhs, uint8_t bits) { \
        int##w##_t res; \
        if ((uint##w##_t)rhs < (uint##w##_t)bits && !drx_shlo_i##w(&res, lhs, (uint8_t)rhs, bits)) return res; \
        return lhs < INT##w##_C(0) ? drx_minInt_i(w, bits) : drx_maxInt_i(w, bits); \
    } \
\
    static inline uint##w##_t drx_adds_u##w(uint##w##_t lhs, uint##w##_t rhs, uint8_t bits) { \
        uint##w##_t res; \
        return drx_addo_u##w(&res, lhs, rhs, bits) ? drx_maxInt_u(w, bits) : res; \
    } \
\
    static inline int##w##_t drx_adds_i##w(int##w##_t lhs, int##w##_t rhs, uint8_t bits) { \
        int##w##_t res; \
        if (!drx_addo_i##w(&res, lhs, rhs, bits)) return res; \
        return res >= INT##w##_C(0) ? drx_minInt_i(w, bits) : drx_maxInt_i(w, bits); \
    } \
\
    static inline uint##w##_t drx_subs_u##w(uint##w##_t lhs, uint##w##_t rhs, uint8_t bits) { \
        uint##w##_t res; \
        return drx_subo_u##w(&res, lhs, rhs, bits) ? drx_minInt_u(w, bits) : res; \
    } \
\
    static inline int##w##_t drx_subs_i##w(int##w##_t lhs, int##w##_t rhs, uint8_t bits) { \
        int##w##_t res; \
        if (!drx_subo_i##w(&res, lhs, rhs, bits)) return res; \
        return res >= INT##w##_C(0) ? drx_minInt_i(w, bits) : drx_maxInt_i(w, bits); \
    } \
\
    static inline uint##w##_t drx_muls_u##w(uint##w##_t lhs, uint##w##_t rhs, uint8_t bits) { \
        uint##w##_t res; \
        return drx_mulo_u##w(&res, lhs, rhs, bits) ? drx_maxInt_u(w, bits) : res; \
    } \
\
    static inline int##w##_t drx_muls_i##w(int##w##_t lhs, int##w##_t rhs, uint8_t bits) { \
        int##w##_t res; \
        if (!drx_mulo_i##w(&res, lhs, rhs, bits)) return res; \
        return (lhs ^ rhs) < INT##w##_C(0) ? drx_minInt_i(w, bits) : drx_maxInt_i(w, bits); \
    }
drx_int_builtins(8)
drx_int_builtins(16)
drx_int_builtins(32)
drx_int_builtins(64)

#define drx_builtin8(name, val) __builtin_##name(val)
typedef unsigned int drx_Builtin8;

#define drx_builtin16(name, val) __builtin_##name(val)
typedef unsigned int drx_Builtin16;

#if INT_MIN <= INT32_MIN
#define drx_builtin32(name, val) __builtin_##name(val)
typedef unsigned int drx_Builtin32;
#elif LONG_MIN <= INT32_MIN
#define drx_builtin32(name, val) __builtin_##name##l(val)
typedef unsigned long drx_Builtin32;
#endif

#if INT_MIN <= INT64_MIN
#define drx_builtin64(name, val) __builtin_##name(val)
typedef unsigned int drx_Builtin64;
#elif LONG_MIN <= INT64_MIN
#define drx_builtin64(name, val) __builtin_##name##l(val)
typedef unsigned long drx_Builtin64;
#elif LLONG_MIN <= INT64_MIN
#define drx_builtin64(name, val) __builtin_##name##ll(val)
typedef unsigned long long drx_Builtin64;
#endif

static inline uint8_t drx_byte_swap_u8(uint8_t val, uint8_t bits) {
    return drx_wrap_u8(val >> (8 - bits), bits);
}

static inline int8_t drx_byte_swap_i8(int8_t val, uint8_t bits) {
    return drx_wrap_i8((int8_t)drx_byte_swap_u8((uint8_t)val, bits), bits);
}

static inline uint16_t drx_byte_swap_u16(uint16_t val, uint8_t bits) {
    uint16_t full_res;
#if drx_has_builtin(bswap16) || defined(drx_gcc)
    full_res = __builtin_bswap16(val);
#else
    full_res = (uint16_t)drx_byte_swap_u8((uint8_t)(val >>  0), 8) <<  8 |
               (uint16_t)drx_byte_swap_u8((uint8_t)(val >>  8), 8) >>  0;
#endif
    return drx_wrap_u16(full_res >> (16 - bits), bits);
}

static inline int16_t drx_byte_swap_i16(int16_t val, uint8_t bits) {
    return drx_wrap_i16((int16_t)drx_byte_swap_u16((uint16_t)val, bits), bits);
}

static inline uint32_t drx_byte_swap_u32(uint32_t val, uint8_t bits) {
    uint32_t full_res;
#if drx_has_builtin(bswap32) || defined(drx_gcc)
    full_res = __builtin_bswap32(val);
#else
    full_res = (uint32_t)drx_byte_swap_u16((uint16_t)(val >>  0), 16) << 16 |
               (uint32_t)drx_byte_swap_u16((uint16_t)(val >> 16), 16) >>  0;
#endif
    return drx_wrap_u32(full_res >> (32 - bits), bits);
}

static inline int32_t drx_byte_swap_i32(int32_t val, uint8_t bits) {
    return drx_wrap_i32((int32_t)drx_byte_swap_u32((uint32_t)val, bits), bits);
}

static inline uint64_t drx_byte_swap_u64(uint64_t val, uint8_t bits) {
    uint64_t full_res;
#if drx_has_builtin(bswap64) || defined(drx_gcc)
    full_res = __builtin_bswap64(val);
#else
    full_res = (uint64_t)drx_byte_swap_u32((uint32_t)(val >>  0), 32) << 32 |
               (uint64_t)drx_byte_swap_u32((uint32_t)(val >> 32), 32) >>  0;
#endif
    return drx_wrap_u64(full_res >> (64 - bits), bits);
}

static inline int64_t drx_byte_swap_i64(int64_t val, uint8_t bits) {
    return drx_wrap_i64((int64_t)drx_byte_swap_u64((uint64_t)val, bits), bits);
}

static inline uint8_t drx_bit_reverse_u8(uint8_t val, uint8_t bits) {
    uint8_t full_res;
#if drx_has_builtin(bitreverse8)
    full_res = __builtin_bitreverse8(val);
#else
    static uint8_t const lut[0x10] = {
        0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
        0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
    };
    full_res = lut[val >> 0 & 0xF] << 4 | lut[val >> 4 & 0xF] << 0;
#endif
    return drx_wrap_u8(full_res >> (8 - bits), bits);
}

static inline int8_t drx_bit_reverse_i8(int8_t val, uint8_t bits) {
    return drx_wrap_i8((int8_t)drx_bit_reverse_u8((uint8_t)val, bits), bits);
}

static inline uint16_t drx_bit_reverse_u16(uint16_t val, uint8_t bits) {
    uint16_t full_res;
#if drx_has_builtin(bitreverse16)
    full_res = __builtin_bitreverse16(val);
#else
    full_res = (uint16_t)drx_bit_reverse_u8((uint8_t)(val >>  0), 8) <<  8 |
               (uint16_t)drx_bit_reverse_u8((uint8_t)(val >>  8), 8) >>  0;
#endif
    return drx_wrap_u16(full_res >> (16 - bits), bits);
}

static inline int16_t drx_bit_reverse_i16(int16_t val, uint8_t bits) {
    return drx_wrap_i16((int16_t)drx_bit_reverse_u16((uint16_t)val, bits), bits);
}

static inline uint32_t drx_bit_reverse_u32(uint32_t val, uint8_t bits) {
    uint32_t full_res;
#if drx_has_builtin(bitreverse32)
    full_res = __builtin_bitreverse32(val);
#else
    full_res = (uint32_t)drx_bit_reverse_u16((uint16_t)(val >>  0), 16) << 16 |
               (uint32_t)drx_bit_reverse_u16((uint16_t)(val >> 16), 16) >>  0;
#endif
    return drx_wrap_u32(full_res >> (32 - bits), bits);
}

static inline int32_t drx_bit_reverse_i32(int32_t val, uint8_t bits) {
    return drx_wrap_i32((int32_t)drx_bit_reverse_u32((uint32_t)val, bits), bits);
}

static inline uint64_t drx_bit_reverse_u64(uint64_t val, uint8_t bits) {
    uint64_t full_res;
#if drx_has_builtin(bitreverse64)
    full_res = __builtin_bitreverse64(val);
#else
    full_res = (uint64_t)drx_bit_reverse_u32((uint32_t)(val >>  0), 32) << 32 |
               (uint64_t)drx_bit_reverse_u32((uint32_t)(val >> 32), 32) >>  0;
#endif
    return drx_wrap_u64(full_res >> (64 - bits), bits);
}

static inline int64_t drx_bit_reverse_i64(int64_t val, uint8_t bits) {
    return drx_wrap_i64((int64_t)drx_bit_reverse_u64((uint64_t)val, bits), bits);
}

#define drx_builtin_popcount_common(w) \
    static inline uint8_t drx_popcount_i##w(int##w##_t val, uint8_t bits) { \
        return drx_popcount_u##w((uint##w##_t)val, bits); \
    }
#if drx_has_builtin(popcount) || defined(drx_gcc) || defined(drx_tinyc)
#define drx_builtin_popcount(w) \
    static inline uint8_t drx_popcount_u##w(uint##w##_t val, uint8_t bits) { \
        (void)bits; \
        return drx_builtin##w(popcount, val); \
    } \
\
    drx_builtin_popcount_common(w)
#else
#define drx_builtin_popcount(w) \
    static inline uint8_t drx_popcount_u##w(uint##w##_t val, uint8_t bits) { \
        (void)bits; \
        uint##w##_t temp = val - ((val >> 1) & (UINT##w##_MAX / 3)); \
        temp = (temp & (UINT##w##_MAX / 5)) + ((temp >> 2) & (UINT##w##_MAX / 5)); \
        temp = (temp + (temp >> 4)) & (UINT##w##_MAX / 17); \
        return temp * (UINT##w##_MAX / 255) >> (UINT8_C(w) - UINT8_C(8)); \
    } \
\
    drx_builtin_popcount_common(w)
#endif
drx_builtin_popcount(8)
drx_builtin_popcount(16)
drx_builtin_popcount(32)
drx_builtin_popcount(64)

#define drx_builtin_ctz_common(w) \
    static inline uint8_t drx_ctz_i##w(int##w##_t val, uint8_t bits) { \
        return drx_ctz_u##w((uint##w##_t)val, bits); \
    }
#if drx_has_builtin(ctz) || defined(drx_gcc) || defined(drx_tinyc)
#define drx_builtin_ctz(w) \
    static inline uint8_t drx_ctz_u##w(uint##w##_t val, uint8_t bits) { \
        if (val == 0) return bits; \
        return drx_builtin##w(ctz, val); \
    } \
\
    drx_builtin_ctz_common(w)
#else
#define drx_builtin_ctz(w) \
    static inline uint8_t drx_ctz_u##w(uint##w##_t val, uint8_t bits) { \
        return drx_popcount_u##w(drx_not_u##w(val, bits) & drx_subw_u##w(val, 1, bits), bits); \
    } \
\
    drx_builtin_ctz_common(w)
#endif
drx_builtin_ctz(8)
drx_builtin_ctz(16)
drx_builtin_ctz(32)
drx_builtin_ctz(64)

#define drx_builtin_clz_common(w) \
    static inline uint8_t drx_clz_i##w(int##w##_t val, uint8_t bits) { \
        return drx_clz_u##w((uint##w##_t)val, bits); \
    }
#if drx_has_builtin(clz) || defined(drx_gcc) || defined(drx_tinyc)
#define drx_builtin_clz(w) \
    static inline uint8_t drx_clz_u##w(uint##w##_t val, uint8_t bits) { \
        if (val == 0) return bits; \
        return drx_builtin##w(clz, val) - (drx_bitSizeOf(drx_Builtin##w) - bits); \
    } \
\
    drx_builtin_clz_common(w)
#else
#define drx_builtin_clz(w) \
    static inline uint8_t drx_clz_u##w(uint##w##_t val, uint8_t bits) { \
        return drx_ctz_u##w(drx_bit_reverse_u##w(val, bits), bits); \
    } \
\
    drx_builtin_clz_common(w)
#endif
drx_builtin_clz(8)
drx_builtin_clz(16)
drx_builtin_clz(32)
drx_builtin_clz(64)

/* ======================== 128-bit Integer Support ========================= */

#if !defined(drx_has_int128)
# if defined(__SIZEOF_INT128__)
#  define drx_has_int128 1
# else
#  define drx_has_int128 0
# endif
#endif

#if drx_has_int128

typedef unsigned __int128 drx_u128;
typedef   signed __int128 drx_i128;

#define drx_make_u128(hi, lo) ((drx_u128)(hi)<<64|(lo))
#define drx_make_i128(hi, lo) ((drx_i128)drx_make_u128(hi, lo))
#define drx_init_u128(hi, lo) drx_make_u128(hi, lo)
#define drx_init_i128(hi, lo) drx_make_i128(hi, lo)
#define drx_hi_u128(val) ((uint64_t)((val) >> 64))
#define drx_lo_u128(val) ((uint64_t)((val) >>  0))
#define drx_hi_i128(val) (( int64_t)((val) >> 64))
#define drx_lo_i128(val) ((uint64_t)((val) >>  0))
#define drx_bitCast_u128(val) ((drx_u128)(val))
#define drx_bitCast_i128(val) ((drx_i128)(val))
#define drx_cmp_int128(Type) \
    static inline int32_t drx_cmp_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return (lhs > rhs) - (lhs < rhs); \
    }
#define drx_bit_int128(Type, operation, operator) \
    static inline drx_##Type drx_##operation##_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return lhs operator rhs; \
    }

#else /* drx_has_int128 */

#if drx_little_endian
typedef struct { drx_align(16) uint64_t lo; uint64_t hi; } drx_u128;
typedef struct { drx_align(16) uint64_t lo;  int64_t hi; } drx_i128;
#else
typedef struct { drx_align(16) uint64_t hi; uint64_t lo; } drx_u128;
typedef struct { drx_align(16)  int64_t hi; uint64_t lo; } drx_i128;
#endif

#define drx_make_u128(hi, lo) ((drx_u128){ .h##i = (hi), .l##o = (lo) })
#define drx_make_i128(hi, lo) ((drx_i128){ .h##i = (hi), .l##o = (lo) })

#if defined(drx_msvc) /* MSVC doesn't allow struct literals in constant expressions */
#define drx_init_u128(hi, lo) { .h##i = (hi), .l##o = (lo) }
#define drx_init_i128(hi, lo) { .h##i = (hi), .l##o = (lo) }
#else /* But non-MSVC doesn't like the unprotected commas */
#define drx_init_u128(hi, lo) drx_make_u128(hi, lo)
#define drx_init_i128(hi, lo) drx_make_i128(hi, lo)
#endif
#define drx_hi_u128(val) ((val).hi)
#define drx_lo_u128(val) ((val).lo)
#define drx_hi_i128(val) ((val).hi)
#define drx_lo_i128(val) ((val).lo)
#define drx_bitCast_u128(val) drx_make_u128((uint64_t)(val).hi, (val).lo)
#define drx_bitCast_i128(val) drx_make_i128(( int64_t)(val).hi, (val).lo)
#define drx_cmp_int128(Type) \
    static inline int32_t drx_cmp_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return (lhs.hi == rhs.hi) \
            ? (lhs.lo > rhs.lo) - (lhs.lo < rhs.lo) \
            : (lhs.hi > rhs.hi) - (lhs.hi < rhs.hi); \
    }
#define drx_bit_int128(Type, operation, operator) \
    static inline drx_##Type drx_##operation##_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return (drx_##Type){ .hi = lhs.hi operator rhs.hi, .lo = lhs.lo operator rhs.lo }; \
    }

#endif /* drx_has_int128 */

#define drx_minInt_u128 drx_make_u128(drx_minInt_u64, drx_minInt_u64)
#define drx_maxInt_u128 drx_make_u128(drx_maxInt_u64, drx_maxInt_u64)
#define drx_minInt_i128 drx_make_i128(drx_minInt_i64, drx_minInt_u64)
#define drx_maxInt_i128 drx_make_i128(drx_maxInt_i64, drx_maxInt_u64)

drx_cmp_int128(u128)
drx_cmp_int128(i128)

drx_bit_int128(u128, and, &)
drx_bit_int128(i128, and, &)

drx_bit_int128(u128,  or, |)
drx_bit_int128(i128,  or, |)

drx_bit_int128(u128, xor, ^)
drx_bit_int128(i128, xor, ^)

static inline drx_u128 drx_shr_u128(drx_u128 lhs, uint8_t rhs);

#if drx_has_int128

static inline drx_u128 drx_not_u128(drx_u128 val, uint8_t bits) {
    return val ^ drx_maxInt_u(128, bits);
}

static inline drx_i128 drx_not_i128(drx_i128 val, uint8_t bits) {
    (void)bits;
    return ~val;
}

static inline drx_u128 drx_shr_u128(drx_u128 lhs, uint8_t rhs) {
    return lhs >> rhs;
}

static inline drx_u128 drx_shl_u128(drx_u128 lhs, uint8_t rhs) {
    return lhs << rhs;
}

static inline drx_i128 drx_shr_i128(drx_i128 lhs, uint8_t rhs) {
    drx_i128 sign_mask = lhs < drx_make_i128(0, 0) ? -drx_make_i128(0, 1) : drx_make_i128(0, 0);
    return ((lhs ^ sign_mask) >> rhs) ^ sign_mask;
}

static inline drx_i128 drx_shl_i128(drx_i128 lhs, uint8_t rhs) {
    return lhs << rhs;
}

static inline drx_u128 drx_add_u128(drx_u128 lhs, drx_u128 rhs) {
    return lhs + rhs;
}

static inline drx_i128 drx_add_i128(drx_i128 lhs, drx_i128 rhs) {
    return lhs + rhs;
}

static inline drx_u128 drx_sub_u128(drx_u128 lhs, drx_u128 rhs) {
    return lhs - rhs;
}

static inline drx_i128 drx_sub_i128(drx_i128 lhs, drx_i128 rhs) {
    return lhs - rhs;
}

static inline drx_u128 drx_mul_u128(drx_u128 lhs, drx_u128 rhs) {
    return lhs * rhs;
}

static inline drx_i128 drx_mul_i128(drx_i128 lhs, drx_i128 rhs) {
    return lhs * rhs;
}

static inline drx_u128 drx_div_trunc_u128(drx_u128 lhs, drx_u128 rhs) {
    return lhs / rhs;
}

static inline drx_i128 drx_div_trunc_i128(drx_i128 lhs, drx_i128 rhs) {
    return lhs / rhs;
}

static inline drx_u128 drx_rem_u128(drx_u128 lhs, drx_u128 rhs) {
    return lhs % rhs;
}

static inline drx_i128 drx_rem_i128(drx_i128 lhs, drx_i128 rhs) {
    return lhs % rhs;
}

#else /* drx_has_int128 */

static inline drx_u128 drx_not_u128(drx_u128 val, uint8_t bits) {
    return (drx_u128){ .hi = drx_not_u64(val.hi, bits - UINT8_C(64)), .lo = drx_not_u64(val.lo, UINT8_C(64)) };
}

static inline drx_i128 drx_not_i128(drx_i128 val, uint8_t bits) {
    return (drx_i128){ .hi = drx_not_i64(val.hi, bits - UINT8_C(64)), .lo = drx_not_u64(val.lo, UINT8_C(64)) };
}

static inline drx_u128 drx_shr_u128(drx_u128 lhs, uint8_t rhs) {
    if (rhs == UINT8_C(0)) return lhs;
    if (rhs >= UINT8_C(64)) return (drx_u128){ .hi = drx_minInt_u64, .lo = lhs.hi >> (rhs - UINT8_C(64)) };
    return (drx_u128){ .hi = lhs.hi >> rhs, .lo = lhs.hi << (UINT8_C(64) - rhs) | lhs.lo >> rhs };
}

static inline drx_u128 drx_shl_u128(drx_u128 lhs, uint8_t rhs) {
    if (rhs == UINT8_C(0)) return lhs;
    if (rhs >= UINT8_C(64)) return (drx_u128){ .hi = lhs.lo << (rhs - UINT8_C(64)), .lo = drx_minInt_u64 };
    return (drx_u128){ .hi = lhs.hi << rhs | lhs.lo >> (UINT8_C(64) - rhs), .lo = lhs.lo << rhs };
}

static inline drx_i128 drx_shr_i128(drx_i128 lhs, uint8_t rhs) {
    if (rhs == UINT8_C(0)) return lhs;
    if (rhs >= UINT8_C(64)) return (drx_i128){ .hi = drx_shr_i64(lhs.hi, 63), .lo = drx_shr_i64(lhs.hi, (rhs - UINT8_C(64))) };
    return (drx_i128){ .hi = drx_shr_i64(lhs.hi, rhs), .lo = lhs.lo >> rhs | (uint64_t)lhs.hi << (UINT8_C(64) - rhs) };
}

static inline drx_i128 drx_shl_i128(drx_i128 lhs, uint8_t rhs) {
    if (rhs == UINT8_C(0)) return lhs;
    if (rhs >= UINT8_C(64)) return (drx_i128){ .hi = lhs.lo << (rhs - UINT8_C(64)), .lo = drx_minInt_u64 };
    return (drx_i128){ .hi = lhs.hi << rhs | lhs.lo >> (UINT8_C(64) - rhs), .lo = lhs.lo << rhs };
}

static inline drx_u128 drx_add_u128(drx_u128 lhs, drx_u128 rhs) {
    drx_u128 res;
    res.hi = lhs.hi + rhs.hi + drx_addo_u64(&res.lo, lhs.lo, rhs.lo, 64);
    return res;
}

static inline drx_i128 drx_add_i128(drx_i128 lhs, drx_i128 rhs) {
    drx_i128 res;
    res.hi = lhs.hi + rhs.hi + drx_addo_u64(&res.lo, lhs.lo, rhs.lo, 64);
    return res;
}

static inline drx_u128 drx_sub_u128(drx_u128 lhs, drx_u128 rhs) {
    drx_u128 res;
    res.hi = lhs.hi - rhs.hi - drx_subo_u64(&res.lo, lhs.lo, rhs.lo, 64);
    return res;
}

static inline drx_i128 drx_sub_i128(drx_i128 lhs, drx_i128 rhs) {
    drx_i128 res;
    res.hi = lhs.hi - rhs.hi - drx_subo_u64(&res.lo, lhs.lo, rhs.lo, 64);
    return res;
}

drx_extern drx_i128 __multi3(drx_i128 lhs, drx_i128 rhs);
static drx_i128 drx_mul_i128(drx_i128 lhs, drx_i128 rhs) {
    return __multi3(lhs, rhs);
}

static drx_u128 drx_mul_u128(drx_u128 lhs, drx_u128 rhs) {
    return drx_bitCast_u128(drx_mul_i128(drx_bitCast_i128(lhs), drx_bitCast_i128(rhs)));
}

drx_extern drx_u128 __udivti3(drx_u128 lhs, drx_u128 rhs);
static drx_u128 drx_div_trunc_u128(drx_u128 lhs, drx_u128 rhs) {
    return __udivti3(lhs, rhs);
}

drx_extern drx_i128 __divti3(drx_i128 lhs, drx_i128 rhs);
static drx_i128 drx_div_trunc_i128(drx_i128 lhs, drx_i128 rhs) {
    return __divti3(lhs, rhs);
}

drx_extern drx_u128 __umodti3(drx_u128 lhs, drx_u128 rhs);
static drx_u128 drx_rem_u128(drx_u128 lhs, drx_u128 rhs) {
    return __umodti3(lhs, rhs);
}

drx_extern drx_i128 __modti3(drx_i128 lhs, drx_i128 rhs);
static drx_i128 drx_rem_i128(drx_i128 lhs, drx_i128 rhs) {
    return __modti3(lhs, rhs);
}

#endif /* drx_has_int128 */

#define drx_div_floor_u128 drx_div_trunc_u128

static inline drx_i128 drx_div_floor_i128(drx_i128 lhs, drx_i128 rhs) {
    drx_i128 rem = drx_rem_i128(lhs, rhs);
    int64_t mask = drx_or_u64((uint64_t)drx_hi_i128(rem), drx_lo_i128(rem)) != UINT64_C(0)
        ? drx_shr_i64(drx_xor_i64(drx_hi_i128(lhs), drx_hi_i128(rhs)), UINT8_C(63)) : INT64_C(0);
    return drx_add_i128(drx_div_trunc_i128(lhs, rhs), drx_make_i128(mask, (uint64_t)mask));
}

#define drx_mod_u128 drx_rem_u128

static inline drx_i128 drx_mod_i128(drx_i128 lhs, drx_i128 rhs) {
    drx_i128 rem = drx_rem_i128(lhs, rhs);
    int64_t mask = drx_or_u64((uint64_t)drx_hi_i128(rem), drx_lo_i128(rem)) != UINT64_C(0)
        ? drx_shr_i64(drx_xor_i64(drx_hi_i128(lhs), drx_hi_i128(rhs)), UINT8_C(63)) : INT64_C(0);
    return drx_add_i128(rem, drx_and_i128(rhs, drx_make_i128(mask, (uint64_t)mask)));
}

static inline drx_u128 drx_min_u128(drx_u128 lhs, drx_u128 rhs) {
    return drx_cmp_u128(lhs, rhs) < INT32_C(0) ? lhs : rhs;
}

static inline drx_i128 drx_min_i128(drx_i128 lhs, drx_i128 rhs) {
    return drx_cmp_i128(lhs, rhs) < INT32_C(0) ? lhs : rhs;
}

static inline drx_u128 drx_max_u128(drx_u128 lhs, drx_u128 rhs) {
    return drx_cmp_u128(lhs, rhs) > INT32_C(0) ? lhs : rhs;
}

static inline drx_i128 drx_max_i128(drx_i128 lhs, drx_i128 rhs) {
    return drx_cmp_i128(lhs, rhs) > INT32_C(0) ? lhs : rhs;
}

static inline drx_u128 drx_wrap_u128(drx_u128 val, uint8_t bits) {
    return drx_and_u128(val, drx_maxInt_u(128, bits));
}

static inline drx_i128 drx_wrap_i128(drx_i128 val, uint8_t bits) {
    if (bits > UINT8_C(64)) return drx_make_i128(drx_wrap_i64(drx_hi_i128(val), bits - UINT8_C(64)), drx_lo_i128(val));
    int64_t lo = drx_wrap_i64((int64_t)drx_lo_i128(val), bits);
    return drx_make_i128(drx_shr_i64(lo, 63), (uint64_t)lo);
}

static inline drx_u128 drx_shlw_u128(drx_u128 lhs, uint8_t rhs, uint8_t bits) {
    return drx_wrap_u128(drx_shl_u128(lhs, rhs), bits);
}

static inline drx_i128 drx_shlw_i128(drx_i128 lhs, uint8_t rhs, uint8_t bits) {
    return drx_wrap_i128(drx_bitCast_i128(drx_shl_u128(drx_bitCast_u128(lhs), rhs)), bits);
}

static inline drx_u128 drx_addw_u128(drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    return drx_wrap_u128(drx_add_u128(lhs, rhs), bits);
}

static inline drx_i128 drx_addw_i128(drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    return drx_wrap_i128(drx_bitCast_i128(drx_add_u128(drx_bitCast_u128(lhs), drx_bitCast_u128(rhs))), bits);
}

static inline drx_u128 drx_subw_u128(drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    return drx_wrap_u128(drx_sub_u128(lhs, rhs), bits);
}

static inline drx_i128 drx_subw_i128(drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    return drx_wrap_i128(drx_bitCast_i128(drx_sub_u128(drx_bitCast_u128(lhs), drx_bitCast_u128(rhs))), bits);
}

static inline drx_u128 drx_mulw_u128(drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    return drx_wrap_u128(drx_mul_u128(lhs, rhs), bits);
}

static inline drx_i128 drx_mulw_i128(drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    return drx_wrap_i128(drx_bitCast_i128(drx_mul_u128(drx_bitCast_u128(lhs), drx_bitCast_u128(rhs))), bits);
}

static inline drx_u128 drx_abs_i128(drx_i128 val) {
    drx_i128 tmp = drx_shr_i128(val, 127);
    return drx_bitCast_u128(drx_sub_i128(drx_xor_i128(val, tmp), tmp));
}

#if drx_has_int128

static inline bool drx_addo_u128(drx_u128 *res, drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow)
    drx_u128 full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u128(full_res, bits);
    return overflow || full_res < drx_minInt_u(128, bits) || full_res > drx_maxInt_u(128, bits);
#else
    *res = drx_addw_u128(lhs, rhs, bits);
    return *res < lhs;
#endif
}

drx_extern drx_i128  __addoti4(drx_i128 lhs, drx_i128 rhs, int *overflow);
static inline bool drx_addo_i128(drx_i128 *res, drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
#if drx_has_builtin(add_overflow)
    drx_i128 full_res;
    bool overflow = __builtin_add_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    drx_i128 full_res =  __addoti4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i128(full_res, bits);
    return overflow || full_res < drx_minInt_i(128, bits) || full_res > drx_maxInt_i(128, bits);
}

static inline bool drx_subo_u128(drx_u128 *res, drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow)
    drx_u128 full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u128(full_res, bits);
    return overflow || full_res < drx_minInt_u(128, bits) || full_res > drx_maxInt_u(128, bits);
#else
    *res = drx_subw_u128(lhs, rhs, bits);
    return *res > lhs;
#endif
}

drx_extern drx_i128  __suboti4(drx_i128 lhs, drx_i128 rhs, int *overflow);
static inline bool drx_subo_i128(drx_i128 *res, drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
#if drx_has_builtin(sub_overflow)
    drx_i128 full_res;
    bool overflow = __builtin_sub_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    drx_i128 full_res = __suboti4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i128(full_res, bits);
    return overflow || full_res < drx_minInt_i(128, bits) || full_res > drx_maxInt_i(128, bits);
}

static inline bool drx_mulo_u128(drx_u128 *res, drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow)
    drx_u128 full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
    *res = drx_wrap_u128(full_res, bits);
    return overflow || full_res < drx_minInt_u(128, bits) || full_res > drx_maxInt_u(128, bits);
#else
    *res = drx_mulw_u128(lhs, rhs, bits);
    return rhs != drx_make_u128(0, 0) && lhs > drx_maxInt_u(128, bits) / rhs;
#endif
}

drx_extern drx_i128  __muloti4(drx_i128 lhs, drx_i128 rhs, int *overflow);
static inline bool drx_mulo_i128(drx_i128 *res, drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
#if drx_has_builtin(mul_overflow)
    drx_i128 full_res;
    bool overflow = __builtin_mul_overflow(lhs, rhs, &full_res);
#else
    int overflow_int;
    drx_i128 full_res =  __muloti4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0;
#endif
    *res = drx_wrap_i128(full_res, bits);
    return overflow || full_res < drx_minInt_i(128, bits) || full_res > drx_maxInt_i(128, bits);
}

#else /* drx_has_int128 */

static inline bool drx_addo_u128(drx_u128 *res, drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    uint64_t hi;
    bool overflow = drx_addo_u64(&hi, lhs.hi, rhs.hi, bits - 64);
    return overflow ^ drx_addo_u64(&res->hi, hi, drx_addo_u64(&res->lo, lhs.lo, rhs.lo, 64), bits - 64);
}

static inline bool drx_addo_i128(drx_i128 *res, drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    int64_t hi;
    bool overflow = drx_addo_i64(&hi, lhs.hi, rhs.hi, bits - 64);
    return overflow ^ drx_addo_i64(&res->hi, hi, drx_addo_u64(&res->lo, lhs.lo, rhs.lo, 64), bits - 64);
}

static inline bool drx_subo_u128(drx_u128 *res, drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    uint64_t hi;
    bool overflow = drx_subo_u64(&hi, lhs.hi, rhs.hi, bits - 64);
    return overflow ^ drx_subo_u64(&res->hi, hi, drx_subo_u64(&res->lo, lhs.lo, rhs.lo, 64), bits - 64);
}

static inline bool drx_subo_i128(drx_i128 *res, drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    int64_t hi;
    bool overflow = drx_subo_i64(&hi, lhs.hi, rhs.hi, bits - 64);
    return overflow ^ drx_subo_i64(&res->hi, hi, drx_subo_u64(&res->lo, lhs.lo, rhs.lo, 64), bits - 64);
}

static inline bool drx_mulo_u128(drx_u128 *res, drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    *res = drx_mulw_u128(lhs, rhs, bits);
    return drx_cmp_u128(*res, drx_make_u128(0, 0)) != INT32_C(0) &&
        drx_cmp_u128(lhs, drx_div_trunc_u128(drx_maxInt_u(128, bits), rhs)) > INT32_C(0);
}

drx_extern drx_i128 __muloti4(drx_i128 lhs, drx_i128 rhs, int *overflow);
static inline bool drx_mulo_i128(drx_i128 *res, drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    int overflow_int;
    drx_i128 full_res = __muloti4(lhs, rhs, &overflow_int);
    bool overflow = overflow_int != 0 ||
        drx_cmp_i128(full_res, drx_minInt_i(128, bits)) < INT32_C(0) ||
        drx_cmp_i128(full_res, drx_maxInt_i(128, bits)) > INT32_C(0);
    *res = drx_wrap_i128(full_res, bits);
    return overflow;
}

#endif /* drx_has_int128 */

static inline bool drx_shlo_u128(drx_u128 *res, drx_u128 lhs, uint8_t rhs, uint8_t bits) {
    *res = drx_shlw_u128(lhs, rhs, bits);
    return drx_cmp_u128(lhs, drx_shr_u128(drx_maxInt_u(128, bits), rhs)) > INT32_C(0);
}

static inline bool drx_shlo_i128(drx_i128 *res, drx_i128 lhs, uint8_t rhs, uint8_t bits) {
    *res = drx_shlw_i128(lhs, rhs, bits);
    drx_i128 mask = drx_bitCast_i128(drx_shl_u128(drx_maxInt_u128, bits - rhs - UINT8_C(1)));
    return drx_cmp_i128(drx_and_i128(lhs, mask), drx_make_i128(0, 0)) != INT32_C(0) &&
           drx_cmp_i128(drx_and_i128(lhs, mask), mask) != INT32_C(0);
}

static inline drx_u128 drx_shls_u128(drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    drx_u128 res;
    if (drx_cmp_u128(rhs, drx_make_u128(0, bits)) >= INT32_C(0))
        return drx_cmp_u128(lhs, drx_make_u128(0, 0)) != INT32_C(0) ? drx_maxInt_u(128, bits) : lhs;
    return drx_shlo_u128(&res, lhs, (uint8_t)drx_lo_u128(rhs), bits) ? drx_maxInt_u(128, bits) : res;
}

static inline drx_i128 drx_shls_i128(drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    drx_i128 res;
    if (drx_cmp_u128(drx_bitCast_u128(rhs), drx_make_u128(0, bits)) < INT32_C(0) && !drx_shlo_i128(&res, lhs, (uint8_t)drx_lo_i128(rhs), bits)) return res;
    return drx_cmp_i128(lhs, drx_make_i128(0, 0)) < INT32_C(0) ? drx_minInt_i(128, bits) : drx_maxInt_i(128, bits);
}

static inline drx_u128 drx_adds_u128(drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    drx_u128 res;
    return drx_addo_u128(&res, lhs, rhs, bits) ? drx_maxInt_u(128, bits) : res;
}

static inline drx_i128 drx_adds_i128(drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    drx_i128 res;
    if (!drx_addo_i128(&res, lhs, rhs, bits)) return res;
    return drx_cmp_i128(res, drx_make_i128(0, 0)) >= INT32_C(0) ? drx_minInt_i(128, bits) : drx_maxInt_i(128, bits);
}

static inline drx_u128 drx_subs_u128(drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    drx_u128 res;
    return drx_subo_u128(&res, lhs, rhs, bits) ? drx_minInt_u(128, bits) : res;
}

static inline drx_i128 drx_subs_i128(drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    drx_i128 res;
    if (!drx_subo_i128(&res, lhs, rhs, bits)) return res;
    return drx_cmp_i128(res, drx_make_i128(0, 0)) >= INT32_C(0) ? drx_minInt_i(128, bits) : drx_maxInt_i(128, bits);
}

static inline drx_u128 drx_muls_u128(drx_u128 lhs, drx_u128 rhs, uint8_t bits) {
    drx_u128 res;
    return drx_mulo_u128(&res, lhs, rhs, bits) ? drx_maxInt_u(128, bits) : res;
}

static inline drx_i128 drx_muls_i128(drx_i128 lhs, drx_i128 rhs, uint8_t bits) {
    drx_i128 res;
    if (!drx_mulo_i128(&res, lhs, rhs, bits)) return res;
    return drx_cmp_i128(drx_xor_i128(lhs, rhs), drx_make_i128(0, 0)) < INT32_C(0) ? drx_minInt_i(128, bits) : drx_maxInt_i(128, bits);
}

static inline uint8_t drx_clz_u128(drx_u128 val, uint8_t bits) {
    if (bits <= UINT8_C(64)) return drx_clz_u64(drx_lo_u128(val), bits);
    if (drx_hi_u128(val) != 0) return drx_clz_u64(drx_hi_u128(val), bits - UINT8_C(64));
    return drx_clz_u64(drx_lo_u128(val), UINT8_C(64)) + (bits - UINT8_C(64));
}

static inline uint8_t drx_clz_i128(drx_i128 val, uint8_t bits) {
    return drx_clz_u128(drx_bitCast_u128(val), bits);
}

static inline uint8_t drx_ctz_u128(drx_u128 val, uint8_t bits) {
    if (drx_lo_u128(val) != 0) return drx_ctz_u64(drx_lo_u128(val), UINT8_C(64));
    return drx_ctz_u64(drx_hi_u128(val), bits - UINT8_C(64)) + UINT8_C(64);
}

static inline uint8_t drx_ctz_i128(drx_i128 val, uint8_t bits) {
    return drx_ctz_u128(drx_bitCast_u128(val), bits);
}

static inline uint8_t drx_popcount_u128(drx_u128 val, uint8_t bits) {
    return drx_popcount_u64(drx_hi_u128(val), bits - UINT8_C(64)) +
           drx_popcount_u64(drx_lo_u128(val), UINT8_C(64));
}

static inline uint8_t drx_popcount_i128(drx_i128 val, uint8_t bits) {
    return drx_popcount_u128(drx_bitCast_u128(val), bits);
}

static inline drx_u128 drx_byte_swap_u128(drx_u128 val, uint8_t bits) {
    drx_u128 full_res;
#if drx_has_builtin(bswap128)
    full_res = __builtin_bswap128(val);
#else
    full_res = drx_make_u128(drx_byte_swap_u64(drx_lo_u128(val), UINT8_C(64)),
                           drx_byte_swap_u64(drx_hi_u128(val), UINT8_C(64)));
#endif
    return drx_shr_u128(full_res, UINT8_C(128) - bits);
}

static inline drx_i128 drx_byte_swap_i128(drx_i128 val, uint8_t bits) {
    return drx_bitCast_i128(drx_byte_swap_u128(drx_bitCast_u128(val), bits));
}

static inline drx_u128 drx_bit_reverse_u128(drx_u128 val, uint8_t bits) {
    return drx_shr_u128(drx_make_u128(drx_bit_reverse_u64(drx_lo_u128(val), UINT8_C(64)),
                                    drx_bit_reverse_u64(drx_hi_u128(val), UINT8_C(64))),
                        UINT8_C(128) - bits);
}

static inline drx_i128 drx_bit_reverse_i128(drx_i128 val, uint8_t bits) {
    return drx_bitCast_i128(drx_bit_reverse_u128(drx_bitCast_u128(val), bits));
}
////////////////////////////////////////
//#ifndef __cplusplus
///////////////////////////////////////
/* ========================== Big Integer Support =========================== */

/*
static inline uint16_t drx_int_bytes(uint16_t bits) {
    uint16_t bytes = (bits + CHAR_BIT - 1) / CHAR_BIT;
    uint16_t alignment = DRX_TARGET_MAX_INT_ALIGNMENT;
    while (alignment / 2 >= bytes) alignment /= 2;
    return (bytes + alignment - 1) / alignment * alignment;
}

static inline int32_t drx_cmp_big(const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    const uint8_t *lhs_bytes = lhs;
    const uint8_t *rhs_bytes = rhs;
    uint16_t byte_offset = 0;
    bool do_signed = is_signed;
    uint16_t remaining_bytes = drx_int_bytes(bits);

#if drx_little_endian
    byte_offset = remaining_bytes;
#endif

    while (remaining_bytes >= 128 / CHAR_BIT) {
        int32_t limb_cmp;

#if drx_little_endian
        byte_offset -= 128 / CHAR_BIT;
#endif

        if (do_signed) {
            drx_i128 lhs_limb;
            drx_i128 rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_cmp = drx_cmp_i128(lhs_limb, rhs_limb);
            do_signed = false;
        } else {
            drx_u128 lhs_limb;
            drx_u128 rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_cmp = drx_cmp_u128(lhs_limb, rhs_limb);
        }

        if (limb_cmp != 0) return limb_cmp;
        remaining_bytes -= 128 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 128 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 64 / CHAR_BIT;
#endif

        if (do_signed) {
            int64_t lhs_limb;
            int64_t rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            if (lhs_limb != rhs_limb) return (lhs_limb > rhs_limb) - (lhs_limb < rhs_limb);
            do_signed = false;
        } else {
            uint64_t lhs_limb;
            uint64_t rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            if (lhs_limb != rhs_limb) return (lhs_limb > rhs_limb) - (lhs_limb < rhs_limb);
        }

        remaining_bytes -= 64 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 64 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 32 / CHAR_BIT;
#endif

        if (do_signed) {
            int32_t lhs_limb;
            int32_t rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            if (lhs_limb != rhs_limb) return (lhs_limb > rhs_limb) - (lhs_limb < rhs_limb);
            do_signed = false;
        } else {
            uint32_t lhs_limb;
            uint32_t rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            if (lhs_limb != rhs_limb) return (lhs_limb > rhs_limb) - (lhs_limb < rhs_limb);
        }

        remaining_bytes -= 32 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 32 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 16 / CHAR_BIT;
#endif

        if (do_signed) {
            int16_t lhs_limb;
            int16_t rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            if (lhs_limb != rhs_limb) return (lhs_limb > rhs_limb) - (lhs_limb < rhs_limb);
            do_signed = false;
        } else {
            uint16_t lhs_limb;
            uint16_t rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            if (lhs_limb != rhs_limb) return (lhs_limb > rhs_limb) - (lhs_limb < rhs_limb);
        }

        remaining_bytes -= 16 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 16 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 8 / CHAR_BIT;
#endif

        if (do_signed) {
            int8_t lhs_limb;
            int8_t rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            if (lhs_limb != rhs_limb) return (lhs_limb > rhs_limb) - (lhs_limb < rhs_limb);
            do_signed = false;
        } else {
            uint8_t lhs_limb;
            uint8_t rhs_limb;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            if (lhs_limb != rhs_limb) return (lhs_limb > rhs_limb) - (lhs_limb < rhs_limb);
        }

        remaining_bytes -= 8 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 8 / CHAR_BIT;
#endif
    }

    return 0;
}

static inline void drx_and_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    uint8_t *res_bytes = res;
    const uint8_t *lhs_bytes = lhs;
    const uint8_t *rhs_bytes = rhs;
    uint16_t byte_offset = 0;
    uint16_t remaining_bytes = drx_int_bytes(bits);
    (void)is_signed;

    while (remaining_bytes >= 128 / CHAR_BIT) {
        drx_u128 res_limb;
        drx_u128 lhs_limb;
        drx_u128 rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_and_u128(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 128 / CHAR_BIT;
        byte_offset += 128 / CHAR_BIT;
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
        uint64_t res_limb;
        uint64_t lhs_limb;
        uint64_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_and_u64(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 64 / CHAR_BIT;
        byte_offset += 64 / CHAR_BIT;
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
        uint32_t res_limb;
        uint32_t lhs_limb;
        uint32_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_and_u32(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 32 / CHAR_BIT;
        byte_offset += 32 / CHAR_BIT;
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
        uint16_t res_limb;
        uint16_t lhs_limb;
        uint16_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_and_u16(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 16 / CHAR_BIT;
        byte_offset += 16 / CHAR_BIT;
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
        uint8_t res_limb;
        uint8_t lhs_limb;
        uint8_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_and_u8(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 8 / CHAR_BIT;
        byte_offset += 8 / CHAR_BIT;
    }
}

static inline void drx_or_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    uint8_t *res_bytes = res;
    const uint8_t *lhs_bytes = lhs;
    const uint8_t *rhs_bytes = rhs;
    uint16_t byte_offset = 0;
    uint16_t remaining_bytes = drx_int_bytes(bits);
    (void)is_signed;

    while (remaining_bytes >= 128 / CHAR_BIT) {
        drx_u128 res_limb;
        drx_u128 lhs_limb;
        drx_u128 rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_or_u128(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 128 / CHAR_BIT;
        byte_offset += 128 / CHAR_BIT;
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
        uint64_t res_limb;
        uint64_t lhs_limb;
        uint64_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_or_u64(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 64 / CHAR_BIT;
        byte_offset += 64 / CHAR_BIT;
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
        uint32_t res_limb;
        uint32_t lhs_limb;
        uint32_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_or_u32(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 32 / CHAR_BIT;
        byte_offset += 32 / CHAR_BIT;
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
        uint16_t res_limb;
        uint16_t lhs_limb;
        uint16_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_or_u16(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 16 / CHAR_BIT;
        byte_offset += 16 / CHAR_BIT;
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
        uint8_t res_limb;
        uint8_t lhs_limb;
        uint8_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_or_u8(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 8 / CHAR_BIT;
        byte_offset += 8 / CHAR_BIT;
    }
}

static inline void drx_xor_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    uint8_t *res_bytes = res;
    const uint8_t *lhs_bytes = lhs;
    const uint8_t *rhs_bytes = rhs;
    uint16_t byte_offset = 0;
    uint16_t remaining_bytes = drx_int_bytes(bits);
    (void)is_signed;

    while (remaining_bytes >= 128 / CHAR_BIT) {
        drx_u128 res_limb;
        drx_u128 lhs_limb;
        drx_u128 rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_xor_u128(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 128 / CHAR_BIT;
        byte_offset += 128 / CHAR_BIT;
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
        uint64_t res_limb;
        uint64_t lhs_limb;
        uint64_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_xor_u64(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 64 / CHAR_BIT;
        byte_offset += 64 / CHAR_BIT;
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
        uint32_t res_limb;
        uint32_t lhs_limb;
        uint32_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_xor_u32(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 32 / CHAR_BIT;
        byte_offset += 32 / CHAR_BIT;
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
        uint16_t res_limb;
        uint16_t lhs_limb;
        uint16_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_xor_u16(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 16 / CHAR_BIT;
        byte_offset += 16 / CHAR_BIT;
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
        uint8_t res_limb;
        uint8_t lhs_limb;
        uint8_t rhs_limb;

        memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
        memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
        res_limb = drx_xor_u8(lhs_limb, rhs_limb);
        memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));

        remaining_bytes -= 8 / CHAR_BIT;
        byte_offset += 8 / CHAR_BIT;
    }
}

static inline bool drx_addo_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    uint8_t *res_bytes = res;
    const uint8_t *lhs_bytes = lhs;
    const uint8_t *rhs_bytes = rhs;
    uint16_t byte_offset = 0;
    uint16_t remaining_bytes = drx_int_bytes(bits);
    uint8_t top_bits = (uint8_t)(remaining_bytes * 8 - bits);
    bool overflow = false;

#if drx_big_endian
    byte_offset = remaining_bytes;
#endif

    while (remaining_bytes >= 128 / CHAR_BIT) {
        uint8_t limb_bits = 128 - (remaining_bytes == 128 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 128 / CHAR_BIT;
#endif

        if (remaining_bytes == 128 / CHAR_BIT && is_signed) {
            drx_i128 res_limb;
            drx_i128 tmp_limb;
            drx_i128 lhs_limb;
            drx_i128 rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_i128(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_i128(&res_limb, tmp_limb, drx_make_i128(INT64_C(0), overflow ? UINT64_C(1) : UINT64_C(0)), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            drx_u128 res_limb;
            drx_u128 tmp_limb;
            drx_u128 lhs_limb;
            drx_u128 rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_u128(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_u128(&res_limb, tmp_limb, drx_make_u128(UINT64_C(0), overflow ? UINT64_C(1) : UINT64_C(0)), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 128 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 128 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
        uint8_t limb_bits = 64 - (remaining_bytes == 64 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 64 / CHAR_BIT;
#endif

        if (remaining_bytes == 64 / CHAR_BIT && is_signed) {
            int64_t res_limb;
            int64_t tmp_limb;
            int64_t lhs_limb;
            int64_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_i64(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_i64(&res_limb, tmp_limb, overflow ? INT64_C(1) : INT64_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            uint64_t res_limb;
            uint64_t tmp_limb;
            uint64_t lhs_limb;
            uint64_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_u64(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_u64(&res_limb, tmp_limb, overflow ? UINT64_C(1) : UINT64_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 64 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 64 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
        uint8_t limb_bits = 32 - (remaining_bytes == 32 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 32 / CHAR_BIT;
#endif

        if (remaining_bytes == 32 / CHAR_BIT && is_signed) {
            int32_t res_limb;
            int32_t tmp_limb;
            int32_t lhs_limb;
            int32_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_i32(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_i32(&res_limb, tmp_limb, overflow ? INT32_C(1) : INT32_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            uint32_t res_limb;
            uint32_t tmp_limb;
            uint32_t lhs_limb;
            uint32_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_u32(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_u32(&res_limb, tmp_limb, overflow ? UINT32_C(1) : UINT32_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 32 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 32 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
        uint8_t limb_bits = 16 - (remaining_bytes == 16 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 16 / CHAR_BIT;
#endif

        if (remaining_bytes == 16 / CHAR_BIT && is_signed) {
            int16_t res_limb;
            int16_t tmp_limb;
            int16_t lhs_limb;
            int16_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_i16(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_i16(&res_limb, tmp_limb, overflow ? INT16_C(1) : INT16_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            uint16_t res_limb;
            uint16_t tmp_limb;
            uint16_t lhs_limb;
            uint16_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_u16(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_u16(&res_limb, tmp_limb, overflow ? UINT16_C(1) : UINT16_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 16 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 16 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
        uint8_t limb_bits = 8 - (remaining_bytes == 8 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 8 / CHAR_BIT;
#endif

        if (remaining_bytes == 8 / CHAR_BIT && is_signed) {
            int8_t res_limb;
            int8_t tmp_limb;
            int8_t lhs_limb;
            int8_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_i8(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_i8(&res_limb, tmp_limb, overflow ? INT8_C(1) : INT8_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            uint8_t res_limb;
            uint8_t tmp_limb;
            uint8_t lhs_limb;
            uint8_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_addo_u8(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_addo_u8(&res_limb, tmp_limb, overflow ? UINT8_C(1) : UINT8_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 8 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 8 / CHAR_BIT;
#endif
    }

    return overflow;
}

static inline bool drx_subo_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    uint8_t *res_bytes = res;
    const uint8_t *lhs_bytes = lhs;
    const uint8_t *rhs_bytes = rhs;
    uint16_t byte_offset = 0;
    uint16_t remaining_bytes = drx_int_bytes(bits);
    uint8_t top_bits = (uint8_t)(remaining_bytes * 8 - bits);
    bool overflow = false;

#if drx_big_endian
    byte_offset = remaining_bytes;
#endif

    while (remaining_bytes >= 128 / CHAR_BIT) {
        uint8_t limb_bits = 128 - (remaining_bytes == 128 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 128 / CHAR_BIT;
#endif

        if (remaining_bytes == 128 / CHAR_BIT && is_signed) {
            drx_i128 res_limb;
            drx_i128 tmp_limb;
            drx_i128 lhs_limb;
            drx_i128 rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_i128(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_i128(&res_limb, tmp_limb, drx_make_i128(INT64_C(0), overflow ? UINT64_C(1) : UINT64_C(0)), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            drx_u128 res_limb;
            drx_u128 tmp_limb;
            drx_u128 lhs_limb;
            drx_u128 rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_u128(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_u128(&res_limb, tmp_limb, drx_make_u128(UINT64_C(0), overflow ? UINT64_C(1) : UINT64_C(0)), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 128 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 128 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
        uint8_t limb_bits = 64 - (remaining_bytes == 64 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 64 / CHAR_BIT;
#endif

        if (remaining_bytes == 64 / CHAR_BIT && is_signed) {
            int64_t res_limb;
            int64_t tmp_limb;
            int64_t lhs_limb;
            int64_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_i64(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_i64(&res_limb, tmp_limb, overflow ? INT64_C(1) : INT64_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            uint64_t res_limb;
            uint64_t tmp_limb;
            uint64_t lhs_limb;
            uint64_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_u64(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_u64(&res_limb, tmp_limb, overflow ? UINT64_C(1) : UINT64_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 64 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 64 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
        uint8_t limb_bits = 32 - (remaining_bytes == 32 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 32 / CHAR_BIT;
#endif

        if (remaining_bytes == 32 / CHAR_BIT && is_signed) {
            int32_t res_limb;
            int32_t tmp_limb;
            int32_t lhs_limb;
            int32_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_i32(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_i32(&res_limb, tmp_limb, overflow ? INT32_C(1) : INT32_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            uint32_t res_limb;
            uint32_t tmp_limb;
            uint32_t lhs_limb;
            uint32_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_u32(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_u32(&res_limb, tmp_limb, overflow ? UINT32_C(1) : UINT32_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 32 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 32 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
        uint8_t limb_bits = 16 - (remaining_bytes == 16 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 16 / CHAR_BIT;
#endif

        if (remaining_bytes == 16 / CHAR_BIT && is_signed) {
            int16_t res_limb;
            int16_t tmp_limb;
            int16_t lhs_limb;
            int16_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_i16(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_i16(&res_limb, tmp_limb, overflow ? INT16_C(1) : INT16_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            uint16_t res_limb;
            uint16_t tmp_limb;
            uint16_t lhs_limb;
            uint16_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_u16(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_u16(&res_limb, tmp_limb, overflow ? UINT16_C(1) : UINT16_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 16 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 16 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
        uint8_t limb_bits = 8 - (remaining_bytes == 8 / CHAR_BIT ? top_bits : 0);

#if drx_big_endian
        byte_offset -= 8 / CHAR_BIT;
#endif

        if (remaining_bytes == 8 / CHAR_BIT && is_signed) {
            int8_t res_limb;
            int8_t tmp_limb;
            int8_t lhs_limb;
            int8_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_i8(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_i8(&res_limb, tmp_limb, overflow ? INT8_C(1) : INT8_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        } else {
            uint8_t res_limb;
            uint8_t tmp_limb;
            uint8_t lhs_limb;
            uint8_t rhs_limb;
            bool limb_overflow;

            memcpy(&lhs_limb, &lhs_bytes[byte_offset], sizeof(lhs_limb));
            memcpy(&rhs_limb, &rhs_bytes[byte_offset], sizeof(rhs_limb));
            limb_overflow = drx_subo_u8(&tmp_limb, lhs_limb, rhs_limb, limb_bits);
            overflow = limb_overflow ^ drx_subo_u8(&res_limb, tmp_limb, overflow ? UINT8_C(1) : UINT8_C(0), limb_bits);
            memcpy(&res_bytes[byte_offset], &res_limb, sizeof(res_limb));
        }

        remaining_bytes -= 8 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 8 / CHAR_BIT;
#endif
    }

    return overflow;
}

static inline void drx_addw_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    (void)drx_addo_big(res, lhs, rhs, is_signed, bits);
}

static inline void drx_subw_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    (void)drx_subo_big(res, lhs, rhs, is_signed, bits);
}

drx_extern void __udivei4(uint32_t *res, const uint32_t *lhs, const uint32_t *rhs, uintptr_t bits);
static inline void drx_div_trunc_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    if (!is_signed) {
        __udivei4(res, lhs, rhs, bits);
        return;
    }

    drx_trap();
}

static inline void drx_div_floor_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    if (!is_signed) {
        drx_div_trunc_big(res, lhs, rhs, is_signed, bits);
        return;
    }

    drx_trap();
}

drx_extern void __umodei4(uint32_t *res, const uint32_t *lhs, const uint32_t *rhs, uintptr_t bits);
static inline void drx_rem_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    if (!is_signed) {
        __umodei4(res, lhs, rhs, bits);
        return;
    }

    drx_trap();
}

static inline void drx_mod_big(void *res, const void *lhs, const void *rhs, bool is_signed, uint16_t bits) {
    if (!is_signed) {
        drx_rem_big(res, lhs, rhs, is_signed, bits);
        return;
    }

    drx_trap();
}

static inline uint16_t drx_clz_big(const void *val, bool is_signed, uint16_t bits) {
    const uint8_t *val_bytes = val;
    uint16_t byte_offset = 0;
    uint16_t remaining_bytes = drx_int_bytes(bits);
    uint16_t skip_bits = remaining_bytes * 8 - bits;
    uint16_t total_lz = 0;
    uint16_t limb_lz;
    (void)is_signed;

#if drx_little_endian
    byte_offset = remaining_bytes;
#endif

    while (remaining_bytes >= 128 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 128 / CHAR_BIT;
#endif

        {
            drx_u128 val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_lz = drx_clz_u128(val_limb, 128 - skip_bits);
        }

        total_lz += limb_lz;
        if (limb_lz < 128 - skip_bits) return total_lz;
        skip_bits = 0;
        remaining_bytes -= 128 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 128 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 64 / CHAR_BIT;
#endif

        {
            uint64_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_lz = drx_clz_u64(val_limb, 64 - skip_bits);
        }

        total_lz += limb_lz;
        if (limb_lz < 64 - skip_bits) return total_lz;
        skip_bits = 0;
        remaining_bytes -= 64 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 64 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 32 / CHAR_BIT;
#endif

        {
            uint32_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_lz = drx_clz_u32(val_limb, 32 - skip_bits);
        }

        total_lz += limb_lz;
        if (limb_lz < 32 - skip_bits) return total_lz;
        skip_bits = 0;
        remaining_bytes -= 32 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 32 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 16 / CHAR_BIT;
#endif

        {
            uint16_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_lz = drx_clz_u16(val_limb, 16 - skip_bits);
        }

        total_lz += limb_lz;
        if (limb_lz < 16 - skip_bits) return total_lz;
        skip_bits = 0;
        remaining_bytes -= 16 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 16 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
#if drx_little_endian
        byte_offset -= 8 / CHAR_BIT;
#endif

        {
            uint8_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_lz = drx_clz_u8(val_limb, 8 - skip_bits);
        }

        total_lz += limb_lz;
        if (limb_lz < 8 - skip_bits) return total_lz;
        skip_bits = 0;
        remaining_bytes -= 8 / CHAR_BIT;

#if drx_big_endian
        byte_offset += 8 / CHAR_BIT;
#endif
    }

    return total_lz;
}

static inline uint16_t drx_ctz_big(const void *val, bool is_signed, uint16_t bits) {
    const uint8_t *val_bytes = val;
    uint16_t byte_offset = 0;
    uint16_t remaining_bytes = drx_int_bytes(bits);
    uint16_t total_tz = 0;
    uint16_t limb_tz;
    (void)is_signed;

#if drx_big_endian
    byte_offset = remaining_bytes;
#endif

    while (remaining_bytes >= 128 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 128 / CHAR_BIT;
#endif

        {
            drx_u128 val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_tz = drx_ctz_u128(val_limb, 128);
        }

        total_tz += limb_tz;
        if (limb_tz < 128) return total_tz;
        remaining_bytes -= 128 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 128 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 64 / CHAR_BIT;
#endif

        {
            uint64_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_tz = drx_ctz_u64(val_limb, 64);
        }

        total_tz += limb_tz;
        if (limb_tz < 64) return total_tz;
        remaining_bytes -= 64 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 64 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 32 / CHAR_BIT;
#endif

        {
            uint32_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_tz = drx_ctz_u32(val_limb, 32);
        }

        total_tz += limb_tz;
        if (limb_tz < 32) return total_tz;
        remaining_bytes -= 32 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 32 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 16 / CHAR_BIT;
#endif

        {
            uint16_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_tz = drx_ctz_u16(val_limb, 16);
        }

        total_tz += limb_tz;
        if (limb_tz < 16) return total_tz;
        remaining_bytes -= 16 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 16 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 8 / CHAR_BIT;
#endif

        {
            uint8_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            limb_tz = drx_ctz_u8(val_limb, 8);
        }

        total_tz += limb_tz;
        if (limb_tz < 8) return total_tz;
        remaining_bytes -= 8 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 8 / CHAR_BIT;
#endif
    }

    return total_tz;
}

static inline uint16_t drx_popcount_big(const void *val, bool is_signed, uint16_t bits) {
    const uint8_t *val_bytes = val;
    uint16_t byte_offset = 0;
    uint16_t remaining_bytes = drx_int_bytes(bits);
    uint16_t total_pc = 0;
    (void)is_signed;

#if drx_big_endian
    byte_offset = remaining_bytes;
#endif

    while (remaining_bytes >= 128 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 128 / CHAR_BIT;
#endif

        {
            drx_u128 val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            total_pc += drx_popcount_u128(val_limb, 128);
        }

        remaining_bytes -= 128 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 128 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 64 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 64 / CHAR_BIT;
#endif

        {
            uint64_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            total_pc += drx_popcount_u64(val_limb, 64);
        }

        remaining_bytes -= 64 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 64 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 32 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 32 / CHAR_BIT;
#endif

        {
            uint32_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            total_pc += drx_popcount_u32(val_limb, 32);
        }

        remaining_bytes -= 32 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 32 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 16 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 16 / CHAR_BIT;
#endif

        {
            uint16_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            total_pc = drx_popcount_u16(val_limb, 16);
        }

        remaining_bytes -= 16 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 16 / CHAR_BIT;
#endif
    }

    while (remaining_bytes >= 8 / CHAR_BIT) {
#if drx_big_endian
        byte_offset -= 8 / CHAR_BIT;
#endif

        {
            uint8_t val_limb;

            memcpy(&val_limb, &val_bytes[byte_offset], sizeof(val_limb));
            total_pc = drx_popcount_u8(val_limb, 8);
        }

        remaining_bytes -= 8 / CHAR_BIT;

#if drx_little_endian
        byte_offset += 8 / CHAR_BIT;
#endif
    }

    return total_pc;
}
*/
/* ========================= Floating Point Support ========================= */

#ifndef __STDC_WANT_IEC_60559_TYPES_EXT__
#define __STDC_WANT_IEC_60559_TYPES_EXT__
#endif

#include <float.h>

#if defined(drx_msvc)
float __cdecl nanf(char const* input);
double __cdecl nan(char const* input);
long double __cdecl nanl(char const* input);

#define drx_msvc_flt_inf ((double)(1e+300 * 1e+300))
#define drx_msvc_flt_inff ((float)(1e+300 * 1e+300))
#define drx_msvc_flt_infl ((long double)(1e+300 * 1e+300))
#define drx_msvc_flt_nan ((double)(drx_msvc_flt_inf * 0.f))
#define drx_msvc_flt_nanf ((float)(drx_msvc_flt_inf * 0.f))
#define drx_msvc_flt_nanl ((long double)(drx_msvc_flt_inf * 0.f))
#define __builtin_nan(str) nan(str)
#define __builtin_nanf(str) nanf(str)
#define __builtin_nanl(str) nanl(str)
#define __builtin_inf() drx_msvc_flt_inf
#define __builtin_inff() drx_msvc_flt_inff
#define __builtin_infl() drx_msvc_flt_infl
#endif

#if (drx_has_builtin(nan) && drx_has_builtin(nans) && drx_has_builtin(inf)) || defined(drx_gcc)
#define  drx_make_special_f16(sign, name, arg, repr) sign drx_make_f16 (__builtin_##name, )(arg)
#define  drx_make_special_f32(sign, name, arg, repr) sign drx_make_f32 (__builtin_##name, )(arg)
#define  drx_make_special_f64(sign, name, arg, repr) sign drx_make_f64 (__builtin_##name, )(arg)
#define  drx_make_special_f80(sign, name, arg, repr) sign drx_make_f80 (__builtin_##name, )(arg)
#define drx_make_special_f128(sign, name, arg, repr) sign drx_make_f128(__builtin_##name, )(arg)
#else
#define  drx_make_special_f16(sign, name, arg, repr) drx_bitCast_f16 (repr)
#define  drx_make_special_f32(sign, name, arg, repr) drx_bitCast_f32 (repr)
#define  drx_make_special_f64(sign, name, arg, repr) drx_bitCast_f64 (repr)
#define  drx_make_special_f80(sign, name, arg, repr) drx_bitCast_f80 (repr)
#define drx_make_special_f128(sign, name, arg, repr) drx_bitCast_f128(repr)
#endif

#define drx_has_f16 1
#define drx_libc_name_f16(name) __##name##h
#define drx_init_special_f16(sign, name, arg, repr) drx_make_special_f16(sign, name, arg, repr)
#if FLT_MANT_DIG == 11
typedef float drx_f16;
#define drx_make_f16(fp, repr) fp##f
#elif DBL_MANT_DIG == 11
typedef double drx_f16;
#define drx_make_f16(fp, repr) fp
#elif LDBL_MANT_DIG == 11
typedef long double drx_f16;
#define drx_make_f16(fp, repr) fp##l
#elif FLT16_MANT_DIG == 11 && (drx_has_builtin(inff16) || defined(drx_gcc))
typedef _Float16 drx_f16;
#define drx_make_f16(fp, repr) fp##f16
#elif defined(__SIZEOF_FP16__)
typedef __fp16 drx_f16;
#define drx_make_f16(fp, repr) fp##f16
#else
#undef drx_has_f16
#define drx_has_f16 0
#define drx_repr_f16 u16
typedef uint16_t drx_f16;
#define drx_make_f16(fp, repr) repr
#undef drx_make_special_f16
#define drx_make_special_f16(sign, name, arg, repr) repr
#undef drx_init_special_f16
#define drx_init_special_f16(sign, name, arg, repr) repr
#endif
#if defined(drx_darwin) && defined(drx_x86)
typedef uint16_t drx_compiler_rt_f16;
#else
typedef drx_f16 drx_compiler_rt_f16;
#endif

#define drx_has_f32 1
#define drx_libc_name_f32(name) name##f
#if defined(drx_msvc)
#define drx_init_special_f32(sign, name, arg, repr) sign drx_make_f32(drx_msvc_flt_##name, )
#else
#define drx_init_special_f32(sign, name, arg, repr) drx_make_special_f32(sign, name, arg, repr)
#endif
#if FLT_MANT_DIG == 24
typedef float drx_f32;
#define drx_make_f32(fp, repr) fp##f
#elif DBL_MANT_DIG == 24
typedef double drx_f32;
#define drx_make_f32(fp, repr) fp
#elif LDBL_MANT_DIG == 24
typedef long double drx_f32;
#define drx_make_f32(fp, repr) fp##l
#elif FLT32_MANT_DIG == 24
typedef _Float32 drx_f32;
#define drx_make_f32(fp, repr) fp##f32
#else
#undef drx_has_f32
#define drx_has_f32 0
#define drx_repr_f32 u32
typedef uint32_t drx_f32;
#define drx_make_f32(fp, repr) repr
#undef drx_make_special_f32
#define drx_make_special_f32(sign, name, arg, repr) repr
#undef drx_init_special_f32
#define drx_init_special_f32(sign, name, arg, repr) repr
#endif

#define drx_has_f64 1
#define drx_libc_name_f64(name) name

#if defined(drx_msvc)
#define drx_init_special_f64(sign, name, arg, repr) sign drx_make_f64(drx_msvc_flt_##name, )
#else
#define drx_init_special_f64(sign, name, arg, repr) drx_make_special_f64(sign, name, arg, repr)
#endif
#if FLT_MANT_DIG == 53
typedef float drx_f64;
#define drx_make_f64(fp, repr) fp##f
#elif DBL_MANT_DIG == 53
typedef double drx_f64;
#define drx_make_f64(fp, repr) fp
#elif LDBL_MANT_DIG == 53
typedef long double drx_f64;
#define drx_make_f64(fp, repr) fp##l
#elif FLT64_MANT_DIG == 53
typedef _Float64 drx_f64;
#define drx_make_f64(fp, repr) fp##f64
#elif FLT32X_MANT_DIG == 53
typedef _Float32x drx_f64;
#define drx_make_f64(fp, repr) fp##f32x
#else
#undef drx_has_f64
#define drx_has_f64 0
#define drx_repr_f64 u64
typedef uint64_t drx_f64;
#define drx_make_f64(fp, repr) repr
#undef drx_make_special_f64
#define drx_make_special_f64(sign, name, arg, repr) repr
#undef drx_init_special_f64
#define drx_init_special_f64(sign, name, arg, repr) repr
#endif

#define drx_has_f80 1
#define drx_libc_name_f80(name) __##name##x
#define drx_init_special_f80(sign, name, arg, repr) drx_make_special_f80(sign, name, arg, repr)
#if FLT_MANT_DIG == 64
typedef float drx_f80;
#define drx_make_f80(fp, repr) fp##f
#elif DBL_MANT_DIG == 64
typedef double drx_f80;
#define drx_make_f80(fp, repr) fp
#elif LDBL_MANT_DIG == 64
typedef long double drx_f80;
#define drx_make_f80(fp, repr) fp##l
#elif FLT80_MANT_DIG == 64
typedef _Float80 drx_f80;
#define drx_make_f80(fp, repr) fp##f80
#elif FLT64X_MANT_DIG == 64
typedef _Float64x drx_f80;
#define drx_make_f80(fp, repr) fp##f64x
#elif defined(__SIZEOF_FLOAT80__)
typedef __float80 drx_f80;
#define drx_make_f80(fp, repr) fp##l
#else
#undef drx_has_f80
#define drx_has_f80 0
#define drx_repr_f80 u128
typedef drx_u128 drx_f80;
#define drx_make_f80(fp, repr) repr
#undef drx_make_special_f80
#define drx_make_special_f80(sign, name, arg, repr) repr
#undef drx_init_special_f80
#define drx_init_special_f80(sign, name, arg, repr) repr
#endif

#if defined(drx_gcc) && defined(drx_x86)
#define drx_f128_has_miscompilations 1
#else
#define drx_f128_has_miscompilations 0
#endif

#define drx_has_f128 1
#define drx_libc_name_f128(name) name##q
#define drx_init_special_f128(sign, name, arg, repr) drx_make_special_f128(sign, name, arg, repr)
#if !drx_f128_has_miscompilations && FLT_MANT_DIG == 113
typedef float drx_f128;
#define drx_make_f128(fp, repr) fp##f
#elif !drx_f128_has_miscompilations && DBL_MANT_DIG == 113
typedef double drx_f128;
#define drx_make_f128(fp, repr) fp
#elif !drx_f128_has_miscompilations && LDBL_MANT_DIG == 113
typedef long double drx_f128;
#define drx_make_f128(fp, repr) fp##l
#elif !drx_f128_has_miscompilations && FLT128_MANT_DIG == 113
typedef _Float128 drx_f128;
#define drx_make_f128(fp, repr) fp##f128
#elif !drx_f128_has_miscompilations && FLT64X_MANT_DIG == 113
typedef _Float64x drx_f128;
#define drx_make_f128(fp, repr) fp##f64x
#elif !drx_f128_has_miscompilations && defined(__SIZEOF_FLOAT128__)
typedef __float128 drx_f128;
#define drx_make_f128(fp, repr) fp##q
#undef drx_make_special_f128
#define drx_make_special_f128(sign, name, arg, repr) sign __builtin_##name##f128(arg)
#else
#undef drx_has_f128
#define drx_has_f128 0
#undef drx_make_special_f128
#undef drx_init_special_f128
#if defined(drx_darwin) || defined(drx_aarch64)
typedef __attribute__((__vector_size__(2 * sizeof(uint64_t)))) uint64_t drx_v2u64;
drx_basic_operator(drx_v2u64, xor_v2u64, ^)
#define drx_repr_f128 v2u64
typedef drx_v2u64 drx_f128;
#define drx_make_f128_drx_make_u128(hi, lo) (drx_f128){ lo, hi }
#define drx_make_f128_drx_init_u128 drx_make_f128_drx_make_u128
#define drx_make_f128(fp, repr) drx_make_f128_##repr
#define drx_make_special_f128(sign, name, arg, repr) drx_make_f128_##repr
#define drx_init_special_f128(sign, name, arg, repr) drx_make_f128_##repr
#else
#define drx_repr_f128 u128
typedef drx_u128 drx_f128;
#define drx_make_f128(fp, repr) repr
#define drx_make_special_f128(sign, name, arg, repr) repr
#define drx_init_special_f128(sign, name, arg, repr) repr
#endif
#endif

#if !defined(drx_msvc) && defined(ZIG_TARGET_ABI_MSVC)
/* Emulate msvc abi on a gnu compiler */
typedef drx_f64 drx_c_longdouble;
#elif defined(drx_msvc) && !defined(ZIG_TARGET_ABI_MSVC)
/* Emulate gnu abi on an msvc compiler */
typedef drx_f128 drx_c_longdouble;
#else
/* Target and compiler abi match */
typedef long double drx_c_longdouble;
#endif

#define drx_bitCast_float(Type, ReprType) \
    static inline drx_##Type drx_bitCast_##Type(ReprType repr) { \
        drx_##Type result; \
        memcpy(&result, &repr, sizeof(result)); \
        return result; \
    }
drx_bitCast_float(f16, uint16_t)
drx_bitCast_float(f32, uint32_t)
drx_bitCast_float(f64, uint64_t)
drx_bitCast_float(f80, drx_u128)
drx_bitCast_float(f128, drx_u128)

#define drx_convert_builtin(ExternResType, ResType, operation, ExternArgType, ArgType, version) \
    drx_extern ExternResType drx_expand_concat(drx_expand_concat(drx_expand_concat(__##operation, \
        drx_compiler_rt_abbrev_##ArgType), drx_compiler_rt_abbrev_##ResType), version)(ExternArgType); \
    static inline ResType drx_expand_concat(drx_expand_concat(drx_##operation, \
        drx_compiler_rt_abbrev_##ArgType), drx_compiler_rt_abbrev_##ResType)(ArgType arg) { \
        ResType res; \
        ExternResType extern_res; \
        ExternArgType extern_arg; \
        memcpy(&extern_arg, &arg, sizeof(extern_arg)); \
        extern_res = drx_expand_concat(drx_expand_concat(drx_expand_concat(__##operation, \
            drx_compiler_rt_abbrev_##ArgType), drx_compiler_rt_abbrev_##ResType), version)(extern_arg); \
        memcpy(&res, &extern_res, sizeof(res)); \
        return extern_res; \
    }
drx_convert_builtin(drx_compiler_rt_f16, drx_f16,   trunc, drx_f32,             drx_f32,  2)
drx_convert_builtin(drx_compiler_rt_f16, drx_f16,   trunc, drx_f64,             drx_f64,  2)
drx_convert_builtin(drx_f16,             drx_f16,   trunc, drx_f80,             drx_f80,  2)
drx_convert_builtin(drx_f16,             drx_f16,   trunc, drx_f128,            drx_f128, 2)
drx_convert_builtin(drx_f32,             drx_f32,  extend, drx_compiler_rt_f16, drx_f16,  2)
drx_convert_builtin(drx_f32,             drx_f32,   trunc, drx_f80,             drx_f80,  2)
drx_convert_builtin(drx_f32,             drx_f32,   trunc, drx_f128,            drx_f128, 2)
drx_convert_builtin(drx_f64,             drx_f64,  extend, drx_compiler_rt_f16, drx_f16,  2)
drx_convert_builtin(drx_f64,             drx_f64,   trunc, drx_f80,             drx_f80,  2)
drx_convert_builtin(drx_f64,             drx_f64,   trunc, drx_f128,            drx_f128, 2)
drx_convert_builtin(drx_f80,             drx_f80,  extend, drx_f16,             drx_f16,  2)
drx_convert_builtin(drx_f80,             drx_f80,  extend, drx_f32,             drx_f32,  2)
drx_convert_builtin(drx_f80,             drx_f80,  extend, drx_f64,             drx_f64,  2)
drx_convert_builtin(drx_f80,             drx_f80,   trunc, drx_f128,            drx_f128, 2)
drx_convert_builtin(drx_f128,            drx_f128, extend, drx_f16,             drx_f16,  2)
drx_convert_builtin(drx_f128,            drx_f128, extend, drx_f32,             drx_f32,  2)
drx_convert_builtin(drx_f128,            drx_f128, extend, drx_f64,             drx_f64,  2)
drx_convert_builtin(drx_f128,            drx_f128, extend, drx_f80,             drx_f80,  2)

#ifdef __ARM_EABI__

drx_extern drx_callconv(pcs("aapcs")) drx_f32 __aeabi_d2f(drx_f64);
static inline drx_f32 drx_truncdfsf(drx_f64 arg) { return __aeabi_d2f(arg); }

drx_extern drx_callconv(pcs("aapcs")) drx_f64 __aeabi_f2d(drx_f32);
static inline drx_f64 drx_extendsfdf(drx_f32 arg) { return __aeabi_f2d(arg); }

#else /* __ARM_EABI__ */

drx_convert_builtin(drx_f32,             drx_f32,   trunc, drx_f64,             drx_f64,  2)
drx_convert_builtin(drx_f64,             drx_f64,  extend, drx_f32,             drx_f32,  2)

#endif /* __ARM_EABI__ */

#define drx_float_negate_builtin_0(w, c, sb) \
    drx_expand_concat(drx_xor_, drx_repr_f##w)(arg, drx_make_f##w(-0x0.0p0, c sb))
#define drx_float_negate_builtin_1(w, c, sb) -arg
#define drx_float_negate_builtin(w, c, sb) \
    static inline drx_f##w drx_neg_f##w(drx_f##w arg) { \
        return drx_expand_concat(drx_float_negate_builtin_, drx_has_f##w)(w, c, sb); \
    }
drx_float_negate_builtin(16,               ,  UINT16_C(1) << 15              )
drx_float_negate_builtin(32,               ,  UINT32_C(1) << 31              )
drx_float_negate_builtin(64,               ,  UINT64_C(1) << 63              )
drx_float_negate_builtin(80,  drx_make_u128, (UINT64_C(1) << 15, UINT64_C(0)))
drx_float_negate_builtin(128, drx_make_u128, (UINT64_C(1) << 63, UINT64_C(0)))

#define drx_float_less_builtin_0(Type, operation) \
    drx_extern int32_t drx_expand_concat(drx_expand_concat(__##operation, \
        drx_compiler_rt_abbrev_drx_##Type), 2)(drx_##Type, drx_##Type); \
    static inline int32_t drx_##operation##_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return drx_expand_concat(drx_expand_concat(__##operation, drx_compiler_rt_abbrev_drx_##Type), 2)(lhs, rhs); \
    }
#define drx_float_less_builtin_1(Type, operation) \
    static inline int32_t drx_##operation##_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return (!(lhs <= rhs) - (lhs < rhs)); \
    }

#define drx_float_greater_builtin_0(Type, operation) \
    drx_float_less_builtin_0(Type, operation)
#define drx_float_greater_builtin_1(Type, operation) \
    static inline int32_t drx_##operation##_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return ((lhs > rhs) - !(lhs >= rhs)); \
    }

#define drx_float_binary_builtin_0(Type, operation, operator) \
    drx_extern drx_##Type drx_expand_concat(drx_expand_concat(__##operation, \
        drx_compiler_rt_abbrev_drx_##Type), 3)(drx_##Type, drx_##Type); \
    static inline drx_##Type drx_##operation##_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return drx_expand_concat(drx_expand_concat(__##operation, drx_compiler_rt_abbrev_drx_##Type), 3)(lhs, rhs); \
    }
#define drx_float_binary_builtin_1(Type, operation, operator) \
    static inline drx_##Type drx_##operation##_##Type(drx_##Type lhs, drx_##Type rhs) { \
        return lhs operator rhs; \
    }

#define drx_common_float_builtins(w) \
    drx_convert_builtin( int64_t,  int64_t, fix,     drx_f##w, drx_f##w, ) \
    drx_convert_builtin(drx_i128, drx_i128, fix,     drx_f##w, drx_f##w, ) \
    drx_convert_builtin(drx_u128, drx_u128, fixuns,  drx_f##w, drx_f##w, ) \
    drx_convert_builtin(drx_f##w, drx_f##w, float,    int64_t,  int64_t, ) \
    drx_convert_builtin(drx_f##w, drx_f##w, float,   drx_i128, drx_i128, ) \
    drx_convert_builtin(drx_f##w, drx_f##w, floatun, drx_u128, drx_u128, ) \
    drx_expand_concat(drx_float_less_builtin_,    drx_has_f##w)(f##w, cmp) \
    drx_expand_concat(drx_float_less_builtin_,    drx_has_f##w)(f##w, ne) \
    drx_expand_concat(drx_float_less_builtin_,    drx_has_f##w)(f##w, eq) \
    drx_expand_concat(drx_float_less_builtin_,    drx_has_f##w)(f##w, lt) \
    drx_expand_concat(drx_float_less_builtin_,    drx_has_f##w)(f##w, le) \
    drx_expand_concat(drx_float_greater_builtin_, drx_has_f##w)(f##w, gt) \
    drx_expand_concat(drx_float_greater_builtin_, drx_has_f##w)(f##w, ge) \
    drx_expand_concat(drx_float_binary_builtin_,  drx_has_f##w)(f##w, add, +) \
    drx_expand_concat(drx_float_binary_builtin_,  drx_has_f##w)(f##w, sub, -) \
    drx_expand_concat(drx_float_binary_builtin_,  drx_has_f##w)(f##w, mul, *) \
    drx_expand_concat(drx_float_binary_builtin_,  drx_has_f##w)(f##w, div, /) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(sqrt)))(drx_f##w, drx_sqrt_f##w, drx_libc_name_f##w(sqrt), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(sin)))(drx_f##w, drx_sin_f##w, drx_libc_name_f##w(sin), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(cos)))(drx_f##w, drx_cos_f##w, drx_libc_name_f##w(cos), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(tan)))(drx_f##w, drx_tan_f##w, drx_libc_name_f##w(tan), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(exp)))(drx_f##w, drx_exp_f##w, drx_libc_name_f##w(exp), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(exp2)))(drx_f##w, drx_exp2_f##w, drx_libc_name_f##w(exp2), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(log)))(drx_f##w, drx_log_f##w, drx_libc_name_f##w(log), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(log2)))(drx_f##w, drx_log2_f##w, drx_libc_name_f##w(log2), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(log10)))(drx_f##w, drx_log10_f##w, drx_libc_name_f##w(log10), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(fabs)))(drx_f##w, drx_abs_f##w, drx_libc_name_f##w(fabs), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(floor)))(drx_f##w, drx_floor_f##w, drx_libc_name_f##w(floor), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(ceil)))(drx_f##w, drx_ceil_f##w, drx_libc_name_f##w(ceil), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(round)))(drx_f##w, drx_round_f##w, drx_libc_name_f##w(round), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(trunc)))(drx_f##w, drx_trunc_f##w, drx_libc_name_f##w(trunc), (drx_f##w x), (x)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(fmod)))(drx_f##w, drx_fmod_f##w, drx_libc_name_f##w(fmod), (drx_f##w x, drx_f##w y), (x, y)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(fmin)))(drx_f##w, drx_min_f##w, drx_libc_name_f##w(fmin), (drx_f##w x, drx_f##w y), (x, y)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(fmax)))(drx_f##w, drx_max_f##w, drx_libc_name_f##w(fmax), (drx_f##w x, drx_f##w y), (x, y)) \
    drx_expand_concat(drx_expand_import_, drx_expand_has_builtin(drx_libc_name_f##w(fma)))(drx_f##w, drx_fma_f##w, drx_libc_name_f##w(fma), (drx_f##w x, drx_f##w y, drx_f##w z), (x, y, z)) \
\
    static inline drx_f##w drx_div_trunc_f##w(drx_f##w lhs, drx_f##w rhs) { \
        return drx_trunc_f##w(drx_div_f##w(lhs, rhs)); \
    } \
\
    static inline drx_f##w drx_div_floor_f##w(drx_f##w lhs, drx_f##w rhs) { \
        return drx_floor_f##w(drx_div_f##w(lhs, rhs)); \
    } \
\
    static inline drx_f##w drx_mod_f##w(drx_f##w lhs, drx_f##w rhs) { \
        return drx_sub_f##w(lhs, drx_mul_f##w(drx_div_floor_f##w(lhs, rhs), rhs)); \
    }
drx_common_float_builtins(16)
drx_common_float_builtins(32)
drx_common_float_builtins(64)
drx_common_float_builtins(80)
drx_common_float_builtins(128)

#define drx_float_builtins(w) \
    drx_convert_builtin( int32_t,  int32_t, fix,     drx_f##w, drx_f##w, ) \
    drx_convert_builtin(uint32_t, uint32_t, fixuns,  drx_f##w, drx_f##w, ) \
    drx_convert_builtin(uint64_t, uint64_t, fixuns,  drx_f##w, drx_f##w, ) \
    drx_convert_builtin(drx_f##w, drx_f##w, float,    int32_t,  int32_t, ) \
    drx_convert_builtin(drx_f##w, drx_f##w, floatun, uint32_t, uint32_t, ) \
    drx_convert_builtin(drx_f##w, drx_f##w, floatun, uint64_t, uint64_t, )
drx_float_builtins(16)
drx_float_builtins(80)
drx_float_builtins(128)

#ifdef __ARM_EABI__

drx_extern drx_callconv(pcs("aapcs")) int32_t __aeabi_f2iz(drx_f32);
static inline int32_t drx_fixsfsi(drx_f32 arg) { return __aeabi_f2iz(arg); }

drx_extern drx_callconv(pcs("aapcs")) uint32_t __aeabi_f2uiz(drx_f32);
static inline uint32_t drx_fixunssfsi(drx_f32 arg) { return __aeabi_f2uiz(arg); }

drx_extern drx_callconv(pcs("aapcs")) uint64_t __aeabi_f2ulz(drx_f32);
static inline uint64_t drx_fixunssfdi(drx_f32 arg) { return __aeabi_f2ulz(arg); }

drx_extern drx_callconv(pcs("aapcs")) drx_f32 __aeabi_i2f(int32_t);
static inline drx_f32 drx_floatsisf(int32_t arg) { return __aeabi_i2f(arg); }

drx_extern drx_callconv(pcs("aapcs")) drx_f32 __aeabi_ui2f(uint32_t);
static inline drx_f32 drx_floatunsisf(uint32_t arg) { return __aeabi_ui2f(arg); }

drx_extern drx_callconv(pcs("aapcs")) drx_f32 __aeabi_ul2f(uint64_t);
static inline drx_f32 drx_floatundisf(uint64_t arg) { return __aeabi_ul2f(arg); }

drx_extern drx_callconv(pcs("aapcs")) int32_t __aeabi_d2iz(drx_f64);
static inline int32_t drx_fixdfsi(drx_f64 arg) { return __aeabi_d2iz(arg); }

drx_extern drx_callconv(pcs("aapcs")) uint32_t __aeabi_d2uiz(drx_f64);
static inline uint32_t drx_fixunsdfsi(drx_f64 arg) { return __aeabi_d2uiz(arg); }

drx_extern drx_callconv(pcs("aapcs")) uint64_t __aeabi_d2ulz(drx_f64);
static inline uint64_t drx_fixunsdfdi(drx_f64 arg) { return __aeabi_d2ulz(arg); }

drx_extern drx_callconv(pcs("aapcs")) drx_f64 __aeabi_i2d(int32_t);
static inline drx_f64 drx_floatsidf(int32_t arg) { return __aeabi_i2d(arg); }

drx_extern drx_callconv(pcs("aapcs")) drx_f64 __aeabi_ui2d(uint32_t);
static inline drx_f64 drx_floatunsidf(uint32_t arg) { return __aeabi_ui2d(arg); }

drx_extern drx_callconv(pcs("aapcs")) drx_f64 __aeabi_ul2d(uint64_t);
static inline drx_f64 drx_floatundidf(uint64_t arg) { return __aeabi_ul2d(arg); }

#else /* __ARM_EABI__ */

drx_float_builtins(32)
drx_float_builtins(64)

#endif /* __ARM_EABI__ */

/* ============================ Atomics Support ============================= */

/* Note that atomics should be implemented as macros because most
   compilers silently discard runtime atomic order information. */

/* Define fallback implementations first that can later be undef'd on compilers with builtin support. */
/* Note that drx_atomicrmw_expected is needed to handle aliasing between res and arg. */
#define drx_atomicrmw_xchg_float(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, arg, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_add_float(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_add_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_sub_float(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_sub_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_min_float(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_min_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_max_float(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_max_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)

#define drx_atomicrmw_xchg_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, arg, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_add_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_add_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_sub_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_sub_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_and_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_and_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_nand_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_not_##Type(drx_and_##Type(drx_atomicrmw_expected, arg), 128); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_or_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_or_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_xor_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_xor_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_min_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_min_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)
#define  drx_atomicrmw_max_int128(res, obj, arg, order, Type, ReprType) do { \
    drx_##Type drx_atomicrmw_expected; \
    drx_##Type drx_atomicrmw_desired; \
    drx_atomic_load(drx_atomicrmw_expected, obj, drx_memory_order_relaxed, Type, ReprType); \
    do { \
        drx_atomicrmw_desired = drx_max_##Type(drx_atomicrmw_expected, arg); \
    } while (!drx_cmpxchg_weak(obj, drx_atomicrmw_expected, drx_atomicrmw_desired, order, drx_memory_order_relaxed, Type, ReprType)); \
    res = drx_atomicrmw_expected; \
} while (0)

#if (__STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)) || (drx_has_include(<stdatomic.h>) && !defined(drx_msvc))
#define drx_c11_atomics
#endif

#if defined(drx_c11_atomics)
#include <stdatomic.h>
typedef enum memory_order drx_memory_order;
#define drx_memory_order_relaxed memory_order_relaxed
#define drx_memory_order_acquire memory_order_acquire
#define drx_memory_order_release memory_order_release
#define drx_memory_order_acq_rel memory_order_acq_rel
#define drx_memory_order_seq_cst memory_order_seq_cst
#define drx_atomic(Type) _Atomic(Type)
#define drx_cmpxchg_strong(     obj, expected, desired, succ, fail, Type, ReprType) atomic_compare_exchange_strong_explicit(obj, &(expected), desired, succ, fail)
#define   drx_cmpxchg_weak(     obj, expected, desired, succ, fail, Type, ReprType) atomic_compare_exchange_weak_explicit  (obj, &(expected), desired, succ, fail)
#define drx_atomicrmw_xchg(res, obj, arg, order, Type, ReprType) res = atomic_exchange_explicit  (obj, arg, order)
#define  drx_atomicrmw_add(res, obj, arg, order, Type, ReprType) res = atomic_fetch_add_explicit (obj, arg, order)
#define  drx_atomicrmw_sub(res, obj, arg, order, Type, ReprType) res = atomic_fetch_sub_explicit (obj, arg, order)
#define   drx_atomicrmw_or(res, obj, arg, order, Type, ReprType) res = atomic_fetch_or_explicit  (obj, arg, order)
#define  drx_atomicrmw_xor(res, obj, arg, order, Type, ReprType) res = atomic_fetch_xor_explicit (obj, arg, order)
#define  drx_atomicrmw_and(res, obj, arg, order, Type, ReprType) res = atomic_fetch_and_explicit (obj, arg, order)
#define drx_atomicrmw_nand(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_nand(obj, arg, order)
#define  drx_atomicrmw_min(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_min (obj, arg, order)
#define  drx_atomicrmw_max(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_max (obj, arg, order)
#define   drx_atomic_store(     obj, arg, order, Type, ReprType)       atomic_store_explicit     (obj, arg, order)
#define    drx_atomic_load(res, obj,      order, Type, ReprType) res = atomic_load_explicit      (obj,      order)
#undef  drx_atomicrmw_xchg_float
#define drx_atomicrmw_xchg_float drx_atomicrmw_xchg
#undef  drx_atomicrmw_add_float
#define drx_atomicrmw_add_float drx_atomicrmw_add
#undef  drx_atomicrmw_sub_float
#define drx_atomicrmw_sub_float drx_atomicrmw_sub
#elif defined(drx_gnuc)
typedef int drx_memory_order;
#define drx_memory_order_relaxed __ATOMIC_RELAXED
#define drx_memory_order_acquire __ATOMIC_ACQUIRE
#define drx_memory_order_release __ATOMIC_RELEASE
#define drx_memory_order_acq_rel __ATOMIC_ACQ_REL
#define drx_memory_order_seq_cst __ATOMIC_SEQ_CST
#define drx_atomic(Type) Type
#define drx_cmpxchg_strong(     obj, expected, desired, succ, fail, Type, ReprType) __atomic_compare_exchange(obj, (ReprType *)&(expected), (ReprType *)&(desired), false, succ, fail)
#define   drx_cmpxchg_weak(     obj, expected, desired, succ, fail, Type, ReprType) __atomic_compare_exchange(obj, (ReprType *)&(expected), (ReprType *)&(desired),  true, succ, fail)
#define drx_atomicrmw_xchg(res, obj, arg, order, Type, ReprType)       __atomic_exchange(obj, (ReprType *)&(arg), &(res), order)
#define  drx_atomicrmw_add(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_add (obj, arg, order)
#define  drx_atomicrmw_sub(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_sub (obj, arg, order)
#define   drx_atomicrmw_or(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_or  (obj, arg, order)
#define  drx_atomicrmw_xor(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_xor (obj, arg, order)
#define  drx_atomicrmw_and(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_and (obj, arg, order)
#define drx_atomicrmw_nand(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_nand(obj, arg, order)
#define  drx_atomicrmw_min(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_min (obj, arg, order)
#define  drx_atomicrmw_max(res, obj, arg, order, Type, ReprType) res = __atomic_fetch_max (obj, arg, order)
#define   drx_atomic_store(     obj, arg, order, Type, ReprType)       __atomic_store     (obj, (ReprType *)&(arg), order)
#define    drx_atomic_load(res, obj,      order, Type, ReprType)       __atomic_load      (obj, &(res), order)
#undef  drx_atomicrmw_xchg_float
#define drx_atomicrmw_xchg_float drx_atomicrmw_xchg
#elif defined(drx_msvc) && defined(drx_x86)
#define drx_memory_order_relaxed 0
#define drx_memory_order_acquire 2
#define drx_memory_order_release 3
#define drx_memory_order_acq_rel 4
#define drx_memory_order_seq_cst 5
#define drx_atomic(Type) Type
#define drx_cmpxchg_strong(     obj, expected, desired, succ, fail, Type, ReprType) drx_msvc_cmpxchg_##Type(obj, &(expected), desired)
#define   drx_cmpxchg_weak(     obj, expected, desired, succ, fail, Type, ReprType) drx_cmpxchg_strong(obj, expected, desired, succ, fail, Type, ReprType)
#define drx_atomicrmw_xchg(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_xchg_##Type(obj, arg)
#define  drx_atomicrmw_add(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_add_ ##Type(obj, arg)
#define  drx_atomicrmw_sub(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_sub_ ##Type(obj, arg)
#define   drx_atomicrmw_or(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_or_  ##Type(obj, arg)
#define  drx_atomicrmw_xor(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_xor_ ##Type(obj, arg)
#define  drx_atomicrmw_and(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_and_ ##Type(obj, arg)
#define drx_atomicrmw_nand(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_nand_##Type(obj, arg)
#define  drx_atomicrmw_min(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_min_ ##Type(obj, arg)
#define  drx_atomicrmw_max(res, obj, arg, order, Type, ReprType) res = drx_msvc_atomicrmw_max_ ##Type(obj, arg)
#define   drx_atomic_store(     obj, arg, order, Type, ReprType)       drx_msvc_atomic_store_  ##Type(obj, arg)
#define    drx_atomic_load(res, obj,      order, Type, ReprType) res = drx_msvc_atomic_load_   ##order##_##Type(obj)
/* TODO: drx_msvc && (drx_thumb || drx_aarch64) */
#else
#define drx_memory_order_relaxed 0
#define drx_memory_order_acquire 2
#define drx_memory_order_release 3
#define drx_memory_order_acq_rel 4
#define drx_memory_order_seq_cst 5
#define drx_atomic(Type) Type
#define drx_cmpxchg_strong(     obj, expected, desired, succ, fail, Type, ReprType) drx_atomics_unavailable
#define   drx_cmpxchg_weak(     obj, expected, desired, succ, fail, Type, ReprType) drx_atomics_unavailable
#define drx_atomicrmw_xchg(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define  drx_atomicrmw_add(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define  drx_atomicrmw_sub(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define   drx_atomicrmw_or(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define  drx_atomicrmw_xor(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define  drx_atomicrmw_and(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define drx_atomicrmw_nand(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define  drx_atomicrmw_min(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define  drx_atomicrmw_max(res, obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define   drx_atomic_store(     obj, arg, order, Type, ReprType) drx_atomics_unavailable
#define    drx_atomic_load(res, obj,      order, Type, ReprType) drx_atomics_unavailable
#endif

#if !defined(drx_c11_atomics) && defined(drx_msvc) && defined(drx_x86)

/* TODO: drx_msvc_atomic_load should load 32 bit without interlocked on x86, and load 64 bit without interlocked on x64 */

#define drx_msvc_atomics(ZigType, Type, SigType, suffix, iso_suffix) \
    static inline bool drx_msvc_cmpxchg_##ZigType(Type volatile* obj, Type* expected, Type desired) { \
        Type comparand = *expected; \
        Type initial = _InterlockedCompareExchange##suffix((SigType volatile*)obj, (SigType)desired, (SigType)comparand); \
        bool exchanged = initial == comparand; \
        if (!exchanged) { \
            *expected = initial; \
        } \
        return exchanged; \
    } \
    static inline Type drx_msvc_atomicrmw_xchg_##ZigType(Type volatile* obj, Type value) { \
        return _InterlockedExchange##suffix((SigType volatile*)obj, (SigType)value); \
    } \
    static inline Type drx_msvc_atomicrmw_add_##ZigType(Type volatile* obj, Type value) { \
        return _InterlockedExchangeAdd##suffix((SigType volatile*)obj, (SigType)value); \
    } \
    static inline Type drx_msvc_atomicrmw_sub_##ZigType(Type volatile* obj, Type value) { \
        bool success = false; \
        Type new; \
        Type prev; \
        while (!success) { \
            prev = *obj; \
            new = prev - value; \
            success = drx_msvc_cmpxchg_##ZigType(obj, &prev, new); \
        } \
        return prev; \
    } \
    static inline Type drx_msvc_atomicrmw_or_##ZigType(Type volatile* obj, Type value) { \
        return _InterlockedOr##suffix((SigType volatile*)obj, (SigType)value); \
    } \
    static inline Type drx_msvc_atomicrmw_xor_##ZigType(Type volatile* obj, Type value) { \
        return _InterlockedXor##suffix((SigType volatile*)obj, (SigType)value); \
    } \
    static inline Type drx_msvc_atomicrmw_and_##ZigType(Type volatile* obj, Type value) { \
        return _InterlockedAnd##suffix((SigType volatile*)obj, (SigType)value); \
    } \
    static inline Type drx_msvc_atomicrmw_nand_##ZigType(Type volatile* obj, Type value) { \
        bool success = false; \
        Type new; \
        Type prev; \
        while (!success) { \
            prev = *obj; \
            new = ~(prev & value); \
            success = drx_msvc_cmpxchg_##ZigType(obj, &prev, new); \
        } \
        return prev; \
    } \
    static inline Type drx_msvc_atomicrmw_min_##ZigType(Type volatile* obj, Type value) { \
        bool success = false; \
        Type new; \
        Type prev; \
        while (!success) { \
            prev = *obj; \
            new = value < prev ? value : prev; \
            success = drx_msvc_cmpxchg_##ZigType(obj, &prev, new); \
        } \
        return prev; \
    } \
    static inline Type drx_msvc_atomicrmw_max_##ZigType(Type volatile* obj, Type value) { \
        bool success = false; \
        Type new; \
        Type prev; \
        while (!success) { \
            prev = *obj; \
            new = value > prev ? value : prev; \
            success = drx_msvc_cmpxchg_##ZigType(obj, &prev, new); \
        } \
        return prev; \
    } \
    static inline void drx_msvc_atomic_store_##ZigType(Type volatile* obj, Type value) { \
        (void)_InterlockedExchange##suffix((SigType volatile*)obj, (SigType)value); \
    }                                                                   \
    static inline Type drx_msvc_atomic_load_drx_memory_order_relaxed_##ZigType(Type volatile* obj) { \
        return __iso_volatile_load##iso_suffix((SigType volatile*)obj); \
    } \
    static inline Type drx_msvc_atomic_load_drx_memory_order_acquire_##ZigType(Type volatile* obj) { \
        Type val = __iso_volatile_load##iso_suffix((SigType volatile*)obj); \
        _ReadWriteBarrier(); \
        return val; \
    } \
    static inline Type drx_msvc_atomic_load_drx_memory_order_seq_cst_##ZigType(Type volatile* obj) { \
        Type val = __iso_volatile_load##iso_suffix((SigType volatile*)obj); \
        _ReadWriteBarrier(); \
        return val; \
    }

drx_msvc_atomics( u8,  uint8_t,    char,  8, 8)
drx_msvc_atomics( i8,   int8_t,    char,  8, 8)
drx_msvc_atomics(u16, uint16_t,   short, 16, 16)
drx_msvc_atomics(i16,  int16_t,   short, 16, 16)
drx_msvc_atomics(u32, uint32_t,    long,   , 32)
drx_msvc_atomics(i32,  int32_t,    long,   , 32)

#if defined(drx_x86_64)
drx_msvc_atomics(u64, uint64_t, __int64, 64, 64)
drx_msvc_atomics(i64,  int64_t, __int64, 64, 64)
#endif

#define drx_msvc_flt_atomics(Type, SigType, suffix, iso_suffix) \
    static inline bool drx_msvc_cmpxchg_##Type(drx_##Type volatile* obj, drx_##Type* expected, drx_##Type desired) { \
        SigType exchange; \
        SigType comparand; \
        SigType initial; \
        bool success; \
        memcpy(&comparand, expected, sizeof(comparand)); \
        memcpy(&exchange, &desired, sizeof(exchange)); \
        initial = _InterlockedCompareExchange##suffix((SigType volatile*)obj, exchange, comparand); \
        success = initial == comparand; \
        if (!success) memcpy(expected, &initial, sizeof(*expected)); \
        return success; \
    } \
    static inline void drx_msvc_atomic_store_##Type(drx_##Type volatile* obj, drx_##Type arg) { \
        SigType value; \
        memcpy(&value, &arg, sizeof(value)); \
        (void)_InterlockedExchange##suffix((SigType volatile*)obj, value); \
    } \
    static inline drx_##Type drx_msvc_atomic_load_drx_memory_order_relaxed_##Type(drx_##Type volatile* obj) { \
        drx_##Type result; \
        SigType initial = __iso_volatile_load##iso_suffix((SigType volatile*)obj); \
        memcpy(&result, &initial, sizeof(result)); \
        return result; \
    } \
    static inline drx_##Type drx_msvc_atomic_load_drx_memory_order_acquire_##Type(drx_##Type volatile* obj) { \
        drx_##Type result; \
        SigType initial = __iso_volatile_load##iso_suffix((SigType volatile*)obj); \
        _ReadWriteBarrier(); \
        memcpy(&result, &initial, sizeof(result));     \
        return result; \
    } \
    static inline drx_##Type drx_msvc_atomic_load_drx_memory_order_seq_cst_##Type(drx_##Type volatile* obj) { \
        drx_##Type result; \
        SigType initial = __iso_volatile_load##iso_suffix((SigType volatile*)obj); \
        _ReadWriteBarrier(); \
        memcpy(&result, &initial, sizeof(result));     \
        return result; \
    }

drx_msvc_flt_atomics(f32,    long,   , 32)
#if defined(drx_x86_64)
drx_msvc_flt_atomics(f64, int64_t, 64, 64)
#endif

#if defined(drx_x86_32)
static inline void drx_msvc_atomic_barrier() {
    int32_t barrier;
    __asm {
        xchg barrier, eax
    }
}

static inline void* drx_msvc_atomicrmw_xchg_p32(void volatile* obj, void* arg) {
    return _InterlockedExchangePointer(obj, arg);
}

static inline void drx_msvc_atomic_store_p32(void volatile* obj, void* arg) {
    (void)_InterlockedExchangePointer(obj, arg);
}

static inline void* drx_msvc_atomic_load_drx_memory_order_relaxed_p32(void volatile* obj) {
    return (void*)__iso_volatile_load32(obj);
}

static inline void* drx_msvc_atomic_load_drx_memory_order_acquire_p32(void volatile* obj) {
    void* val = (void*)__iso_volatile_load32(obj);
    _ReadWriteBarrier();
    return val;
}

static inline void* drx_msvc_atomic_load_drx_memory_order_seq_cst_p32(void volatile* obj) {
    return drx_msvc_atomic_load_drx_memory_order_acquire_p32(obj);
}

static inline bool drx_msvc_cmpxchg_p32(void volatile* obj, void* expected, void* desired) {
    void* comparand = *(void**)expected;
    void* initial = _InterlockedCompareExchangePointer(obj, desired, comparand);
    bool success = initial == comparand;
    if (!success) *(void**)expected = initial;
    return success;
}
#else /* drx_x86_32 */
static inline void* drx_msvc_atomicrmw_xchg_p64(void volatile* obj, void* arg) {
    return _InterlockedExchangePointer(obj, arg);
}

static inline void drx_msvc_atomic_store_p64(void volatile* obj, void* arg) {
    (void)_InterlockedExchangePointer(obj, arg);
}

static inline void* drx_msvc_atomic_load_drx_memory_order_relaxed_p64(void volatile* obj) {
    return (void*)__iso_volatile_load64(obj);
}

static inline void* drx_msvc_atomic_load_drx_memory_order_acquire_p64(void volatile* obj) {
    void* val = (void*)__iso_volatile_load64(obj);
    _ReadWriteBarrier();
    return val;
}

static inline void* drx_msvc_atomic_load_drx_memory_order_seq_cst_p64(void volatile* obj) {
    return drx_msvc_atomic_load_drx_memory_order_acquire_p64(obj);
}

static inline bool drx_msvc_cmpxchg_p64(void volatile* obj, void* expected, void* desired) {
    void* comparand = *(void**)expected;
    void* initial = _InterlockedCompareExchangePointer(obj, desired, comparand);
    bool success = initial == comparand;
    if (!success) *(void**)expected = initial;
    return success;
}

static inline bool drx_msvc_cmpxchg_u128(drx_u128 volatile* obj, drx_u128* expected, drx_u128 desired) {
    return _InterlockedCompareExchange128((__int64 volatile*)obj, (__int64)drx_hi_u128(desired), (__int64)drx_lo_u128(desired), (__int64*)expected);
}

static inline drx_u128 drx_msvc_atomic_load_u128(drx_u128 volatile* obj) {
    drx_u128 expected = drx_make_u128(UINT64_C(0), UINT64_C(0));
    (void)drx_cmpxchg_strong(obj, expected, expected, drx_memory_order_seq_cst, drx_memory_order_seq_cst, u128, drx_u128);
    return expected;
}

static inline void drx_msvc_atomic_store_u128(drx_u128 volatile* obj, drx_u128 arg) {
    drx_u128 expected = drx_make_u128(UINT64_C(0), UINT64_C(0));
    while (!drx_cmpxchg_weak(obj, expected, arg, drx_memory_order_seq_cst, drx_memory_order_seq_cst, u128, drx_u128));
}

static inline bool drx_msvc_cmpxchg_i128(drx_i128 volatile* obj, drx_i128* expected, drx_i128 desired) {
    return _InterlockedCompareExchange128((__int64 volatile*)obj, (__int64)drx_hi_i128(desired), (__int64)drx_lo_i128(desired), (__int64*)expected);
}

static inline drx_i128 drx_msvc_atomic_load_i128(drx_i128 volatile* obj) {
    drx_i128 expected = drx_make_i128(INT64_C(0), UINT64_C(0));
    (void)drx_cmpxchg_strong(obj, expected, expected, drx_memory_order_seq_cst, drx_memory_order_seq_cst, i128, drx_i128);
    return expected;
}

static inline void drx_msvc_atomic_store_i128(drx_i128 volatile* obj, drx_i128 arg) {
    drx_i128 expected = drx_make_i128(INT64_C(0), UINT64_C(0));
    while (!drx_cmpxchg_weak(obj, expected, arg, drx_memory_order_seq_cst, drx_memory_order_seq_cst, i128, drx_i128));
}

#endif /* drx_x86_32 */

#endif /* !drx_c11_atomics && drx_msvc && drx_x86 */

/* ======================== Special Case Intrinsics ========================= */

#if defined(drx_msvc)
#include <intrin.h>
#endif

#if defined(drx_thumb)

static inline void* drx_thumb_windows_teb(void) {
    void* teb = 0;
#if defined(drx_msvc)
    teb = (void*)_MoveFromCoprocessor(15, 0, 13, 0, 2);
#elif defined(drx_gnuc_asm)
    __asm__ ("mrc p15, 0, %[ptr], c13, c0, 2" : [ptr] "=r" (teb));
#endif
    return teb;
}

#elif defined(drx_aarch64)

static inline void* drx_aarch64_windows_teb(void) {
    void* teb = 0;
#if defined(drx_msvc)
    teb = (void*)__readx18qword(0x0);
#elif defined(drx_gnuc_asm)
    __asm__ ("mov %[ptr], x18" : [ptr] "=r" (teb));
#endif
    return teb;
}

#elif defined(drx_x86_32)

static inline void* drx_x86_windows_teb(void) {
    void* teb = 0;
#if defined(drx_msvc)
    teb = (void*)__readfsdword(0x18);
#elif defined(drx_gnuc_asm)
    __asm__ ("movl %%fs:0x18, %[ptr]" : [ptr] "=r" (teb));
#endif
    return teb;
}

#elif defined(drx_x86_64)

static inline void* drx_x86_64_windows_teb(void) {
    void* teb = 0;
#if defined(drx_msvc)
    teb = (void*)__readgsqword(0x30);
#elif defined(drx_gnuc_asm)
    __asm__ ("movq %%gs:0x30, %[ptr]" : [ptr] "=r" (teb));
#endif
    return teb;
}

#endif

#if defined(drx_x86)

static inline void drx_x86_cpuid(uint32_t leaf_id, uint32_t subid, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
#if defined(drx_msvc)
    int cpu_info[4];
    __cpuidex(cpu_info, leaf_id, subid);
    *eax = (uint32_t)cpu_info[0];
    *ebx = (uint32_t)cpu_info[1];
    *ecx = (uint32_t)cpu_info[2];
    *edx = (uint32_t)cpu_info[3];
#elif defined(drx_gnuc_asm)
    __asm__("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "a"(leaf_id), "c"(subid));
#else
    *eax = 0;
    *ebx = 0;
    *ecx = 0;
    *edx = 0;
#endif
}

static inline uint32_t drx_x86_get_xcr0(void) {
#if defined(drx_msvc)
    return (uint32_t)_xgetbv(0);
#elif defined(drx_gnuc_asm)
    uint32_t eax;
    uint32_t edx;
    __asm__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
    return eax;
#else
    *eax = 0;
    *ebx = 0;
    *ecx = 0;
    *edx = 0;
#endif
}

#endif
//#endif