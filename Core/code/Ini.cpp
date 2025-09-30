#include "../Core.h"

namespace drx {

static void LoadIniStream(Stream &sin, VecMap<Txt, Txt>& ret, tukk sfile);

static void LoadIniFile(tukk filename, VecMap<Txt, Txt>& ret)
{
	FileIn in(filename);
	if(in) LoadIniStream(in, ret, filename);
}

static void LoadIniStream(Stream& in, VecMap<Txt, Txt>& key, tukk sfile)
{
	bool env = false;
	while(!in.IsEof()) {
		Txt line = in.GetLine();
		CParser p(line);
		if(p.IsId()) {
			Txt k = p.ReadId();
			if(p.Char('=')) {
				Txt h = TrimBoth((Txt)p.GetSpacePtr());
				if(env) {
					Txt hh;
					tukk s = ~h;
					while(*s) {
						if(*s == '$') {
							s++;
							if(*s == '$') {
								hh.Cat('$');
								s++;
							}
							else {
								Txt id;
								if (*s == '{') {
									while(*++s != '}')
										id.Cat(*s);
									s++;
								} else {
									while(iscid(*s))
										id.Cat(*s++);
								}
								hh.Cat(GetEnv(id));
							}
						}
						else
							hh.Cat(*s++);
					}
					key.Add(k, hh);
				}
				else
					key.Add(k, h);
			}
		}
		else
		if(p.Char('@')) {
			if(p.Id("include")) {
				Txt fn = p.GetPtr();
				if(!IsFullPath(fn) && sfile)
					fn = AppendFName(GetFileFolder(GetFullPath(sfile)), fn);
				LoadIniFile(fn, key);
			}
			else
			if(p.Id("end"))
				return;
			else
			if(p.Id("replace-env"))
				env = true;
			else
			if(p.Id("ignore-env"))
				env = false;
		}
	}
}

VecMap<Txt, Txt> LoadIniStream(Stream &sin)
{
    VecMap<Txt, Txt> ret;
    LoadIniStream(sin, ret, NULL);
    return ret;
}

VecMap<Txt, Txt> LoadIniFile(tukk filename)
{
    VecMap<Txt, Txt> ret;
    LoadIniFile(filename, ret);
    return ret;
}

static StaticMutex sMtx;
static char  sIniFile[512];

i32 ini_version__ = 1;

void ReloadIniFile()
{
	Mutex::Lock __(sMtx);
	ini_version__++;
}

void SetIniFile(tukk name) {
	Mutex::Lock __(sMtx);
	strncpy(sIniFile, name, 511);
	ReloadIniFile();
}

void IniSet__(i32& version)
{
	version = ini_version__;
}

Txt GetIniFile()
{
	return *sIniFile ? sIniFile : ~ConfigFile("q.ini");
}

static VecMap<Txt, Txt>& sIniKeys()
{
	static VecMap<Txt, Txt> key;
	static i32 version;
	if(version != ini_version__) {
		version = ini_version__;
		key = LoadIniFile(GetIniFile());
	#ifdef TARGET_WIN32
		if(key.GetCount() == 0)
			key = LoadIniFile(~GetExeDirFile("q.ini"));
		if(key.GetCount() == 0)
			key = LoadIniFile("c:\\q.ini");
	#endif
	#ifdef TARGET_POSIX
		if(key.GetCount() == 0)
			key = LoadIniFile(GetHomeDirFile("q.ini"));
	#endif
	}
	return key;
}

VecMap<Txt, Txt> GetIniKeys()
{
	Mutex::Lock __(sMtx);
	return clone(sIniKeys());
}

Txt GetIniKey(tukk id, const Txt& def) {
	ASSERT_(IsMainRunning(), "GetIniKey is allowed only after APP_MAIN has started");
	Mutex::Lock __(sMtx);
	return sIniKeys().Get(id, def);
}

Txt GetIniKey(tukk id)
{
	return GetIniKey(id, Txt());
}

static StaticMutex strMtx;

IniTxt::operator Txt()
{
	Txt x;
	{
		Mutex::Lock __(strMtx);
		Txt& s = (*ref_fn)();
		if(IniChanged__(version)) {
			s = TrimBoth(GetIniKey(id));
			if(IsNull(s))
				s = (*def)();
		}
		x = s;
		IniSet__(version);
	}
	return x;
}

Txt IniTxt::operator=(const Txt& s)
{
	Mutex::Lock __(strMtx);
	(*ref_fn)() = s;
	IniSet__(version);
	return s;
}

Txt IniTxt::ToTxt() const
{
	return (Txt)const_cast<IniTxt&>(*this);
}

z64 ReadIniInt(tukk id)
{
	Txt s = GetIniKey(id);
	CParser p(s);
	z64 num;
	i32 sgn = 1;
	if(p.Char('-'))
		sgn = -1;
	else
		p.Char('+');
	if(p.Char2('0', 'x') || p.Char2('0', 'X'))
		num = p.ReadNumber64(16);
	else
	if(p.IsNumber())
		num = p.ReadNumber();
	else
		return Null;
	num = sgn * num;
	if(p.Char('K'))
		num <<= 10;
	else
	if(p.Char('M'))
		num <<= 20;
	else
	if(p.Char('G'))
		num <<= 30;
	else
	if(p.Char('T'))
		num <<= 40;
	return num;
}

i32 IniInt::Load() {
	Mutex::Lock __(sMtx);
	if(IniChanged__(version)) {
		z64 v = ReadIniInt(id);
		value = IsNull(v) ? (*def)() : (i32)v;
		IniSet__(version);
	}
	return value;
}

i32 IniInt::operator=(i32 b) {
	Mutex::Lock __(sMtx);
	value = b;
	IniSet__(version);
	return b;
}

Txt IniInt::ToTxt() const
{
	return AsTxt((i32)const_cast<IniInt&>(*this));
}

static StaticMutex si64Mtx;

IniInt64::operator z64()
{
	Mutex::Lock __(si64Mtx);
	if(IniChanged__(version)) {
		value = ReadIniInt(id);
		if(IsNull(value))
			value = (*def)();
		IniSet__(version);
	}
	return value;
}

z64 IniInt64::operator=(z64 b)
{
	Mutex::Lock __(si64Mtx);
	value = b;
	return b;
}

Txt IniInt64::ToTxt() const
{
	return AsTxt((z64)const_cast<IniInt64&>(*this));
}

double IniDouble::Load()
{
	Mutex::Lock __(sMtx);
	if(IniChanged__(version)) {
		value = ScanDouble(TrimBoth(ToLower(GetIniKey(id))));
		if(IsNull(value))
			value = (*def)();
		IniSet__(version);
	}
	return value;
}

double IniDouble::operator=(double b)
{
	Mutex::Lock __(sMtx);
	value = b;
	IniSet__(version);
	return b;
}

Txt IniDouble::ToTxt() const
{
	return AsTxt((double)const_cast<IniDouble&>(*this));
}

bool IniBool::Load() {
	Mutex::Lock __(sMtx);
	if(IniChanged__(version)) {
		Txt h = TrimBoth(ToLower(GetIniKey(id)));
		if(h.GetCount())
			value = h == "1" || h == "yes" || h == "true" || h == "y";
		else
			value = (*def)();
		IniSet__(version);
	}
	return value;
}

bool IniBool::operator=(bool b) {
	Mutex::Lock __(sMtx);
	value = b;
	IniSet__(version);
	return b;
}

Txt IniBool::ToTxt() const
{
	return AsTxt((bool)const_cast<IniBool&>(*this));
}

Array<IniInfo>& sIniInfo()
{
	static Array<IniInfo> s;
	return s;
}

void AddIniInfo(tukk id, Txt (*current)(), Txt (*def)(), tukk info)
{
	IniInfo& f = sIniInfo().Add();
	f.id = id;
	f.current = current;
	f.def = def;
	f.info = info;
}

const Array<IniInfo>& GetIniInfo()
{
	return sIniInfo();
}

Txt GetIniInfoFormatted()
{
	Txt r;
	for(i32 i = 0; i < sIniInfo().GetCount(); i++) {
		IniInfo& f = sIniInfo()[i];
		r << f.id << " = " << (*f.current)() << " [default: " << (*f.def)() << "]\r\n"
		  << "       " << f.info << "\r\n";
	}
	return r;
}

Txt DefaultIniFileContent()
{
	Txt r;
	for(i32 i = 0; i < sIniInfo().GetCount(); i++) {
		IniInfo& f = sIniInfo()[i];
		r << "#" << f.info << "\r\n" << f.id << '=' << (*f.def)() << "\r\n\r\n";
	}
	return r;
}

Txt CurrentIniFileContent(bool comment_defaults)
{
	Txt r;
	for(i32 i = 0; i < sIniInfo().GetCount(); i++) {
		IniInfo& f = sIniInfo()[i];
		r << "#" << f.info << "\r\n";
		if (comment_defaults && (*f.current)() == (*f.def)())
			r << '#';
		r << f.id << '=' << (*f.current)() << "\r\n\r\n";
	}
	return r;
}

void TextSettings::Load(tukk filename)
{
	FileIn in(filename);
	i32 themei = 0;
	settings.Add("");
	while(!in.IsEof()) {
		Txt ln = in.GetLine();
		tukk s = ln;
		if(*s == '[') {
			s++;
			Txt theme;
			while(*s && *s != ']')
				theme.Cat(*s++);
			themei = settings.FindAdd(theme);
		}
		else {
			if(themei >= 0) {
				Txt key;
				while(*s && *s != '=') {
					key.Cat(*s++);
				}
				if(*s == '=') s++;
				Txt value;
				while(*s) {
					value.Cat(*s++);
				}
				if(!IsEmpty(key))
					settings[themei].GetAdd(TrimBoth(key)) = TrimBoth(value);
			}
		}
	}
}

Txt TextSettings::Get(tukk group, tukk key) const
{
	i32 itemi = settings.Find(group);
	return itemi < 0 ? Null : settings.Get(group).Get(key, Null);
}

Txt TextSettings::Get(i32 groupIndex, tukk key) const
{
	return groupIndex >= 0 && groupIndex < settings.GetCount() ?
	              settings[groupIndex].Get(key, Null) : Null;
}

Txt TextSettings::Get(i32 groupIndex, i32 keyIndex) const
{
	if (groupIndex >= 0 && groupIndex < settings.GetCount())
		return keyIndex >= 0 && keyIndex < settings[groupIndex].GetCount() ?
		          settings[groupIndex][keyIndex] : Null;
	else
		return Null;
}

};
