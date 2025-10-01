#include "Synth.h"

struct QSFStatus : Moveable<QSFStatus> {
	double tempo = 120;
	i32    sound = -1;
	double duration = 1;
	i32    shift = 24;

	double volume = 1;
	double volume_low = 0;
	double volume_high = 1;
	double volume_start_at = 0;
	double volume_mul = 0;
	
	double GetVolume(double at) const { return clamp(volume + (at - volume_start_at) * volume_mul, volume_low, volume_high); }
};

struct QSFParser : SoundSequence, QSFStatus {
	i32         lineno;
	Txt      line;
	tukk ptr;
	VecMap<Txt, Txt> macro;
	VecMap<i32, Txt> errors;

	double at = 0;
	double at0 = 0;
	double end = 0;

	Vec<QSFStatus> stack;
	Vec<double> at_stack;
	
	void   Error(tukk s);
	void   Spaces();
	Txt ReadID();
	void   Expand();
	
	void   Sound(tukk s);
	void   Tone(i32 tone, double duration);
	void   Process();
	
	void   Parse(Stream& s);
	
	QSFParser() {
		Sound("130.81:L250V100R30::::");
	}
};

void QSFParser::Error(tukk s)
{
	errors.Add(lineno, s);
}

void QSFParser::Spaces()
{
	while(IsSpace(*ptr)) ptr++;
}

Txt QSFParser::ReadID()
{
	Spaces();
	Txt id;
	if(IsAlpha(*ptr) || *ptr == '_') {
		tukk b = ptr;
		while(IsAlNum(*ptr) || *ptr == '_')
			ptr++;
		id = Txt(b, ptr);
	}
	return id;
}

void QSFParser::Expand()
{
	bool expanded;
	auto Error = [&](tukk s) { errors.Add(lineno, s); };
	Index<i32> used_macro;
	do {
		expanded = false;
		Txt r;
		ptr = line;
		Index<i32> pm;
		auto GetMacro = [&]()->Txt {
			Txt id = ReadID();
			if(id.GetCount()) {
				i32 q = macro.Find(id);
				if(q >= 0) {
					if(used_macro.Find(q) < 0) {
						pm.FindAdd(q);
						expanded = true;
						return macro[q];
					}
					else
						Error("рекурсивный макрос " + id);
				}
				else
					Error("неизвестный макрос " + id);
			}
			else
				Error("отсутствует идентификатор макроса");
			return Null;
		};
		while(*ptr) {
			if(*ptr == '$') {
				ptr++;
				Spaces();
				if(IsDigit(*ptr)) {
					i32 n = 0;
					while(IsDigit(*ptr)) {
						n = 10 * n + *ptr - '0';
						ptr++;
					}
					Spaces();
					Txt txt;
					i32 lvl = 0;
					auto Pars = [&](i32 l, i32 r) {
						while(*ptr) {
							if(*ptr == l)
								lvl++;
							if(*ptr == r && lvl-- == 0)
								break;
							ptr++;
						}
					};
					if(*ptr == '[') {
						tukk b = ptr++;
						Pars('[', ']');
						if(*ptr) ptr++;
						txt = Txt(b, ptr);
					}
					else
					if(*ptr == '(') {
						tukk b = ++ptr;
						Pars('(', ')');
						txt = Txt(b, ptr);
						if(*ptr) ptr++;
					}
					else
						txt = GetMacro();
					
					if(n > 0 && n < 100000) {
						while(n--)
							r << txt << ' ';
						expanded = true;
					}
					else
						Error("неверное число повторений");
				}
				else
					r << GetMacro();
			}
			else
				r << *ptr++;
		}
		line = r;
		for(i32 q : pm)
			used_macro.Add(q);
	}
	while(expanded);
}

void QSFParser::Parse(Stream& src)
{
	while(!src.IsEof()) {
		lineno++;
		line = src.GetLine();
		i32 q = line.Find("//");
		if(q >= 0)
			line.Trim(q);
		line = TrimBoth(line);
		if(*line == '#') {
			ptr = ~line + 1;
			Txt id = ReadID();
			Spaces();
			if(id.GetCount())
				macro.GetAdd(id) = ptr;
			else
				Error("отсутствует имя макроса");
		}
		else {
			Expand();
			Process();
		}
	}
	
}

void QSFParser::Sound(tukk s)
{
	sound = SoundIndex(s);
}

void QSFParser::Tone(i32 tone, double duration)
{
	duration = duration / tempo * 240;
	if(tone == -1)
		At(at);
	else
		Put(at, sound, 100 * GetVolume(at), 65.406 * exp2(tone / 12.0), 1000 * duration, IsNull(tone));
	at += duration;
	end = max(at, end);
}

void QSFParser::Process()
{
	ptr = line;
	auto ReadNumber = [&]() -> double {
		CParser p(ptr);
		p.NoSkipSpaces().NoSkipComments();
		p.SkipSpaces();
		if(p.IsDouble()) {
			double h = p.ReadDouble();
			ptr = p.GetPtr();
			return h;
		}
		return 0;
	};
	auto ReadCNumber = [&]() -> double {
		Spaces();
		if(*ptr == ':')
			ptr++;
		return ReadNumber();
	};

	auto DoTone = [&](i32 tone) {
		ptr++;
		double ln = 1;
		bool chrd = false;
		for(;;) {
			if(*ptr == '\'' || *ptr == '^')
				tone += 12;
			else
			if(*ptr == ',')
				tone -= 12;
			else
			if(*ptr == '=')
				ln = ln + 1;
			else
			if(*ptr == '.')
				ln = ln + 0.5;
			else
			if(*ptr == ':')
				ln = ln + 0.75;
			else
			if(*ptr == '&')
				chrd = true;
			else
			if(*ptr != ' ')
				break;
			ptr++;
		}
		double h = at;
		Tone(shift + tone, ln * duration);
		if(chrd)
			at = h;
	};
	while(*ptr) {
		if(*ptr == ';') {
			ptr++;
			if(*ptr == ';') {
				at0 = at;
				ptr++;
			}
			else
				at = at0;
			duration = 1;
			shift = 24;
		}
		else
		if(*ptr == '$') {
			ptr++;
			tempo = ReadNumber();
			if(tempo < 1) {
				Error("неверное значение темпа");
				tempo = 600;
			}
		}
		else
		if(*ptr == '+') {
			ptr++;
			shift += 12;
		}
		else
		if(*ptr == '-') {
			ptr++;
			shift -= 12;
		}
		else
		if(*ptr == '@') {
			ptr++;
			shift += (i32)ReadNumber();
		}
		else
		if(*ptr >= '0' && *ptr <= '9')
			DoTone(*ptr - '0');
		else
		if(*ptr == 't')
			DoTone(10);
		else
		if(*ptr == 'e')
			DoTone(11);
		else
		if(*ptr == '_') {
			Tone(-1, duration);
			ptr++;
		}
		else
		if(*ptr == '*') {
			Tone(Null, duration);
			ptr++;
		}
		else
		if(*ptr == '/') {
			ptr++;
			duration = 1.0 / ReadNumber();
		}
		else
		if(*ptr == 'q') {
			ptr++;
			duration = 1.0 / 4;
		}
		else
		if(*ptr == 'o' || *ptr == 'w') {
			ptr++;
			duration = 1.0 / 8;
		}
		else
		if(*ptr == 'x') {
			ptr++;
			duration = 1.0 / 16;
		}
		else
		if(*ptr == 'y') {
			ptr++;
			duration = 1.0 / 32;
		}
		else
		if(*ptr == 'm') {
			ptr++;
			duration = 1.0 / 2;
		}
		else
		if(*ptr == 'n') {
			ptr++;
			duration = 1.0;
		}
		else
		if(*ptr == '%') {
			ptr++;
			duration *= ReadNumber();
			if(duration <= 0) {
				Error("неверная длина тона");
				duration = 1;
			}
		}
		else
		if(*ptr == '!') {
			ptr++;
			Txt snd;
			Spaces();
			if(IsAlpha(*ptr)) {
				Txt id = ReadID();
				if(id == "loop")
					LoopAt(at);
				if(id == "shift")
					shift += (i32)ReadCNumber();
				if(id == "volume") {
					volume = ReadCNumber();
					volume_mul = 0;
				}
				if(id == "volume_to") {
					volume = GetVolume(at);
					double v = ReadCNumber();
					double len = ReadCNumber();
					volume_low = min(v, volume);
					volume_high = max(v, volume);
					volume_start_at = at;
					volume_mul = (v - volume) / (240 / tempo) / len;
				}
				if(id == "cursor")
					SoundSequence::cursor = GetAt(at);
				if(id == "cut") {
					event.Trim(GetAt(at));
					end = at;
				}
				if(id == "tempo") {
					double t = ReadCNumber();
					if(t < 1 || t > 10000) {
						Error("неверное значение темпа");
					}
					else
						tempo = t;
				}
			}
			else {
				while(*ptr && *ptr != ' ')
					snd.Cat(*ptr++);
				Sound(snd);
			}
		}
		else
		if(*ptr == '[') {
			ptr++;
			if(stack.GetCount() < 100)
				stack.Add() = *this;
			else
				Error("стэк полон");
		}
		else
		if(*ptr == ']') {
			ptr++;
			if(stack.GetCount())
				(QSFStatus&)*this = stack.Pop();
			else
				Error("стэк пуст");
		}
		else
		if(*ptr == '{') {
			ptr++;
			if(stack.GetCount() < 100) {
				stack.Add() = *this;
				at_stack.Add(at);
			}
			else
				Error("стэк полон");
		}
		else
		if(*ptr == '}') {
			ptr++;
			if(stack.GetCount() && at_stack.GetCount()) {
				(QSFStatus&)*this = stack.Pop();
				at = at_stack.Pop();
			}
			else
				Error("стэк пуст");
		}
		else
			ptr++;
	}
	i32 q = GetAt(end);
	if(q > 0)
		event.At(q);
}

SoundSequence ParseQSF(const Txt& data)
{
	TxtStream ss(data);
	QSFParser p;
	p.Parse(ss);
	return pick(p);
}
