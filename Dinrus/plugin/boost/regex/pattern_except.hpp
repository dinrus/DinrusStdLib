/*
 *
 * Copyright (c) 1998-2002
 * John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
 
 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         pattern_except.hpp
  *   VERSION      see <Dinrus/plugin/boost/version.hpp>
  *   DESCRIPTION: Declares pattern-matching exception classes.
  */

#ifndef BOOST_RE_PAT_EXCEPT_HPP
#define BOOST_RE_PAT_EXCEPT_HPP

#ifndef BOOST_REGEX_CONFIG_HPP
#include <Dinrus/plugin/boost/regex/config.hpp>
#endif

#ifdef BOOST_REGEX_CXX03
#include <Dinrus/plugin/boost/regex/v4/pattern_except.hpp>
#else
#include <Dinrus/plugin/boost/regex/v5/pattern_except.hpp>
#endif

#endif
