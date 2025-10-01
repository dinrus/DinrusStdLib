#include "Synth.h"

#include <drx/Core/Core.h>

using namespace drx;

enum {
	WAVECOUNT = 2048,
	WAVEMASK = 2047,
	
	NOISECOUNT = 1024 * 1024 * 4,
	NOISEMASK = NOISECOUNT - 1,
};

struct WaveForm {
	float wave[2048];
};

float BrownNoise[NOISECOUNT];

INITBLOCK {
	float p;
	for(i32 i = 0; i < NOISECOUNT; i++) {
		p = clamp(p + (0.02 * (Randomf() * 2 - 1)) / 1.02, -1/3.5, 1/3.5);
		BrownNoise[i] = p;
	}
}

void MakeWave(tukk s, WaveForm& h)
{
	for(i32 i = 0; i < WAVECOUNT; i++)
		h.wave[i] = 0;
	
	try {
		CParser p(s);
		i32 harm = 1;
		i32 i = 0;
		double a = 1;
		auto fn0 = [&]() -> double { return sin(M_2PI * i * harm / WAVECOUNT); };
		Function<double ()> fn = fn0;
		for(;;) {
			i32 ii = (i * harm)/* & WAVEMASK*/;
			if(p.Char('T'))
				fn = [&]() -> double { return (1024.0 - abs(WAVECOUNT / 2 - ii)) / WAVECOUNT / 4 - 1; };
			else
			if(p.Char('t'))
				fn = [&]() -> double { return -(1024.0 - abs(WAVECOUNT / 2 - ii)) / WAVECOUNT / 4 + 1; };
			else
			if(p.Char('Z'))
				fn = [&]() -> double { return 2.0f * ii / (WAVECOUNT - 1) - 1; };
			else
			if(p.Char('z'))
				fn = [&]() -> double { return -2.0f * ii / (WAVECOUNT - 1) + 1; };
			else
			if(p.Char('S'))
				fn = fn0;
			else
			if(p.IsDouble()) {
				double a = p.ReadDouble();
				if(p.Char(':'))
					harm = (i32)a;
				else {
					for(i = 0; i < WAVECOUNT; i++)
						h.wave[i] += (float)(a * fn());
					harm++;
				}
			}
			else
				break;
		}
	}
	catch(...) {}
}

float *GetWave(const Txt& h)
{
	static Mutex _;
	Mutex::Lock __(_);
	static ArrayMap<Txt, WaveForm> cache;
	i32 q = cache.Find(h);
	if(q >= 0)
		return cache[q].wave;
	MakeWave(h, cache.Add(h));
	return cache.Top().wave;
}

Instrument::Instrument()
{
	delay = 0;
	attack = 0;
	decay = 0;
	sustain = 1;
	release = 0;
	wave = "1";
	mod_wave = "1";
	mod_amplitude = 0;
	mod_frequency = 1;
	noise_kind = 0;
	noise_amplitude = 1;
};

i32k TABN = 4096;

static double table[TABN + 2];

double LOGVOL(double x)
{
	return pow(10, x * 60 / 20) / pow(10, 60 / 20);
}

force_inline
double LogVol(double x)
{
	double id = TABN * x;
	i32 ii = (i32)id;
	if(ii < 0) return 0;
	if(ii > TABN) return x;
	double f = id - ii;
	return (1 - f) * table[ii] + f * table[ii + 1];
}

INITBLOCK {
	for(i32 i = 0; i <= TABN; i++) {
		double q = i / (double)TABN;
		table[i] = LOGVOL(q);
	}
	table[TABN + 1] = (TABN + 1) / 256.0;
	table[0] = 0;
}

double MakeNoise(i32 kind)
{
	static dword state = 1;
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	double white = 2.0 / 4294967295.0 * state - 1;

	static double p;
	static double b0, b1, b2;

	switch(kind) {
	case 1:
		return white;
	case 2:
		b0 = 0.99765 * b0 + white * 0.0990460;
		b1 = 0.96300 * b1 + white * 0.2965164;
		b2 = 0.57000 * b2 + white * 1.0526913;
		return b0 + b1 + b2 + white * 0.1848;
	case 3:
		p = clamp(p + (0.02 * (Randomf() * 2 - 1)) / 1.02, -1/3.5, 1/3.5);
		return p * 3.5;
	}
	return 0;
}

struct SynthSound : SoundGenerator {
	virtual bool Get(float *data, i32 len);

	float  volume;
	float  fdelta;
	float  sustain;
	float  mdelta;
	float  mod_amp;
	i32    duration;
	i32    delay;
	i32    attack;
	i32    decay;
	i32    release;
	float *wave;
	float *mod_wave;
	i32    noise_kind;
	float  noise_amplitude;
	float  release_from = 0;
	float  last = 0;
	float  frequency_mul;
	bool   has_noise;
	float  noise[8];

	float  q = 0;
	float  w = 0;
	i32    t = 0;

	void SetVolume(float vol)          { volume = vol; }
	void SetFrequency(float frequency) { fdelta = (WAVEMASK + 1) * frequency * frequency_mul / 44200; }
	
	void Set(float volume, float frequency, float duration, const Instrument& m);

	SynthSound(float volume, float frequency, float duration, const Instrument& m) {
		Set(volume, frequency, duration, m);
	}
};

void SynthSound::Set(float volume, float frequency, float duration_, const Instrument& m)
{
	frequency_mul = m.frequency_mul;
	
	SetVolume(volume);
	SetFrequency(frequency);
	
	sustain = m.sustain;
	mdelta = (WAVEMASK + 1) * m.mod_frequency / 44200;
	mod_amp = (WAVEMASK + 1) * m.mod_amplitude / 44200;
	duration = i32(44200 * duration_);
	delay = i32(44200 * m.delay);
	attack = i32(44200 * m.attack);
	decay = i32(44200 * m.decay);
	release = i32(44200 * m.release);
	wave = GetWave(m.wave);
	mod_wave = GetWave(m.mod_wave);
	noise_kind = m.noise_kind;
	noise_amplitude = m.noise_amplitude;
	
	has_noise = m.has_noise;
	for(i32 i = 0; i < 8; i++)
		noise[i] = m.noise[i];
}

bool SynthSound::Get(float *b, i32 len)
{
	bool plays = true;
	float sustain_volume = sustain * volume;
	for(i32 i = 0; i < len; i++) {
		if(t < delay)
			*b++ = 0;
		else {
			float envelope = 0;
			if(t < delay + attack) {
				envelope = (t * volume) / attack;
				release_from = envelope;
				last = t;
			}
			else
			if(t < delay + attack + decay) {
				envelope = volume - (volume - sustain_volume) * (t - delay - attack) / decay;
				release_from = envelope;
				last = t;
			}
			else
			if(t < duration || duration < 0) {
				envelope = sustain_volume;
				release_from = envelope;
				last = t;
			}
			else
			if(release) {
				envelope = release_from - release_from * (t - last) / release;
				if(envelope <= 0) {
					plays = false;
					envelope = 0;
				}
			}
			else
				plays = false;
			double a = wave[(i32)q & WAVEMASK];
			if(has_noise) {
				a += BrownNoise[(i32)q & NOISEMASK];
			}
			if(noise_kind)
				a += noise_amplitude * MakeNoise(noise_kind);
			*b++ = envelope * a;
			w += mdelta;
			q += fdelta + mod_amp * mod_wave[(i32)w & WAVEMASK];
		}
		t++;
	}
	return plays;
}

z64 Play(float volume, float frequency, float duration, const Instrument& m)
{
	return AddSound(new SynthSound(volume, frequency, duration, m));
}

z64 Play(float volume, float frequency, const Instrument& m)
{
	return Play(volume, frequency, -1, m);
}


void SetVolume(z64 id, float volume)
{
	AlterSound(id, [=, this](SoundGenerator *sg) {
		SynthSound *ss = dynamic_cast<SynthSound *>(sg);
		if(ss)
			ss->SetVolume(volume);
	});
}

void SetFrequency(z64 id, float frequency)
{
	AlterSound(id, [=, this](SoundGenerator *sg) {
		SynthSound *ss = dynamic_cast<SynthSound *>(sg);
		if(ss)
			ss->SetFrequency(frequency);
	});
}

void StopSound(z64 id)
{
	AlterSound(id, [=, this](SoundGenerator *sg) {
		SynthSound *ss = dynamic_cast<SynthSound *>(sg);
		if(ss)
			ss->duration = 0;
	});
}

void Instrument::Read(CParser& p)
{
	while(!p.IsEof()) {
		if(p.Char('{')) {
			tukk s = p.GetPtr();
			for(;;) {
				if(p.IsChar('}') || p.IsEof()) {
					wave = Txt(s, p.GetPtr());
					break;
				}
				p.SkipTerm();
			}
			p.Char('}');
			if(p.Char('@'))
				frequency_mul = p.ReadDouble();
		}
		else
		if(p.Char('[')) {
			tukk s = p.GetPtr();
			for(;;) {
				if(p.IsChar(']') || p.IsEof()) {
					mod_wave = Txt(s, p.GetPtr());
					break;
				}
				p.SkipTerm();
			}
			p.Char(']');
			for(;;) {
				if(p.Char('@'))
					mod_amplitude = p.ReadDouble();
				else
				if(p.Char('^'))
					mod_frequency = p.ReadDouble();
				else
					break;
			}
		}
		else
		if(p.Char('<')) {
			has_noise = true;
			i32 ii = 0;
			while(!p.Char('>') && ii < 8)
				noise[ii++] = p.ReadDouble();
		}
		else
		if(p.Char('a'))
			attack = p.ReadDouble();
		else
		if(p.Char('d'))
			decay = p.ReadDouble();
		else
		if(p.Char('s'))
			sustain = p.ReadDouble();
		else
		if(p.Char('r'))
			release = p.ReadDouble();
		else
			break;
	}
}

void Instrument::Read(tukk s)
{
	try {
		CParser p(s);
		Read(p);
	}
	catch(...) {}
}
