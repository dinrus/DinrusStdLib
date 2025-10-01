#include "SSH.h"

namespace drx {

#define LLOG(x)       do { if(SSH::sTrace) RLOG(SSH::GetName(ssh->otype, ssh->oid) << x); } while(false)
#define LDUMPHEX(x)	  do { if(SSH::sTraceVerbose) RDUMPHEX(x); } while(false)

bool SFtp::Init()
{
	if(!ssh->session)
		ThrowError(-1, "Сессия SSH неполноценна.");
	auto session = libssh2_sftp_init(ssh->session);
	if(!session && !WouldBlock())
		ThrowError(-1);
	if(session) {
		sftp_session = MakeOne<LIBSSH2_SFTP*>(session);
		LLOG("Session successfully initialized.");
	}
	return session;
}

void SFtp::Exit()
{
	if(!sftp_session)
		return;

	Run([=, this]() mutable {
		if(WouldBlock(libssh2_sftp_shutdown(*sftp_session)))
			return false;
		ssh->init = false;
		sftp_session.Clear();
		LLOG("Session deinitalized.");
		return true;
	}, false);
}

i32 SFtp::FStat(SFtpHandle handle, SFtpAttrs& a, bool set)
{
	i32 rc = libssh2_sftp_fstat_ex(handle, &a, set);
	return WouldBlock(rc) ?  1 : rc;
}

i32 SFtp::LStat(const Txt& path, SFtpAttrs& a, i32 type)
{
	i32 rc = libssh2_sftp_stat_ex(*sftp_session, ~path, path.GetLength(), type, &a);
	return WouldBlock(rc) ?  1 : rc;
}

SFtpHandle SFtp::Open(const Txt& path, dword flags, long mode)
{
	SFtpHandle h = nullptr;

	Run([=, this, &h] () mutable {
		h = libssh2_sftp_open(*sftp_session, path, flags, mode);
		if(!h && !WouldBlock())
			ThrowError(-1);
		if(h)
			LLOG(Format("Файл '%s' успешно открыт.", path));
		return h;
	});
	return h;
}

void SFtp::Close(SFtpHandle handle)
{
	if(!handle)
		return;

	Run([=, this] () mutable {
		i32 rc = libssh2_sftp_close_handle(handle);
		if(!rc)	LLOG("File handle freed.");
		return !rc;
	}, false);
}

bool SFtp::Rename(const Txt& oldpath, const Txt& newpath)
{
	return Run([=, this] () mutable {
		i32 rc = libssh2_sftp_rename(*sftp_session, oldpath, newpath);
		if(!WouldBlock(rc) && rc != 0)
			ThrowError(rc);
		if(rc == 0)
			LLOG(Format("'%s' is successfully renamed to '%s'", oldpath, newpath));
		return !rc;
	});
}

bool SFtp::Delete(const Txt& path)
{
	return Run([=, this] () mutable {
		i32 rc = libssh2_sftp_unlink(*sftp_session, path);
		if(!WouldBlock(rc) && rc != 0)
			ThrowError(rc);
		if(rc == 0)
			LLOG(Format("File '%s' is successfully deleted.", path));
		return !rc;
	});
}

bool SFtp::Sync(SFtpHandle handle)
{
	return Run([=, this] () mutable {
		i32 rc = libssh2_sftp_fsync(handle);
		if(!WouldBlock(rc) && rc != 0)
			ThrowError(rc);
		if(rc == 0)
			LLOG("File successfully synchronized to disk.");
		return !rc;
	});
}

SFtp& SFtp::Seek(SFtpHandle handle, z64 position)
{
	INTERLOCKED
	{
		// LLOG("Seeking to offset: " << position);
		libssh2_sftp_seek64(handle, position);
	}
	return *this;
}

z64 SFtp::GetPos(SFtpHandle handle)
{
	z64 pos = 0;

	INTERLOCKED
	{
		pos = libssh2_sftp_tell64(handle);
		// LLOG("File position: " << pos);
	};
	return pos;
}

i32 SFtp::Get(SFtpHandle handle, uk ptr, i32 size)
{
	done = 0;

	Run([=, this]() mutable {
		while(done < size && !IsTimeout()) {
			i32 rc = static_cast<i32>(
				libssh2_sftp_read(handle, (char*) ptr + done, min(size - done, ssh->chunk_size))
			);
			if(rc < 0) {
				if(!WouldBlock(rc))
					ThrowError(rc);
				return false;
			}
			else
			if(rc == 0) {
				LLOG("EOF received.");
				break;
			}
			done += rc;
			ssh->start_time = msecs();
			UpdateClient();
		}
		return true;
	});

	return GetDone();
}

i32 SFtp::Put(SFtpHandle handle, ukk ptr, i32 size)
{
	done = 0;

	Run([=, this]() mutable {
		while(done < size && !IsTimeout()) {
			i32 rc = static_cast<i32>(
				libssh2_sftp_write(handle, (tukk) ptr + done, min(size - done, ssh->chunk_size))
			);
			if(rc < 0) {
				if(!WouldBlock(rc))
					ThrowError(rc);
				return false;
			}
			else
			if(rc == 0) {
				LLOG("EOF received.");
				break;
			}
			done += rc;
			ssh->start_time = msecs();
			UpdateClient();
		}
		return true;
	});

	return GetDone();
}

bool SFtp::CopyData(Stream& dest, Stream& src, z64 maxsize)
{
	if(IsError())
		return false;

	z64 size = src.GetSize(), count = 0;
	Buffer<u8> chunk(ssh->chunk_size, 0);

	WhenProgress(0, size);

	while(!src.IsEof()) {
		i32 n = src.Get(chunk, (i32) min<z64>(size - count, ssh->chunk_size));
		if(n > 0) {
			dest.Put(chunk, n);
			if(dest.IsError()) {
				LLOG("Stream write error. " + src.GetErrorText());
				return false;
			}
			count += n;
			if(WhenProgress(count, size)) {
				return false;
			}
		}
		if(src.IsError()) {
			LLOG("Stream read error. " + src.GetErrorText());
			break;
		}
	}
	return !src.IsError();
}

bool SFtp::SaveFile(tukk path, const Txt& data)
{
	TxtStream in(data);
	SFtpFileOut out(*this, path);
	return CopyData(out, in);
}

Txt SFtp::LoadFile(tukk path)
{
	TxtStream out;
	SFtpFileIn in(*this, path);
	CopyData(out, in, INT_MAX);
	return pick(out.GetResult());
}

bool SFtp::SaveFile(tukk path, Stream& in)
{
	SFtpFileOut out(*this, path);
	return CopyData(out, in);
}

bool SFtp::LoadFile(Stream& out, tukk path)
{
	SFtpFileIn in(*this, path);
	return CopyData(out, in);
}

SFtpHandle SFtp::OpenDir(const Txt& path)
{
	SFtpHandle h = nullptr;

	Run([=, this, &h] () mutable {
		h = libssh2_sftp_opendir(*sftp_session, path);
		if(!h && !WouldBlock())
			ThrowError(-1);
		if(h)
			LLOG(Format("Dir '%s' is successfully opened.", path));
		return h;
	});
	return h;
}

bool SFtp::MakeDir(const Txt& path, long mode)
{
	return Run([=, this] () mutable {
		i32 rc = libssh2_sftp_mkdir(*sftp_session, path, mode);
		if(!WouldBlock(rc) && rc != 0)
			ThrowError(rc);
		if(!rc)
			LLOG(Format("Dir '%s' is succesfully created.", path));
		return !rc;
	});
}

bool SFtp::RemoveDir(const Txt& path)
{
	return Run([=, this] () mutable {
		i32 rc = libssh2_sftp_rmdir(*sftp_session, path);
		if(!WouldBlock(rc) && rc != 0)
			ThrowError(rc);
		if(!rc)
			LLOG(Format("Dir '%s' is succesfully deleted.", path));
		return !rc;
	});
}

bool SFtp::ListDir(SFtpHandle handle, DirList& list)
{
	SFtpAttrs attrs;
	return Run([=, this, &attrs, &list] () mutable {
		char label[512];
		char longentry[512];

		while(InProgress()) {
			Zero(attrs);
			i32 rc = libssh2_sftp_readdir_ex(
						handle,
						label, sizeof(label),
						longentry, sizeof(longentry),
						&attrs
			);
			if(rc < 0) {
				if(!WouldBlock(rc))
					ThrowError(rc);
				break;
			}
			else
			if(rc > 0) {
				DirEntry& entry	= list.Add();
				entry.filename	= label;
				*entry.a		= attrs;
				entry.valid		= true;
//				DUMP(entry);
			}
			if(rc == 0) {
				LLOG(Format("Dir listing is successful. (%d entries)", list.GetCount()));
				return true;
			}
		}
		return false;
	});
}

bool SFtp::ListDir(const Txt& path, DirList& list)
{
	SFtpHandle h = OpenDir(path);
	if(h) {
		bool b = ListDir(h, list);
		Close(h);
		if(!b) ssh->status = FAILED;
	}
	return !IsError();
}

bool SFtp::SymLink(const Txt& path, Txt& target, i32 type)
{
	Buffer<char> buffer(512, 0);

	if(type == LIBSSH2_SFTP_SYMLINK)
		return Run([=, this, &path, &buffer, &target] () mutable {
			i32 rc = libssh2_sftp_symlink_ex(
						*sftp_session,
						path,
						path.GetLength(),
						buffer,
						512,
						type
					);
			if(!WouldBlock(rc) && rc != 0)
				ThrowError(rc);
			if(!rc) {
				target.Set(buffer, rc);
				LLOG(Format("Symbolic link '%s' for path '%s' is successfult created.", target, path));
			}
			return !rc;
		});
	else
		return Run([=, this, &path, &buffer, &target] () mutable {
			Buffer<char> sbuf(512, 0);
			i32 rc = libssh2_sftp_symlink_ex(
						*sftp_session,
						path,
						path.GetLength(),
						buffer,
						512,
						type
					);
			if(!WouldBlock(rc) && rc < 0)
				ThrowError(rc);
			if(rc > 0) {
				target.Set(buffer, rc);
				LLOG("Symbolic link operation is successful. Target: " << target);
			}
			return rc > 0;
		});
}

SFtp::DirEntry SFtp::GetInfo(const Txt& path)
{
	DirEntry finfo;

	if(!GetAttrs(path, *finfo))
		return Null;

	finfo.filename = path;
	finfo.valid = true;

	return pick(finfo);
}

bool SFtp::GetAttrs(SFtpHandle handle, SFtpAttrs& attrs)
{
	return Run([=, this, &attrs] () mutable {
		i32 rc = FStat(handle, attrs, false);
		if(rc < 0)	ThrowError(rc);
		if(!rc)	LLOG("File attributes successfully retrieved.");
		return !rc;
	});
}

bool SFtp::GetAttrs(const Txt& path, SFtpAttrs& attrs)
{
	return Run([=, this, &path, &attrs] () mutable {
		i32 rc = LStat(path, attrs, LIBSSH2_SFTP_STAT);
		if(rc < 0)	ThrowError(rc);
		if(!rc) LLOG(Format("File attributes of '%s' is successfully retrieved.", path));
		return !rc;
	});
}

bool SFtp::SetAttrs(SFtpHandle handle, const SFtpAttrs& attrs)
{
	return Run([=, this, &attrs] () mutable {
		i32 rc = FStat(handle, const_cast<SFtpAttrs&>(attrs), true);
		if(rc < 0)	ThrowError(rc);
		if(!rc)	LLOG("File attributes successfully modified.");
		return !rc;
	});
}

bool SFtp::SetAttrs(const Txt& path, const SFtpAttrs& attrs)
{
	return Run([=, this, &attrs] () mutable {
		i32 rc = LStat(path,  const_cast<SFtpAttrs&>(attrs), LIBSSH2_SFTP_SETSTAT);
		if(rc < 0)	ThrowError(rc);
		if(!rc)	LLOG(Format("File attributes of '%s' is successfully modified.", path));
		return !rc;
	});
}

Val SFtp::QueryAttr(const Txt& path, i32 attr)
{
	DirEntry finfo;
	Val v;

	if(!GetAttrs(path, *finfo))
		return Null;

	switch(attr) {
		case SFTP_ATTR_FILE:
			v = finfo.IsFile();
			break;
		case SFTP_ATTR_DIRECTORY:
			v = finfo.IsDir();
			break;
		case SFTP_ATTR_SOCKET:
			v = finfo.IsSocket();
			break;
		case SFTP_ATTR_SYMLINK:
			v = finfo.IsSymLink();
			break;
		case SFTP_ATTR_PIPE:
			v = finfo.IsPipe();
			break;
		case SFTP_ATTR_BLOCK:
			v = finfo.IsBlock();
			break;
		case SFTP_ATTR_SPECIAL:
			v = finfo.IsSpecial();
			break;
		case SFTP_ATTR_SIZE:
			v = finfo.GetSize();
			break;
		case SFTP_ATTR_LAST_MODIFIED:
			v = finfo.GetLastModified();
			break;
		case SFTP_ATTR_LAST_ACCESSED:
			v = finfo.GetLastAccessed();
			break;
		default:
			break;
	}
	return pick(v);
}

bool SFtp::ModifyAttr(const Txt& path, i32 attr, const Val& v)
{
	DirEntry finfo;

	if(!GetAttrs(path, *finfo))
		return false;

	SFtpAttrs& attrs = *finfo;

	switch(attr) {
		case SFTP_ATTR_SIZE:
			attrs.flags |= LIBSSH2_SFTP_ATTR_SIZE;
			attrs.filesize = v.To<z64>();
			break;
		case SFTP_ATTR_LAST_MODIFIED:
			attrs.flags |= LIBSSH2_SFTP_ATTR_ACMODTIME;
			attrs.mtime = GetUTCSeconds(v);
			break;
		case SFTP_ATTR_LAST_ACCESSED:
			attrs.flags |= LIBSSH2_SFTP_ATTR_ACMODTIME;
			attrs.atime = GetUTCSeconds(v);
			break;
		default:
			break;
	}
	return SetAttrs(path, ~finfo);;
}

SFtp::SFtp(SshSession& session)
: Ssh()
, done(0)
{
	ssh->otype		= SFTP;
	ssh->session	= session.GetHandle();
	ssh->socket		= &session.GetSocket();
	ssh->timeout	= session.GetTimeout();
	ssh->waitstep   = session.GetWaitStep();
	ssh->whenwait   = Proxy(session.WhenWait);
}

SFtp::~SFtp()
{
	Exit();
}

Txt SFtp::DirEntry::ToTxt() const
{
	if(!valid) return "<N/A>";
	static tukk hypen = "-", r = "r", w = "w", x = "x";
	return Format("%c%c%c%c%c%c%c%c%c%c %5<d %5<d %12>d %s %s %s",
				(IsFile()
				? *hypen : (IsDir()
				? 'd' : (IsSymLink()
				? 'l' : (IsSocket()
				? 's' : (IsPipe()
				? 'p' : (IsBlock()
				? 'b' : (IsSpecial()
				? 'c' : 'o' ))))))),
				((a->permissions & IRUSR) ? *r : *hypen),
				((a->permissions & IWUSR) ? *w : *hypen),
				((a->permissions & IXUSR) ? *x : *hypen),
				((a->permissions & IRGRP) ? *r : *hypen),
				((a->permissions & IWGRP) ? *w : *hypen),
				((a->permissions & IXGRP) ? *x : *hypen),
				((a->permissions & IROTH) ? *r : *hypen),
				((a->permissions & IWOTH) ? *w : *hypen),
				((a->permissions & IXOTH) ? *x : *hypen),
				GetUid(),
				GetGid(),
				GetSize(),
				AsTxt(GetLastModified()),
				AsTxt(GetLastAccessed()),
				GetName());
}

Txt SFtp::DirEntry::ToXml() const
{
	if(!valid) return XmlTag("<N/A>").Text("<N/A>");
	static tukk hypen = "-", r = "r", w = "w", x = "x";
	return XmlTag("sftp:direntry")
			("type", (IsFile()
				? "file" : (IsDir()
				? "directory" : (IsSymLink()
				? "symlink" : (IsSocket()
				? "socket" : (IsPipe()
				? "pipe" : (IsBlock()
				? "block-special" : (IsSpecial()
				? "character-special" : "other")
			)))))))
			("uid", AsTxt(GetUid()))
			("gid", AsTxt(GetGid()))
			("size", AsTxt(GetSize()))
			("modified", AsTxt(GetLastModified()))
			("accessed", AsTxt(GetLastAccessed()))
			("permissions", Format("%c%c%c%c%c%c%c%c%c",
				((a->permissions & IRUSR) ? *r : *hypen),
				((a->permissions & IWUSR) ? *w : *hypen),
				((a->permissions & IXUSR) ? *x : *hypen),
				((a->permissions & IRGRP) ? *r : *hypen),
				((a->permissions & IWGRP) ? *w : *hypen),
				((a->permissions & IXGRP) ? *x : *hypen),
				((a->permissions & IROTH) ? *r : *hypen),
				((a->permissions & IWOTH) ? *w : *hypen),
				((a->permissions & IXOTH) ? *x : *hypen)
			))
			.Text(GetName());
}

FSInfo::FileInfo SFtp::DirEntry::ToFileInfo() const
{
	FSInfo::FileInfo fi;
	fi.filename         = GetName();
	fi.length           = GetSize();
	fi.unix_mode        = GetPermissions();
	fi.creation_time    = GetLastModified();
	fi.last_access_time = GetLastAccessed();
	fi.last_write_time  = GetLastModified();
	fi.is_file          = IsFile();
	fi.is_directory     = IsDir();
	fi.is_symlink       = IsSymLink();
	fi.is_read_only     = IsReadOnly();
	fi.root_style       = FSInfo::ROOT_FIXED;
	return pick(fi);
}

void SFtp::DirEntry::Serialize(Stream& s)
{
	ASSERT(a);

	s % a->flags;
	s % a->permissions;
	s % a->uid;
	s % a->gid;
	s % a->filesize;
	s % a->mtime;
	s % a->atime;
	s % filename;
}

bool SFtp::DirEntry::CanMode(dword u, dword g, dword o) const
{
	return (a->flags & LIBSSH2_SFTP_ATTR_PERMISSIONS)
		&& (a->permissions & o || a->permissions & g || a->permissions & u);
}

void SFtp::DirEntry::Zero()
{
	a.Create();
	drx::Zero(*a);
	valid = false;
}

SFtp::DirEntry::DirEntry(const Txt& path)
: DirEntry()
{
	filename = path;
}

SFtp::DirEntry::DirEntry(const Txt& path, const SFtpAttrs& attrs)
: DirEntry()
{
	filename = path;
	*a = attrs;
}

Array<FSInfo::FileInfo> SFtpFSInfo::Find(Txt mask, i32 max_count, bool unmounted) const
{
	ASSERT(browser);
	Array<FileInfo> fi;
	Txt dir;
	if(!browser->InProgress()) {
		if(IsNull(mask))
			dir = browser->GetDefaultDir();
		else {
			Txt s;
			dir = GetFileDir(browser->RealizePath(mask, s) ? s : mask);
		}
		bool haswc = HasWildcards(mask);
		if(!haswc && max_count == 1) { // A small optimization.
			SFtp::DirEntry e = browser->GetInfo(mask);
			if(e)
				fi.Add(e.ToFileInfo());
		}
		else {
			Txt pattern;
			SFtp::DirList ls;
			if(browser->ListDir(dir, ls)) {
				if(haswc)
					pattern = GetFName(mask);
				for(i32 i = 0, j = 0; i < ls.GetCount() && j < max_count; i++) {
					const SFtp::DirEntry& e = ls[i];
					if(!haswc || PatternMatch(pattern, e.GetName())) {
						fi.Add(e.ToFileInfo());
						j++;
					}
				}
			}
		}
	}
	return pick(fi);
}

bool SFtpFSInfo::CreateFolder(Txt path, Txt& error) const
{
	ASSERT(browser);
	if(!browser->MakeDir(path, 0))
		error = browser->GetErrorDesc();
	return !browser->IsError();
}
}
