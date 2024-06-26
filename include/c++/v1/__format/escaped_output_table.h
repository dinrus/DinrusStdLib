// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// WARNING, this entire header is generated by
// utils/generate_escaped_output_table.py
// DO NOT MODIFY!

// UNICODE, INC. LICENSE AGREEMENT - DATA FILES AND SOFTWARE
//
// See Terms of Use <https://www.unicode.org/copyright.html>
// for definitions of Unicode Inc.'s Data Files and Software.
//
// NOTICE TO USER: Carefully read the following legal agreement.
// BY DOWNLOADING, INSTALLING, COPYING OR OTHERWISE USING UNICODE INC.'S
// DATA FILES ("DATA FILES"), AND/OR SOFTWARE ("SOFTWARE"),
// YOU UNEQUIVOCALLY ACCEPT, AND AGREE TO BE BOUND BY, ALL OF THE
// TERMS AND CONDITIONS OF THIS AGREEMENT.
// IF YOU DO NOT AGREE, DO NOT DOWNLOAD, INSTALL, COPY, DISTRIBUTE OR USE
// THE DATA FILES OR SOFTWARE.
//
// COPYRIGHT AND PERMISSION NOTICE
//
// Copyright (c) 1991-2022 Unicode, Inc. РНЦП Динрус.
// Distributed under the Terms of Use in https://www.unicode.org/copyright.html.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of the Unicode data files and any associated documentation
// (the "Data Files") or Unicode software and any associated documentation
// (the "Software") to deal in the Data Files or Software
// without restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, and/or sell copies of
// the Data Files or Software, and to permit persons to whom the Data Files
// or Software are furnished to do so, provided that either
// (a) this copyright and permission notice appear with all copies
// of the Data Files or Software, or
// (b) this copyright and permission notice appear in associated
// Documentation.
//
// THE DATA FILES AND SOFTWARE ARE PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT OF THIRD PARTY RIGHTS.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS
// NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL
// DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THE DATA FILES OR SOFTWARE.
//
// Except as contained in this notice, the name of a copyright holder
// shall not be used in advertising or otherwise to promote the sale,
// use or other dealings in these Data Files or Software without prior
// written authorization of the copyright holder.

#ifndef _LIBCPP___FORMAT_ESCAPED_OUTPUT_TABLE_H
#define _LIBCPP___FORMAT_ESCAPED_OUTPUT_TABLE_H

#include <__algorithm/ranges_upper_bound.h>
#include <__config>
#include <cstddef>
#include <cstdint>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 20

namespace __escaped_output_table {

/// The entries of the characters to escape in format's debug string.
///
/// Contains the entries for [format.string.escaped]/2.2.1.2.1
///   CE is a Unicode encoding and C corresponds to either a UCS scalar value
///   whose Unicode property General_Category has a value in the groups
///   Separator (Z) or Other (C) or to a UCS scalar value which has the Unicode
///   property Grapheme_Extend=Yes, as described by table 12 of UAX #44
///
/// Separator (Z) consists of General_Category
/// - Space_Separator,
/// - Line_Separator,
/// - Paragraph_Separator.
///
/// Other (C) consists of General_Category
/// - Control,
/// - Format,
/// - Surrogate,
/// - Private_Use,
/// - Unassigned.
///
/// The data is generated from
/// - https://www.unicode.org/Public/UCD/latest/ucd/DerivedCoreProperties.txt
/// - https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedGeneralCategory.txt
///
/// The table is similar to the table
///  __extended_grapheme_custer_property_boundary::__entries
/// which explains the details of these classes. The only difference is this
/// table lacks a property, thus having more bits available for the size.
///
/// The data has 2 values:
/// - bits [0, 10] The size of the range, allowing 2048 elements.
/// - bits [11, 31] The lower bound code point of the range. The upper bound of
///   the range is lower bound + size.
inline constexpr uint32_t __entries[893] = {
    0x00000020,
    0x0003f821,
    0x00056800,
    0x0018006f,
    0x001bc001,
    0x001c0003,
    0x001c5800,
    0x001c6800,
    0x001d1000,
    0x00241806,
    0x00298000,
    0x002ab801,
    0x002c5801,
    0x002c802d,
    0x002df800,
    0x002e0801,
    0x002e2001,
    0x002e3808,
    0x002f5803,
    0x002fa810,
    0x0030800a,
    0x0030e000,
    0x00325814,
    0x00338000,
    0x0036b007,
    0x0036f805,
    0x00373801,
    0x00375003,
    0x00387001,
    0x00388800,
    0x0039801c,
    0x003d300a,
    0x003d900d,
    0x003f5808,
    0x003fd802,
    0x0040b003,
    0x0040d808,
    0x00412802,
    0x00414806,
    0x0041f800,
    0x0042c804,
    0x0042f800,
    0x00435804,
    0x00447810,
    0x00465038,
    0x0049d000,
    0x0049e000,
    0x004a0807,
    0x004a6800,
    0x004a8806,
    0x004b1001,
    0x004c0800,
    0x004c2000,
    0x004c6801,
    0x004c8801,
    0x004d4800,
    0x004d8800,
    0x004d9802,
    0x004dd002,
    0x004df000,
    0x004e0805,
    0x004e4801,
    0x004e6800,
    0x004e780c,
    0x004ef000,
    0x004f1003,
    0x004ff004,
    0x00502000,
    0x00505803,
    0x00508801,
    0x00514800,
    0x00518800,
    0x0051a000,
    0x0051b800,
    0x0051d003,
    0x00520817,
    0x0052e800,
    0x0052f806,
    0x00538001,
    0x0053a800,
    0x0053b80b,
    0x00542000,
    0x00547000,
    0x00549000,
    0x00554800,
    0x00558800,
    0x0055a000,
    0x0055d002,
    0x00560807,
    0x00565000,
    0x00566802,
    0x0056880e,
    0x00571003,
    0x00579006,
    0x0057d007,
    0x00582000,
    0x00586801,
    0x00588801,
    0x00594800,
    0x00598800,
    0x0059a000,
    0x0059d002,
    0x0059f001,
    0x005a0805,
    0x005a4801,
    0x005a680e,
    0x005af000,
    0x005b1003,
    0x005bc00a,
    0x005c2000,
    0x005c5802,
    0x005c8800,
    0x005cb002,
    0x005cd800,
    0x005ce800,
    0x005d0002,
    0x005d2802,
    0x005d5802,
    0x005dd004,
    0x005e0000,
    0x005e1802,
    0x005e4800,
    0x005e6802,
    0x005e8814,
    0x005fd805,
    0x00602000,
    0x00606800,
    0x00608800,
    0x00614800,
    0x0061d002,
    0x0061f002,
    0x00622812,
    0x0062d801,
    0x0062f001,
    0x00631003,
    0x00638006,
    0x00640800,
    0x00646800,
    0x00648800,
    0x00654800,
    0x0065a000,
    0x0065d002,
    0x0065f800,
    0x00661000,
    0x00662801,
    0x00664800,
    0x00666010,
    0x0066f800,
    0x00671003,
    0x00678000,
    0x0067a00d,
    0x00686800,
    0x00688800,
    0x0069d801,
    0x0069f000,
    0x006a0804,
    0x006a4800,
    0x006a6800,
    0x006a8003,
    0x006ab800,
    0x006b1003,
    0x006c0001,
    0x006c2000,
    0x006cb802,
    0x006d9000,
    0x006de000,
    0x006df001,
    0x006e3808,
    0x006e9005,
    0x006ef806,
    0x006f8001,
    0x006fa80b,
    0x00718800,
    0x0071a00a,
    0x00723807,
    0x0072e024,
    0x00741800,
    0x00742800,
    0x00745800,
    0x00752000,
    0x00753000,
    0x00758800,
    0x0075a008,
    0x0075f001,
    0x00762800,
    0x00763808,
    0x0076d001,
    0x0077001f,
    0x0078c001,
    0x0079a800,
    0x0079b800,
    0x0079c800,
    0x007a4000,
    0x007b6811,
    0x007c0004,
    0x007c3001,
    0x007c6830,
    0x007e3000,
    0x007e6800,
    0x007ed824,
    0x00816803,
    0x00819005,
    0x0081c801,
    0x0081e801,
    0x0082c001,
    0x0082f002,
    0x00838803,
    0x00841000,
    0x00842801,
    0x00846800,
    0x0084e800,
    0x00863000,
    0x00864004,
    0x00867001,
    0x00924800,
    0x00927001,
    0x0092b800,
    0x0092c800,
    0x0092f001,
    0x00944800,
    0x00947001,
    0x00958800,
    0x0095b001,
    0x0095f800,
    0x00960800,
    0x00963001,
    0x0096b800,
    0x00988800,
    0x0098b001,
    0x009ad804,
    0x009be802,
    0x009cd005,
    0x009fb001,
    0x009ff001,
    0x00b40000,
    0x00b4e802,
    0x00b7c806,
    0x00b89002,
    0x00b8b008,
    0x00b99001,
    0x00b9b808,
    0x00ba900d,
    0x00bb6800,
    0x00bb880e,
    0x00bda001,
    0x00bdb806,
    0x00be3000,
    0x00be480a,
    0x00bee802,
    0x00bf5005,
    0x00bfd005,
    0x00c05804,
    0x00c0d005,
    0x00c3c806,
    0x00c42801,
    0x00c54800,
    0x00c55804,
    0x00c7b009,
    0x00c8f803,
    0x00c93801,
    0x00c96003,
    0x00c99000,
    0x00c9c806,
    0x00ca0802,
    0x00cb7001,
    0x00cba80a,
    0x00cd6003,
    0x00ce5005,
    0x00ced802,
    0x00d0b801,
    0x00d0d802,
    0x00d2b000,
    0x00d2c008,
    0x00d31000,
    0x00d32807,
    0x00d3980c,
    0x00d45005,
    0x00d4d005,
    0x00d57055,
    0x00d9a006,
    0x00d9e000,
    0x00da1000,
    0x00da6802,
    0x00db5808,
    0x00dbf802,
    0x00dd1003,
    0x00dd4001,
    0x00dd5802,
    0x00df3000,
    0x00df4001,
    0x00df6800,
    0x00df7802,
    0x00dfa007,
    0x00e16007,
    0x00e1b004,
    0x00e25002,
    0x00e44806,
    0x00e5d801,
    0x00e6400a,
    0x00e6a00c,
    0x00e71006,
    0x00e76800,
    0x00e7a000,
    0x00e7c001,
    0x00e7d804,
    0x00ee003f,
    0x00f8b001,
    0x00f8f001,
    0x00fa3001,
    0x00fa7001,
    0x00fac000,
    0x00fad000,
    0x00fae000,
    0x00faf000,
    0x00fbf001,
    0x00fda800,
    0x00fe2800,
    0x00fea001,
    0x00fee000,
    0x00ff8001,
    0x00ffa800,
    0x00fff810,
    0x01014007,
    0x0102f810,
    0x01039001,
    0x01047800,
    0x0104e802,
    0x0106083e,
    0x010c6003,
    0x01213818,
    0x01225814,
    0x015ba001,
    0x015cb000,
    0x01677802,
    0x0167a004,
    0x01693000,
    0x01694004,
    0x01697001,
    0x016b4006,
    0x016b880e,
    0x016cb808,
    0x016d3800,
    0x016d7800,
    0x016db800,
    0x016df800,
    0x016e3800,
    0x016e7800,
    0x016eb800,
    0x016ef820,
    0x0172f021,
    0x0174d000,
    0x0177a00b,
    0x017eb019,
    0x017fe004,
    0x01815005,
    0x01820000,
    0x0184b803,
    0x01880004,
    0x01898000,
    0x018c7800,
    0x018f200b,
    0x0190f800,
    0x05246802,
    0x05263808,
    0x05316013,
    0x05337803,
    0x0533a009,
    0x0534f001,
    0x05378001,
    0x0537c007,
    0x053e5804,
    0x053e9000,
    0x053ea000,
    0x053ed017,
    0x05401000,
    0x05403000,
    0x05405800,
    0x05412801,
    0x05416003,
    0x0541d005,
    0x0543c007,
    0x05462009,
    0x0546d017,
    0x0547f800,
    0x05493007,
    0x054a380a,
    0x054aa00a,
    0x054be805,
    0x054d9800,
    0x054db003,
    0x054de001,
    0x054e7000,
    0x054ed003,
    0x054f2800,
    0x054ff800,
    0x05514805,
    0x05518801,
    0x0551a80a,
    0x05521800,
    0x05526000,
    0x05527001,
    0x0552d001,
    0x0553e000,
    0x05558000,
    0x05559002,
    0x0555b801,
    0x0555f001,
    0x05560800,
    0x05561817,
    0x05576001,
    0x0557b00a,
    0x05583801,
    0x05587801,
    0x0558b808,
    0x05593800,
    0x05597800,
    0x055b6003,
    0x055f2800,
    0x055f4000,
    0x055f6802,
    0x055fd005,
    0x06bd200b,
    0x06be3803,
    0x06bfe7ff,
    0x06ffe7ff,
    0x073fe7ff,
    0x077fe7ff,
    0x07bfe103,
    0x07d37001,
    0x07d6d025,
    0x07d8380b,
    0x07d8c004,
    0x07d8f000,
    0x07d9b800,
    0x07d9e800,
    0x07d9f800,
    0x07da1000,
    0x07da2800,
    0x07de180f,
    0x07ec8001,
    0x07ee4006,
    0x07ee801f,
    0x07f0000f,
    0x07f0d015,
    0x07f29800,
    0x07f33800,
    0x07f36003,
    0x07f3a800,
    0x07f7e803,
    0x07fcf001,
    0x07fdf802,
    0x07fe4001,
    0x07fe8001,
    0x07fec001,
    0x07fee802,
    0x07ff3800,
    0x07ff780c,
    0x07fff001,
    0x08006000,
    0x08013800,
    0x0801d800,
    0x0801f000,
    0x08027001,
    0x0802f021,
    0x0807d804,
    0x08081803,
    0x0809a002,
    0x080c7800,
    0x080ce802,
    0x080d082e,
    0x080fe882,
    0x0814e802,
    0x0816880f,
    0x0817e003,
    0x08192008,
    0x081a5804,
    0x081bb009,
    0x081cf000,
    0x081e2003,
    0x081eb029,
    0x0824f001,
    0x08255005,
    0x0826a003,
    0x0827e003,
    0x08294007,
    0x082b200a,
    0x082bd800,
    0x082c5800,
    0x082c9800,
    0x082cb000,
    0x082d1000,
    0x082d9000,
    0x082dd000,
    0x082de842,
    0x0839b808,
    0x083ab009,
    0x083b4017,
    0x083c3000,
    0x083d8800,
    0x083dd844,
    0x08403001,
    0x08404800,
    0x0841b000,
    0x0841c802,
    0x0841e801,
    0x0842b000,
    0x0844f807,
    0x0845802f,
    0x08479800,
    0x0847b004,
    0x0848e002,
    0x0849d004,
    0x084a003f,
    0x084dc003,
    0x084e8001,
    0x0850080e,
    0x0850a000,
    0x0850c000,
    0x0851b009,
    0x08524806,
    0x0852c806,
    0x0855001f,
    0x08572805,
    0x0857b808,
    0x0859b002,
    0x085ab001,
    0x085b9804,
    0x085c9006,
    0x085ce80b,
    0x085d804f,
    0x08624836,
    0x0865980c,
    0x08679806,
    0x0869200b,
    0x0869d125,
    0x0873f800,
    0x08755002,
    0x08757001,
    0x0875904d,
    0x08794007,
    0x087a300a,
    0x087ad015,
    0x087c1003,
    0x087c5025,
    0x087e6013,
    0x087fb808,
    0x08800800,
    0x0881c00e,
    0x08827003,
    0x08838000,
    0x08839801,
    0x0883b00b,
    0x08859803,
    0x0885c801,
    0x0885e800,
    0x0886100d,
    0x08874806,
    0x0887d008,
    0x08893804,
    0x08896808,
    0x088a4007,
    0x088b9800,
    0x088bb80a,
    0x088db008,
    0x088e4803,
    0x088e7800,
    0x088f0000,
    0x088fa80a,
    0x08909000,
    0x08917802,
    0x0891a000,
    0x0891b001,
    0x0891f000,
    0x0892083e,
    0x08943800,
    0x08944800,
    0x08947000,
    0x0894f000,
    0x08955005,
    0x0896f800,
    0x0897180c,
    0x0897d007,
    0x08982000,
    0x08986801,
    0x08988801,
    0x08994800,
    0x08998800,
    0x0899a000,
    0x0899d002,
    0x0899f000,
    0x089a0000,
    0x089a2801,
    0x089a4801,
    0x089a7001,
    0x089a880b,
    0x089b209b,
    0x08a1c007,
    0x08a21002,
    0x08a23000,
    0x08a2e000,
    0x08a2f000,
    0x08a3101d,
    0x08a58000,
    0x08a59805,
    0x08a5d000,
    0x08a5e800,
    0x08a5f801,
    0x08a61001,
    0x08a64007,
    0x08a6d0a5,
    0x08ad7800,
    0x08ad9005,
    0x08ade001,
    0x08adf801,
    0x08aee023,
    0x08b19807,
    0x08b1e800,
    0x08b1f801,
    0x08b2280a,
    0x08b2d005,
    0x08b36812,
    0x08b55800,
    0x08b56800,
    0x08b58005,
    0x08b5b800,
    0x08b5d005,
    0x08b65035,
    0x08b8d804,
    0x08b91003,
    0x08b93808,
    0x08ba38b8,
    0x08c17808,
    0x08c1c801,
    0x08c1e063,
    0x08c7980b,
    0x08c83801,
    0x08c85001,
    0x08c8a000,
    0x08c8b800,
    0x08c98000,
    0x08c9b000,
    0x08c9c803,
    0x08c9f000,
    0x08ca1800,
    0x08ca3808,
    0x08cad045,
    0x08cd4001,
    0x08cea007,
    0x08cf0000,
    0x08cf281a,
    0x08d00809,
    0x08d19805,
    0x08d1d803,
    0x08d23808,
    0x08d28805,
    0x08d2c802,
    0x08d4500c,
    0x08d4c001,
    0x08d5180c,
    0x08d7c806,
    0x08d850f5,
    0x08e04800,
    0x08e1800d,
    0x08e1f800,
    0x08e23009,
    0x08e36802,
    0x08e48018,
    0x08e55006,
    0x08e59001,
    0x08e5a84a,
    0x08e83800,
    0x08e85000,
    0x08e98814,
    0x08ea3808,
    0x08ead005,
    0x08eb3000,
    0x08eb4800,
    0x08ec7803,
    0x08eca800,
    0x08ecb800,
    0x08ecc806,
    0x08ed5135,
    0x08f79801,
    0x08f7c808,
    0x08f88800,
    0x08f9b007,
    0x08fa0000,
    0x08fa1000,
    0x08fad055,
    0x08fd880e,
    0x08ff900c,
    0x091cd065,
    0x09237800,
    0x0923a80a,
    0x092a27ff,
    0x096a224b,
    0x097f980c,
    0x09a18010,
    0x09a23fff,
    0x09e23fb8,
    0x0a323fff,
    0x0a723fff,
    0x0ab23fff,
    0x0af23fff,
    0x0b3239b8,
    0x0b51c806,
    0x0b52f800,
    0x0b535003,
    0x0b55f800,
    0x0b565005,
    0x0b577006,
    0x0b57b009,
    0x0b598006,
    0x0b5a3009,
    0x0b5ad000,
    0x0b5b1000,
    0x0b5bc004,
    0x0b5c82af,
    0x0b74d864,
    0x0b7a5804,
    0x0b7c400a,
    0x0b7d003f,
    0x0b7f200b,
    0x0b7f900d,
    0x0c3fc007,
    0x0c66b029,
    0x0c684fff,
    0x0ca84fff,
    0x0ce84fff,
    0x0d284fff,
    0x0d684ae6,
    0x0d7fa000,
    0x0d7fe000,
    0x0d7ff800,
    0x0d89180e,
    0x0d89981c,
    0x0d8a9801,
    0x0d8ab00d,
    0x0d8b4007,
    0x0d97e7ff,
    0x0dd7e103,
    0x0de35804,
    0x0de3e802,
    0x0de44806,
    0x0de4d001,
    0x0de4e801,
    0x0de507ff,
    0x0e2507ff,
    0x0e6502af,
    0x0e7e203b,
    0x0e87b009,
    0x0e893801,
    0x0e8b2800,
    0x0e8b3802,
    0x0e8b7014,
    0x0e8c2806,
    0x0e8d5003,
    0x0e8f5814,
    0x0e921002,
    0x0e923079,
    0x0e96a00b,
    0x0e97a00b,
    0x0e9ab808,
    0x0e9bc886,
    0x0ea2a800,
    0x0ea4e800,
    0x0ea50001,
    0x0ea51801,
    0x0ea53801,
    0x0ea56800,
    0x0ea5d000,
    0x0ea5e000,
    0x0ea62000,
    0x0ea83000,
    0x0ea85801,
    0x0ea8a800,
    0x0ea8e800,
    0x0ea9d000,
    0x0ea9f800,
    0x0eaa2800,
    0x0eaa3802,
    0x0eaa8800,
    0x0eb53001,
    0x0ebe6001,
    0x0ed00036,
    0x0ed1d831,
    0x0ed3a800,
    0x0ed42000,
    0x0ed46473,
    0x0ef8f805,
    0x0ef95904,
    0x0f037091,
    0x0f096809,
    0x0f09f001,
    0x0f0a5003,
    0x0f0a813f,
    0x0f157011,
    0x0f176003,
    0x0f17d004,
    0x0f1801cf,
    0x0f276003,
    0x0f27d2e5,
    0x0f3f3800,
    0x0f3f6000,
    0x0f3f7800,
    0x0f3ff800,
    0x0f462801,
    0x0f46802f,
    0x0f4a2006,
    0x0f4a6003,
    0x0f4ad003,
    0x0f4b0310,
    0x0f65a84b,
    0x0f69f0c1,
    0x0f702000,
    0x0f710000,
    0x0f711800,
    0x0f712801,
    0x0f714000,
    0x0f719800,
    0x0f71c000,
    0x0f71d000,
    0x0f71e005,
    0x0f721803,
    0x0f724000,
    0x0f725000,
    0x0f726000,
    0x0f728000,
    0x0f729800,
    0x0f72a801,
    0x0f72c000,
    0x0f72d000,
    0x0f72e000,
    0x0f72f000,
    0x0f730000,
    0x0f731800,
    0x0f732801,
    0x0f735800,
    0x0f739800,
    0x0f73c000,
    0x0f73e800,
    0x0f73f800,
    0x0f745000,
    0x0f74e004,
    0x0f752000,
    0x0f755000,
    0x0f75e033,
    0x0f77910d,
    0x0f816003,
    0x0f84a00b,
    0x0f857801,
    0x0f860000,
    0x0f868000,
    0x0f87b009,
    0x0f8d7037,
    0x0f90180c,
    0x0f91e003,
    0x0f924806,
    0x0f92900d,
    0x0f933099,
    0x0fb6c003,
    0x0fb76802,
    0x0fb7e802,
    0x0fbbb803,
    0x0fbed005,
    0x0fbf6003,
    0x0fbf880e,
    0x0fc06003,
    0x0fc24007,
    0x0fc2d005,
    0x0fc44007,
    0x0fc57001,
    0x0fc5904d,
    0x0fd2a00b,
    0x0fd37001,
    0x0fd3e802,
    0x0fd44806,
    0x0fd5f000,
    0x0fd63007,
    0x0fd6e003,
    0x0fd74806,
    0x0fd7c806,
    0x0fdc9800,
    0x0fde5824,
    0x0fdfd405,
    0x1537001f,
    0x15b9d005,
    0x15c0f001,
    0x1675100d,
    0x175f0fff,
    0x179f0c1e,
    0x17d0f5e1,
    0x189a5804};

/// At the end of the valid Unicode code points space a lot of code points are
/// either reserved or a noncharacter. Adding all these entries to the
/// lookup table would add 446 entries to the table (in Unicode 14).
/// Instead the only the start of the region is stored, every code point in
/// this region needs to be escaped.
inline constexpr uint32_t __unallocated_region_lower_bound = 0x000323b0;

/// Returns whether the code unit needs to be escaped.
///
/// \pre The code point is a valid Unicode code point.
[[nodiscard]] _LIBCPP_HIDE_FROM_ABI constexpr bool __needs_escape(const char32_t __code_point) noexcept {
  // Since __unallocated_region_lower_bound contains the unshifted range do the
  // comparison without shifting.
  if (__code_point >= __unallocated_region_lower_bound)
    return true;

  ptrdiff_t __i = std::ranges::upper_bound(__entries, (__code_point << 11) | 0x7ffu) - __entries;
  if (__i == 0)
    return false;

  --__i;
  uint32_t __upper_bound = (__entries[__i] >> 11) + (__entries[__i] & 0x7ffu);
  return __code_point <= __upper_bound;
}

} // namespace __escaped_output_table

#endif //_LIBCPP_STD_VER > 20

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___FORMAT_ESCAPED_OUTPUT_TABLE_H
