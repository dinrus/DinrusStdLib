#include <drx/CtrlLib/CtrlLib.h>

namespace drx {

INITBLOCK {
	RegisterGlobalConfig("GlobalFileSelectorLRU");
}

#ifdef GUI_WIN
void AvoidPaintingCheck__();

Image ProcessSHIcon(HICON hIcon)
{
	AvoidPaintingCheck__();
	Color c = White();
	Image m[2];
	for(i32 i = 0; i < 2; i++) {
		ICONINFO iconinfo;
		if(!hIcon || !GetIconInfo(hIcon, &iconinfo))
			return Image();
		BITMAP bm;
		::GetObject((HGDIOBJ)iconinfo.hbmMask, sizeof(BITMAP), (LPVOID)&bm);
		Size sz(bm.bmWidth, bm.bmHeight);
		ImageDraw iw(sz);
		iw.DrawRect(sz, c);
		::DrawIconEx(iw.GetHandle(), 0, 0, hIcon, 0, 0, 0, NULL, DI_NORMAL|DI_COMPAT);
		::DeleteObject(iconinfo.hbmColor);
		::DeleteObject(iconinfo.hbmMask);
		c = Black();
		m[i] = iw;
	}
	::DestroyIcon(hIcon);
	return RecreateAlpha(m[0], m[1]);
}

struct FileIconMaker : ImageMaker {
	Txt file;
	bool   exe;
	bool   dir;
	bool   large;

	virtual Txt Key() const {
		return file + "\n" + (exe ? "1" : "0") + (dir ? "1" : "0");
	}

	virtual Image Make() const {
		SHFILEINFOW info;
		AvoidPaintingCheck__();
		SHGetFileInfoW(ToSystemCharsetW(file), dir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
		               &info, sizeof(info),
		               SHGFI_ICON|(large ? SHGFI_LARGEICON : SHGFI_SMALLICON)|(exe ? 0 : SHGFI_USEFILEATTRIBUTES));
		return ProcessSHIcon(info.hIcon);
	}
};


Image GetFileIcon(tukk path, bool dir, bool force, bool large, bool quick = false)
{
	FileIconMaker m;
	Txt ext = GetFileExt(path);
	m.exe = false;
	m.dir = false;
	m.file = path;
	m.large = large;
	if(force)
		m.exe = true;
	else
	if(dir) {
		m.dir = true;
		m.exe = true;
	}
	else
	if(findarg(ext, ".exe", ".lnk") >= 0)
		m.exe = true;
	else
		m.file = "x" + ext;
	if(quick) {
		m.exe = false;
		m.file = "x" + ext;
	}
	return MakeImage(m);
}

#define GETFILEICON_DEFINED

#endif

#if defined(GUI_GTK)

Image GtkThemeIcon(tukk name, i32 sz);

Image GnomeImage(tukk s, bool large = false)
{
	return GtkThemeIcon(Txt("gnome-") + s, DPI(large ? 48 : 16));
}

Image SystemImage(tukk s, bool large = false)
{
	return GtkThemeIcon(s, DPI(large ? 48 : 16));
}

struct ExtToMime {
	Index<Txt> major;
	Index<Txt> minor;
	VecMap<Txt, dword> map;

	void Load(tukk file);
	void Refresh();
	bool GetMime(const Txt& ext, Txt& maj, Txt& min);
};

void ExtToMime::Load(tukk fn)
{
	FileIn in(fn);
	if(in)
		while(!in.IsEof()) {
			Txt ln = TrimLeft(TrimRight(in.GetLine()));
			if(ln[0] != '#') {
				i32 q = ln.Find(':');
				if(q >= 0) {
					Txt h = ln.Mid(0, q);
					i32 w = h.Find('/');
					if(w >= 0) {
						i32 x = ln.Find("*.", q);
						if(x >= 0) {
							Txt ext = ln.Mid(x + 2);
							if(ext.GetCount() && map.Find(ext) < 0)
								map.Add(ext, MAKELONG(minor.FindAdd(h.Mid(w + 1)), major.FindAdd(h.Mid(0, w))));
						}
					}
				}
			}
		}
}

void ExtToMime::Refresh()
{
	major.Clear();
	minor.Clear();
	map.Clear();
	Load("/usr/local/share/mime/globs");
	Load("/usr/share/mime/globs");
}

bool ExtToMime::GetMime(const Txt& ext, Txt& maj, Txt& min)
{
	ONCELOCK {
		Refresh();
	}
	i32 q = map.Find(ext);
	if(q < 0)
		return false;
	dword x = map[q];
	maj = major[HIWORD(x)];
	min = minor[LOWORD(x)];
	return true;
}

struct FileExtMaker : ImageMaker {
	Txt ext;
	bool   large;

	virtual Txt Key() const {
		return ext;
	}

	virtual Image Make() const {
		Txt major;
		Txt minor;
		if(!Single<ExtToMime>().GetMime(ext, major, minor))
			return Null;
		Image img = SystemImage(major + '-' + minor, large);
		return IsNull(img) ? SystemImage(major) : img;
	}
};

Image PosixGetDriveImage(Txt dir, bool large)
{
	static bool init = false;
	static Image cdrom;
	static Image lcdrom;
	static Image harddisk;
	static Image lharddisk;
	static Image floppy;
	static Image lfloppy;
	static Image computer;
	static Image lcomputer;

	if (!init) {
		bool KDE = Environment().Get("KDE_FULL_SESSION", Txt()) == "true";
		if (KDE) {
			cdrom     = SystemImage("media-optical");
			lcdrom    = SystemImage("media-optical", true);
			harddisk  = SystemImage("drive-harddisk");
			lharddisk = SystemImage("drive-harddisk", true);
			floppy    = SystemImage("media-floppy");
			lfloppy   = SystemImage("media-floppy", true);
			computer  = SystemImage("computer");
			lcomputer = SystemImage("computer", true);
		}
		else {
			cdrom     = GnomeImage("dev-cdrom");
			lcdrom    = GnomeImage("dev-cdrom", true);
			harddisk  = GnomeImage("dev-harddisk");
			lharddisk = GnomeImage("dev-harddisk", true);
			floppy    = GnomeImage("dev-floppy");
			lfloppy   = GnomeImage("dev-floppy", true);
			computer  = GnomeImage("dev-computer");
			lcomputer = GnomeImage("dev-computer", true);
		}

		init = true;
	}
	if(dir.GetCount() == 0 || dir == "/") {
		Image m = large ? lcomputer : computer;
		return IsNull(m) ? CtrlImg::Computer() : m;
	}
	if(dir.Find("cdrom") == 0 || dir.Find("cdrecorder") == 0) {
		Image m = large ? lcdrom : cdrom;
		return IsNull(m) ? CtrlImg::CdRom() : m;
	}
	if(dir.Find("floppy") == 0 || dir.Find("zip") == 0) {
		Image m = large ? lfloppy : floppy;
		return IsNull(m) ? CtrlImg::Diskette() : m;
	}
	Image m = large ? lharddisk : harddisk;
	return IsNull(m) ? CtrlImg::Hd() : m;
}

Image GetFileIcon(const Txt& folder, const Txt& filename, bool isdir, bool isexe, bool large)
{
	static bool init = false;
	static bool KDE  = Environment().Get("KDE_FULL_SESSION", Txt()) == "true";

	static Image file;
	static Image lfile;
	static Image dir;
	static Image ldir;
	static Image exe;
	static Image lexe;
	static Image home;
	static Image lhome;
	static Image desktop;
	static Image ldesktop;
	static Image music;
	static Image lmusic;
	static Image pictures;
	static Image lpictures;
	static Image video;
	static Image lvideo;
	static Image documents;
	static Image ldocuments;
	static Image download;
	static Image ldownload;
	static Image help;
	static Image lhelp;
	static Image translation;
	static Image ltranslation;
	static Image layout;
	static Image llayout;

	static Image fileImage;
	static Image fileMusic  = SystemImage("audio-x-generic");
	static Image fileScript = SystemImage("text-x-script");

	if (!init) {
		if (KDE) {
			file         = SystemImage("text-plain");
			lfile        = SystemImage("text-plain", true);
			dir          = SystemImage("folder");
			ldir         = SystemImage("folder", true);
			exe          = SystemImage("application-x-executable");
			lexe         = SystemImage("application-x-executable", true);
			home         = SystemImage("user-home");
			lhome        = SystemImage("user-home", true);
			desktop      = SystemImage("user-desktop");
			ldesktop     = SystemImage("user-desktop", true);
			music        = SystemImage("folder-sound");
			lmusic       = SystemImage("folder-sound", true);
			pictures     = SystemImage("folder-image");
			lpictures    = SystemImage("folder-image", true);
			video        = SystemImage("folder-video");
			lvideo       = SystemImage("folder-video", true);
			documents    = SystemImage("folder-documents");
			ldocuments   = SystemImage("folder-documents", true);
			download     = SystemImage("folder-downloads");
			ldownload    = SystemImage("folder-downloads", true);
			help         = SystemImage("help-contents");
			lhelp        = SystemImage("help-contents", true);
			translation  = SystemImage("applications-education-language");
			ltranslation = SystemImage("applications-education-language", true);
			layout       = SystemImage("applications-development");
			llayout      = SystemImage("applications-development", true);

			fileImage    = SystemImage("application-x-egon");
		}
		else {
			file         = GnomeImage("fs-regular");
			lfile        = GnomeImage("fs-regular", true);
			dir          = GnomeImage("fs-directory");
			ldir         = GnomeImage("fs-directory", true);
			exe          = GnomeImage("fs-executable");
			lexe         = GnomeImage("fs-executable", true);
			home         = GnomeImage("fs-home");
			lhome        = GnomeImage("fs-home", true);
			desktop      = GnomeImage("fs-desktop");
			ldesktop     = GnomeImage("fs-desktop", true);
			music        = SystemImage("folder-music");
			lmusic       = SystemImage("folder-music", true);
			pictures     = SystemImage("folder-pictures");
			lpictures    = SystemImage("folder-pictures", true);
			video        = SystemImage("folder-video");
			lvideo       = SystemImage("folder-video", true);
			documents    = SystemImage("folder-documents");
			ldocuments   = SystemImage("folder-documents", true);
			download     = SystemImage("folder-downloads");
			ldownload    = SystemImage("folder-downloads", true);
			help         = SystemImage("help");
			lhelp        = SystemImage("help", true);
			translation  = SystemImage("preferences-desktop-locale");
			ltranslation = SystemImage("preferences-desktop-locale", true);
			layout       = SystemImage("applications-development");
			llayout      = SystemImage("applications-development", true);

			fileImage    = SystemImage("image-x-generic");
		}

		init = true;
	}
	if (filename == "Тематики Справки")
		return large ? lhelp : help;
	if(isdir) {
		Image img = dir;
		if(AppendFName(folder, filename) == GetHomeDir())
			return large ? lhome : home;
		else
		if(AppendFName(folder, filename) ==  GetDesktopFolder ())
			return large ? ldesktop : desktop;
		else
		if(AppendFName(folder, filename) == GetMusicFolder ())
			return large ? lmusic : music;
		else
		if(AppendFName(folder, filename) == GetPicturesFolder())
			return large ? lpictures : pictures;
		else
		if(AppendFName(folder, filename) == GetVideoFolder())
			return large ? lvideo : video;
		else
		if(AppendFName(folder, filename) == GetDocumentsFolder())
			return large ? ldocuments : documents;
		else
		if(AppendFName(folder, filename) == GetDownloadFolder())
			return large ? ldownload : download;
		else
		if(folder == "/media" || filename.GetCount() == 0)
			return PosixGetDriveImage(filename, large);
		return dir;
	}
	FileExtMaker m;
	m.ext = GetFileExt(filename);
	for (i32 i = 1; i < m.ext.GetCount(); ++i)
		m.ext.Set (i, ToLower(m.ext[i]));

	// Fixing format problems
	if (m.ext == ".gz") m.ext = ".tar.gz";

	// Ultimate++ - files extensions
	if (m.ext == ".t" || m.ext == ".lng") return large ? ltranslation : translation;
	if (m.ext == ".lay") return large ? llayout : layout;
	if (m.ext == ".iml") return fileImage;
	if (m.ext == ".usc") return fileScript;

	// Binary - files extensions (It seems that KDE has problem with multimedia MIME icons handling)
	if (KDE) {
		if (m.ext == ".bmp" || m.ext == ".dib" ||
			m.ext == ".gif" ||
			m.ext == ".jpg" || m.ext == ".jpeg" || m.ext == ".jpe" ||
			m.ext == ".png" ||
			m.ext == ".tif" || m.ext == ".tiff" ||
			m.ext == ".svg" ||
			m.ext == ".ico" ||
			m.ext == ".xcf")
			return fileImage;
		if (m.ext == ".aac" || m.ext == ".ogg" || m.ext == ".mp3")  return fileMusic;
	}

	Image img;
	if(m.ext.GetCount()) {
		m.ext = m.ext.Mid(1);
		m.large = large;
		img = MakeImage(m);
		isexe = false;
	}
	return IsNull(img) ? isexe ? (large ? lexe : exe) : (large ? lfile : file) : img;
}

#define GETFILEICON_DEFINED

#endif

#ifdef GUI_COCOA
struct FileIconMaker : ImageMaker {
	Txt file;
	bool   exe;
	bool   dir;
	bool   large;

	virtual Txt Key() const {
		return file + (exe ? "1" : "0") + (dir ? "1" : "0");
	}

	virtual Image Make() const {
		return GetIconForFile(file);
	}
};

#define GETFILEICON_DEFINED

Image GetFileIcon(tukk path, bool dir, bool exe, bool large, bool quick = false)
{
	FileIconMaker m;
	m.exe = exe;
	m.dir = dir;
	m.file = path;
	m.large = large;
	return MakeImage(m);
}
#endif

#ifndef GETFILEICON_DEFINED
Image PosixGetDriveImage(Txt dir, bool)
{
	if(dir.GetCount() == 0 || dir == "/")
		return CtrlImg::Computer();
	if(dir.Find("cdrom") == 0 || dir.Find("cdrecorder") == 0)
		return CtrlImg::CdRom();
	if(dir.Find("floppy") == 0 || dir.Find("zip") == 0)
		return CtrlImg::Diskette();
	return CtrlImg::Hd();
}

#ifdef TARGET_WIN32
Image GetFileIcon(const Txt& folder, bool, bool, bool, bool = false)
{
	return CtrlImg::File();
}
#else
Image GetFileIcon(const Txt& folder, const Txt& filename, bool isdir, bool isexe, bool = false)
{
	return isdir ? CtrlImg::Dir() : CtrlImg::File();
}

#endif
#endif

Image NativePathIcon0(tukk path, bool folder, bool large)
{
#if defined(TARGET_WIN32)
	if (folder)
		return GetFileIcon(path, true, true, large);
	else
		return GetFileIcon(path, false, false, large);
#endif
#ifdef TARGET_POSIX
	Txt p = path;
	FindFile ff(path);
#ifdef GUI_COCOA
	return GetFileIcon(path, folder, ff.GetMode() & 0111, large);
#else
	bool isdrive = folder && ((p == "/media") || (p == "/mnt"));
	return isdrive ? PosixGetDriveImage(GetFName(path), large)
				   : GetFileIcon(path, GetFName(path), folder, ff.GetMode() & 0111, large);
#endif
#endif
}

Image NativePathIcon(tukk path, bool folder)
{
	return NativePathIcon0(path, folder, false);
}

Image NativePathIcon(tukk path)
{
	FindFile ff(path);
	return NativePathIcon(path, ff.IsFolder());
}

Image NativePathLargeIcon(tukk path, bool folder)
{
	return NativePathIcon0(path, folder, true);
}

Image NativePathLargeIcon(tukk path)
{
	FindFile ff(path);
	return NativePathLargeIcon(path, ff.IsFolder());
}

bool MatchSearch(const Txt& filename, const Txt& search)
{
	return search.GetCount() ? Filter(filename, CharFilterDefaultToUpperAscii).Find(search) >= 0 : true;
}

bool Load(FileList& list, const Txt& dir, tukk patterns, bool dirs,
          Event<bool, const Txt&, Image&> WhenIcon, FSInfo& filesystem,
          const Txt& search, bool hidden, bool hiddenfiles, bool lazyicons)
{
	if(dir.IsEmpty()) {
		Array<FSInfo::FileInfo> root = filesystem.Find(Null);
		for(i32 i = 0; i < root.GetCount(); i++)
			if(MatchSearch(root[i].filename, search))
				list.Add(root[i].filename,
					GetDriveImage(root[i].root_style),
					StdFont().Bold(), SColorText, true, -1, Null, SColorDisabled,
			#ifdef TARGET_WIN32
					Nvl(root[i].root_desc, Txt(" ") + t_("Локальный Диск")),
			#else
					root[i].root_desc,
			#endif
					StdFont()
				);
		#ifdef TARGET_WIN32
			list.Add(t_("Сеть"), CtrlImg::Network(), StdFont().Bold(), SColorText,
			         true, -1, Null, SColorDisabled, Null, StdFont());
		#endif
	}
	else {
		Array<FSInfo::FileInfo> ffi =
			filesystem.Find(AppendFName(dir, filesystem.IsWin32() ? "*.*" : "*"));
		if(ffi.IsEmpty())
			return false;
	#if defined(TARGET_POSIX) && !defined(GUI_COCOA)
		bool isdrive = dir == "/media" || dir == "/mnt";
	#endif
		for(i32 t = 0; t < ffi.GetCount(); t++) {
			const FSInfo::FileInfo& fi = ffi[t];
			bool nd = dirs && !fi.is_directory;
			bool show = hidden;
			if(!show && filesystem.IsWin32() ? !fi.is_hidden : fi.filename[0] != '.')
				show = true;
			if(!show && hiddenfiles && fi.is_file)
				show = true;
			if(fi.filename != "." && fi.filename != ".." &&
			#ifdef TARGET_WIN32
			   (fi.is_directory || FileSel::IsLnkFile(fi.filename) || PatternMatchMulti(patterns, fi.filename)) &&
			#else
			   (fi.is_directory || PatternMatchMulti(patterns, fi.filename)) &&
			#endif
			   MatchSearch(fi.filename, search) && show) {
				Image img;
			#ifdef TARGET_POSIX
			#ifdef GUI_COCOA
				img = GetFileIcon(AppendFName(dir, fi.filename), fi.is_directory, fi.unix_mode & 0111, false, lazyicons);
			#else
				img = isdrive ? PosixGetDriveImage(fi.filename, false)
				              : GetFileIcon(dir, fi.filename, fi.is_directory, fi.unix_mode & 0111, false);
			#endif
			#endif
			#ifdef GUI_WIN
				img = GetFileIcon(AppendFName(dir, fi.filename), fi.is_directory, false, false, lazyicons);
			#endif
				if(IsNull(img))
					img = fi.is_directory ? CtrlImg::Dir() : CtrlImg::File();
				WhenIcon(fi.is_directory, fi.filename, img);
				list.Add(fi.filename, fi.is_hidden ? Contrast(img, 200) : img,
						 StdFont().Bold(fi.is_directory),
						 nd ? SColorDisabled : fi.is_hidden ? Blend(SColorText, Gray, 200) : SColorText, fi.is_directory,
						 fi.is_directory ? -1 : fi.length,
						 fi.last_write_time,
						 nd ? SColorDisabled
						    : fi.is_directory ? SColorText
						                      : fi.is_hidden ? Blend(SColorMark, Gray, 200)
						                                     : SColorMark,
				         Null, Null, Null, Null,
#ifdef TARGET_WIN32
                         false,
#else
						 fi.unix_mode & 0111,
#endif
				         fi.is_hidden
				);
			}
		}
	}
	return true;
}

#ifdef GUI_WIN
static Mutex       sExeMutex;
static WCHAR       sExePath[1025];
static bool        sExeRunning;
static SHFILEINFOW sExeInfo;

static auxthread_t auxthread__ sExeIconThread(uk )
{
	SHFILEINFOW info;
	WCHAR path[1025];
	CoInitialize(NULL);
	sExeMutex.Enter();
	wcscpy(path, sExePath);
	sExeMutex.Leave();
	AvoidPaintingCheck__();
	SHGetFileInfoW(path, FILE_ATTRIBUTE_NORMAL, &info, sizeof(info), SHGFI_ICON|SHGFI_SMALLICON);
	sExeMutex.Enter();
	memcpy(&sExeInfo, &info, sizeof(info));
	sExeRunning = false;
	sExeMutex.Leave();
	return 0;
}

void LazyExeFileIcons::Done(Image img)
{
	if(pos >= ndx.GetCount())
		return;
	i32 ii = ndx[pos];
	if(ii < 0 || ii >= list->GetCount())
		return;
	const FileList::File& f = list->Get(ii);
	WhenIcon(false, f.name, img);
	if(!IsNull(img)) {
		if(f.hidden)
			img = Contrast(img, 200);
		list->SetIcon(ii, img);
	}
	pos++;
}

WTxt LazyExeFileIcons::Path()
{
	if(pos >= ndx.GetCount())
		return Null;
	i32 ii = ndx[pos];
	if(ii < 0 || ii >= list->GetCount())
		return Null;
	const FileList::File& f = list->Get(ii);
	return NormalizePath(AppendFName(dir, f.name)).ToWTxt();
}

void LazyExeFileIcons::Do()
{
	i32 start = msecs();
	for(;;) {
		for(;;) {
			SHFILEINFOW info;
			bool done = false;
			WTxt path = Path();
			Vec<char16> path16 = ToUtf16(path);
			if(IsNull(path) || path16.GetCount() > 1000)
				return;
			sExeMutex.Enter();
			bool running = sExeRunning;
			if(!running) {
				done = path == ToUtf32(sExePath);
				memcpy(&info, &sExeInfo, sizeof(info));
				*sExePath = '\0';
				memset(&sExeInfo, 0, sizeof(sExeInfo));
			}
			sExeMutex.Leave();
			Image img = ProcessSHIcon(info.hIcon);
			if(done)
				Done(img);
			if(!running)
				break;
			Sleep(0);
			if(msecs(start) > 10 || Ctrl::IsWaitingEvent()) {
				Restart(0);
				return;
			}
		}

		WTxt path = Path();
		Vec<WCHAR> path16 = ToUtf16(path);
		if(IsNull(path) || path16.GetCount() > 1000)
			return;
		sExeMutex.Enter();
		memset(sExePath, 0, sizeof(sExePath));
		memcpy(sExePath, path16.begin(), sizeof(char16) * path16.GetCount());
		sExeRunning = true;
		StartAuxThread(sExeIconThread, NULL);
		sExeMutex.Leave();
	}
}

void LazyExeFileIcons::ReOrder()
{ // gather .exe and .lnk files; sort based on length so that small .exe get resolved first
	ndx.Clear();
	Vec<i32> len;
	for(i32 i = 0; i < list->GetCount(); i++) {
		const FileList::File& f = list->Get(i);
		if(findarg(ToLower(GetFileExt(f.name)), ".exe", ".lnk") >= 0 && !f.isdir) {
			ndx.Add(i);
			len.Add((i32)min((z64)INT_MAX, f.length));
		}
	}
	IndexSort(len, ndx);
	Restart(0);
}

void LazyExeFileIcons::Start(FileList& list_, const Txt& dir_, Event<bool, const Txt&, Image&> WhenIcon_)
{
	list = &list_;
	dir = dir_;
	WhenIcon = WhenIcon_;
	pos = 0;
	ReOrder();
}
#endif

Txt FileSel::GetDir() const
{
	Txt s = ~dir;
	if(s.IsEmpty()) return basedir;
	if(basedir.IsEmpty()) return s;
	return AppendFName(basedir, s);
}

void FileSel::SetDir(const Txt& _dir) {
#ifdef TARGET_WIN32
	netstack.Clear();
	netnode.Clear();
#endif
	dir <<= _dir;
	Load();
	Update();
}

Txt FileSel::FilePath(const Txt& fn) {
	return AppendFName(GetDir(), fn);
}

Image GetDriveImage(char drive_style)
{
	switch(drive_style)
	{
	case FSInfo::ROOT_NO_ROOT_DIR: return Null;
	case FSInfo::ROOT_REMOTE:
	case FSInfo::ROOT_NETWORK:   return CtrlImg::Share();
	case FSInfo::ROOT_COMPUTER:  return CtrlImg::Computer();
	case FSInfo::ROOT_REMOVABLE: return CtrlImg::Flash();
	case FSInfo::ROOT_CDROM:     return CtrlImg::CdRom();
	default:                             return CtrlImg::Hd();
	}
}

Txt FileSel::GetMask()
{
	Txt emask = "*";
	if(!IsNull(type)) {
		if(IsTxt(~type))
			emask = ~type;
		else {
			i32 q = ~type;
			if(q >= 0 && q < mask.GetCount())
				emask = mask[q];
		}
	}
	return emask;
}

void FileSel::Load()
{
	search <<= Null;
	SearchLoad();
}

void FileSel::LoadNet()
{
#ifdef TARGET_WIN32
	list.Clear();
	for(i32 i = 0; i < netnode.GetCount(); i++) {
		Image m = CtrlImg::Group();
		switch(netnode[i].GetDisplayType()) {
		case NetNode::NETWORK:
			m = CtrlImg::Network();
			break;
		case NetNode::SHARE:
			m = CtrlImg::Share();
			break;
		case NetNode::SERVER:
			m = CtrlImg::Computer();
			break;
		}
		list.Add(netnode[i].GetName(), m);
	}
	places.FindSetCursor("\\");
#endif
}

void FileSel::SelectNet()
{
#ifdef TARGET_WIN32
	i32 q = list.GetCursor();
	if(q >= 0 && q < netnode.GetCount()) {
		NetNode& n = netnode[q];
		Txt p = n.GetPath();
		if(p.GetCount())
			SetDir(p);
		else {
			NetNode n = netnode[q];
			netstack.Add() = n;
			ScanNetwork([=, this] {
				return n.Enum();
			});
		}
	}
#endif
}

#ifdef TARGET_WIN32
bool FileSel::ScanNetwork(Function<Array<NetNode> ()> fn)
{
	Progress pi(t_("Сканируется сеть.."));
	loading_network = true;
	for(;;) {
		Ptr<FileSel> fs = this;
		if(CoWork::TrySchedule([=, this] {
			Array<NetNode> n = fn();
			GuiLock __;
			if(fs) {
				fs->netnode = pick(n);
				fs->loading_network = false;
			}
		}))
			break;
		if(pi.StepCanceled()) {
			SetDir("");
			return false;
		}
		Sleep(10);
	}
	for(;;) {
		if(pi.StepCanceled()) {
			SetDir("");
			return false;
		}
		if(!loading_network) {
			LoadNet();
			break;
		}
		Sleep(10);
	}
	return true;
}

bool FileSel::netroot_loaded;
Array<NetNode> FileSel::netroot;

void FileSel::ScanNetworkRoot()
{
	if(netroot_loaded) {
		netnode = clone(netroot);
		LoadNet();
		return;
	}
	netroot_loaded = ScanNetwork([] {
		Array<NetNode> n;
		n = NetNode::EnumRoot();
		n.Append(NetNode::EnumRemembered());
		return n;
	});
	netroot = clone(netnode);
}
#endif

void FileSel::SearchLoad()
{
	loaded = true;
	list.EndEdit();
	list.Clear();
	Txt d = GetDir();
#ifdef TARGET_WIN32
	if(d == "\\") {
		ScanNetworkRoot();
		return;
	}
#endif
	Txt emask = GetMask();
	if(!DRX::Load(list, d, emask, mode == SELECTDIR, WhenIcon, *filesystem, ~search, ~hidden, ~hiddenfiles, true)) {
		loaded = false;
		Exclamation(t_("[A3* Не удаётся прочесть папку !]&&") + DeQtf((Txt)~dir) + "&&" +
		            GetErrorMessage(GetLastError()));
		if(!basedir.IsEmpty() && Txt(~dir).IsEmpty()) {
			Break(IDCANCEL);
			return;
		}
		dir <<= olddir;
		olddir = Null;
		SearchLoad();
	}

	places.KillCursor();
	if(d.GetCount())
		places.FindSetCursor(d);
	hiddenfiles.Enable(!hidden);
	if(d.IsEmpty()) {
		if(filesystem->IsWin32()) {
			mkdir.Disable();
			plus.Disable();
			minus.Disable();
			toggle.Disable();
			list.Renaming(false);
		}
		dir <<= d;
		dirup.Disable();
	}
	else {
		dirup.Enable();
		mkdir.Enable();
		plus.Enable();
		minus.Enable();
		toggle.Enable();
		list.Renaming(true);
	}
	if(filesystem->IsPosix())
		if(d == "/" || (!IsEmpty(basedir) && Txt(~dir).IsEmpty()))
			dirup.Disable();
	if(filesystem->IsWin32())
		if(!IsEmpty(basedir) && Txt(~dir).IsEmpty())
			dirup.Disable();
	olddir = ~dir;
	if(olddir.GetCount() || basedir.GetCount())
		SortBy(list, ~sortby);
	Update();
#ifdef GUI_WIN
	if(!noexeicons)
		lazyicons.Start(list, d, WhenIcon);
#endif
	StartLI();
}

StaticMutex FileSel::li_mutex;
void      (*FileSel::li_current)(const Txt& path, Image& result);
Txt      FileSel::li_path;
Image       FileSel::li_result;
bool        FileSel::li_running;
i32         FileSel::li_pos;

void FileSel::LIThread()
{
	Txt path;
	void (*li)(const Txt& path, Image& result);
	{
		Mutex::Lock __(li_mutex);
		path = li_path;
		li = li_current;
	}
	Image result;
	if(path.GetCount())
		li(path, result);
	if(!IsNull(result) && max(result.GetWidth(), result.GetHeight()) > DPI(16))
		result = Rescale(result, DPI(16), DPI(16));
	{
		Mutex::Lock __(li_mutex);
		li_result = result;
		li_running = false;
	}
}

Txt FileSel::LIPath()
{
	return li_pos >= 0 && li_pos < list.GetCount() ? FilePath(list.Get(li_pos).name) : Null;
}

void FileSel::DoLI()
{
	i32 start = msecs();
	for(;;) {
		for(;;) {
			bool done = false;
			Txt path = LIPath();
			if(IsNull(path))
				return;
			bool running;
			Image img;
			{
				Mutex::Lock __(li_mutex);
				running = li_running;
				if(!running) {
					done = li_path == path && li_current == WhenIconLazy;
					img = li_result;
				}
			}
			if(done) {
				if(li_pos < 0 || li_pos >= list.GetCount())
					return;
				if(!IsNull(img)) {
					const FileList::File& f = list.Get(li_pos);
					WhenIcon(f.isdir, f.name, img);
					if(f.hidden)
						img = Contrast(img, 200);
					list.SetIcon(li_pos, img);
				}
				li_pos++;
			}
			if(!running)
				break;
			Sleep(0);
			if(msecs(start) > 10 || Ctrl::IsWaitingEvent()) {
				ScheduleLI();
				return;
			}
		}

		Txt path = LIPath();
		if(IsNull(path))
			return;
		{
			Mutex::Lock __(li_mutex);
			if(!li_running) {
				li_current = WhenIconLazy;
				li_path = path;
				li_running = true;
				Thread::Start(callback(LIThread));
			}
		}
	}
}

void FileSel::StartLI()
{
	if(WhenIconLazy) {
		li_pos = 0;
		ScheduleLI();
	}
}

Txt TrimDot(Txt f) {
	i32 i = f.Find('.');
	if(i >= 0 && i == f.GetLength() - 1)
		f.Trim(i);
	return f;
}

void FileSel::AddName(Vec<Txt>& fn, Txt& f) {
	if(!f.IsEmpty()) {
		f = TrimDot(f);
		if(f[0] == '\"' && f.GetCount() > 2)
			f = f.Mid(1, f.GetCount() - 2);
		i32 q = f.ReverseFind('.');
		Txt typed_ext; // typed by user
		if(q >= 0)
			typed_ext = f.Mid(q + 1);
		if(q < 0 || // no extension
		   (force_ext && allowed_ext.Find(typed_ext) < 0) ||
		   Filter(f.Mid(q + 1), // "(file.xxx)" should add extension too, allow just some
		                   [](wchar c) { return IsAlNum(c) || findarg(c, '_', '-') >= 0 ? 0 : c; }
		   ).GetCount()) {
			Txt t = GetMask();
			i32 q = t.Find('.');
			if(q >= 0 && IsAlNum(t[q + 1])) {
				i32 w = q + 2;
				while(IsAlNum(t[w]))
					w++;
				f << t.Mid(q, w - q);
			}
			else
			if(defext.GetCount())
				f << '.' << defext;
		}
		fn.Add(f);
	}
	f.Clear();
}

bool FileSel::IsLnkFile(const Txt& p)
{
	i32 l = p.GetLength() - 4;
	return l >= 0 && p[l] == '.' && ToLower(p[l + 1]) == 'l' && ToLower(p[l + 2]) == 'n' && ToLower(p[l + 3]) == 'k';
}

Txt FileSel::ResolveLnk(const Txt& name) const
{
#ifdef TARGET_WIN32
	if(IsLnkFile(name))
		return GetSymLinkPath(AppendFName(GetDir(), name));
#endif
	return Null;
}

Txt FileSel::ResolveLnkDir(const Txt& name) const
{
#ifdef TARGET_WIN32
	Txt p = ResolveLnk(name);
	if(p.GetCount() && DirExists(p))
		return p;
#endif
	return Null;
}

Txt FileSel::ResolveLnkFile(const Txt& name) const
{
#ifdef TARGET_WIN32
	Txt p = ResolveLnk(name);
	if(p.GetCount() && FileExists(p))
		return p;
#endif
	return Null;
}

void FileSel::Finish() {
	if(filesystem->IsWin32())
		if(GetDir().IsEmpty()) {
			file.Clear();
			return;
		}
	fn.Clear();
	if(mode == SELECTDIR) {
		Txt p = GetDir();
		if(list.IsSelection() && multi) {
			for(i32 i = 0; i < list.GetCount(); i++)
				if(list.IsSelected(i)) {
					const FileList::File& m = list[i];
					if(m.isdir)
						fn.Add(AppendFName(p, m.name));
				#ifdef TARGET_WIN32
					else {
						Txt p = ResolveLnkDir(m.name);
						if(p.GetCount())
							fn.Add(p);
					}
				#endif
				}
		}
		else {
			Txt p = GetDir();
			if(list.GetCursor() >= 0) {
				const FileList::File& m = list[list.GetCursor()];
				if(m.isdir)
					p = AppendFName(p, m.name);
			#ifdef TARGET_WIN32
				else {
					Txt pp = ResolveLnkDir(m.name);
					if(p.GetCount())
						p = pp;
				}
			#endif
			}
			fn.Add(p);
		}
		Break(IDOK);
		return;
	}
	Txt f = file.GetText().ToTxt();
	if(f.IsEmpty()) return;
	Txt o;
	if(mode == OPEN && IsMulti()) {
		for(tukk s = f; *s; s++) {
			if(*s == ' ')
				AddName(fn, o);
			else
			if(*s == '\"') {
				AddName(fn, o);
				s++;
				for(;;) {
					if(*s == '\0' || *s == '\"') {
						AddName(fn, o);
						break;
					}
					o.Cat(*s++);
				}
			}
			else
				o.Cat(*s);
		}
		AddName(fn, o);
	}
	else {
		o = f;
		AddName(fn, o);
	}
	if(!IsMulti() && fn.GetCount())
		fn.SetCount(1);
	Txt d = GetDir();
	Txt nonexist;
	i32 ne = 0;
	for(i32 i = 0; i < fn.GetCount(); i++) {
		Txt p = fn[i];
		if(!IsFullPath(p))
			p = NormalizePath(AppendFName(d, fn[i]));
		Array<FSInfo::FileInfo> ff = filesystem->Find(p, 1);
		p = DeQtf(p);
		if(!ff.IsEmpty() && ff[0].is_directory) {
			Exclamation(p + t_("является папкой."));
			return;
		}
		if(asking) {
			if(mode == SAVEAS) {
				if(!ff.IsEmpty() && !PromptOKCancel(p + t_(" уже существует.&Продолжить ?")))
					return;
			}
			else
			if(ff.IsEmpty()) {
				if(ne) nonexist << '&';
				nonexist << p;
				ne++;
			}
		}
	}
	if(ne) {
		nonexist << (ne == 1 ? t_(" не существует.") : t_("&не существуют."));
		if(!PromptOKCancel(nonexist + t_("&Продолжить ?")))
			return;
	}
	Break(IDOK);
}

bool FileSel::OpenItem() {
	fw.Clear();
	if(list.IsCursor()) {
	#ifdef TARGET_WIN32
		if(netnode.GetCount()) {
			SelectNet();
			return true;
		}
	#endif
		const FileList::File& m = list.Get(list.GetCursor());
		Txt path = AppendFName(~dir, m.name);
	#ifdef TARGET_WIN32
		if(IsNull(dir) && m.name == t_("Сеть")) {
			ScanNetworkRoot();
			return true;
		}
		Txt p = ResolveLnkDir(m.name);
		if(p.GetCount()) {
			SetDir(p);
			return true;
		}
	#endif
		if(m.isdir) {
			SetDir(path);
			return true;
		}
	}
	if(mode != SELECTDIR)
		Finish();
	return false;
}

void FileSel::Open() {
	if(mode == SELECTDIR) {
	#ifdef TARGET_WIN32
		if(netnode.GetCount())
			return;
	#endif
		Finish();
		return;
	}
	if(list.HasFocus() || type.HasFocus()) {
		if(OpenItem()) list.SetCursor(0);
	}
	else
	if(list.IsSelection())
		for(i32 i = 0; i < list.GetCount(); i++) {
			const FileList::File& m = list[i];
			if(!m.isdir) Finish();
		}
	else
	if(file.HasFocus()) {
		Txt fn = file.GetText().ToTxt();
	#ifdef TARGET_WIN32
		if(fn[0] == '\\' && fn[1] == '\\') {
			FindFile ff(AppendFName(fn, "*.*"));
			if(ff)
				SetDir(TrimDot(fn));
			return;
		}
	#endif
		if(fn == "." || fn == "..") {
			DirUp();
			return;
		}
		if(HasWildcards(fn)) {
			file.Clear();
			i32 q = FindIndex(mask, fn);
			if(q >= 0)
				type.SetIndex(q);
			else {
				type.Add(fn, t_("Кастомный тип файла (") + fn + ')');
				type.SetIndex(type.GetCount() - 1);
			}
			Load();
			return;
		}
		if(fn.Find('\"') < 0) {
			if(filesystem->IsWin32())
			{
				if(fn.GetLength() >= 2 && fn[1] == ':' && fn.GetLength() <= 3) {
					fn.Set(0, ToUpper(fn[0]));
					if(fn.GetLength() == 2)
						fn.Cat('\\');
					SetDir(fn);
					return;
				}
			}
			if(!IsFullPath(fn))
				fn = AppendFName(~dir, fn);
			if(filesystem->IsWin32() && (!fn.IsEmpty() && (*fn.Last() == '\\' || *fn.Last() == '/'))
			|| filesystem->IsPosix() && (fn != "/" && (*fn.Last() == '\\' || *fn.Last() == '/')))
			{
				fn.Trim(fn.GetLength() - 1);
				SetDir(TrimDot(fn));
				return;
			}
			Array<FSInfo::FileInfo> ff = filesystem->Find(fn, 1);
			if(!ff.IsEmpty()) {
				if(ff[0].is_directory) {
					SetDir(TrimDot(fn));
					return;
				}
				else {
					SetDir(TrimDot(GetFileFolder(fn)));
					file.SetText(GetFName(fn).ToWTxt());
				}
			}
		}
		if(mode != SELECTDIR)
			Finish();
	}
	else
	if(mode == SAVEAS)
		Finish();
}

Txt DirUp(Txt& dir, bool basedir)
{
	while(*dir.Last() == '\\' || *dir.Last() == '/')
		dir.Trim(dir.GetCount() - 1);
	Txt s = dir;
	Txt name;
#ifdef TARGET_WIN32
	if(s.GetLength() < 3 || s.GetLength() == 3 && s[1] == ':') {
		dir.Clear();
		name = s;
	}
	else
#endif
#ifdef TARGET_POSIX
	if(s != "/")
#endif
	{
#ifdef TARGET_WIN32
		i32 i = max(s.ReverseFind('/'), s.ReverseFind('\\'));
#endif
#ifdef TARGET_POSIX
		i32 i = s.ReverseFind('/');
#endif
		if(basedir)
			if(i < 0)
				dir.Clear();
			else {
				dir = s.Mid(0, i);
				name = s.Mid(i + 1);
			}
		else {
#ifdef TARGET_WIN32
			if(s.GetLength() && s[1] == ':') {
				if(i > 3) {
					dir = s.Mid(0, i);
					name = s.Mid(i + 1);
				}
				else {
					dir = s.Mid(0, 3);
					name = s.Mid(3);
				}
			}
			if(s.GetLength() && s[0] == DIR_SEP && s[1] == DIR_SEP) {
				if(i > 2) {
					dir = s.Mid(0, i);
					name = s.Mid(i + 1);
				}
				else {
					dir.Clear();
					name = s;
				}
			}
#endif
#ifdef TARGET_POSIX
			if(i == 0 && s.GetLength() > 1) {
				dir = "/";
				name = s.Mid(1);
			}
			else
			if(s.GetLength() && s[0] == '/' && s[1] != '/') {
				dir = s.Mid(0, i);
				name = s.Mid(i + 1);
			}
#endif
		}
	}
	return name;
}

void FileSel::DirUp()
{
#ifdef TARGET_WIN32
	if(netstack.GetCount()) {
		netstack.Drop();
		if(netstack.GetCount()) {
			netnode = netstack.Top().Enum();
			LoadNet();
		}
		netnode = NetNode::EnumRoot();
		return;
	}
	if(netnode.GetCount()) {
		netnode.Clear();
		SetDir("");
		return;
	}
#endif
	Txt s = ~dir;
	fw.Add(s);
	Txt name = drx::DirUp(s, !basedir.IsEmpty());
#ifdef TARGET_WIN32
	if(s[0] == '\\' && s[1] == '\\' && s.Find('\\', 2) < 0) {
		s.Clear();
		name.Clear();
	}
#endif
	SetDir(s);
	if(list.HasFocus())
		list.FindSetCursor(name);
}

void FileSel::MkDir() {
	if(Txt(~dir).IsEmpty() && basedir.IsEmpty()) return;
	Txt name, error;
	if(EditText(name, t_("Новая папка"), t_("Имя")) && !name.IsEmpty()) {
		if(filesystem->CreateFolder(FilePath(name), error)) {
			Load();
			list.FindSetCursor(name);
		}
		else
			Exclamation(t_("[A3* Неудача при создании папки !&&]") + error);
	}
}

void FileSel::PlusMinus(tukk title, bool sel) {
	Txt pattern;
	if(EditText(pattern, title, t_("Маска")) && !pattern.IsEmpty())
		for(i32 i = 0; i < list.GetCount(); i++)
			if(!list.Get(i).isdir)
				if(PatternMatchMulti(pattern, list.Get(i).name))
					list.SelectOne(i, sel);
}

void FileSel::Plus() {
	PlusMinus(t_("Добавить в выделенное"), true);
}

void FileSel::Minus() {
	PlusMinus(t_("Удалить из выделенного"), false);
}

void FileSel::Toggle() {
	for(i32 i = 0; i < list.GetCount(); i++)
		if(!list.Get(i).isdir)
			list.SelectOne(i, !list.IsSelected(i));
}

void FileSel::Reload()
{
	Txt fn = list.GetCurrentName();
	i32 a = list.GetScroll();
	SearchLoad();
	list.ScrollTo(a);
	list.FindSetCursor(fn);
}

void FileSel::Activate()
{
	if(loaded && !loading_network && GetDir() != "\\"
#ifdef TARGET_WIN32
	   && netnode.GetCount() == 0
#endif
	)
		Reload();
	TopWindow::Activate();
}

bool FileSel::Key(dword key, i32 count) {
	switch(key) {
	case K_F9:
#ifdef TARGET_WIN32
		netroot_loaded = false;
#endif
		Reload();
		return true;
	case K_MOUSE_FORWARD:
		if(fw.GetCount())
			SetDir(fw.Pop());
		return true;
	case K_MOUSE_BACKWARD:
	case '.':
	case K_CTRL_UP:
		list.SetFocus();
		dirup.PseudoPush();
		return true;
	case '+':
		plus.PseudoPush();
		return true;
	case '-':
		minus.PseudoPush();
		return true;
	case '*':
		toggle.PseudoPush();
		return true;
	case K_F5:
		Reload();
		return true;
	case K_F6:
		list.StartEdit();
		return true;
	case K_F7:
		mkdir.PseudoPush();
		return true;
	case K_ENTER:
		if(mode == SELECTDIR && OpenItem())
			return true;
		break;
	case K_UP:
	case K_DOWN:
	case K_PAGEUP:
	case K_PAGEDOWN:
		list.SetFocus();
		return list.Key(key, count);
	}
	if(CharFilterDefaultToUpperAscii(key) || key == K_BACKSPACE)
		return search.Key(key, count);
	return TopWindow::Key(key, count);
}

void Catq(Txt& s, const Txt& fn) {
	if(!s.IsEmpty())
		s << ' ';
	if(fn.Find(' ') >= 0)
		s << '"' << fn << '"';
	else
		s << fn;
}

Txt FormatFileSize(z64 n)
{
	if(n < 10000)
		return Format("%d B  ", n);
	else
	if(n < 10000 * 1024)
		return Format("%d.%d K  ", n >> 10, (n & 1023) / 103);
	else
	if(n < I64(10000000) * 1024)
		return Format("%d.%d M  ", n >> 20, (n & 1023) / 103);
	else
		return Format("%d.%d G  ", n >> 30, (n & 1023) / 103);
}

void FileSel::Update() {
	Txt fn;
	if(list.IsSelection()) {
		for(i32 i = 0; i < list.GetCount(); i++)
			if(list.IsSelected(i))
				Catq(fn, list[i].name);
	}
	else
	if(list.IsCursor()) {
		const FileList::File& m = list[list.GetCursor()];
		if(!m.isdir)
			Catq(fn, m.name);
	}
	if(mode == OPEN)
		file <<= fn;
	filename = Txt();
	filesize = Txt();
	filetime = Txt();
	if(preview)
		*preview <<= Null;
	if(list.IsCursor()) {
		fn = list[list.GetCursor()].name;
		if(fn[1] == ':' && fn.GetLength() <= 3)
			filename = t_("  Диск");
		else {
			Txt path = FilePath(fn);
			Array<FSInfo::FileInfo> ff = filesystem->Find(path, 1);
			if(!ff.IsEmpty()) {
				filename = "  " + fn;
				if(ff[0].is_directory)
					filesize = t_("Папка  ");
				else {
					if(mode == SAVEAS)
						file <<= fn;
					filesize = FormatFileSize(ff[0].length);
					if(preview)
						*preview <<= path;
				}
				Time tm = ff[0].last_write_time;
				filetime = "     " + Format(tm);
			}
		}
	}
	else {
		i32 drives = 0;
		i32 dirs = 0;
		i32 files = 0;
		z64 length = 0;
		for(i32 i = 0; i < list.GetCount(); i++)
			if(!list.IsSelection() || list.IsSelected(i)) {
				const FileList::File& f = list[i];
				if(f.isdir)
#ifdef TARGET_WIN32
					(*f.name.Last() == ':' ? drives : dirs)++;
#else
					dirs++;
#endif
				else {
					files++;
					length += f.length;
				}
			}
		Txt s;
		if(drives)
			s << drives << t_(" диск(ов)");
		else {
			if(dirs)
				s << dirs << t_(" папка(ок)");
			if(files) {
				if(s.GetCount())
					s << ", ";
				s << files << t_(" файл(ов)");
			}
		}
		filename = "  " + s;
		if(length >= 0)
			filesize = FormatFileSize(length);
	}
	FileUpdate();
}

void FileSel::FileUpdate() {
	if(mode == SELECTDIR) {
		ok.Enable(!IsNull(~dir));
		return;
	}
	bool b = list.IsCursor() || !Txt(~file).IsEmpty();
	ok.Enable(b);
	if(mode != SAVEAS || list.IsCursor() && list[list.GetCursor()].isdir)
		ok.SetLabel(t_("Открыть"));
	else
		ok.SetLabel(t_("Сохранить"));
}

void FileSel::Rename(const Txt& on, const Txt& nn) {
	if(on == nn) return;
#ifdef TARGET_WIN32
	if(FileMove(FilePath(on), FilePath(nn)))
#else
	if(rename(FilePath(on), FilePath(nn)) == 0)
#endif
	{
		Load();
		list.FindSetCursor(nn);
	}
	else
		Exclamation(t_("[A3* Неудача при переименовании файла!&&]") + GetErrorMessage(GetLastError()));
}

void FileSel::Choice() {
	Load();
}

FileSel& FileSel::Type(tukk name, tukk ext) {
	type.Add(type.GetCount(), name);
	mask.Add(ext);
	if(IsNull(type))
		type.SetIndex(0);
	return *this;
}

FileSel& FileSel::Types(tukk d) {
	Vec<Txt> s = Split(d, '\n');
	for(i32 i = 0; i < s.GetCount(); i++) {
		Vec<Txt> h = Split(s[i], '\t');
		if(h.GetCount() == 2)
			Type(h[0], h[1]);
		if(h.GetCount() == 1)
			Type(h[0], h[0]);
	}
	return *this;
}

FileSel& FileSel::ClearTypes()
{
	type.Clear();
	mask.Clear();
	return *this;
}

FileSel& FileSel::ActiveType(i32 i)
{
	activetype.Clear();
	if(i >= 0 && i < type.GetCount())
		activetype = type.GetVal(i);
	return *this;
}

FileSel& FileSel::AllFilesType() {
	return Type(t_("Все файлы"), "*.*");
}

struct FolderDisplay : public Display {
	virtual void Paint(Draw& w, const Rect& r, const Val& q,
	                   Color ink, Color paper, dword style) const;
};

Image GetDirIcon(const Txt& s)
{
	Image img;
#ifdef TARGET_X11
	img = GetFileIcon(GetFileFolder(s), GetFName(s), true, false, false);
#endif
#ifdef TARGET_WIN32
	if((u8)*s.Last() == 255)
		img = CtrlImg::Network();
	else {
		i32 q = s.Find(0);
		if(q >= 0 && q + 1 < s.GetCount())
			img = GetDriveImage(s[q + 1]);
		else
			img = s.GetCount() ? GetFileIcon(s, false, true, false) : CtrlImg::Computer();
	}
#endif
	if(IsNull(img))
		img = CtrlImg::Dir();
	return DPI(img);
}

void FolderDisplay::Paint(Draw& w, const Rect& r, const Val& q,
                          Color ink, Color paper, dword style) const
{
	Txt s = q;
	w.DrawRect(r, paper);
	Image img = GetDirIcon(s);
	w.DrawImage(r.left, r.top + (r.Height() - img.GetSize().cx) / 2, img);
	w.DrawText(r.left + Zx(20),
	           r.top + (r.Height() - StdFont().Bold().Info().GetHeight()) / 2,
			   ~s, StdFont().Bold(), ink);
}

struct HomeDisplay : public Display {
	virtual void Paint(Draw& w, const Rect& r, const Val& q,
	                   Color ink, Color paper, dword style) const {
		w.DrawRect(r, paper);
		Image img = CtrlImg::Home();
		w.DrawImage(r.left, r.top + (r.Height() - img.GetSize().cx) / 2,
			        CtrlImg::Home());
		w.DrawText(r.left + Zx(20),
		           r.top + (r.Height() - StdFont().Bold().Info().GetHeight()) / 2,
				   Txt(q), StdFont().Bold(), ink);
	}
};

void FileSel::Set(const Txt& s)
{
	fn.Clear();
	if(IsFullPath(s)) {
		ActiveDir(GetFileFolder(s));
		fn.Add(GetFName(s));
	}
	else
		fn.Add(s);
	bidname = true;
}

void FileSel::GoToPlace()
{
	if(places.IsCursor()) {
#ifdef TARGET_WIN32
		netnode.Clear();
#endif
		dir <<= places.GetKey();
		Load();
	}
}

Image SynthetisePathIcon(const Txt& path)
{
	Size isz = DPI(16, 16);
	ImagePainter iw(isz);
	iw.Clear(RGBAZero());
	i32 x = FoldHash(GetHashVal(path));
	auto cl = [](i32 x) { return 128 + (x & 127); };
	iw.Circle(DPI(8), DPI(8), DPI(7)).Fill(Color(cl(x), cl(x >> 7), cl(x >> 14))).Stroke(1, SBlack());
	WTxt s = GetFileTitle(path).ToWTxt();
	if(s.GetCount()) {
		s = s.Mid(0, 1);
		Font fnt = Serif(DPI(12));
		Size tsz = GetTextSize(s, fnt);
		iw.DrawText((isz.cx - tsz.cx) / 2, (isz.cy - tsz.cy) / 2, s, fnt, Black());
	}
	return iw;
}

Txt PathName(const Txt& path)
{
	i32 cx = Zx(100);
	Txt p = path;
	p.Replace("\\", "/");
	if(GetTextSize(p, StdFont()).cx < cx) return p;
	cx -= GetTextSize("..", StdFont()).cx;
	while(GetTextSize(p, StdFont()).cx > cx && p.GetCount() > 1)
		p = p.Mid(1);
	return ".." + p;
}

bool FileSel::Execute(i32 _mode) {
	mode = _mode;

	i32 fixed_places = places.GetCount();

	Vec<Txt> glru;
	LoadFromGlobal(glru, "GlobalFileSelectorLRU");
	if(glru.GetCount()) {
		AddPlaceSeparator();
		for(const Txt& path : glru)
			AddPlace(path, SynthetisePathIcon(path), PathName(path), "PLACES:FOLDER");
	}

	i32 system_row = -1;
	for(i32 i = places.GetCount() - 1; i >= 0; i--) {
		if(places.Get(i, 3) == "PLACES:SYSTEM") {
			system_row = i;
			places.Remove(i);
		}
	}
	AddSystemPlaces(system_row);

	if(mode == SELECTDIR) {
		if(!fn.IsEmpty()) {
			Txt h = ~dir;
			dir <<= NormalizePath(fn[0]);
			if(!DirExists(~~dir))
				dir <<= h;
		}
		type.Hide();
		type_lbl.Hide();
		file.Hide();
		file_lbl.Hide();
		sortby.Hide();
		sort_lbl.Hide();
		ok.SetLabel(t_("&Выбрать"));
		Logc p = filename.GetPos().y;
		i32 q = ok.GetPos().y.GetA() + ok.GetPos().y.GetB() + Zy(16);
		p.SetA(q);
		filename.SetPosY(p);
		filesize.SetPosY(p);
		filetime.SetPosY(p);
		p = splitter.Ctrl::GetPos().y;
		p.SetB(q + Zy(28));
		splitter.SetPosY(p);
		LogPos ps = search.GetPos();
		LogPos pl = sort_lbl.GetPos();
		pl.x.SetB(ps.x.GetB());
		pl.y.SetA(ok.GetPos().y.GetA());
		pl.y.SetB(ps.y.GetB());
		search.SetPos(pl);
		bidname = false;
	}
	else {
		for(Ctrl *q = GetFirstChild(); q; q = q->GetNext())
			if(q != &mkdir)
				q->Show();
		Rect r = GetRect();
		CtrlLayout(*this);
		ArrangeOKCancel(ok, cancel);
		SetRect(r);
	}

	if(file_ctrl) {
		LogPos sp = search.GetPos();
		LogPos fp = file.GetPos();
		file.HSizePos(fp.x.GetA(), 2 * sp.x.GetA() + file_ctrl_cx);
		AddChild(file_ctrl, &file);
		file_ctrl->BottomPos(fp.y.GetA(), fp.y.GetB()).RightPos(sp.x.GetA(), file_ctrl_cx);
	}

	readonly.Show(rdonly && mode == OPEN);
	list.Multi(multi && (mode == OPEN || mode == SELECTDIR));
	list.SelectDir(multi && mode == SELECTDIR);
	dir.ClearList();
	file <<= Null;
	if(basedir.IsEmpty()) {
		dir.Add(GetHomeDir());
	#ifdef TARGET_POSIX
		Array<FSInfo::FileInfo> root = filesystem->Find("/media/*");
		dir.Add(GetDesktopFolder());
		dir.Add("/");
		for(i32 i = 0; i < root.GetCount(); i++) {
			Txt ugly = root[i].filename;
			if(ugly[0] != '.') {
				dir.Add("/media/" + root[i].filename);
			}
		}
	#else
		dir.Add(GetDesktopFolder());
		Array<FSInfo::FileInfo> root = filesystem->Find(Null);
		for(i32 i = 0; i < root.GetCount(); i++) {
			Txt ugly = root[i].filename;
			if(ugly != "A:\\" && ugly != "B:\\") {
				ugly.Cat('\0');
				ugly.Cat(root[i].root_style);
				dir.Add(root[i].filename, ugly);
			}
		}
		if(filesystem == &StdFileSystemInfo())
			dir.Add("\\", Txt(t_("Сеть")) + Txt(0, 1) + "\xff");
	#endif
		if(filesystem->IsPosix() && Txt(~dir).IsEmpty())
			dir <<= GetHomeDir();
		if(lru.GetCount())
			dir.AddSeparator();
		for(i32 i = 0; i < lru.GetCount(); i++)
			if(IsFullPath(lru[i]) && filesystem->FolderExists(lru[i]))
				dir.Add(lru[i]);
		dir.SetDisplay(Single<FolderDisplay>(), max(16, Draw::GetStdFontCy()));
	}
	else {
		dir.SetDisplay(Single<HomeDisplay>(), max(16, Draw::GetStdFontCy()));
		if(filesystem->IsPosix()) {
			if(Txt(~dir)[0] == '/')
				dir <<= "";
		}
	}
	Rect lr = splitter.GetRect();
	Rect dr = dir.GetRect();
	i32 dp = max(20, dir.Ctrl::GetPos().y.GetB());
	i32 px = GetSize().cx - lr.right;
/*	if(IsMulti()) { // Cxl: Have we ever used these?!
		toggle.RightPos(px, dp).TopPos(dr.top, dp);
		minus.RightPos(px + 2 * dp, dp).TopPos(dr.top, dp);
		plus.RightPos(px + 3 * dp, dp).TopPos(dr.top, dp);
		px += 3 * dp;
		toggle.Show();
		minus.Show();
		plus.Show();
	}
	else {*/
		toggle.Hide();
		minus.Hide();
		plus.Hide();
//	}
	if(mkdir.IsShown()) {
		mkdir.RightPos(px, dp).TopPos(dr.top, dp);
		dirup.RightPos(px + dp, dp).TopPos(dr.top, dp);
		px += 2 * dp;
	}
	else {
		dirup.RightPos(px, dp).TopPos(dr.top, dp);
		px += dp;
	}
	dir.HSizePos(dr.left, px + 4);
	if(preselect.GetCount()) {
		for(i32 i = 0; i < mask.GetCount(); i++) {
			if(PatternMatchMulti(mask[i], preselect)) {
				ActiveType(i);
				break;
			}
		}
	}
	i32 q = type.FindVal(activetype);
	if(q >= 0)
		type <<= q;
	else
	if(type.GetCount())
		type.SetIndex(0);
	i32 dlc = type.GetCount();
	Load();
	ActiveFocus(file.IsEditable() ? (Ctrl&)file : (Ctrl&)list);
	if(bidname) {
		Txt s;
		for(i32 i = 0; i < fn.GetCount(); i++)
			Catq(s, fn[i]);
		file <<= s;
		ActiveFocus(file);
		bidname = false;
	}
	list.SetSbPos(lastsby);
	if(preselect.GetCount()) {
		if(mode == SAVEAS)
			file <<= preselect;
		else
			for(i32 i = 0; i < list.GetCount(); i++)
			    if(list[i].name == preselect) {
					list.SetCursor(i);
					ActiveFocus(list);
					break;
				}
		preselect.Clear();
	}
	if(default_name.GetCount() && mode == SAVEAS)
		file <<= default_name;

	force_ext = mode == SAVEAS;
	allowed_ext.Clear();
	for(Txt mm : mask) {
		if(!force_ext)
			break;
		for(Txt m : Split(mm, ' ')) {
			if(!force_ext)
				break;
			i32 q = m.Find('.');
			if(q < 0)
				force_ext = false;
			else {
				m = m.Mid(q + 1);
				if(m.Find('*') >= 0 || m.Find('?') >= 0)
					force_ext = false;
				else
					allowed_ext.FindAdd(m);
			}
		}
	}

	FileUpdate();
	Update();
	i32 c = TopWindow::Run(appmodal);
	TopWindow::Close();
	lastsby = list.GetSbPos();
	if(IsNumber(~type)) {
		i32 ti = ~type;
		type.Trim(dlc);
		if(ti >= 0 && ti < type.GetCount())
			activetype = type.GetVal(ti);
	}
	else
		type.Trim(dlc);

	Txt d = ~dir;
	if((filesystem->IsWin32() && d.GetLength() == 3 && d[1] == ':') ||
	   (filesystem->IsPosix() && d == "/") ||
	   !IsFullPath(d))
		d.Clear();

	if(d.GetCount()) {
		LruAdd(lru, d, 12);
		Vec<Txt> glru;
		d = NormalizePath(d);
		Index<Txt> h;
		for(i32 i = 0; i < fixed_places; i++)
			h.Add(NormalizePath(~places.Get(i, 0)));
		if(h.Find(d) < 0) {
			LoadFromGlobal(glru, "GlobalFileSelectorLRU");
			LruAdd(glru, d, 5);
			StoreToGlobal(glru, "GlobalFileSelectorLRU");
		}
	}

	places.SetCount(fixed_places);

	return c == IDOK;
}

bool FileSel::ExecuteOpen(tukk title) {
	Title(title ? title : t_("Открыть"));
	return Execute(OPEN);
}

bool FileSel::ExecuteSaveAs(tukk title) {
	Title(title ? title : t_("Сохранить как"));
	ok.SetLabel(t_("Сохранить"));
	return Execute(SAVEAS);
}

bool FileSel::ExecuteSelectDir(tukk title)
{
	Title(title ? title : t_("Выбрать папку"));
	return Execute(SELECTDIR);
}

void FileSel::Serialize(Stream& s) {
#ifdef TARGET_WIN32
	if(s.IsLoading()) {
		netnode.Clear();
		netstack.Clear();
	}
#endif
	i32 version = 10;
	s / version;
	Txt ad = ~dir;
	i32 dummy = 0;
	if(version < 10)
		s / dummy;
	else
		s % activetype;
	s % ad;
	dir <<= ad;
	if(version < 1) {
		Txt n = fn.At(0);
		s % n;
		fn.At(0) = n;
	}
	else {
		if(version < 4)
			s % fn;
		else {
			Vec<Txt> __;
			s % __;
		}
	}
	if(version >= 2) {
		SerializePlacement(s);
		list.SerializeSettings(s);
	}
	if(version >= 3) {
		s % lastsby;
	}
	if(version >= 4) {
		s % lru;
	}
	if(version >= 5) {
		s % sortby;
	}
	if(version >= 6) {
		if(version >= 9)
			s % splitter;
		else {
			Splitter dummy;
			s % dummy;
		}
	}
	if(version >= 7) {
		s % hidden;
	}
	if(version >= 8) {
		s % hiddenfiles;
	}
}

Txt FileSel::GetFile(i32 i) const {
	Txt p;
	if(i >= 0 && i < fn.GetCount()) {
		p = fn[i];
		if(!IsFullPath(p))
			p = AppendFName(dir.GetData(), p);
	}
#ifdef TARGET_WIN32
	if(IsLnkFile(p))
		p = Nvl(GetSymLinkPath(p), p);
#endif
	return p;
}

void FileSel::SyncSplitter()
{
	splitter.Clear();
	if(places.GetCount() && basedir.IsEmpty())
		splitter.Add(places);
	splitter.Add(list);
	if(preview)
		splitter.Add(*preview);
}

FileSel& FileSel::PreSelect(const Txt& path)
{
	ActiveDir(GetFileFolder(path));
	preselect = GetFName(path);
	return *this;
}

void FileSel::InitSplitter()
{
	i32 n = splitter.GetCount();
	i32 i = 0;
	if(places.GetCount())
		splitter.SetPos(2000, i++);
	splitter.SetPos(10000 - 2000 * (n - 1), i);
}

FileSel& FileSel::Preview(Ctrl& ctrl)
{
	if(!preview) {
		Size sz = GetRect().GetSize();
		sz.cx = 5 * sz.cx / 3;
		SetRect(sz);
	}
	preview = &ctrl;
	SyncSplitter();
	InitSplitter();
	return *this;
}

FileSel& FileSel::Preview(const Display& d)
{
	preview_display.SetDisplay(d);
	return Preview(preview_display);
}

void FileSel::AddPlaceRaw(const Txt& path, const Image& m, const Txt& name, tukk group, i32 row)
{
	if(path.GetCount()) {
		row = row < 0 ? places.GetCount() : row;
		places.Insert(row);
		places.Set(row, 0, path);
		places.Set(row, 1, DPI(m));
		places.Set(row, 2, name);
		places.Set(row, 3, group);
		places.SetLineCy(row, max(m.GetSize().cy + 4, GetStdFontCy() + 4));
		SyncSplitter();
		InitSplitter();
	}
}

FileSel& FileSel::AddPlace(const Txt& path, const Image& m, const Txt& name, tukk group, i32 row)
{
	if(path.GetCount())
		AddPlaceRaw(NormalizePath(path), DPI(m), name, group, row);
	return *this;
}

FileSel& FileSel::AddPlace(const Txt& path, const Txt& name, tukk group, i32 row)
{
#ifdef GUI_COCOA
	return AddPlace(path, GetFileIcon(NormalizePath(path), true, false, false), name, group, row);
#else
	return AddPlace(path, GetDirIcon(NormalizePath(path)), name, group, row);
#endif
}

FileSel& FileSel::AddPlace(const Txt& path, tukk group, i32 row)
{
	return AddPlace(path, GetFileTitle(path), group, row);
}

FileSel& FileSel::AddPlaceSeparator()
{
	places.AddSeparator();
	SyncSplitter();
	InitSplitter();
	return *this;
}

FileSel& FileSel::ClearPlaces()
{
	places.Clear();
	SyncSplitter();
	return *this;
}

void FileSel::AddSystemPlaces(i32 row)
{
	row = row < 0 ? places.GetCount() : row;
	Array<FSInfo::FileInfo> root;
#ifdef TARGET_WIN32
	root = filesystem->Find(Null);
	for(i32 i = 0; i < root.GetCount(); i++) {
		Txt desc = root[i].root_desc;
		Txt n = root[i].filename;
		if(n != "A:\\" && n != "B:\\") {
		#ifdef TARGET_WIN32
			if(*n.Last() == '\\')
				n.Trim(n.GetCount() - 1);
		#endif
			if(desc.GetCount() == 0)
			    desc << " " << t_("Локальный Диск");
			desc << " (" << n << ")";
			AddPlace(root[i].filename, GetDriveImage(root[i].root_style), desc, "PLACES:SYSTEM", row++);
		}
	}

	if(GetSystemMetrics(SM_REMOTESESSION))
		for(i32 drive = 'A'; drive < 'Z'; drive++) {
			Txt path = Format("\\\\tsclient\\%c", drive);
			if(FindFile(path + "\\*.*"))
				AddPlace(path, Format(t_("%c на клиенте"), drive), "PLACES:SYSTEM", row++);
		}
#endif

#ifdef TARGET_POSIX
	root = filesystem->Find("/media/*");
	for(i32 i = 0; i < root.GetCount(); i++) {
		Txt fn = root[i].filename;
		if(*fn != '.' && fn.Find("floppy") < 0)
			AddPlace("/media/" + fn, fn, "PLACES:SYSTEM", row++);
	}
	AddPlace("/", t_("Компьютер"), "PLACES:SYSTEM", row++);
#endif
}

FileSel& FileSel::AddStandardPlaces()
{
	AddPlace(GetHomeDir(), t_("Дом"), "PLACES:FOLDER");
#ifdef GUI_COCOA
	AddPlace(GetSpecialDir(SF_NSDesktopDir), t_("Рабочий стол"), "PLACES:FOLDER");
	AddPlace(GetSpecialDir(SF_NSMusicDir), t_("Музыка"), "PLACES:FOLDER");
	AddPlace(GetSpecialDir(SF_NSPicturesDir), t_("Изображения"), "PLACES:FOLDER");
	AddPlace(GetSpecialDir(SF_NSMoviesDir), t_("Видео"), "PLACES:FOLDER");
	AddPlace(GetSpecialDir(SF_NSDocumentDir), t_("Документы"), "PLACES:FOLDER");
	AddPlace(GetSpecialDir(SF_NSDownloadsDir), t_("Загрузки"), "PLACES:FOLDER");
#else
	AddPlace(GetDesktopFolder(), t_("Рабочий Стол"), "PLACES:FOLDER");
	AddPlace(GetMusicFolder(), t_("Музыка"), "PLACES:FOLDER");
	AddPlace(GetPicturesFolder(), t_("Картинки"), "PLACES:FOLDER");
	AddPlace(GetVideoFolder(), t_("Видео"), "PLACES:FOLDER");
	AddPlace(GetDocumentsFolder(), t_("Документы"), "PLACES:FOLDER");
	AddPlace(GetDownloadFolder(), t_("Загрузки"), "PLACES:FOLDER");
#endif
	AddPlaceSeparator();
	AddSystemPlaces();
#ifdef TARGET_WIN32
	AddPlaceSeparator();
	AddPlaceRaw("\\", CtrlImg::Network(), t_("Сеть"), "PLACES:NETWORK");
#endif
	return *this;
}

struct DisplayPlace : Display {
	virtual void Paint(Draw& w, const Rect& r, const Val& q, Color ink, Color paper,
	                   dword style) const
	{
		w.DrawRect(r, paper);
		ValArray va = q;
		Image m = va[0];
		Txt txt = va[1];
		Size isz = m.GetSize();
		w.DrawImage(r.left, r.top + (r.Height() - isz.cy) / 2, m);
		w.DrawText(r.left + isz.cx + 2, r.top + (r.Height() - GetStdFontCy()) / 2, txt,
		           StdFont(), ink);
	}
	virtual Size GetStdSize(const Val& q) const {
		ValArray va = q;
		Image m = va[0];
		Txt txt = va[1];
		Size isz = m.GetSize();
		return Size(isz.cx + GetTextSize(txt, StdFont()).cx + 2, max(isz.cy, GetStdFontCy()));
	}
};

FileSel::FileSel()
{
	loaded = false;
	filesystem = &StdFileSystemInfo();
	CtrlLayout(*this);
	ArrangeOKCancel(ok, cancel);
	Acceptor(ok, IDOK); ok.Ok();
	Rejector(cancel, IDCANCEL); cancel.Cancel();
	list.IconWidth(DPI(16)).Renaming().Columns(3).ClickKill();
	list.WhenLeftDouble = OTVET(OpenItem2);
	dirup <<= OTVET(DirUp);
	Add(dirup);
	sortby <<= OTVET(SearchLoad);
	Add(sortby);
	hidden <<= OTVET(SearchLoad);
	Add(hidden);
	hiddenfiles <<= OTVET(SearchLoad);
	Add(hiddenfiles);
	mkdir <<= OTVET(MkDir);
	Add(mkdir);
	plus <<= OTVET(Plus);
	Add(plus);
	minus <<= OTVET(Minus);
	Add(minus);
	toggle <<= OTVET(Toggle);
	Add(toggle);

	ok <<= OTVET(Open);
	list <<= OTVET(Update);
	file <<= OTVET(FileUpdate);
	list.WhenRename = OTVET(Rename);
	Sizeable();
	dirup.SetImage(CtrlImg::DirUp()).NoWantFocus();
	dirup.Tip(t_("На папку выше") + Txt(" (Ctrl+Up)"));
	mkdir.SetImage(CtrlImg::MkDir()).NoWantFocus();
	mkdir.Tip(t_("Создать папку") + Txt(" (F7)"));
	plus.SetImage(CtrlImg::Plus()).NoWantFocus();
	plus.Tip(t_("Выбрать файлы"));
	minus.SetImage(CtrlImg::Minus()).NoWantFocus();
	minus.Tip(t_("Отменить выбор файлов"));
	toggle.SetImage(CtrlImg::Toggle()).NoWantFocus();
	toggle.Tip(t_("Переключить файлы"));
	type <<= OTVET(Load);
	for(i32 pass = 0; pass < 2; pass++) {
		i32 k = pass * FILELISTSORT_DESCENDING;
		Txt d = pass ? t_(" нисходяще") : "";
		sortby.Add(FILELISTSORT_NAME|k, t_("Имя") + d);
		sortby.Add(FILELISTSORT_EXT|k, t_("Расширение") + d);
		sortby.Add(FILELISTSORT_TIME|k, t_("Последнее изменение") + d);
		sortby.Add(FILELISTSORT_SIZE|k, t_("Размер") + d);
	}
	sortby <<= FILELISTSORT_NAME;

	search.NullText(t_("Поиск"), StdFont().Italic(), SColorDisabled());
	search.SetFilter(CharFilterDefaultToUpperAscii);
	search <<= OTVET(SearchLoad);

	filename.SetFont(StdFont());
	filename.SetFrame(ViewFrame());
	filesize.SetFont(StdFont()).SetAlign(ALIGN_RIGHT);
	filesize.SetFrame(ViewFrame());
	filetime.SetFont(StdFont());
	filetime.SetFrame(ViewFrame());

	dir <<= OTVET(Choice);
	dir.DisplayAll();
	dir.SetDropLines(24);

	readonly.Hide();

	lastsby = 0;

	asking = true;
	rdonly = false;
	multi = false;
	bidname = false;
	appmodal = true;
	noexeicons = false;

	AddChildBefore(GetFirstChild(), &sizegrip);

	preview = NULL;
	preview_display.SetFrame(FieldFrame());

	SyncSplitter();

	BackPaintHint();

	places.AddKey();
	places.AddColumn().AddIndex().SetDisplay(Single<DisplayPlace>());
	places.AddIndex();
	places.NoHeader().NoGrid();
	places.WhenLeftClick = OTVET(GoToPlace);
	places.NoWantFocus();

	list.NoRoundSize();

#ifdef TARGET_WIN32
	i32 icx = GetFileIcon(GetHomeDir(), true, false, false).GetSize().cx;
	if(icx)
		list.IconWidth(icx);
#endif

	AddStandardPlaces();

	list.AutoHideSb();
	places.AutoHideSb();

	WhenIconLazy = NULL;
}

FileSel::~FileSel() {}

}