/*
    SPDX-FileCopyrightText: 2023 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <knscore/knewstuffcore_export.h>

#if KNEWSTUFFCORE_ENABLE_DEPRECATED_SINCE(5, 29)
#if KNEWSTUFFCORE_DEPRECATED_WARNINGS_SINCE >= 0x051d00
#pragma message("Deprecated header. Since 5.29, use #include <knewstuff_version.h> & KNEWSTUFF_VERSION instead")
#endif
#else
#error "Include of deprecated header is disabled"
#endif

#ifndef KNEWSTUFFCORE_VERSION_H
#define KNEWSTUFFCORE_VERSION_H

#define KNEWSTUFFCORE_VERSION_STRING "5.110.0"
#define KNEWSTUFFCORE_VERSION_MAJOR 5
#define KNEWSTUFFCORE_VERSION_MINOR 110
#define KNEWSTUFFCORE_VERSION_PATCH 0
#define KNEWSTUFFCORE_VERSION ((5<<16)|(110<<8)|(0))

#endif
