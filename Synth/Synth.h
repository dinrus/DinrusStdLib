#ifndef _Synth_Synth_h
#define _Synth_Synth_h

#include <drx/Core/Core.h>

using namespace drx;

enum FORMS {
	WAVEFORM_SIN = 0,
	WAVEFORM_SQUARE = 1,
	WAVEFORM_TRIANGLE = 2,
	WAVEFORM_SAWTOOTH = 3,

	WAVEFORM_FIRSTSAMPLE = 4,
	WAVEFORM_SAXOPHONE = WAVEFORM_FIRSTSAMPLE,
	WAVEFORM_VIOLIN = 5,
	WAVEFORM_DOUBLEBASS = 6,
	WAVEFORM_BANJO = 7,
	WAVEFORM_TRUMPET = 8,
	WAVEFORM_LASTSAMPLE = 8,

	WAVEFORM_BROWN = 100,
	WAVEFORM_WHITE = 101,
};

struct FMOP {
	double duration = 99000;
	double volume = 0;

	double f = 1;
	double fdrift = 0;

	double attack = 100;
	double decay = 100;
	double sustain = 100;
	double release = 100;
	
	i32    waveform = WAVEFORM_SIN;
	
	Txt Save() const;
	tukk Load(tukk s);
};

#define OPCOUNT 5

struct Sound {
	double f = 440;
	FMOP   op[OPCOUNT];
	double pan = 0.5;
	
	Txt Save() const;
	void   Load(tukk s);
	
	Sound();
};

struct SoundGen {
	struct FMOPGen : FMOP {
		i32     p;
		double  v;
		double  n;
		double *wave_tab;
		
		void   Start() { v = 1e-3; p = 0; n = 0; }
		void   Comp();
		Txt ToTxt() const;
		
		double Evaluate(i32 t, double mf, double mod, double& current_volume);
	};
	
	i32      serial;
	i32      param_serial;
	i32      id;
	i32      priority;
	double   f = 440;
	float    lpan = 0.5f;
	float    rpan = 0.5f;
	i32      t;
	i32      delay;
	FMOPGen  op[OPCOUNT];
	float    feedback[8192];
	double   current_volume = 0;
	double   lfo_mod = 0;
	
	void     Start(const Sound& s);
	float    Get();
	Txt   ToTxt() const;
};

#define CHUNK_SIZE   512
#define NUM_CHANNELS 20

void InitSoundSynth(bool initsdl = true);
void CloseSoundSynth(bool exitsdl = true);

void SetChannel(i32 chi, const Sound& c, i32 priority = INT_MAX, i32 id = 0);
void SetChannelVolume(i32 chi, double volume);
void StopChannelById(i32 id);
i32  FindChannel(i32 priority, i32 from, double new_volume);
void StopChannels(i32 id);

void SetGlobalVolume(float vol);

struct SoundEvent : Moveable<SoundEvent> {
	Sound *snd;
	float  duration;
	float  frequency;
	float  volume;
};

struct SoundSequence {
	mutable i32  at = 0;
	i32 cursor = 0;
	i32 loop = Null;
	ArrayMap<Txt, Sound>    bank;
	Vec<Vec<SoundEvent>> event;

	i32                 GetAt(double at)  { return (i32)(at * 44100 / 512); }
	Vec<SoundEvent>& At(double at)     { return event.At(GetAt(at)); }
	void                LoopAt(double at) { loop = GetAt(at); }
	i32  SoundIndex(const Txt& s);
	void Put(double at, i32 i,
	         double volume, double freqency, double duration, bool direct = false);
	void Put(double at, const Txt& snd,
	         double volume, double freqency, double duration, bool direct = false);
};

void PlaySequence(const SoundSequence& s);
void PlayTempSequence(SoundSequence&& s);

void StopSequencer();
bool IsPlayingSequence();

SoundSequence ParseQSF(const Txt& data);

#endif
