#include "SSH.h"

namespace drx {

#define LLOG(x)       do { if(SSH::sTrace) RLOG(SSH::GetName(ssh->otype, ssh->oid) << x); } while(false)
#define VLOG(x)       if(SSH::sTraceVerbose) LLOG(x);
#define LDUMPHEX(x)	  do { if(SSH::sTraceVerbose) RDUMPHEX(x); } while(false)

bool SshChannel::Init()
{
	ASSERT(!IsOpen());

	LIBSSH2_CHANNEL *ch = libssh2_channel_open_session(ssh->session);
	if(!ch && !WouldBlock())
		ThrowError(-1);
	if(ch) {
		channel = MakeOne<LIBSSH2_CHANNEL*>(ch);
		LLOG("A new channel is opened.");
	}
	return ch;
}

void SshChannel::Exit()
{
	if(!channel)
		return;

	Run([=, this]() mutable {
		i32 rc = libssh2_channel_free(*channel);
		if(!WouldBlock(rc) && rc < 0)
			ThrowError(rc);
		if(!rc) {
			ssh->init = false;
			channel.Clear();
			LLOG("Channel succesfully freed.");
		}
		return !rc;
	}, false);
}

bool SshChannel::Open()
{
	if(IsOpen())
		Close();
	return Run([=, this]() mutable { return Init(); });
}

bool SshChannel::Close()
{
	return Run([=, this]() mutable {
		i32 rc = libssh2_channel_close(*channel);
		if(!WouldBlock(rc) && rc < 0) ThrowError(rc);
		if(!rc) LLOG("Channel close message is sent to the server.");
		return !rc;
	});
}

bool SshChannel::WaitClose()
{
	return Run([=, this]() mutable {
		i32 rc = libssh2_channel_wait_closed(*channel);
		if(!WouldBlock(rc) && rc < 0) ThrowError(rc);
		if(!rc)	LLOG("Channel close message is acknowledged by the server.");
		return !rc;
	});
}

bool SshChannel::Request(const Txt& request, const Txt& params)
{
	return Run([=, this]() mutable {
		i32 rc = libssh2_channel_process_startup(
			*channel,
			request,
			request.GetLength(),
			params.GetLength() ? ~params : nullptr,
			params.GetLength()
		);
		if(!WouldBlock(rc) && rc < 0)
			ThrowError(rc);
		if(!rc)
			LLOG("\"" << request << "\" request (params: " << params << ") is successful.");
		return !rc;
	});
}

bool SshChannel::RequestTerminal(const Txt& term, i32 width, i32 height, const Txt& modes)
{
	return Run([=, this]() mutable {
		i32 rc = libssh2_channel_request_pty_ex(
			*channel,
			~term,
			term.GetLength(),
			~modes,
			modes.GetLength(),
			width,
			height,
			LIBSSH2_TERM_WIDTH_PX,
			LIBSSH2_TERM_HEIGHT_PX
		);
		if(!WouldBlock(rc) && rc < 0)
			ThrowError(rc);
		if(!rc)
			LLOG("Terminal (" << term << ") [W:" << width << ", H:" << height << "] opened.");
		return !rc;
	});
}

bool SshChannel::SetEnv(const Txt& variable, const Txt& value)
{
	return Run([=, this]() mutable {
		i32 rc = libssh2_channel_setenv(*channel, variable, value);
		if(!WouldBlock(rc) && rc < 0) ThrowError(rc);
		if(!rc)	LLOG("Environment variable '" << variable << "' set to " << value);
		return !rc;
	});
}

bool SshChannel::PutEof()
{
	return Run([=, this]() mutable {
		i32 rc = libssh2_channel_send_eof(*channel);
		if(!WouldBlock(rc) && rc < 0) ThrowError(rc);
		if(!rc)	LLOG("EOF message is sent to the server.");
		return !rc;
	});
}

bool SshChannel::GetEof()
{
	return Run([=, this]() mutable {
		i32 rc = libssh2_channel_wait_eof(*channel);
		if(!WouldBlock(rc) && rc < 0) ThrowError(rc);
		if(!rc) LLOG("EOF message is acknowledged by the server.");;
		return !rc;
	});
}

bool SshChannel::IsEof()
{
	bool b = false;
	INTERLOCKED
	{
		b = libssh2_channel_eof(*channel) != 0;
	}
	if(b)
		LLOG("EOF received.");
	return b;
}

bool SshChannel::SetTerminalSize(i32 width, i32 height)
{
	return Run([=, this]() mutable { return SetPtySz(width, height) >= 0; });
}

i32 SshChannel::SetPtySz(i32 w, i32 h)
{
	i32 rc = libssh2_channel_request_pty_size(*channel, w, h);
	if(!WouldBlock(rc) && rc < 0) rc = 1;
	if(rc == 1)	LLOG("Предупреждение: Couldn't set terminal size!");
	if(rc == 0)	LLOG("Terminal size adjusted. [W:" << w << ", H:" << h << "]");
	return rc;
}

bool SshChannel::SetReadWindowSize(uint32 size, bool force)
{
	return Run([=, this]() mutable { return SetWndSz(size, force); });
}

bool SshChannel::SetWndSz(uint32 size, bool force)
{
	i32 rc = libssh2_channel_receive_window_adjust2(*channel, size, (u8) force, nullptr);
	if(!WouldBlock(rc) && rc < 0) ThrowError(rc);
	if(!rc) LLOG(Format("Receive window size set is to %d.", AsTxt(size)));
	return !rc;
}

i32 SshChannel::GetExitCode()
{
	i32 rc = 0;
	INTERLOCKED
	{
		rc = libssh2_channel_get_exit_status(*channel);
	}
	LLOG("Exit code: " << rc);
	return rc;
}

Txt SshChannel::GetExitSignal()
{
	char *sig = (char *)"none";
	size_t len = 0;
	Txt s;
	INTERLOCKED
	{
		libssh2_channel_get_exit_signal(*channel, &sig, &len, nullptr, nullptr, nullptr, nullptr);
	}
	s.Set(sig, len);
	LLOG("Exit signal: " << s);
	return s;
}

i32 SshChannel::Get(uk ptr, i32 size, i32 sid)
{
	done = 0;
	Run([=, this]() mutable {
		while(done < size && !IsEof() && !IsTimeout()) {
			i32 rc = Read(ptr, size, sid);
			if(rc < 0) return false;
			if(!rc) break;
			UpdateClient();
		}
		return true;
	});
	return GetDone();
}

Txt SshChannel::Get(i32 size, i32 sid)
{
	TxtBuf sb(size);
	i32 len = Get(~sb, size, sid);
	sb.SetCount(len);
	return pick(Txt(sb));
}

Txt SshChannel::GetLine(i32 maxlen, i32 sid)
{
	done = 0;
	Txt line;
	Run([=, this, &line]{
		bool eol = false;
		do {
			i32 c = Read(sid);
			if(c == -1)
				break;
			ssh->start_time = msecs();
			if(c == '\r')
				continue;
			if(line.GetLength() >= maxlen)
				line = Null;
			eol = c == '\n';
			if(!eol) {
				line.Cat(c);
				done++;
			}
		}
		while(!eol && !IsEof() && !IsTimeout());
		return eol || IsEof();
	});
	return line;
}

i32 SshChannel::Put(ukk ptr, i32 size, i32 sid)
{
	done = 0;
	Run([=, this]() mutable {
		while(done < size && !IsEof() && !IsTimeout()) {
			i32 rc = Write(ptr, size, sid);
			if(rc < 0) return false;
			if(!rc) break;
			UpdateClient();
		}
		return true;
	});
	return GetDone();
}

i32 SshChannel::Read(uk ptr, i32 size, i32 sid)
{
	i32 sz = min(size - done, ssh->chunk_size);

	i32 rc = static_cast<i32>(
		libssh2_channel_read_ex(*channel, sid, (char*) ptr + done, size_t(sz))
		);
	if(rc < 0 && !WouldBlock(rc)) {
		ThrowError(rc);
	}
	else
	if(rc > 0) {
		done += rc;
		ssh->start_time = msecs();
		VLOG("Read stream #" << sid << ": " << rc << " bytes read.");
	}
	return rc;
}

i32 SshChannel::Read(i32 sid)
{
	char c;
	done = 0;
	return Read(&c, 1, sid) == 1 ? i32(c) : -1;
}

i32 SshChannel::Write(ukk ptr, i32 size, i32 sid)
{
	i32 sz = min(size - done, ssh->chunk_size);

	i32 rc = static_cast<i32>(
		libssh2_channel_write_ex(*channel, sid, (tukk) ptr + done, size_t(sz))
		);
	if(rc < 0 && !WouldBlock(rc)) {
		ThrowError(rc);
	}
	else
	if(rc > 0) {
		done += rc;
		ssh->start_time = msecs();
		VLOG("Write stream #" << sid << ": " << rc << " bytes written.");
	}
	return rc;
}

bool SshChannel::Write(char c, i32 sid)
{
	done = 0;
	return Write(&c, 1, sid) == 1;
}

dword SshChannel::EventWait(i32 fd, dword events, i32 tv)
{
	SocketWaitEvent we;
	we.Add(fd, events);
	return we.Wait(tv) > 0 ? we[0] : 0;
}

bool SshChannel::ProcessEvents(Txt& input)
{
	Buffer<char> buffer(ssh->chunk_size, 0);

	return Run([=, this, &buffer, &input]{
		done = 0;
		i32 len = Read(buffer, ssh->chunk_size);
		ReadWrite(input, buffer, len);
		while(!input.IsEmpty() && InProgress()) {
			done = 0;
			len = Write(~input, input.GetLength());
			if(len <= 0)
				break;
			input.Remove(0, len);
		}
		return IsEof();
	});
}


bool SshChannel::Shut(const Txt& msg, bool nowait)
{
	bool eof = false;
	if(PutEof() && !nowait)
		eof = GetEof();
	if(Close() && eof)
		WaitClose();
	if(!IsNull(msg))
		SetError(-1, msg);
	return !IsError();
}

SshChannel::SshChannel(SshSession& session)
: done(0)
{
	ssh->otype		= CHANNEL;
	ssh->session	= session.GetHandle();
	ssh->socket		= &session.GetSocket();
	ssh->timeout	= session.GetTimeout();
	ssh->waitstep   = session.GetWaitStep();
	ssh->whenwait	= Proxy(session.WhenWait);
}

SshChannel::~SshChannel()
{
	Exit();
}
}