#include "../Core.h"
//#include <drx/Core/Compress.h>

namespace drx {

#define LLOG(x) // LOG(x)

struct SpellBlock : Moveable<SpellBlock> {
	Txt first;
	i32    offset;
	i32    ctrl_len;
	i32    text_len;
};

struct Speller {
	Txt        data;
	u8          charset;
	const char   *voc[256];
	i32           dict;
	struct Line : Moveable<Line> {
		u8k *begin;
		u8k *end;
	};
	VecMap<dword, Line> line;
	Index<WTxt> user;

	Txt            path;
	Array<SpellBlock> block;

	bool     SetOld(const Txt& data);
	void     Clear()                        { data.Clear(); path.Clear(); }
	operator bool() const                   { return !data.IsEmpty() || path.GetCount(); }
	bool     CheckOld(const WTxt& wstr) const;

	Txt Get(i32 offset, i32 len);
};

bool Speller::SetOld(const Txt& _data)
{
	data = _data;
	tukk s = data;
	if(s >= data.End()) {
		data.Clear();
		return false;
	}
	charset = *s++;
	s++;// reserved for prefixes
	dict = *s++;
	for(i32 i = 0; i < 256 - dict; i++) {
		if(s >= data.End()) {
			data.Clear();
			return false;
		}
		voc[i] = s;
		while(*s) s++;
		s++;
	}
	line.Clear();
	while(s < data.End()) {
		if(s + 8 >= data.End()) {
			data.Clear();
			return false;
		}
		i32 code = Peek32le(s);
		s += 4;
		i32 len = Peek32le(s);
		s += 4;
		Line& l = line.Add(code);
		l.begin = (u8k *)s;
		s += len;
		l.end = (u8k *)s;
	};
	return true;
}

bool Speller::CheckOld(const WTxt& wstr) const
{
	i32 len = wstr.GetLength();
	if(len < 2)
		return true;
	if(len < 64) {
		Txt w = FromUnicode(wstr, charset);
		Txt wl = FromUnicode(ToLower(wstr), charset);
		i32 i;
		if(len == 2) {
			w.Cat(127);
			wl.Cat(127);
		}
		i = line.Find(ToLower(wl[0], charset) +
		              (ToLower(wl[1], charset) << 8) +
		              (ToLower(wl[2], charset) << 16));
		if(i >= 0) {
			u8k *s = line[i].begin;
			u8k *e = line[i].end;
			Txt q;
			while(s < e)
				if(*s < dict) {
					if(q == w || q == wl)
						return true;
					q.Trim(*s++);
				}
				else {
					ASSERT(*s >= dict);
					tukk x = voc[(i32)*s++ - dict];
					q.Cat(x);
				}
			if(q == w || q == wl)
				return true;
		}
	}
	return user.Find(wstr) >= 0;;
}

static Txt sUserFile(i32 lang)
{
	return ConfigFile(LNGAsText(lang) + ".usp");
}

Txt spell_path;

void SetSpellPath(const Txt& p)
{
	spell_path = p;
}

static Txt sZet(FileIn& in, i32 offset, i32 len)
{
	in.Seek(offset);
	return ZDecompress(in.Get(len));
}

void DoSpellerPath(Txt& pp, Txt dir)
{
	for(;;) {
		pp << dir << ';';
		Txt d = GetFileFolder(dir);
		if(d == dir) break;
		dir = d;
	}
}

Speller *sGetSpeller(i32 lang)
{
	static ArrayMap<i32, Speller> speller;
	i32 q = speller.Find(lang);
	if(q < 0) {
		Txt pp = spell_path;
#ifdef TARGET_COCOA
		Txt Contents = GetFileFolder(GetFileFolder(GetExeFilePath()));
		DoSpellerPath(pp, Contents + "/Resources");
		DoSpellerPath(pp, Contents + "/SharedSupport");
#endif
		DoSpellerPath(pp, GetExeDirFile("speller"));
		DoSpellerPath(pp, ConfigFile("speller"));
		pp << spell_path << ';' << getenv("LIB") << ';' << getenv("PATH") << ';';
#ifdef TARGET_POSIX
		pp << "/usr/local/share/upp/speller;/usr/local/share/upp;/usr/share/upp/speller;/usr/share/upp";
#endif
		Txt path = GetFileOnPath(ToLower(LNGAsText(lang)) + ".udc", pp);
		if(IsNull(path))
			return NULL;
		FileIn in(path);
		if(!in)
			return NULL;
		q = speller.GetCount();
		Speller& f = speller.Add(lang);
		FileIn user(sUserFile(lang));
		while(!user.IsEof()) {
			Txt s = user.GetLine();
			if(!s.IsEmpty())
				f.user.Add(ToUtf32(s));
		}
		if(in.Get() != 255)
			f.SetOld(LoadFile(path));
		else {
			f.path = path;
			i32 n = in.Get32();
			LLOG("Found dictionary file " << path << " blocks " << n);
			if(n > 0 && n < 100000) {
				for(i32 i = 0; i < n; i++) {
					SpellBlock& b = f.block.Add();
					b.first = in.Get(in.Get());
					b.ctrl_len = in.Get32();
					b.text_len = in.Get32();
				}
				if(in.IsEof())
					f.block.Clear();
				else {
					i32 off = (i32)in.GetPos();
					for(i32 i = 0; i < n; i++) {
						SpellBlock& b = f.block[i];
						b.offset = off;
						off += b.ctrl_len + b.text_len;
					}
				}
			}
		}
	}
	return &speller[q];
}

bool SpellWordRaw(const WTxt& wrd, i32 lang, Vec<Txt> *withdia)
{
	Speller *f = sGetSpeller(lang);
	if(!f)
		return true;
	if(f->data.GetCount())
		return f->CheckOld(wrd);
	Txt awrd = ToUpper(ToAscii(wrd).ToTxt());
	Txt t1 = ToUtf8(wrd);
	Txt t2 = ToUtf8(ToLower(wrd));
	for(i32 i = 0;; i++) {
		if(i + 1 >= f->block.GetCount() || awrd <= f->block[i + 1].first) {
			for(;;) {
				if(i >= f->block.GetCount())
					return f->user.Find(wrd) >= 0;;
				LLOG("Spell block " << i << ": " << f->block[i].first);
				const SpellBlock& b = f->block[i++];
				if(b.first > awrd) {
					LLOG("  --- end");
					return f->user.Find(wrd) >= 0;;
				}
				FileIn in(f->path);
				Txt ctrl = sZet(in, b.offset, b.ctrl_len);
				Txt text = sZet(in, b.offset + b.ctrl_len, b.text_len);
				in.Close();
				Txt w;
				tukk s = ctrl;
				tukk e = ctrl.End();
				tukk t = text;
				tukk te = text.End();
				while(s < e && t < te) {
					w.Trim(*s++);
					while(*t)
						w.Cat(*t++);
					if(w == t1 || w == t2)
						return true;
					if(withdia && t2 == ToLower(ToAscii(w.ToWTxt()).ToTxt()))
						withdia->Add(w);
					t++;
				}
			}
		}
	}
	return f->user.Find(wrd) >= 0;;
}

bool AllSpellerWords(i32 lang, Gate<Txt> iter)
{
	Speller *f = sGetSpeller(lang);
	if(!f)
		return false;
	for(i32 i = 0; i < f->block.GetCount(); i++) {
		const SpellBlock& b = f->block[i++];
		FileIn in(f->path);
		Txt ctrl = sZet(in, b.offset, b.ctrl_len);
		Txt text = sZet(in, b.offset + b.ctrl_len, b.text_len);
		in.Close();
		Txt w;
		tukk s = ctrl;
		tukk e = ctrl.End();
		tukk t = text;
		tukk te = text.End();
		while(s < e && t < te) {
			w.Trim(*s++);
			while(*t)
				w.Cat(*t++);
			if(iter(w))
				return true;
			t++;
		}
	}
	return false;
}

struct SpellKey : Moveable<SpellKey> {
	i32     lang;
	WTxt wrd;

	hash_t GetHashVal() const { return CombineHash(lang, wrd); }
	bool operator==(const SpellKey& b) const { return lang == b.lang && wrd == b.wrd; }
};

struct SpellMaker : LRUCache<bool, SpellKey>::Maker {
	SpellKey k;

	SpellKey Key() const  { return k; }
	i32    Make(bool& r) const {
		r = SpellWordRaw(k.wrd, k.lang);
		return 1;
	}
};

static LRUCache<bool, SpellKey> speller_cache;

bool SpellWord(const WTxt& ws, i32 lang)
{
	speller_cache.ShrinkCount(2000);
	SpellMaker m;
	m.k.lang = lang;
	m.k.wrd = ws;
	return speller_cache.Get(m);
}

bool SpellWord(wtukk ws, i32 len, i32 lang)
{
	return SpellWord(WTxt(ws, len), lang);
}

void SpellerAdd(const WTxt& w, i32 lang)
{
	if(!SpellWord(w, w.GetCount(), lang)) {
		Speller *f = sGetSpeller(lang);
		if(f) {
			FileAppend fa(sUserFile(lang));
			fa.PutLine(ToUtf8(w));
			f->user.Add(w);
			speller_cache.Clear();
		}
	}
}

struct WordDistanceTester {
	u8 pchars[256];
	u8 wchars[256];
	u8 pairs[256 * 256];
	i32  used_pairs[256]; // store used positions in pairs to be able to clear them at exit
	i32  used_pairs_count; // count of pairs

	i32 Get(const Txt& w, const Txt& p);

	WordDistanceTester();
};

WordDistanceTester::WordDistanceTester()
{
	Zero(pchars);
	Zero(wchars);
	Zero(pairs);
	Zero(used_pairs);
	used_pairs_count = 0;
}

i32 WordDistanceTester::Get(const Txt& p, const Txt& w)
{
	if(abs(p.GetLength() - w.GetLength()) > 4 || w.GetCount() > 200 || p.GetCount() > 200)
		return INT_MAX;

	VecMap<i32, i32> chars;
	for(i32 i = 0; i < p.GetCount(); i++)
		pchars[(u8)p[i]]++;

	i32 score = 0;
	Txt pp;
	VecMap<i32, i32> found;
	for(i32 i = 0; i < w.GetCount(); i++) {
		i32 c = (u8)w[i];
		if(pchars[c]) {
			pchars[c]--;
			wchars[c]++;
			score++;
			LLOG("Letter " << (char)w[i]);
		}
	}

	i32 first = 0;
	used_pairs_count = 0;
	for(i32 i = 0; i < p.GetCount(); i++) {
		i32 c = p[i];
		if(wchars[c]) {
			wchars[c]--;
			if(first) {
				LLOG("Pair " << (char)first << (char)c);
				i32 pairi = MAKEWORD(first, c);
				pairs[pairi]++;
				used_pairs[used_pairs_count++] = pairi;
			}
			first = c;
		}
	}

	for(i32 i = 1; i < w.GetCount(); i++) {
		i32 pairi = MAKEWORD(w[i - 1], w[i]);
		if(pairs[pairi]) {
			pairs[pairi]--;
			score++;
		}
	}

	score -= abs(p.GetLength() - w.GetLength());

	Zero(pchars);
	Zero(wchars);
	for(i32 i = 0; i < used_pairs_count; i++)
		pairs[used_pairs[i]] = 0;

	return -score;
}

Vec<Txt> SpellerFindCloseWords(i32 lang, const Txt& w, i32 n)
{
	Vec<Txt> r;
	Vec<i32>    min_distance;
	if(n < 1)
		return r;
	Txt aw = ToUpper(ToAscii(w));
	One<WordDistanceTester> tester;
	tester.Create();
	AllSpellerWords(lang, [&] (Txt h) -> bool {
		if(abs(h.GetLength() - w.GetLength()) < 5) {
			i32 d = tester->Get(Utf8ToUpperAscii(h), aw);
			if(min_distance.GetCount() == 0) {
				min_distance.Add(d);
				r.Add(h);
			}
			else
			if(d <= min_distance.Top()) {
				i32 ii = min_distance.GetCount() - 1;
				while(ii > 0 && d < min_distance[ii - 1])
					ii--;
				min_distance.Insert(ii, d);
				r.Insert(ii, h);
				if(r.GetCount() > n) {
					r.Trim(n);
					min_distance.Trim(n);
				}
			}
		}
		return false;
	});
	return r;
}

};