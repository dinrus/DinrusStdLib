#include "SSH.h"

namespace drx {

void SFtpStream::SetPos(z64 pos)
{
	sftp->Seek(handle, pos);
}

void SFtpStream::SetStreamSize(z64 size)
{
	// СДЕЛАТЬ
}

dword SFtpStream::Read(z64 at, uk ptr, dword size)
{
	SetPos(at);
	i32 n = sftp->Get(handle, ptr, size);
	if(sftp->IsError()) SetError();
	return dword(n);
}

void SFtpStream::Write(z64 at, ukk data, dword size)
{
	SetPos(at);
	sftp->Put(handle, data, size);
	if(sftp->IsError()) SetError();
}

void SFtpStream::Close()
{
	if(handle) {
		Flush();
		sftp->Close(handle);
		handle = nullptr;
	}
}

bool SFtpStream::IsOpen() const
{
	return handle;
}

bool SFtpStream::Open(SFtp& sftp_, tukk filename, dword mode, i32 acm)
{
	if(IsOpen())
		Close();
	sftp = &sftp_;
	i32 iomode = mode & ~SHAREMASK;
	handle = sftp->Open(filename,
						iomode == READ
							? SFtp::READ
							: iomode == CREATE
								? SFtp::READ|SFtp::WRITE|SFtp::CREATE|SFtp::TRUNCATE
								: SFtp::READ|SFtp::WRITE,
						acm
						);
				
	if(handle) {
		SFtpAttrs attrs;
		if(!sftp->GetAttrs(handle, attrs)) {
			sftp->Close(handle);
			handle = nullptr;
			return false;
		}
		OpenInit(mode, attrs.filesize);
	}
	return handle;
}

SFtpStream::SFtpStream(SFtp& sftp, tukk filename, dword mode, i32 acm)
: SFtpStream()
{
	Open(sftp, filename, mode, acm);
}

SFtpStream::SFtpStream()
: sftp(nullptr)
, handle(nullptr)
{
}

SFtpStream::~SFtpStream()
{
	Close();
}

};