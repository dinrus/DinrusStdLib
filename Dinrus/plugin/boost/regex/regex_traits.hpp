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
  *   FILE         regex_traits.hpp
  *   VERSION      see <Dinrus/plugin/boost/version.hpp>
  *   DESCRIPTION: Declares regular expression traits classes.
  */

#ifndef BOOST_REGEX_TRAITS_HPP
#define BOOST_REGEX_TRAITS_HPP

#ifndef BOOST_REGEX_CONFIG_HPP
#  include <Dinrus/plugin/boost/regex/config.hpp>
#endif

#  ifndef BOOST_REGEX_TRAITS_HPP_INCLUDED
#ifdef BOOST_REGEX_CXX03
#     include <Dinrus/plugin/boost/regex/v4/regex_traits.hpp>
#else
#     include <Dinrus/plugin/boost/regex/v5/regex_traits.hpp>
#endif
#  endif

#endif // include





