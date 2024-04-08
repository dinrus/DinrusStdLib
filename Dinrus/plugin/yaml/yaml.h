#ifndef YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66


#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include <Dinrus/plugin/yaml/parser.h>
#include <Dinrus/plugin/yaml/emitter.h>
#include <Dinrus/plugin/yaml/emitterstyle.h>
#include <Dinrus/plugin/yaml/stlemitter.h>
#include <Dinrus/plugin/yaml/exceptions.h>

#include <Dinrus/plugin/yaml/node/node.h>
#include <Dinrus/plugin/yaml/node/impl.h>
#include <Dinrus/plugin/yaml/node/convert.h>
#include <Dinrus/plugin/yaml/node/iterator.h>
#include <Dinrus/plugin/yaml/node/detail/impl.h>
#include <Dinrus/plugin/yaml/node/parse.h>
#include<Dinrus/plugin/yaml/node/emit.h>

#endif  // YAML_H_62B23520_7C8E_11DE_8A39_0800200C9A66
