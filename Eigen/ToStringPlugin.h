// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
	
DRX:Txt ToTxt() const {
	std::stringstream s;
	s << *this;
	return s.str();
}

