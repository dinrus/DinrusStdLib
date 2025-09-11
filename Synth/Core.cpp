#include "Synth.h"

#include <SDL2/SDL.h>


SoundGen app_sch[NUM_CHANNELS]; // to avoid lengthy locking, keep copy under different mutex
SoundGen gen_sch[NUM_CHANNELS]; // this is copied to generator and used to actually generate the sound

INITBLOCK {
	for(i32 i = 0; i < NUM_CHANNELS; i++) // initialize wave_tab
		for(i32 j = 0; j < OPCOUNT; j++) {
			app_sch[i].op[j].Comp();
			gen_sch[i].op[j].Comp();
		}
}

i32      in_sch_serial;

#if 1
struct AppSoundLock {
	AppSoundLock() { SDL_LockAudio(); }
	~AppSoundLock() { SDL_UnlockAudio(); }
};

struct GenSoundLock {
	GenSoundLock() {}
	~GenSoundLock() {}
};
#else
SpinLock s_in_sch_lock;

struct AppSoundLock {
	AppSoundLock() { s_in_sch_lock.Enter(); }
	~AppSoundLock() { s_in_sch_lock.Leave(); }
};

struct GenSoundLock {
	GenSoundLock() { s_in_sch_lock.Enter(); }
	~GenSoundLock() { s_in_sch_lock.Leave(); }
};
#endif

void SetChannel(i32 chi, const Sound& c, i32 priority, i32 id)
{
	AppSoundLock __;
	SoundGen& ch = app_sch[chi];
	ch.Start(c);
	ch.priority = priority;
	ch.serial = ++in_sch_serial;
	ch.id = id;
}

void StopChannels(i32 id)
{
	AppSoundLock __;
	for(i32 i = 0; i < NUM_CHANNELS; i++) {
		SoundGen& ch = app_sch[i];
		if(ch.id == id) {
			ch.serial = ++in_sch_serial;
			ch.current_volume = app_sch[i].op[0].volume = 0;
		}
	}
}

void SetChannelVolume(i32 chi, double volume)
{
	AppSoundLock __;
	SoundGen& ch = app_sch[chi];
	ch.param_serial = ++in_sch_serial;
	ch.op[0].volume = pow(10, (volume - 100) / 40);
}

i32 FindChannel(i32 priority, i32 from, double new_volume)
{
	new_volume = pow(10, (new_volume - 100) / 40);
	AppSoundLock __;
	i32    besti = -1;
	i32    best_priority = INT_MAX;
	double best_volume = 100;
	for(i32 i = from; i < NUM_CHANNELS; i++) {
		SoundGen& ch = app_sch[i];
//		if(ch.current_volume < 0.001) {
//			besti = i;
//			break;
//		}
		if(/*ch.priority <= priority &&
		   (ch.priority < best_priority || ch.priority == best_priority && */ch.current_volume < best_volume) {
		       besti = i;
//		       best_priority = ch.priority;
		       best_volume = ch.current_volume;
		}
	}
//	return besti;
	return best_volume < new_volume ? besti : -1;
//	DDUMP(best_volume);
//	DDUMP(besti);
/*	if(besti < 0)
		for(i32 i = from; i < NUM_CHANNELS; i++)
			if(app_sch[i].current_volume < 0.01) {
				besti = i;
				break;
			}
	return besti;*/
}

extern void Sequencer(i32 chunk_size);

float global_volume = 1;

void SetGlobalVolume(float vol)
{
	AppSoundLock __;
	global_volume = vol;
}

void MyAudioCallback(uk , Uint8 *stream, i32 len)
{
	i32 chunk_size = len / (2 * sizeof(float));
	Sequencer(chunk_size);
	{
#if 0
		RLOG("====================================");
#endif
		GenSoundLock __;
		for(i32 i = 0; i < NUM_CHANNELS; i++) {
			if(app_sch[i].serial != gen_sch[i].serial)
				gen_sch[i] = app_sch[i];
			if(app_sch[i].param_serial != gen_sch[i].param_serial) {
				gen_sch[i].op[0].volume = app_sch[i].op[0].volume;
				gen_sch[i].current_volume = 1;
				app_sch[i].param_serial = gen_sch[i].param_serial;
			}
#if 0
			RLOG(i);
			RLOG(gen_sch[i]);
#endif
		}
	}
	
	float *d = (float *)stream;

	memset(d, 0, len);
	for(i32 j = 0; j < NUM_CHANNELS; j++) {
		SoundGen& ch = gen_sch[j];
		d = (float *)stream;
		double v = 0;
		for(i32 i = 0; i < chunk_size; i++) {
			float h = ch.Get();
			*d++ += ch.lpan * h;
			*d++ += ch.rpan * h;
		}
	}

	float *e = (float *)stream + 2 * chunk_size;
	for(float *d = (float *)stream; d < e; d++)
		*d = clamp(*d * global_volume, -1.0f, 1.0f);

	{
		GenSoundLock __;
		for(i32 i = 0; i < NUM_CHANNELS; i++) {
			app_sch[i].current_volume = gen_sch[i].current_volume;
		}
	}
}

void InitSoundSynth(bool initsdl)
{
	if(initsdl && SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
		return;
	
	SDL_AudioSpec want;
	memset(&want, 0, sizeof(want));
	want.freq = 44100;
	want.format = AUDIO_F32SYS;
	want.channels = 2;
	want.samples = CHUNK_SIZE;
	want.callback = MyAudioCallback;
	
	if(SDL_OpenAudio(&want, NULL) < 0)
	   LOG("Failed to open audio: " + (Txt)SDL_GetError());
	
	SDL_PauseAudio(0);
}

void CloseSoundSynth(bool exitsdl)
{
	SDL_CloseAudio();
	if(exitsdl)
		SDL_Quit();
}
