// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifdef _WIN32

#include <drx/CtrlLib/CtrlLib.h>
#include "ActiveX.h"


namespace drx {

bool VLCPlayer::AddTarget(const Txt movie) {
	if (IsPlaying())
		return false;
	Detach();
	Attach(GetHWND());
	{
		IIVLC vlc(this);
		if (!vlc)
			return false;

		VARIANT options;
		VariantInit(&options);
		VariantClear(&options);

		CBSTR bsmovie;
		if(!bsmovie.Set(movie))
			return false;

		if (S_OK != vlc->addTarget(bsmovie, options, VLCPlayListAppendAndGo, 0))
			return false;
	}
	return true;
}

bool VLCPlayer::Play() {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->play())
		return false;
	return true;
}

bool VLCPlayer::Pause() {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->pause())
		return false;
	return true;
}

bool VLCPlayer::PlayFaster() {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->playFaster())
		return false;
	return true;
}

bool VLCPlayer::PlaySlower() {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->playSlower())
		return false;
	return true;
}

bool VLCPlayer::Stop() {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->stop())
		return false;
	return true;
}

bool VLCPlayer::Shuttle(long secs) {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->shuttle(secs))
		return false;
	return true;
}

bool VLCPlayer::IsPlaying() {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	VARIANT_BOOL isplaying;
	if (S_OK != vlc->get_Playing(&isplaying))
		return false;
	return isplaying == VARIANT_TRUE;
}

i32 VLCPlayer::GetVolume() {
	IIVLC vlc(this);
	if (!vlc)
		return -1;

	i32 volume;
	if (S_OK != vlc->get_Volume(&volume))
		return -1;
	return volume;
}

bool VLCPlayer::PutVolume(i32 volume) {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->put_Volume(volume))
		return false;
	return true;
}

i32 VLCPlayer::GetTime() {
	IIVLC vlc(this);
	if (!vlc)
		return -1;

	i32 mseconds;
	if (S_OK != vlc->get_Time(&mseconds))
		return -1;
	return mseconds;
}

bool VLCPlayer::PutTime(i32 mseconds) {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->put_Time(mseconds))
		return false;
	return true;
}

float VLCPlayer::GetPosition() {
	IIVLC vlc(this);
	if (!vlc)
		return -1;

	float position;
	if (S_OK != vlc->get_Position(&position))
		return -1;
	return position;
}

bool VLCPlayer::PutPosition(float position) {
	IIVLC vlc(this);
	if (!vlc)
		return false;

	if (S_OK != vlc->put_Position(position))
		return false;
	return true;
}

i32 VLCPlayer::GetLength() {
	IIVLC vlc(this);
	if (!vlc)
		return -1;

	i32 seconds;
	if (S_OK != vlc->get_Length(&seconds))
		return -1;
	return seconds;
}

}

#endif
