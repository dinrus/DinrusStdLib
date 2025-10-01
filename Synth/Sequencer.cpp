#include "Synth.h"

void SoundSequence::Put(double at, i32 i, double volume, double frequency, double duration, bool direct)
{
	SoundEvent& e = At(at).Add();
	e.frequency = (float)frequency;
	e.duration = (float)duration;
	e.volume = (float)volume;
	e.snd = &bank[i];
	if(direct) {
		e.duration = (float)e.snd->op[0].duration;
		e.frequency = (float)e.snd->f;
	}
}

i32 SoundSequence::SoundIndex(const Txt& s)
{
	i32 q = bank.Find(s);
	if(q < 0) {
		q = bank.GetCount();
		bank.Add(s).Load(s);
	}
	return q;
}

void SoundSequence::Put(double at, const Txt& snd, double volume, double frequency, double duration,
 bool direct)
{
	Put(at, SoundIndex(snd), volume, frequency, duration, direct);
}


i32k SEQUENCER_CHANNEL_ID = -123;
std::atomic<const SoundSequence *> sS;

void PlaySequence(const SoundSequence& s)
{
	s.at = s.cursor;
	sS = &s;
}

bool IsPlayingSequence()
{
	return sS.load();
}

void StopSequencer()
{
	sS = NULL;
	StopChannels(SEQUENCER_CHANNEL_ID);
}

void PlayTempSequence(SoundSequence&& s)
{
	static i32 ii;
	static SoundSequence h[2];
	
	h[ii] = pick(s);
	PlaySequence(h[ii]);
	ii = !ii;
}

void Sequencer(i32 chunk_size)
{
	static i32 ch;
	ch += chunk_size;
	if(ch < CHUNK_SIZE)
		return;
	ch -= CHUNK_SIZE;

	i32 PR = 10;
	const SoundSequence *ss = sS.load();
	if(ss) {
		if(ss->at >= ss->event.GetCount()) {
			if(!IsNull(ss->loop) && ss->event.GetCount())
				ss->at = ss->loop;
			else {
				sS = NULL;
				return;
			}
		}
		const Vec<SoundEvent>& e = ss->event[ss->at++];
		for(const SoundEvent& s : e) {
			i32 ii = FindChannel(PR, 1, s.volume);
			if(ii >= 0) {
				Sound snd = *s.snd;
				if(s.frequency > 0) {
					snd.f = s.frequency;
					snd.op[0].volume *= s.volume / 100;
					snd.op[0].duration = s.duration;
				}
				SetChannel(ii, snd, 10, SEQUENCER_CHANNEL_ID);
			}
		}
	}
}
