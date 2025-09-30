// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <drx/Core/Core.h>

#include "../bsdiff.h"

namespace drx {

Txt errMsg;

Txt BsGetLastError() {
	return errMsg;
}

bool Err(Txt str) {
	errMsg = str;
	return false;
}

}
